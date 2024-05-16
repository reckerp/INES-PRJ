#ifndef MOTORCONTROL_H
#include "MotorControlState.h"
#define MOTORCONTROL_H

#define MOT_PWM_PIN 14   // PWM pin for motor on ESP
#define MOT_AIN2_PIN 7 // Motor control pins on MCP23017 (A)
#define MOT_AIN1_PIN 6
#define MOT_STBY_PIN 5

void motorcontrol_task(void *parameter);

void motorcontrol_set_speed(MotorControlState *state);
void motorcontrol_set_direction(MotorControlState *state);
void motorcontrol_set_on(MotorControlState *state);

#endif