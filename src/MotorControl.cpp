#include "MotorControl.h"
#include <Arduino.h>
#include <Wire.h>
#include "MutexDef.h"

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

// CW - AIN1 HIGH, AIN2 LOW, STBY HIGH => 10100000 => 0xA0

void motorcontrol_task(void *parameter) {
    Serial.println("[MOTORCONTROL] Starting motor control task");
    MotorControlState *state = (MotorControlState *)parameter;

    pinMode(MOT_PWM_PIN, OUTPUT);

    ledcSetup(motChannel, freq, resolution);
    ledcAttachPin(MOT_PWM_PIN, motChannel);

    while (1) {
        motorcontrol_set_on(state);
        motorcontrol_set_speed(state);
        delay(10);
    }

    // Wire.beginTransmission(0x20);
    // Wire.write(0x12); // IODIRA register
    // Wire.write((byte)0b10100000); // CW
    // Wire.endTransmission();

    // int count = 0;
    // while(count < 10) {
    //     Serial.print("Count: ");
    //     Serial.println(count);
    //     for (int i = 0; i < 255; i++) {
    //         ledcWrite(motChannel, i);
    //         vTaskDelay(100 / portTICK_PERIOD_MS);
    //     }

    //     for (int i = 255; i > 0; i--) {
    //         ledcWrite(motChannel, i);
    //         vTaskDelay(100 / portTICK_PERIOD_MS);
    //     }
    //     count++;
    // }
    // Wire.beginTransmission(0x20);
    // Wire.write(0x12); // IODIRA register
    // Wire.write(0x00); // Stop motor
    // Wire.endTransmission();
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
    if (xSemaphoreTake(wireMutex, portMAX_DELAY) == pdTRUE) {
        Wire.beginTransmission(0x20);
        Wire.write(0x12); // IODIRA register
        if (state->direction) {
            state->status = 0b10100000;
            Wire.write((byte)state->status); // CW
        } else {
            state->status = 0b01000000;
            Wire.write((byte)state->status); // CCW
        }
        Wire.endTransmission();
        xSemaphoreGive(wireMutex);
    }
}

void motorcontrol_set_on(MotorControlState *state) {
    // if status shows on and state is on then do nothing
    if (state->status & 0b10000000 && state->on || state->status == 0x00 && !state->on) {
        return;
    }


    if (xSemaphoreTake(wireMutex, portMAX_DELAY) == pdTRUE) {
        Wire.beginTransmission(0x20);
        Wire.write(0x12); // IODIRA register
        if (state->on) {
            state->direction = 1;
            state->speed = 10;
            state->status = 0b10100000;

            Wire.write((byte)state->status); // CW
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