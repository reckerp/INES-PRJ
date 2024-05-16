#ifndef MOTORCONTROLSTATE_H
#define MOTORCONTROLSTATE_H

struct MotorControlState {
    unsigned char status;
    int speed; // 0 - 255
    int direction; // 0 - 1
    bool on; // 0 - 1
};


#endif