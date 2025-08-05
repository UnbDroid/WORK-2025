#ifndef MOTORDC_H
#define MOTORDC_H

#include <Arduino.h>
#include "driver/ledc.h"

class MotorDC {
public:
    // Construtor
    MotorDC(int pin_r_pwm, int pin_l_pwm, ledc_channel_t channel_r, ledc_channel_t channel_l, int freq, int resolution);

    // Método para definir a velocidade (-100 a 100)
    void setSpeed(int speedPercent);

private:
    // Variáveis internas da classe
    int R_PWM_PIN;
    int L_PWM_PIN;
    ledc_channel_t R_PWM_CHANNEL;
    ledc_channel_t L_PWM_CHANNEL;
};

#endif