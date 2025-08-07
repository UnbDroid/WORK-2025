#ifndef BRACO_SERVO_H
#define BRACO_SERVO_H

#include <ESP32Servo.h>
#include <math.h>

class BracoServo {
public:
    BracoServo(int pinoOmbro, int pinoCotovelo, int pinoGarra, float L1, float L2);
    void iniciar();

    void moverParaPlano(float r, float z);

    void retornarAoDescanso();
    void abrirGarra();
    void fecharGarra();

private:
    Servo _servoOmbro, _servoCotovelo, _servoGarra;
    int _pinoOmbro, _pinoCotovelo, _pinoGarra;
    float _L1, _L2;
    int _anguloAtualOmbro, _anguloAtualCotovelo, _anguloAtualGarra;

    const int GARRA_ABERTA = 70;
    const int GARRA_FECHADA = 0;
    const int VELOCIDADE_MOVIMENTO = 15;

    bool calcular_cinematica_inversa(float r, float z, int& anguloOmbro, int& anguloCotovelo);
    void mover_suave(int anguloOmbroFinal, int anguloCotoveloFinal, int anguloGarraFinal);
};

#endif