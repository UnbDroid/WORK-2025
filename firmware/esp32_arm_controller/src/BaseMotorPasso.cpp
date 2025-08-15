#include "BaseMotorPasso.h"

// Implementação do Construtor
BaseMotorPasso::BaseMotorPasso(int pinoStep, int pinoDir)
    // Inicializa o objeto _stepper usando a "lista de inicialização".
    // AccelStepper::DRIVER informa à biblioteca que estamos usando um driver STEP/DIR.
    : _stepper(AccelStepper::DRIVER, pinoStep, pinoDir) {
}

// Implementação da função iniciar()
void BaseMotorPasso::iniciar(float maxSpeed, float acceleration) {
    _stepper.setMaxSpeed(maxSpeed);
    _stepper.setAcceleration(acceleration);
}

// Implementação de girarParaAngulo()
void BaseMotorPasso::girarParaAngulo(float angulo) {
    // Converte o ângulo desejado (em graus) para a posição de passo alvo.
    // Usamos 'long' para garantir que o número possa ser grande o suficiente.
    long posicaoAlvo = (angulo / 360.0) * _passosPorRevolucao;
    _stepper.moveTo(posicaoAlvo);
}

// Implementação de retornarAoInicio()
void BaseMotorPasso::retornarAoInicio() {
    girarParaAngulo(0);
}

// Implementação de estaMovendo()
bool BaseMotorPasso::estaMovendo() {
    // A função distanceToGo() da biblioteca retorna 0 apenas quando o motor atinge o destino.
    return _stepper.distanceToGo() != 0;
}

// Implementação de run()
void BaseMotorPasso::run() {
    // Esta função simplesmente repassa a chamada para a biblioteca AccelStepper.
    // Sem chamá-la constantemente no loop, o motor não se move.
    _stepper.run();
}