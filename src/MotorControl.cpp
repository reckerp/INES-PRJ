#include "MotorControl.h"
#include "MutexDef.h"
#include <Arduino.h>
#include <Wire.h>

const int freq = 5000;
const int resolution = 8;
const int motChannel = 0;

// 7 - 10000000 - 128 - 0x80
// 6 - 01000000 - 64 - 0x40
// 5 - 00100000 - 32 - 0x20
// 4 - 00010000 - 16 - 0x10
// 3 - 00001000 - 8 - 0x08
// 2 - 00000100 - 4 - 0x04
// 1 - 00000010 - 2 - 0x02
// 0 - 00000001 - 1 - 0x01

// AIN2 - 7
// AIN1 - 6
// STBY - 5

void motorcontrol_task(void *parameter) {
    Serial.println("[MOTORCONTROL] Starting motor control task");
    MotorControlState *state = (MotorControlState *)parameter;

    pinMode(MOT_PWM_PIN, OUTPUT);

    ledcSetup(motChannel, freq, resolution);
    ledcAttachPin(MOT_PWM_PIN, motChannel);

    while (1) {
        motorcontrol_set_on(state);
        delay(10);
        motorcontrol_set_direction(state);
        delay(10);
        motorcontrol_set_speed(state);
        delay(10);
    }
}

void motorcontrol_set_speed(MotorControlState *state) {
    if (state->speed > 255) {
        state->speed = 255;
    }

    if (state->speed < 0) {
        state->speed = 0;
    }

    ledcWrite(motChannel, state->speed);
}

void motorcontrol_set_direction(MotorControlState *state) {
    // Determine the desired status based on the direction
    if (state->on == 0) {
        return;
    }

    byte desiredStatus = state->direction == 0 ? 0b10100000 : 0b01100000;

    // If the desired direction is the same as the current status, return early
    if (state->status == desiredStatus) {
        return;
    }

    // If there is a change in direction, first decelerate to zero speed
    if (xSemaphoreTake(wireMutex, portMAX_DELAY) == pdTRUE) {
        // Decelerate to zero
        int oldSpeed = state->speed;
        for (int i = state->speed; i >= 0; i -= 50) {
            ledcWrite(motChannel, i);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        ledcWrite(motChannel, 0);
        state->speed = 0;

        // Change direction
        Wire.beginTransmission(0x20);
        Wire.write(0x12); // GPIOA register
        state->status = desiredStatus;
        Wire.write((byte)state->status);
        Wire.endTransmission();
        xSemaphoreGive(wireMutex);

        // Speed up to previous speed
        for (int i = 0; i <= oldSpeed; i += 50) {
            ledcWrite(motChannel, i);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        state->speed = oldSpeed;
        ledcWrite(motChannel, state->speed);
    }
}

void motorcontrol_set_on(MotorControlState *state) {
    // if status shows on and state is on then do nothing
    bool isMotorOn = (state->status & 0b00100000) != 0;

    // If motor should be on and is already on, return
    if (state->on && isMotorOn) {
        return;
    }

    // If motor should be off and is already off, return
    if (!state->on && !isMotorOn) {
        return;
    }

    if (xSemaphoreTake(wireMutex, portMAX_DELAY) == pdTRUE) {
        Wire.beginTransmission(0x20);
        Wire.write(0x12); // IODIRA register
        if (state->on) {
            state->status |= 0b00100000; // Set only STBY HIGH

            Wire.write((byte)state->status); // Start motor
        } else {
            for (int i = state->speed; i >= 0; i = -50) {
                ledcWrite(motChannel, i);
                delay(100);
            }
            ledcWrite(motChannel, 0);
            state->speed = 0;
            state->direction = 0;
            state->status = 0x00;

            Wire.write((byte)state->status); // Stop motor
        }
        Wire.endTransmission();
        xSemaphoreGive(wireMutex);
    }
}