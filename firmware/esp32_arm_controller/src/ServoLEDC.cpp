#include "ServoLEDC.h"

// Constantes do PWM que serão usadas por todos os servos
const int PWM_FREQ = 50;
const int PWM_RESOLUTION = 16;

ServoLEDC::ServoLEDC() {
    _current_angle = 90; 
}

void ServoLEDC::attach(int pin, int channel, int min_pulse_us, int max_pulse_us) {
    _channel = channel;
    _min_pulse_us = min_pulse_us;
    _max_pulse_us = max_pulse_us;

    ledcSetup(_channel, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(pin, _channel);
}

void ServoLEDC::write(int angle) {
    angle = constrain(angle, 0, 180);
    _current_angle = angle;
    ledcWrite(_channel, angleToDutyCycle(angle));
}

// --- IMPLEMENTAÇÃO DA NOVA FUNÇÃO DE VELOCIDADE ---
void ServoLEDC::writeSlow(int targetAngle, int speed) {
    // Garante que os valores de entrada estão dentro dos limites
    targetAngle = constrain(targetAngle, 0, 180);
    speed = constrain(speed, 1, 100);

    // Mapeia a velocidade (1-100) para um valor de delay em milissegundos.
    // speed = 1 (mais lento) -> delay de 30ms
    // speed = 100 (mais rápido) -> delay de 1ms
    int delayTime = map(speed, 1, 100, 30, 1);

    int currentAngle = read(); // Pega a posição atual

    // Se o alvo for maior que a posição atual, movemos para frente
    if (targetAngle > currentAngle) {
        for (int angle = currentAngle; angle <= targetAngle; angle++) {
            write(angle); // Usa a função write original para mover 1 grau
            delay(delayTime); // A pausa que define a velocidade
        }
    }
    // Se o alvo for menor, movemos para trás
    else if (targetAngle < currentAngle) {
        for (int angle = currentAngle; angle >= targetAngle; angle--) {
            write(angle); // Usa a função write original para mover 1 grau
            delay(delayTime); // A pausa que define a velocidade
        }
    }
}

int ServoLEDC::read() {
    return _current_angle;
}

uint32_t ServoLEDC::angleToDutyCycle(int angle) {
    long pulse_us = map(angle, 0, 180, _min_pulse_us, _max_pulse_us);
    return (uint32_t)(((float)pulse_us / 20000.0) * 65535);
}