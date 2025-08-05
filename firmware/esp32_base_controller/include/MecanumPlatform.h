#ifndef MECANUM_PLATFORM_H
#define MECANUM_PLATFORM_H

#include "MotorDC.h"

class MecanumPlatform {
public:
    MecanumPlatform();
    void setup();
    void setSpeed(float linear_x, float linear_y, float angular_z);
    void update();

private:
    MotorDC motor1;
    MotorDC motor2;
    MotorDC motor3;
    MotorDC motor4;
};

#endif