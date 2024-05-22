#include "Heartbeat.h"
#include "IOReader.h"
#include "MotorControl.h"
#include "MotorControlState.h"
#include "Display.h"
#include "MutexDef.h"
#include <Arduino.h>
#include <Wire.h>

MotorControlState motorControlState = {
    .status = 0x00, .speed = 0, .direction = 0, .on = 0};
SemaphoreHandle_t wireMutex = NULL; // This is the definition

void setup() {
    Serial.begin(9600);
    Serial.println("Starting up");
    // -----------------------------------------------------------
    TaskHandle_t heartbeat_taskhandle;
    TaskHandle_t motorcontrol_taskhandle;
    TaskHandle_t ioreader_taskhandle;
    TaskHandle_t display_taskhandle;

    // -----------------------------------------------------------
    wireMutex = xSemaphoreCreateMutex();
    if (wireMutex == NULL) {
        Serial.println("Failed to create wire mutex");
        while (1)
            ;
    }

    // SETUP HEARTBEAT
    xTaskCreate(heartbeat_task, "heartbeat_task", 2000, NULL, 1,
                &heartbeat_taskhandle);

    // -----------------------------------------------------------
    // SETUP I2C
    // DEFINE BANK A AS OUTPUTS
    if (xSemaphoreTake(wireMutex, portMAX_DELAY) == pdTRUE) {
        Wire.begin();
        Wire.beginTransmission(0x20);
        Wire.write(0x00); // IODIRA register
        Wire.write(0x00); // Set all of port A to outputs
        Wire.endTransmission();

        // DEFINE BANK B AS INPUTS
        Wire.beginTransmission(0x20);
        Wire.write(0x01); // IODIRB register
        Wire.write(0xFF); // Set all of port B to inputs
        Wire.endTransmission();

        // DEFINE PULLUP FOR BANK B PIN 0
        Wire.beginTransmission(0x20);
        Wire.write(0x0D); // GPPUB register
        Wire.write(0x0F); // Set pullup for pin 0, 1, 2, 3
        Wire.endTransmission();
        xSemaphoreGive(wireMutex);
    }

    // -----------------------------------------------------------
    // SETUP IO READER
    xTaskCreate(ioreader_task, "ioreader_task", 10000, &motorControlState, 5,
                &ioreader_taskhandle);

    // -----------------------------------------------------------
    // SETUP MOTOR CONTROL
    xTaskCreate(motorcontrol_task, "motorcontrol_task", 10000,
                &motorControlState, 2, &motorcontrol_taskhandle);

    // -----------------------------------------------------------
    // SETUP DISPLAY
    xTaskCreate(display_task, "display_task", 10000, &motorControlState, 1,
                &display_taskhandle);

}

void loop() {
    // READ BANK B PIN 0
    Wire.beginTransmission(0x20);
    Wire.write(0x13); // GPIOB register
    Wire.endTransmission();
    // Request PIN 0
    Wire.requestFrom(0x20, 1);
    byte bankB = Wire.read();
    Serial.print("Bank B: ");
    Serial.println(bankB, BIN);
    delay(1000);
}
