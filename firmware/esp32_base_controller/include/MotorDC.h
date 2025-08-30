#ifndef MOTORDC_H
#define MOTORDC_H

#include "driver/pcnt.h" 
#include <Arduino.h>
#include "driver/ledc.h"

class MotorDC {
public:
    // Construtor
    MotorDC(int input_1_pin, int input_2_pin, int pwm_pin, int pwm_channel, int pin_enca, int pin_encb, const pcnt_unit_t PCNT_CHANNEL) : INPUT_1_PIN(input_1_pin), INPUT_2_PIN(input_2_pin), PWM_PIN(pwm_pin), PWM_CHANNEL(pwm_channel), PIN_ENCA(pin_enca), PIN_ENCB(pin_encb),
        totalPulseCount(0), rpm(0.0), lastHardwareCount(0), pcnt_unit(PCNT_CHANNEL) {
            // Configuração do PWM
            ledcSetup(this->PWM_CHANNEL, 5000, 8);
            ledcAttachPin(this->PWM_PIN, this->PWM_CHANNEL);

            // Configuração dos pinos de controle do motor
            pinMode(this->INPUT_1_PIN, OUTPUT);
            pinMode(this->INPUT_2_PIN, OUTPUT);

            setupEncoderPNCT(this);
        }

    void setupEncoderPNCT(MotorDC* motor);

    void updateEncoder(MotorDC* motor);

    // void readEncoder();

    // Define a velocidade alvo para o motor em radianos por segundo (unidade do ROS)
    void setTargetSpeed(float rads_per_sec);

    void moveMotor(MotorDC* motor, int velocity);

    // Atualiza o controle PID
    // void updatePID();

private:
    void setSpeedPercent(int speedPercent);

    // Variáveis internas da classe
    const int INPUT_1_PIN, INPUT_2_PIN, PWM_PIN, PWM_CHANNEL, PIN_ENCA, PIN_ENCB;

    // Variáveis do encoder
    const pcnt_unit_t pcnt_unit;
    volatile int64_t totalPulseCount;
    double rpm;
    int16_t lastHardwareCount;
    unsigned long lastUpdateRPM = 0;


    // PID
    float target_speed_rps = 0.0;
    long prev_position = 0;
    float prev_error = 0.0;
    float integral_error = 0.0;
    unsigned long prev_time = 0;
};

#endif