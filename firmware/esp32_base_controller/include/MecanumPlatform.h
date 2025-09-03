#ifndef MECANUM_PLATFORM_H
#define MECANUM_PLATFORM_H

#include "MotorDC.h"

class MecanumPlatform {
public:
    MotorDC motor1;
    MotorDC motor2;
    MotorDC motor3;
    MotorDC motor4;

    MecanumPlatform();
    void init();
    void setup();
    void setSpeed(float linear_x, float linear_y, float angular_z);
    void update();
};

#endif