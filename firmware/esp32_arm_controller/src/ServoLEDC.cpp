#include "ServoLEDC.h"

// Constantes do PWM que serão usadas por todos os servos
const int PWM_FREQ = 50;
const int PWM_RESOLUTION = 16;

ServoLEDC::ServoLEDC() {
}

void ServoLEDC::attach(int pin, int channel, int min_pulse_us, int max_pulse_us) {
    _channel = channel;
    _min_pulse_us = min_pulse_us;
    _max_pulse_us = max_pulse_us;

    // 1. Configura o canal LEDC com a frequência e resolução
    ledcSetup(_channel, PWM_FREQ, PWM_RESOLUTION);

    // 2. Anexa o pino desejado ao canal que acabamos de configurar
    ledcAttachPin(pin, _channel);
}

void ServoLEDC::write(int angle) {
    // Garante que o ângulo esteja entre 0 e 180
    angle = constrain(angle, 0, 180);
    // Escreve o valor de duty cycle correspondente ao ângulo no canal do servo
    ledcWrite(_channel, angleToDutyCycle(angle));
}

// Esta é a mesma função de conversão que usamos no teste anterior
uint32_t ServoLEDC::angleToDutyCycle(int angle) {
    long pulse_us = map(angle, 0, 180, _min_pulse_us, _max_pulse_us);
    return (uint32_t)(((float)pulse_us / 20000.0) * 65535);
}