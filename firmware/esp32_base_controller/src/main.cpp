#include <Arduino.h>
#include "MecanumPlatform.h" 

MecanumPlatform platform;

// Parâmetros para testes
const float TEST_SPEED_LINEAR = 0.3; 
const float TEST_SPEED_ANGULAR = 1.0; 
const int MOVE_DURATION = 2000;
const int STOP_DURATION = 1000;

// Função auxiliar para executar um movimento
void executeMove(float vx, float vy, float wz, int duration_ms) {
    Serial.printf("Comando: vx=%.2f, vy=%.2f, wz=%.2f\n", vx, vy, wz);
    
    platform.setSpeed(vx, vy, wz);


    unsigned long startTime = millis();
    while (millis() - startTime < duration_ms) {
        platform.update();
        delay(5); 
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nINICIANDO TESTE DE MOVIMENTAÇÃO\n");

    platform.setup();

    Serial.println("Setup completo. Robô pronto para iniciar a sequência de testes em 5 segundos...\n");
    delay(5000);
}

void loop() {

    Serial.println("\n1. Teste: ANDAR PARA FRENTE\n");
    executeMove(TEST_SPEED_LINEAR, 0, 0, MOVE_DURATION);
    executeMove(0, 0, 0, STOP_DURATION); 

    Serial.println("\n2. Teste: ANDAR PARA TRÁS\n");
    executeMove(-TEST_SPEED_LINEAR, 0, 0, MOVE_DURATION);
    executeMove(0, 0, 0, STOP_DURATION); 

    Serial.println("\n3. Teste: DESLIZAR PARA A DIREITA (STRAFE)\n");
    executeMove(0, -TEST_SPEED_LINEAR, 0, MOVE_DURATION); 
    executeMove(0, 0, 0, STOP_DURATION); 

    Serial.println("\n4. Teste: DESLIZAR PARA A ESQUERDA (STRAFE)\n");
    executeMove(0, TEST_SPEED_LINEAR, 0, MOVE_DURATION); 
    executeMove(0, 0, 0, STOP_DURATION); 

    Serial.println("\n5. Teste: GIRAR SENTIDO HORÁRIO\n");
    executeMove(0, 0, -TEST_SPEED_ANGULAR, MOVE_DURATION);
    executeMove(0, 0, 0, STOP_DURATION); 

    Serial.println("\n6. Teste: GIRAR SENTIDO ANTI-HORÁRIO\n");
    executeMove(0, 0, TEST_SPEED_ANGULAR, MOVE_DURATION);