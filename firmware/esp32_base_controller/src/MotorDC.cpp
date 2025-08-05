#include "MotorDC.h"

// O construtor agora é responsável apenas por configurar o hardware do PWM
MotorDC::MotorDC(int pin_r_pwm, int pin_l_pwm, ledc_channel_t channel_r, ledc_channel_t channel_l, int freq, int resolution) {
    // Guarda os pinos e canais internamente
    this->R_PWM_PIN = pin_r_pwm;
    this->L_PWM_PIN = pin_l_pwm;
    this->R_PWM_CHANNEL = channel_r;
    this->L_PWM_CHANNEL = channel_l;

    // Configura os canais PWM
    ledcSetup(this->R_PWM_CHANNEL, freq, resolution);
    ledcSetup(this->L_PWM_CHANNEL, freq, resolution);

    // Anexa os pinos aos canais configurados
    ledcAttachPin(this->R_PWM_PIN, this->R_PWM_CHANNEL);
    ledcAttachPin(this->L_PWM_PIN, this->L_PWM_CHANNEL);
}

// Este é o ÚNICO método necessário para controlar o motor.
// Recebe uma porcentagem (-100 para trás, 100 para frente) e a converte em PWM.
void MotorDC::setSpeed(int speedPercent) {
    // Limita o valor de entrada entre -100 e 100
    speedPercent = constrain(speedPercent, -100, 100);

    // Mapeia o valor absoluto (0-100) para a resolução PWM (0-65535)
    // Usamos 16 bits (65535) porque foi o que definimos no main.cpp
    int pwmValue = map(abs(speedPercent), 0, 100, 0, 65535);

    if (speedPercent > 0) {
        // Move para frente
        ledcWrite(this->R_PWM_CHANNEL, pwmValue);
        ledcWrite(this->L_PWM_CHANNEL, 0);
    } 
    else if (speedPercent < 0) {
        // Move para trás
        ledcWrite(this->R_PWM_CHANNEL, 0);
        ledcWrite(this->L_PWM_CHANNEL, pwmValue);
    } 
    else {
        // Para o motor (freio)
        ledcWrite(this->R_PWM_CHANNEL, 0);
        ledcWrite(this->L_PWM_CHANNEL, 0);
    }
}