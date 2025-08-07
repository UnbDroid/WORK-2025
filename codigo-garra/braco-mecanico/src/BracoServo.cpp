#include "BracoServo.h"
#include "Arduino.h"

// Implementação do Construtor (sem alterações)
BracoServo::BracoServo(int pinoOmbro, int pinoCotovelo, int pinoGarra, float L1, float L2) {
    _pinoOmbro = pinoOmbro;
    _pinoCotovelo = pinoCotovelo;
    _pinoGarra = pinoGarra;
    _L1 = L1;
    _L2 = L2;
}

// Implementação da função iniciar() (sem alterações)
void BracoServo::iniciar() {
    _servoOmbro.attach(_pinoOmbro);
    _servoCotovelo.attach(_pinoCotovelo);
    _servoGarra.attach(_pinoGarra);
    retornarAoDescanso();
    delay(1000);
}

// ALTERADO: Função renomeada para moverParaPlano
void BracoServo::moverParaPlano(float r, float z) {
    int anguloOmbroFinal, anguloCotoveloFinal;
    // A lógica interna permanece, mas agora usa 'r' explicitamente como o alcance horizontal.
    if (calcular_cinematica_inversa(r, z, anguloOmbroFinal, anguloCotoveloFinal)) {
        mover_suave(anguloOmbroFinal, anguloCotoveloFinal, _anguloAtualGarra);
    } else {
        Serial.println("ERRO: Posição fora de alcance.");
    }
}

// Funções de descanso e da garra (sem alterações)
void BracoServo::retornarAoDescanso() {
    mover_suave(150, 150, _anguloAtualGarra);
}

void BracoServo::abrirGarra() {
    mover_suave(_anguloAtualOmbro, _anguloAtualCotovelo, GARRA_ABERTA);
}

void BracoServo::fecharGarra() {
    mover_suave(_anguloAtualOmbro, _anguloAtualCotovelo, GARRA_FECHADA);
}

// Função de movimento suave (sem alterações)
void BracoServo::mover_suave(int anguloOmbroFinal, int anguloCotoveloFinal, int anguloGarraFinal) {
    int passos = max(abs(anguloOmbroFinal - _anguloAtualOmbro), max(abs(anguloCotoveloFinal - _anguloAtualCotovelo), abs(anguloGarraFinal - _anguloAtualGarra)));
    if (passos == 0) return;

    float incOmbro = (float)(anguloOmbroFinal - _anguloAtualOmbro) / passos;
    float incCotovelo = (float)(anguloCotoveloFinal - _anguloAtualCotovelo) / passos;
    float incGarra = (float)(anguloGarraFinal - _anguloAtualGarra) / passos;

    for (int i = 1; i <= passos; i++) {
        float novoOmbro = _anguloAtualOmbro + incOmbro * i;
        float novoCotovelo = _anguloAtualCotovelo + incCotovelo * i;
        float novoGarra = _anguloAtualGarra + incGarra * i;
        _servoOmbro.write((int)novoOmbro);
        _servoCotovelo.write((int)novoCotovelo);
        _servoGarra.write((int)novoGarra);
        delay(VELOCIDADE_MOVIMENTO);
    }
    _anguloAtualOmbro = anguloOmbroFinal;
    _anguloAtualCotovelo = anguloCotoveloFinal;
    _anguloAtualGarra = anguloGarraFinal;
    _servoOmbro.write(_anguloAtualOmbro);
    _servoCotovelo.write(_anguloAtualCotovelo);
    _servoGarra.write(_anguloAtualGarra);
}

// ALTERADO: Parâmetro 'x' renomeado para 'r' para maior clareza.
bool BracoServo::calcular_cinematica_inversa(float r, float z, int& anguloOmbro, int& anguloCotovelo) {
    float d = sqrt(r * r + z * z);
    if (d > _L1 + _L2 || d < abs(_L1 - _L2)) {
        return false;
    }
    float alpha1 = atan2(z, r);
    float alpha2 = acos((_L1 * _L1 + d * d - _L2 * _L2) / (2 * _L1 * d));
    float theta1 = alpha1 + alpha2;
    float beta = acos((_L1 * _L1 + _L2 * _L2 - d * d) / (2 * _L1 * _L2));
    float theta2 = M_PI - beta;
    anguloOmbro = (int)(theta1 * 180.0 / M_PI);
    anguloCotovelo = (int)(theta2 * 180.0 / M_PI);
    anguloOmbro = constrain(anguloOmbro, 0, 180);
    anguloCotovelo = constrain(anguloCotovelo, 0, 180);
    return true;
}