#include <Arduino.h>
#include <Wire.h>
#include "Heartbeat.h"
#include "MotorControl.h"

void setup() {
  // SETUP HEARTBEAT
  TaskHandle_t heartbeat_taskhandle;
  TaskHandle_t motorcontrol_taskhandle;
  Serial.begin(9600);
  Serial.println("Starting up");
  xTaskCreate(heartbeat_task, "heartbeat_task", 10000, NULL, 1, &heartbeat_taskhandle);
  
  // SETUP I2C
  Wire.begin();
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // Set all of port A to outputs
  Wire.endTransmission();

  // SETUP MOTOR CONTROL
  xTaskCreate(motorcontrol_task, "motorcontrol_task", 10000, NULL, 2, &motorcontrol_taskhandle);
}

void loop() {
}
