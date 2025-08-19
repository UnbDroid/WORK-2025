#ifndef SERVO_LEDC_H
#define SERVO_LEDC_H

#include <Arduino.h>

class ServoLEDC {
public:
    ServoLEDC();

    void attach(int pin, int channel, int min_pulse_us = 500, int max_pulse_us = 2500);

    void write(int angle);

private:
    int _channel;
    int _min_pulse_us;
    int _max_pulse_us;

    uint32_t angleToDutyCycle(int angle);
};

#endif