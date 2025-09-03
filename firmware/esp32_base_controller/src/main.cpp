#include <Arduino.h>
#include "config.h"
#include "MecanumPlatform.h"
#include "MotorDC.h"

void setup() {
  Serial.begin(115200);
  delay(2000); 
  Serial.println(">>> INICIANDO TESTE DO CONTROLADOR PID + FEEDFORWARD <<<");

  MotorDC motor1(M1_IN1_PIN, M1_IN2_PIN, M1_PWM_PIN, LEDC_CHANNEL_0, M1_ENCODER_A_PIN, M1_ENCODER_B_PIN, M1_PCNT_UNIT, 0.0, 0.0, 0.0, 0.0);
  motor1.init();

  unsigned long tempoInicioTeste = millis();
}

void loop() {

}