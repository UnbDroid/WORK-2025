#include <Arduino.h>
#include "BracoServo.h"
#include "BaseMotorPasso.h"

// --- Definições de Pinos e Medidas (sem alterações) ---
#define PINO_OMBRO       13
#define PINO_COTOVELO    22
#define PINO_GARRA       21
#define PINO_BASE_STEP   25
#define PINO_BASE_DIR    26
#define COMPRIMENTO_L1   19.0
#define COMPRIMENTO_L2   24.0

// --- Cria os objetos (sem alterações) ---
BracoServo braco(PINO_OMBRO, PINO_COTOVELO, PINO_GARRA, COMPRIMENTO_L1, COMPRIMENTO_L2);
BaseMotorPasso base(PINO_BASE_STEP, PINO_BASE_DIR);

// --- NOVA FUNÇÃO COORDENADORA ---
// Esta função recebe coordenadas 3D e as converte em movimentos para o robô.
void moverParaCoordenadaXYZ(float x, float y, float z) {
    Serial.printf("\nRecebido novo alvo -> X: %.1f, Y: %.1f, Z: %.1f\n", x, y, z);

    // --- Parte 1: Calcular o ângulo da base e a distância horizontal (r) ---
    // atan2 é uma função que calcula o arco-tangente e já nos dá o quadrante correto.
    float angulo_base_rad = atan2(y, x);
    // Converter o resultado de radianos para graus.
    float angulo_base_graus = angulo_base_rad * 180.0 / M_PI;

    // Calcular a distância horizontal 'r' usando o Teorema de Pitágoras.
    float r = sqrt(x * x + y * y);

    Serial.printf("Calculado -> Angulo da Base: %.1f graus, Distancia Horizontal (r): %.1f cm\n", angulo_base_graus, r);

    // --- Parte 2: Comandar os motores ---
    // Gira a base para o ângulo calculado.
    base.girarParaAngulo(angulo_base_graus);
    // Move o braço para a distância horizontal (r) e altura (z) calculadas.
    braco.moverParaPlano(r, z);

    // Espera a base terminar seu movimento, já que o braço se move ao mesmo tempo.
    while (base.estaMovendo()) {
        base.run(); // É crucial chamar run() para o motor de passo se mover.
        // Pequeno delay para não sobrecarregar o processador.
        delay(1); 
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n--- INICIANDO SISTEMA DO BRACO MECANICO 3D ---");
    braco.iniciar();
    base.iniciar(800.0, 500.0);
    Serial.println("Sistema pronto. A sequencia de teste comecara em 3 segundos...");
    delay(3000);
}

void loop() {
    Serial.println("\n=== INICIANDO CICLO DE TESTE 3D ===");
    
    // --- Ponto 1: À frente (X=30, Y=0, Z=10) ---
    Serial.println("Movendo para o Ponto 1: FRENTE");
    braco.abrirGarra();
    moverParaCoordenadaXYZ(30.0, 0.0, 10.0);
    delay(500);
    braco.fecharGarra();
    delay(500);
    braco.retornarAoDescanso();
    delay(500);

    // --- Ponto 2: À direita (X=0, Y=30, Z=10) ---
    Serial.println("Movendo para o Ponto 2: DIREITA");
    moverParaCoordenadaXYZ(0.0, 30.0, 10.0);
    delay(500);
    braco.abrirGarra();
    delay(500);
    braco.retornarAoDescanso();
    delay(500);

    // --- Ponto 3: Na diagonal esquerda (X=-20, Y=20, Z=5) ---
    Serial.println("Movendo para o Ponto 3: DIAGONAL ESQUERDA");
    braco.fecharGarra(); // Levar a garra fechada
    moverParaCoordenadaXYZ(-20.0, 20.0, 5.0);
    delay(500);
    braco.abrirGarra();
    delay(500);

    // --- Finalizar: Retornar à posição inicial ---
    Serial.println("Retornando a posicao inicial...");
    braco.retornarAoDescanso();
    base.retornarAoInicio();
    while (base.estaMovendo()) { base.run(); }

    Serial.println("\n=== CICLO COMPLETO. Reiniciando em 10 segundos. ===");
    delay(10000);
}