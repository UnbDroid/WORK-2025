#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "driver/ledc.h"

// PARÂMETROS FÍSICOS DO ROBÔ //
#define WHEEL_RADIUS 0.05       // Raio da roda em metros
#define LX 0.22                 // Metade da LARGURA do robô (distância do centro à roda)
#define LY 0.22                 // Metade do COMPRIMENTO do robô (distância do centro à roda)
#define TICKS_PER_REVOLUTION 2400 // Pulsos do encoder para uma volta completa da RODA 

// PINOS DOS MOTORES E ENCODERS //

// Motor 1 (Frontal Esquerdo)
#define M1_RPWM_PIN 1
#define M1_LPWM_PIN 2
#define M1_ENCA_PIN 3
#define M1_ENCB_PIN 4

// Motor 2 (Frontal Direito)
#define M2_RPWM_PIN 1
#define M2_LPWM_PIN 2
#define M2_ENCA_PIN 3
#define M2_ENCB_PIN 4

// Motor 3 (Traseiro Esquerdo)
#define M3_RPWM_PIN 1  
#define M3_LPWM_PIN 2   
#define M3_ENCA_PIN 3   
#define M3_ENCB_PIN 4   

// Motor 4 (Traseiro Direito) 
#define M4_RPWM_PIN 1   
#define M4_LPWM_PIN 2    
#define M4_ENCA_PIN 3    
#define M4_ENCB_PIN 4    

// CONFIGURAÇÃO DO HARDWARE //

// PWM
#define PWM_FREQ 10000
#define PWM_RESOLUTION 16 

// CANAIS PWM 
const ledc_channel_t M1_R_CHANNEL = LEDC_CHANNEL_0;
const ledc_channel_t M1_L_CHANNEL = LEDC_CHANNEL_1;
const ledc_channel_t M2_R_CHANNEL = LEDC_CHANNEL_2;
const ledc_channel_t M2_L_CHANNEL = LEDC_CHANNEL_3;
const ledc_channel_t M3_R_CHANNEL = LEDC_CHANNEL_4;
const ledc_channel_t M3_L_CHANNEL = LEDC_CHANNEL_5;
const ledc_channel_t M4_R_CHANNEL = LEDC_CHANNEL_6;
const ledc_channel_t M4_L_CHANNEL = LEDC_CHANNEL_7;


// PID 
#define KP 1.2
#define KI 0.001
#define KD 0.025

#endif