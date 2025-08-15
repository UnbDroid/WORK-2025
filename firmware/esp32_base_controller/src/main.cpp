#include <Arduino.h>
#include "config.h"

const int PULSOS_POR_REVOLUCAO = 1440; 

// ===================================================================

// --- Variáveis Globais ---

// Variável para contar os pulsos do encoder.
// "volatile" é crucial para que a variável seja compartilhada de forma segura
// entre a rotina de interrupção e o loop principal.
volatile long encoderPulsos = 0;

// Constantes para cálculo
const float DOIS_PI = 2.0 * PI;

// --- Variáveis para o Cálculo de Velocidade ---
long pulsoAnterior = 0;
unsigned long tempoAnterior = 0;
const int intervaloCalculo = 100; // Calcular a velocidade a cada 100 ms

// ===================================================================
// === ROTINA DE SERVIÇO DE INTERRUPÇÃO (ISR) ===
// ===================================================================
// Esta função é chamada AUTOMATICAMENTE pelo hardware da ESP32
// toda vez que o pino M1_ENCODER_A_PIN muda de LOW para HIGH (RISING).
// Deve ser o mais RÁPIDA e CURTA possível.
void IRAM_ATTR contarPulsoISR() {
  encoderPulsos++;
}
// ===================================================================

/**
 * @brief Controla a potência e direção de um motor DC.
 * @param power A potência desejada, de -100 (ré máxima) a 100 (frente máxima).
 */
void setMotorPower(int power) {
  power = constrain(power, -100, 100);

  if (power > 0) {
    digitalWrite(M1_IN1_PIN, HIGH);
    digitalWrite(M1_IN2_PIN, LOW);
  } else if (power < 0) {
    digitalWrite(M1_IN1_PIN, LOW);
    digitalWrite(M2_IN2_PIN, HIGH);
  } else {
    digitalWrite(M1_IN1_PIN, LOW);
    digitalWrite(M2_IN2_PIN, LOW);
  }
  uint32_t pwmValue = map(abs(power), 0, 100, 0, 255);
  ledcWrite(M1_PWM_CHANNEL, pwmValue);
}


void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Leitura de Encoder e Cálculo de Velocidade (rad/s)");

  // --- Configuração dos Pinos ---
  pinMode(M1_IN1_PIN, OUTPUT);
  pinMode(M2_IN2_PIN, OUTPUT);
  pinMode(M1_ENCODER_A_PIN, INPUT_PULLUP); // INPUT_PULLUP é bom se o encoder for de coletor aberto

  // --- Configuração do PWM (LEDC) ---
  ledcSetup(M1_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(M1_PWM_PIN, M1_PWM_CHANNEL);

  // --- Configuração da Interrupção ---
  // Anexa a função 'contarPulsoISR' ao pino do encoder.
  // Ela será acionada na borda de subida (RISING) do sinal.
  attachInterrupt(digitalPinToInterrupt(M1_ENCODER_A_PIN), contarPulsoISR, RISING);

  tempoAnterior = millis();
}

void loop() {
  // --- Bloco de Cálculo de Velocidade (não bloqueante) ---
  if (millis() - tempoAnterior >= intervaloCalculo) {
    
    // 1. Lê e reinicia o contador de pulsos de forma segura
    // Desabilitar interrupções temporariamente garante que não vamos ler
    // o valor de 'encoderPulsos' enquanto a ISR está tentando modificá-lo.
    noInterrupts();
    long pulsosNoIntervalo = encoderPulsos;
    encoderPulsos = 0;
    interrupts();

    // 2. Calcula o tempo decorrido em segundos
    float deltaTempo_s = (millis() - tempoAnterior) / 1000.0;
    
    // 3. Calcula as revoluções no intervalo
    float revolucoes = (float)pulsosNoIntervalo / PULSOS_POR_REVOLUCAO;
    
    // 4. Calcula a velocidade angular em rad/s
    // Fórmula: (revoluções * 2 * PI) / tempo_em_segundos
    float velocidade_rad_s = (revolucoes * DOIS_PI) / deltaTempo_s;

    // 5. Imprime o resultado
    Serial.print("Pulsos contados: ");
    Serial.print(pulsosNoIntervalo);
    Serial.print(" | Velocidade: ");
    Serial.print(velocidade_rad_s);
    Serial.println(" rad/s");

    // Atualiza o tempo para o próximo cálculo
    tempoAnterior = millis();
  }

  // --- Bloco de Teste do Motor ---
  // Para este exemplo, vamos apenas ligar o motor com uma potência fixa.
  // Em um sistema PID, a velocidade calculada acima seria a "Entrada" (Input) do PID,
  // e a função setMotorPower seria chamada com a "Saída" (Output) do PID.
  setMotorPower(100); // Liga o motor com 50% da potência
}