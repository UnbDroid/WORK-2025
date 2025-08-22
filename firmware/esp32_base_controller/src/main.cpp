#include <Arduino.h>
#include "config.h"

const int PULSOS_POR_REVOLUCAO = 1440;
const float DIAMETRO_RODA_CM = 8;
const float CIRCUNFERENCIA_CM = PI * DIAMETRO_RODA_CM;
const float DISTANCIA_CM = 30.0;

const long PULSOS_ALVO = (DISTANCIA_CM / CIRCUNFERENCIA_CM) * PULSOS_POR_REVOLUCAO;

volatile long encoderPulsos = 0;

// ISR usando canal A e B (quadratura)
void IRAM_ATTR contarPulsoISR() {
  // Se B == HIGH, sentido 1; se LOW, sentido oposto
  if (digitalRead(M1_ENCODER_B_PIN) == HIGH) {
    encoderPulsos++;
  } else {
    encoderPulsos--;
  }
}

// Liga motor: positivo para frente, negativo para trás
void setMotorPower(int power) {
  power = constrain(power, -100, 100); 
  uint32_t pwmValue = map(abs(power), 0, 100, 0, 255);

  if (power > 0) {
    digitalWrite(M1_IN1_PIN, HIGH);
    digitalWrite(M1_IN2_PIN, LOW);
  } else if (power < 0) {
    digitalWrite(M1_IN1_PIN, LOW);
    digitalWrite(M1_IN2_PIN, HIGH);
  } else {
    digitalWrite(M1_IN1_PIN, LOW);
    digitalWrite(M1_IN2_PIN, LOW);
  }

  ledcWrite(M1_PWM_CHANNEL, pwmValue);
}

void pararMotor() {
  digitalWrite(M1_IN1_PIN, LOW);
  digitalWrite(M1_IN2_PIN, LOW);
  ledcWrite(M1_PWM_CHANNEL, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando ida e volta 30cm cada");

  pinMode(M1_IN1_PIN, OUTPUT);
  pinMode(M1_IN2_PIN, OUTPUT);
  pinMode(M1_PWM_PIN, OUTPUT);

  pinMode(M1_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(M1_ENCODER_B_PIN, INPUT_PULLUP);

  ledcSetup(M1_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(M1_PWM_PIN, M1_PWM_CHANNEL);

  // Interrupção no canal A usando CHANGE (mais confiável)
  attachInterrupt(digitalPinToInterrupt(M1_ENCODER_A_PIN), contarPulsoISR, CHANGE);
}

void loop() {
  // === Fase 1: Andar 30cm para frente ===
  encoderPulsos = 0;
  setMotorPower(70); // frente
  while (abs(encoderPulsos) < PULSOS_ALVO) {
    Serial.print("Indo para frente: ");
    Serial.print(abs(encoderPulsos));
    Serial.print(" / ");
    Serial.println(PULSOS_ALVO);
    delay(100);
  }
  pararMotor();
  Serial.println("Alcançou 30cm à frente");
  delay(1000);

  // === Fase 2: Voltar 30cm ===
  encoderPulsos = 0;
  setMotorPower(-70); // ré
  while (abs(encoderPulsos) < PULSOS_ALVO) {
    Serial.print("Voltando: ");
    Serial.print(abs(encoderPulsos));
    Serial.print(" / ");
    Serial.println(PULSOS_ALVO);
    delay(100);
  }
  pararMotor();
  Serial.println("Voltou 30cm");
  
  // Para de vez
  while (true) delay(1000);
}
