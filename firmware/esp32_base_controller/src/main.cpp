#include <Arduino.h>
#include "config.h"
#include "driver/ledc.h"

#define PWM_FREQ 1500
#define PWM_RESOLUTION 8

void stop_mov(int tempo){
    ledcWrite(M1_PWM_CHANNEL, 0);
    ledcWrite(M2_PWM_CHANNEL, 0);
    ledcWrite(M3_PWM_CHANNEL, 0);
    ledcWrite(M4_PWM_CHANNEL, 0);
    delay(tempo);
}

void front_mov(int tempo) {
    digitalWrite(M1_INPUT_1_PIN, HIGH);
    digitalWrite(M1_INPUT_2_PIN, LOW);
    ledcWrite(M1_PWM_CHANNEL, 100);
    digitalWrite(M2_INPUT_1_PIN, HIGH);
    digitalWrite(M2_INPUT_2_PIN, LOW);
    ledcWrite(M2_PWM_CHANNEL, 100);
    digitalWrite(M3_INPUT_1_PIN, HIGH);
    digitalWrite(M3_INPUT_2_PIN, LOW);
    ledcWrite(M3_PWM_CHANNEL, 100);
    digitalWrite(M4_INPUT_1_PIN, HIGH);
    digitalWrite(M4_INPUT_2_PIN, LOW);
    ledcWrite(M4_PWM_CHANNEL, 100);
    delay(tempo);
}

void right_mov(int tempo){
    digitalWrite(M1_INPUT_1_PIN, LOW);
    digitalWrite(M1_INPUT_2_PIN, HIGH);
    ledcWrite(M1_PWM_CHANNEL, 100);
    digitalWrite(M2_INPUT_1_PIN, HIGH);
    digitalWrite(M2_INPUT_2_PIN, LOW);
    ledcWrite(M2_PWM_CHANNEL, 100);
    digitalWrite(M3_INPUT_1_PIN, HIGH);
    digitalWrite(M3_INPUT_2_PIN, LOW);
    ledcWrite(M3_PWM_CHANNEL, 100);
    digitalWrite(M4_INPUT_1_PIN, LOW);
    digitalWrite(M4_INPUT_2_PIN, HIGH);
    ledcWrite(M4_PWM_CHANNEL, 100);
    delay(tempo);
}

void left_mov(int tempo){
    digitalWrite(M1_INPUT_1_PIN, HIGH);
    digitalWrite(M1_INPUT_2_PIN, LOW);
    ledcWrite(M1_PWM_CHANNEL, 100);
    digitalWrite(M2_INPUT_1_PIN, LOW);
    digitalWrite(M2_INPUT_2_PIN, HIGH);
    ledcWrite(M2_PWM_CHANNEL, 100);
    digitalWrite(M3_INPUT_1_PIN, LOW);
    digitalWrite(M3_INPUT_2_PIN, HIGH);
    ledcWrite(M3_PWM_CHANNEL, 100);
    digitalWrite(M4_INPUT_1_PIN, HIGH);
    digitalWrite(M4_INPUT_2_PIN, LOW);
    ledcWrite(M4_PWM_CHANNEL, 100);
    delay(tempo);
}

void spin_anthor(int tempo){
    digitalWrite(M1_INPUT_1_PIN, HIGH);
    digitalWrite(M1_INPUT_2_PIN, LOW);
    ledcWrite(M1_PWM_CHANNEL, 100);
    digitalWrite(M2_INPUT_1_PIN, LOW);
    digitalWrite(M2_INPUT_2_PIN, HIGH);
    ledcWrite(M2_PWM_CHANNEL, 100);
    digitalWrite(M3_INPUT_1_PIN, HIGH);
    digitalWrite(M3_INPUT_2_PIN, LOW);
    ledcWrite(M3_PWM_CHANNEL, 100);
    digitalWrite(M4_INPUT_1_PIN, LOW);
    digitalWrite(M4_INPUT_2_PIN, HIGH);
    ledcWrite(M4_PWM_CHANNEL, 100);
    delay(tempo);
}

void spin_hor(int tempo){
    digitalWrite(M1_INPUT_1_PIN, LOW);
    digitalWrite(M1_INPUT_2_PIN, HIGH);
    ledcWrite(M1_PWM_CHANNEL, 100);
    digitalWrite(M2_INPUT_1_PIN, HIGH);
    digitalWrite(M2_INPUT_2_PIN, LOW);
    ledcWrite(M2_PWM_CHANNEL, 100);
    digitalWrite(M3_INPUT_1_PIN, LOW);
    digitalWrite(M3_INPUT_2_PIN, HIGH);
    ledcWrite(M3_PWM_CHANNEL, 100);
    digitalWrite(M4_INPUT_1_PIN, HIGH);
    digitalWrite(M4_INPUT_2_PIN, LOW);
    ledcWrite(M4_PWM_CHANNEL, 100);
    delay(tempo);
}

void setup() {
    pinMode(M1_INPUT_1_PIN, OUTPUT);
    pinMode(M1_INPUT_2_PIN, OUTPUT);
    pinMode(M1_PWM_PIN, OUTPUT);
    pinMode(M2_INPUT_1_PIN, OUTPUT);
    pinMode(M2_INPUT_2_PIN, OUTPUT);
    pinMode(M2_PWM_PIN, OUTPUT);
    pinMode(M3_INPUT_1_PIN, OUTPUT);
    pinMode(M3_INPUT_2_PIN, OUTPUT);
    pinMode(M3_PWM_PIN, OUTPUT);
    pinMode(M4_INPUT_1_PIN, OUTPUT);
    pinMode(M4_INPUT_2_PIN, OUTPUT);
    pinMode(M4_PWM_PIN, OUTPUT);

    ledcSetup(M1_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(M1_PWM_PIN, M1_PWM_CHANNEL);
    ledcSetup(M2_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(M2_PWM_PIN, M2_PWM_CHANNEL);
    ledcSetup(M3_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(M3_PWM_PIN, M3_PWM_CHANNEL);
    ledcSetup(M4_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(M4_PWM_PIN, M4_PWM_CHANNEL);
}

void loop() {
    right_mov(1000);
    stop_mov(500);
    left_mov(1000);
    delay(500);
    spin_anthor(2000);
    delay(500);
    spin_hor(2000);
    delay(500);
}
