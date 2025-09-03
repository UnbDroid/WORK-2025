#ifndef MOTORDC_H
#define MOTORDC_H

#include <Arduino.h>
#include "driver/pcnt.h"
#include "driver/ledc.h"

class MotorDC {
public:
    volatile int64_t totalPulseCount; 

    MotorDC(int input_1_pin, int input_2_pin, int pwm_pin, ledc_channel_t pwm_channel, 
            int pin_enca, int pin_encb, pcnt_unit_t pcnt_unit, 
            float m_f, float c_f, float m_b, float c_b);

    void init(); 
    void setTargetSpeed(float rads_per_sec);
    void updatePID();
    float getTargetSpeedRPS();

    float rpm;
    float current_speed_rps;

private:
    void setupEncoderPCNT(); 
    void setSpeedPercent(int speedPercent);

    const int INPUT_1_PIN, INPUT_2_PIN, PWM_PIN;
    const int PIN_ENCA, PIN_ENCB;
    const ledc_channel_t PWM_CHANNEL; 
    const pcnt_unit_t PCNT_UNIT;

    const float M_FRONT, C_FRONT, M_BACK, C_BACK;
    
    int16_t lastHardwareCount;
    float target_speed_rps;
    float prev_error;
    float integral_error;
    unsigned long prev_time;
};

#endif // MOTORDC_H