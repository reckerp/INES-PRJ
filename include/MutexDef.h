#ifndef MUTEXDEF_H
#define MUTEXDEF_H
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t wireMutex;

#endif