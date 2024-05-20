#include "IOReader.h"
#include "MotorControlState.h"
#include <Arduino.h>
#include <Wire.h>

void ioreader_task(void *parameter) {
    MotorControlState *state = (MotorControlState *)parameter;
    while (1) {
        // READ BANK B PIN 0
        Wire.beginTransmission(0x20);
        Wire.write(0x13); // GPIOB register
        Wire.endTransmission();
        // Request PIN 0
        Wire.requestFrom(0x20, 1);
        uint8_t bankB = Wire.read();
        unsigned char on = !(bankB & 0b00000001);
        unsigned char direction = bankB & 0b00001000;
        unsigned char speedDecr = !(bankB & 0b00000100);
        unsigned char speedIncr = !(bankB & 0b00000010);

        if(on) {
            state->on = !state->on;
            Serial.print("Motor is ");
            Serial.println(state->on ? "ON" : "OFF");
        }

        state->direction = direction;
        if (speedDecr) {
            state->speed -= 10;
        } else if (speedIncr) {
            state->speed += 10;
        }
        
        delay(100);
    }
}
