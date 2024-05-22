#ifndef MUTEXDEF_H
#define MUTEXDEF_H
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// extern keyword is used to declare a variable that is defined in another file
extern SemaphoreHandle_t wireMutex;

#endif