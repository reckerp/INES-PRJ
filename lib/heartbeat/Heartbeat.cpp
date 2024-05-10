#include "Heartbeat.h"
#include <Arduino.h>

void heartbeat(void *parameter) {
    const uint8_t ledPin = 1;
    pinMode(ledPin, OUTPUT);
    while (1) {
        Serial.println("[HEARTBEAT]: ALIVE AT " + String(millis()));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        digitalWrite(ledPin, HIGH);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        digitalWrite(ledPin, LOW);
    }
}