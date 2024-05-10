#include "Heartbeat.h"
#include <Arduino.h>
#define LED 4

void heartbeat_task(void *parameter) {
    Serial.println("[HEARTBEAT] Starting heartbeat task");
    pinMode(LED, OUTPUT);
    for(;;) {
        Serial.print("[HEARTBEAT] I am alive at: ");
        Serial.println(millis());
        digitalWrite(LED, HIGH);
        delay(1000);
        digitalWrite(LED, LOW);
        delay(1000);
    }

}