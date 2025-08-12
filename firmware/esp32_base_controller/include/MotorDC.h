#ifndef MOTORDC_H
#define MOTORDC_H

#include <Arduino.h>
#include "driver/ledc.h"

class MotorDC {
public:
    // Construtor
    MotorDC(int input_1_pin, int input_2_pin, int pwm_pin, ledc_channel_t pwm_channel, int pin_enca, int pin_encb);

    // Configura as interrupções do encoder
    void setupEncoder(void (*isr_a)(), void (*isr_b)());

    // Define a velocidade alvo para o motor em radianos por segundo (unidade do ROS)
    void setTargetSpeed(float rads_per_sec);

    // Atualiza o controle PID
    void updatePID();

    // Posição
    volatile long position = 0;

private:
    void setSpeedPercent(int speedPercent);

    // Variáveis internas da classe
    int INPUT_1_PIN; // Mexe para a direita
    int INPUT_2_PIN; // Mexe para a esquerda
    int PWM_PIN;
    ledc_channel_t PWM_CHANNEL;
    int PIN_ENCA;
    int PIN_ENCB;

    float target_speed_rps = 0.0;
    long prev_position = 0;
    float prev_error = 0.0;
    float integral_error = 0.0;
    unsigned long prev_time = 0;
};

#endif