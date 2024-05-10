#include "MotorControl.h"
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

// CW - AIN1 HIGH, AIN2 LOW, STBY HIGH => 10100000 => 0xA0

void motorcontrol_task(void *parameter) {
    Serial.println("[MOTORCONTROL] Starting motor control task");   

    pinMode(MOT_PWM_PIN, OUTPUT);

    ledcSetup(motChannel, freq, resolution);
    ledcAttachPin(MOT_PWM_PIN, motChannel);

    Wire.beginTransmission(0x20);
    Wire.write(0x12); // IODIRA register
    Wire.write((byte)0b10100000); // CW 
    Wire.endTransmission();

    int count = 0;
    while(count < 10) {
        Serial.print("Count: ");
        Serial.println(count);
        for (int i = 0; i < 255; i++) {
            ledcWrite(motChannel, i);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        for (int i = 255; i > 0; i--) {
            ledcWrite(motChannel, i);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        count++;
    }
    Wire.beginTransmission(0x20);
    Wire.write(0x12); // IODIRA register
    Wire.write(0x00); // Stop motor
    Wire.endTransmission();
}