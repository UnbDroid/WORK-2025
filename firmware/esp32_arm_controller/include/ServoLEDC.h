#ifndef SERVO_LEDC_H
#define SERVO_LEDC_H

#include <Arduino.h>

class ServoLEDC {
public:
    ServoLEDC();

    void attach(int pin, int channel, int min_pulse_us = 500, int max_pulse_us = 2500);

    void write(int angle);

    // --- NOVA FUNÇÃO ---
    // Move o servo para o ângulo desejado com uma velocidade controlada.
    // 'speed' pode ser um valor de 1 (mais lento) a 100 (mais rápido).
    void writeSlow(int targetAngle, int speed);

    // Retorna o último ângulo que foi enviado para o servo
    int read();

private:
    int _channel;
    int _min_pulse_us;
    int _max_pulse_us;
    int _current_angle;

    uint32_t angleToDutyCycle(int angle);
};

#endif