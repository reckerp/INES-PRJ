#include <Arduino.h>
#include "Heartbeat.h"

void setup() {
  xTaskCreate(
    heartbeat,   /* Task function. */
    "heartbeat", /* String with name of task. */
    2000,
    (void *)NULL,
    1,
    NULL
  );
}

void loop() {
}
