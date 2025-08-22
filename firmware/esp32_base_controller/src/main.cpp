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


void IRAM_ATTR contarPulsoISR() {
  encoderPulsos++;
}

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

  attachInterrupt(digitalPinToInterrupt(M1_ENCODER_A_PIN), contarPulsoISR, RISING);

  tempoAnterior = millis();
}

void loop() {
  if (millis() - tempoAnterior >= intervaloCalculo) {
    
    // 1. Lê e reinicia o contador de pulsos de forma segura
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

    tempoAnterior = millis();
  }
  setMotorPower(100); 
}