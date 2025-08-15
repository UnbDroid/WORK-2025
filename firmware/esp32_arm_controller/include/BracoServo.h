#ifndef BRACO_SERVO_H
#define BRACO_SERVO_H

#include <ESP32Servo.h>
#include <math.h>

class BracoServo {
public:
    // Construtor atualizado para aceitar dois pinos para o ombro
    BracoServo(int pinoOmbro1, int pinoOmbro2, int pinoCotovelo, int pinoGarra, float L1, float L2);
    void iniciar();

    void moverParaPlano(float r, float z);

    void retornarAoDescanso();
    void abrirGarra();
    void fecharGarra();

private:
    // Adicionado um segundo servo para o ombro
    Servo _servoOmbro1, _servoOmbro2, _servoCotovelo, _servoGarra;
    // Adicionado um segundo pino para o ombro
    int _pinoOmbro1, _pinoOmbro2, _pinoCotovelo, _pinoGarra;
    float _L1, _L2;
    int _anguloAtualOmbro, _anguloAtualCotovelo, _anguloAtualGarra;

    const int GARRA_ABERTA = 70;
    const int GARRA_FECHADA = 0;
    const int VELOCIDADE_MOVIMENTO = 15;

    bool calcular_cinematica_inversa(float r, float z, int& anguloOmbro, int& anguloCotovelo);
    void mover_suave(int anguloOmbroFinal, int anguloCotoveloFinal, int anguloGarraFinal);
};

#endif