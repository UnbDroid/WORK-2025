#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "driver/ledc.h"

// PARÂMETROS FÍSICOS DO ROBÔ //
#define WHEEL_RADIUS 0.04        // Raio da roda em metros
#define LX 0.225                 // Metade da LARGURA do robô (distância do centro à roda)
#define LY 0.075                 // Metade do COMPRIMENTO do robô (distância do centro à roda)
#define TICKS_PER_REVOLUTION 2400 // Pulsos do encoder para uma volta completa da RODA 

// PINOS DOS MOTORES E ENCODERS //

// Motor 1 (Frontal Direito)
#define M1_PWM_PIN 13
#define M1_INPUT_1_PIN 12
#define M1_INPUT_2_PIN 14
#define M1_ENCA_PIN 34
#define M1_ENCB_PIN 35

// Motor 2 (Frontal Esquerdo)
#define M2_PWM_PIN 15
#define M2_INPUT_1_PIN 27
#define M2_INPUT_2_PIN 26
#define M2_ENCA_PIN 36
#define M2_ENCB_PIN 39

// Motor 3 (Traseiro Direito)
#define M3_PWM_PIN 2
#define M3_INPUT_1_PIN 25
#define M3_INPUT_2_PIN 33
#define M3_ENCA_PIN 18
#define M3_ENCB_PIN 19 

// Motor 4 (Frontal Esquerdo)
#define M4_PWM_PIN 4
#define M4_INPUT_1_PIN 16
#define M4_INPUT_2_PIN 17
#define M4_ENCA_PIN 22
#define M4_ENCB_PIN 23 

// CONFIGURAÇÃO DO HARDWARE //

// PWM
#define PWM_FREQ 10000
#define PWM_RESOLUTION 16 

// CANAIS PWM 
const ledc_channel_t M1_PWM_CHANNEL = LEDC_CHANNEL_0;
const ledc_channel_t M2_PWM_CHANNEL = LEDC_CHANNEL_1;
const ledc_channel_t M3_PWM_CHANNEL = LEDC_CHANNEL_2;
const ledc_channel_t M4_PWM_CHANNEL = LEDC_CHANNEL_3;

// PID 
#define KP 1.2
#define KI 0.001
#define KD 0.025

#endif