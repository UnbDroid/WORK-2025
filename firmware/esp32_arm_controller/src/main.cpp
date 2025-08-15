#include <Arduino.h>
#include "BracoServo.h"
#include "BaseMotorPasso.h"

// --- Definições de Pinos e Medidas (ALTERADO) ---
#define PINO_OMBRO_1     13 // Pino para o primeiro servo do ombro
#define PINO_OMBRO_2     12 // Pino para o segundo servo do ombro (escolha um pino disponível)
#define PINO_COTOVELO    22
#define PINO_GARRA       21
#define PINO_BASE_STEP   27
#define PINO_BASE_DIR    22
#define COMPRIMENTO_L1   19.0
#define COMPRIMENTO_L2   24.0

// --- Cria os objetos (ALTERADO) ---
// Atualiza a criação do objeto 'braco' para passar os dois pinos do ombro
BracoServo braco(PINO_OMBRO_1, PINO_OMBRO_2, PINO_COTOVELO, PINO_GARRA, COMPRIMENTO_L1, COMPRIMENTO_L2);
BaseMotorPasso base(PINO_BASE_STEP, PINO_BASE_DIR);

// --- O RESTANTE DO CÓDIGO PERMANECE O MESMO ---

void moverParaCoordenadaXYZ(float x, float y, float z) {
    Serial.printf("\nRecebido novo alvo -> X: %.1f, Y: %.1f, Z: %.1f\n", x, y, z);
    float angulo_base_rad = atan2(y, x);
    float angulo_base_graus = angulo_base_rad * 180.0 / M_PI;
    float r = sqrt(x * x + y * y);
    Serial.printf("Calculado -> Angulo da Base: %.1f graus, Distancia Horizontal (r): %.1f cm\n", angulo_base_graus, r);
    base.girarParaAngulo(angulo_base_graus);
    braco.moverParaPlano(r, z);
    while (base.estaMovendo()) {
        base.run();
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
    
    Serial.println("Movendo para o Ponto 1: FRENTE");
    braco.abrirGarra();
    moverParaCoordenadaXYZ(30.0, 0.0, 10.0);
    delay(500);
    braco.fecharGarra();
    delay(500);
    braco.retornarAoDescanso();
    delay(500);

    Serial.println("Movendo para o Ponto 2: DIREITA");
    moverParaCoordenadaXYZ(0.0, 30.0, 10.0);
    delay(500);
    braco.abrirGarra();
    delay(500);
    braco.retornarAoDescanso();
    delay(500);

    Serial.println("Movendo para o Ponto 3: DIAGONAL ESQUERDA");
    braco.fecharGarra();
    moverParaCoordenadaXYZ(-20.0, 20.0, 5.0);
    delay(500);
    braco.abrirGarra();
    delay(500);

    Serial.println("Retornando a posicao inicial...");
    braco.retornarAoDescanso();
    base.retornarAoInicio();
    while (base.estaMovendo()) { base.run(); }

    Serial.println("\n=== CICLO COMPLETO. Reiniciando em 10 segundos. ===");
    delay(10000);
}