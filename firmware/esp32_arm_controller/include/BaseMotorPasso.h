#ifndef BASE_MOTOR_PASSO_H
#define BASE_MOTOR_PASSO_H

#include <AccelStepper.h>

class BaseMotorPasso {
public:
    // Construtor: Prepara a classe com os pinos de STEP e DIRECTION do driver do motor.
    BaseMotorPasso(int pinoStep, int pinoDir);

    // Configura os parâmetros iniciais de velocidade e aceleração.
    void iniciar(float maxSpeed, float acceleration);

    // Move a base para um ângulo específico em graus (0 a 360).
    void girarParaAngulo(float angulo);

    // Retorna a base para a posição zero.
    void retornarAoInicio();

    // Verifica se o motor ainda está em movimento.
    bool estaMovendo();
    
    // Função essencial que deve ser chamada repetidamente no loop principal para gerar os pulsos.
    void run();

private:
    // Objeto da biblioteca AccelStepper que fará o trabalho pesado.
    AccelStepper _stepper;
    
    // Constante para conversão de ângulo para passos. Ajuste conforme seu motor e microrpassos.
    // Exemplo: motor de 1.8 graus/passo (200 passos/volta) com driver em modo 1/8 (8 microrpassos).
    // Total de passos = 200 * 8 = 1600.
    const int _passosPorRevolucao = 1600; 
};

#endif