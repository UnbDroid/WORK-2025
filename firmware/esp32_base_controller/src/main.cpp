#include <Arduino.h>
#include "config.h"
#include "MecanumPlatform.h"

// Cria o objeto principal que representa a nossa plataforma de rodas
MecanumPlatform plataforma;

// --- Variáveis para controlar a sequência de teste ---
unsigned long tempoInicioTeste = 0;
unsigned long ultimoTempoPrint = 0;
const int INTERVALO_PRINT_MS = 200; // Imprime o estado a cada 200ms
const int DURACAO_FASE_MS = 5000;   // Cada fase do teste dura 5 segundos
int faseTeste = 0;

// --- Função para imprimir o estado de todos os motores ---
void printMotorStatus() {
    // Fator para converter rad/s para RPM (para facilitar a leitura)
    const float RADS_PARA_RPM = 9.5492965855;

    // Limpa a tela para uma visualização mais limpa (funciona no Monitor Serial do Arduino IDE 2.x e PlatformIO)
    Serial.print("\033[2J\033[H"); 
    Serial.println("----------- STATUS DOS MOTORES -----------");
    Serial.println("Motor | Alvo (RPM) | Atual (RPM) | Pulsos Totais");
    Serial.println("--------------------------------------------");

    char buffer[100];
    
    // Imprime dados para cada motor
    sprintf(buffer, " M1   | %-10.2f | %-11.2f | %lld", 
            plataforma.motor1.getTargetSpeedRPS() * RADS_PARA_RPM, 
            plataforma.motor1.rpm, 
            plataforma.motor1.totalPulseCount);
    Serial.println(buffer);

    sprintf(buffer, " M2   | %-10.2f | %-11.2f | %lld", 
            plataforma.motor2.getTargetSpeedRPS() * RADS_PARA_RPM, 
            plataforma.motor2.rpm, 
            plataforma.motor2.totalPulseCount);
    Serial.println(buffer);

    sprintf(buffer, " M3   | %-10.2f | %-11.2f | %lld", 
            plataforma.motor3.getTargetSpeedRPS() * RADS_PARA_RPM, 
            plataforma.motor3.rpm, 
            plataforma.motor3.totalPulseCount);
    Serial.println(buffer);

    sprintf(buffer, " M4   | %-10.2f | %-11.2f | %lld", 
            plataforma.motor4.getTargetSpeedRPS() * RADS_PARA_RPM, 
            plataforma.motor4.rpm, 
            plataforma.motor4.totalPulseCount);
    Serial.println(buffer);
    Serial.println("--------------------------------------------");
}


void setup() {
  Serial.begin(115200);
  delay(2000); 
  Serial.println(">>> INICIANDO TESTE DO CONTROLADOR PID + FEEDFORWARD <<<");

  // Chama a função que inicializa os 4 motores (PWM, Pinos, Encoders)
  plataforma.init();
  
  tempoInicioTeste = millis();
  ultimoTempoPrint = millis();
}

void loop() {
  // A função update() DEVE ser chamada em cada ciclo do loop.
  // É ela que executa o cálculo do PID para cada motor.
  plataforma.update();

  unsigned long tempoAtual = millis();

  // --- Máquina de Estados para a Sequência de Teste ---
  if (tempoAtual - tempoInicioTeste > DURACAO_FASE_MS) {
    tempoInicioTeste = tempoAtual; 
    faseTeste = (faseTeste + 1) % 5; // Avança para a próxima fase (0 a 4)

    switch (faseTeste) {
      case 0:
        Serial.println("\n\n>>> FASE 1: MOVER PARA FRENTE (Velocidade Média)");
        plataforma.setSpeed(0.5, 0.0, 0.0); // 0.5 m/s para frente
        break;
      case 1:
        Serial.println("\n\n>>> FASE 2: RODAR (Velocidade Baixa)");
        plataforma.setSpeed(0.0, 0.0, 1.0); // 1.0 rad/s de rotação
        break;
      case 2:
        Serial.println("\n\n>>> FASE 3: MOVER PARA TRÁS (Velocidade Alta)");
        plataforma.setSpeed(-0.8, 0.0, 0.0); // 0.8 m/s para trás
        break;
      case 3:
        Serial.println("\n\n>>> FASE 4: MOVER NA DIAGONAL");
        plataforma.setSpeed(0.3, 0.3, 0.0); // Para frente e para a direita
        break;
      case 4:
        Serial.println("\n\n>>> FASE 5: PARADO");
        plataforma.setSpeed(0.0, 0.0, 0.0); // Parar
        break;
    }
  }

  // --- Lógica de Impressão ---
  // Imprime o estado dos motores no intervalo definido
  if (tempoAtual - ultimoTempoPrint >= INTERVALO_PRINT_MS) {
    ultimoTempoPrint = tempoAtual;
    printMotorStatus();
  }
}