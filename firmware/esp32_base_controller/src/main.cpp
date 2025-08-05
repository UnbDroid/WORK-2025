#include <Arduino.h>
#include "MotorDC.h"

// --- Pinos para Motor 1 ---
#define ENCA1 22
#define ENCB1 23
#define RPWM_PIN1 14
#define LPWM_PIN1 25

// --- Pinos para Motor 2 ---
#define ENCA2 26
#define ENCB2 27
#define RPWM_PIN2 32
#define LPWM_PIN2 33

// --- Pinos para Motor 3 ---
#define ENCA3 26
#define ENCB3 27
#define RPWM_PIN3 32
#define LPWM_PIN3 33

// --- Pinos para Motor 4 ---
#define ENCA4 26
#define ENCB4 27
#define RPWM_PIN4 32
#define LPWM_PIN4 33

// --- PWM ---
#define PWM_FREQ 10000
#define PWM_RESOLUTION 16
const ledc_channel_t RPWM_CHANNEL1 = LEDC_CHANNEL_0;
const ledc_channel_t LPWM_CHANNEL1 = LEDC_CHANNEL_1;
const ledc_channel_t RPWM_CHANNEL2 = LEDC_CHANNEL_2;
const ledc_channel_t LPWM_CHANNEL2 = LEDC_CHANNEL_3;
const ledc_channel_t RPWM_CHANNEL3 = LEDC_CHANNEL_4;
const ledc_channel_t LPWM_CHANNEL3 = LEDC_CHANNEL_5;
const ledc_channel_t RPWM_CHANNEL4 = LEDC_CHANNEL_6;
const ledc_channel_t LPWM_CHANNEL4 = LEDC_CHANNEL_7;

// --- PID ---
const float kp = 1.2, kd = 0.025, ki = 0.001;

// --- Objetos de Motor ---
MotorDC motor1(RPWM_PIN1, LPWM_PIN1, RPWM_CHANNEL1, LPWM_CHANNEL1, PWM_FREQ, PWM_RESOLUTION);
MotorDC motor2(RPWM_PIN2, LPWM_PIN2, RPWM_CHANNEL2, LPWM_CHANNEL2, PWM_FREQ, PWM_RESOLUTION);
MotorDC motor3(RPWM_PIN3, LPWM_PIN3, RPWM_CHANNEL1, LPWM_CHANNEL1, PWM_FREQ, PWM_RESOLUTION);
MotorDC motor4(RPWM_PIN4, LPWM_PIN4, RPWM_CHANNEL2, LPWM_CHANNEL2, PWM_FREQ, PWM_RESOLUTION);

// --- Encoders ---
volatile int64_t pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// --- PID por motor ---
uint64_t prevT1 = 0, prevT2 = 0, prevT3 = 0, prevT4 = 0;
float ePrev1 = 0, eInt1 = 0;
float ePrev2 = 0, eInt2 = 0;
float ePrev3 = 0, eInt3 = 0;
float ePrev4 = 0, eInt4 = 0;

