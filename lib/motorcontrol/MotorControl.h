#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#define MOT_PWM_PIN 14   // PWM pin for motor on ESP
#define MOT_AIN2_PIN 7 // Motor control pins on MCP23017 (A)
#define MOT_AIN1_PIN 6
#define MOT_STBY_PIN 5

void motorcontrol_task(void *parameter);

#endif