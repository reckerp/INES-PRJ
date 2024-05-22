#include "Display.h"
#include "MotorControlState.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 23);

void display_task(void *parameter) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false,
                       false)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    MotorControlState *state = (MotorControlState *)parameter;

    display.setTextColor(WHITE);
    display.setTextSize(1);


    while (1) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("MOTOR CONTROL");
        display.setCursor(0, 10);
        display.print("On: " + String(state->on == 0 ? "OFF" : "ON"));
        display.setCursor(0, 20);
        display.print("Direction: " + String(state->direction == 0 ? "CW" : "CCW"));
        display.setCursor(0, 30);
        display.print("Speed: " + String(state->speed));
        display.display();

        delay(10);
    }
}