// --- Interrupções Motor 1 ---
void IRAM_ATTR handleA1() {
  bool a = digitalRead(ENCA1), b = digitalRead(ENCB1);
  portENTER_CRITICAL_ISR(&mux);
  pos1 += (a == b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR handleB1() {
  bool a = digitalRead(ENCA1), b = digitalRead(ENCB1);
  portENTER_CRITICAL_ISR(&mux);
  pos1 += (a != b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}

// --- Interrupções Motor 2 ---
void IRAM_ATTR handleA2() {
  bool a = digitalRead(ENCA2), b = digitalRead(ENCB2);
  portENTER_CRITICAL_ISR(&mux);
  pos2 += (a == b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR handleB2() {
  bool a = digitalRead(ENCA2), b = digitalRead(ENCB2);
  portENTER_CRITICAL_ISR(&mux);
  pos2 += (a != b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}

// --- Interrupções Motor 3 ---
void IRAM_ATTR handleA3() {
  bool a = digitalRead(ENCA3), b = digitalRead(ENCB3);
  portENTER_CRITICAL_ISR(&mux);
  pos2 += (a == b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR handleB3() {
  bool a = digitalRead(ENCA3), b = digitalRead(ENCB3);
  portENTER_CRITICAL_ISR(&mux);
  pos2 += (a != b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}

// --- Interrupções Motor 4 ---
void IRAM_ATTR handleA4() {
  bool a = digitalRead(ENCA4), b = digitalRead(ENCB4);
  portENTER_CRITICAL_ISR(&mux);
  pos2 += (a == b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR handleB4() {
  bool a = digitalRead(ENCA4), b = digitalRead(ENCB4);
  portENTER_CRITICAL_ISR(&mux);
  pos2 += (a != b) ? 1 : -1;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  Serial.begin(115200);

  // --- Encoder Pinos ---
  pinMode(ENCA1, INPUT); pinMode(ENCB1, INPUT);
  pinMode(ENCA2, INPUT); pinMode(ENCB2, INPUT);

  // --- Attach Interrupções ---
  attachInterrupt(digitalPinToInterrupt(ENCA1), handleA1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCB1), handleB1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCA2), handleA2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCB2), handleB2, CHANGE);

  prevT1 = micros();
  prevT2 = micros();
  prevT3 = micros();
  prevT4 = micros();
}

void loop() {
  float t = micros() / 1e6;
  float target1 = 2500 * sin(2 * PI * 0.5 * t);
  float target2 = 2500 * sin(2 * PI * 0.5 * t); 
  float target3 = 2500 * sin(2 * PI * 0.5 * t);
  float target4 = 2500 * sin(2 * PI * 0.5 * t); 

  // --- Motor 1 ---
  int64_t p1;
  portENTER_CRITICAL(&mux); p1 = pos1; portEXIT_CRITICAL(&mux);

  uint64_t now1 = micros();
  float dt1 = (now1 - prevT1) / 1e6;
  prevT1 = now1;
  float e1 = target1 - p1;
  eInt1 += e1 * dt1;
  float d1 = (e1 - ePrev1) / dt1;
  ePrev1 = e1;
  float u1 = kp * e1 + ki * eInt1 + kd * d1;
  motor1.setSpeed(u1);

  // --- Motor 2 ---
  int64_t p2;
  portENTER_CRITICAL(&mux); p2 = pos2; portEXIT_CRITICAL(&mux);

  uint64_t now2 = micros();
  float dt2 = (now2 - prevT2) / 1e6;
  prevT2 = now2;
  float e2 = target2 - p2;
  eInt2 += e2 * dt2;
  float d2 = (e2 - ePrev2) / dt2;
  ePrev2 = e2;
  float u2 = kp * e2 + ki * eInt2 + kd * d2;
  motor2.setSpeed(u2);

  // --- Motor 3 ---
  int64_t p3;
  portENTER_CRITICAL(&mux); p3 = pos3; portEXIT_CRITICAL(&mux);

  uint64_t now3 = micros();
  float dt3 = (now3 - prevT3) / 1e6;
  prevT3 = now3;
  float e3 = target3 - p3;
  eInt3 += e3 * dt3;
  float d3 = (e3 - ePrev3) / dt3;
  ePrev3 = e3;
  float u3 = kp * e3 + ki * eInt3 + kd * d3;
  motor2.setSpeed(u2);

  // --- Motor 4 ---
  int64_t p4;
  portENTER_CRITICAL(&mux); p4 = pos4; portEXIT_CRITICAL(&mux);

  uint64_t now4 = micros();
  float dt4 = (now4 - prevT4) / 1e6;
  prevT4 = now4;
  float e4 = target4 - p4;
  eInt4 += e4 * dt4;
  float d4 = (e4 - ePrev4) / dt4;
  ePrev4 = e4;
  float u4 = kp * e4 + ki * eInt4 + kd * d4;
  motor2.setSpeed(u4);

  Serial.printf("M1 → Target: %.1f | Pos: %lld | PWM: %.1f\n\n", target1, p1, u1);
  Serial.printf("M2 → Target: %.1f | Pos: %lld | PWM: %.1f\n\n", target2, p2, u2);
  Serial.printf("M3 → Target: %.1f | Pos: %lld | PWM: %.1f\n\n", target3, p3, u3);
  Serial.printf("M4 → Target: %.1f | Pos: %lld | PWM: %.1f\n\n", target4, p4, u4);

  delay(40);
}
