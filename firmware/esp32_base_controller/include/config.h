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

// Motor 1 (Direito Frontal)
#define M1_PWM_PIN 15 // Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define M1_IN1_PIN 5 // Pino de direção 1, conectado ao IN1 ou IN3
#define M1_IN2_PIN 18 // Pino de direção 2, conectado ao IN2 ou IN4
#define M1_ENCODER_A_PIN 32 // Pino do encoder A do motor 1
#define M1_ENCODER_B_PIN 33 // Pino do encoder B do motor 1
#define M1_PCNT_UNIT PCNT_UNIT_0

// Motor 2 (Esquerdo Frontal)
#define M2_PWM_PIN 13 // Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define M2_IN1_PIN  22 // Pino de direção 1, conectado ao IN1 ou IN3
#define M2_IN2_PIN  23 // Pino de direção 2, conectado ao IN2 ou IN4
#define M2_ENCODER_A_PIN  25 // Pino do encoder A do motor 2
#define M2_ENCODER_B_PIN  26// Pino do encoder B do motor 2
#define M2_PCNT_UNIT PCNT_UNIT_1

// Motor 3 (Direito Traseiro)
#define M3_PWM_PIN 2 // Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define M3_IN1_PIN 16 // Pino de direção 1, conectado ao IN1 ou IN3
#define M3_IN2_PIN 17 // Pino de direção 2, conectado ao IN2 ou IN4
#define M3_ENCODER_A_PIN 34 // Pino do encoder A do motor 3
#define M3_ENCODER_B_PIN 35 // Pino do encoder B do motor 3
#define M3_PCNT_UNIT PCNT_UNIT_2

//Motor 4 (Esquerdo Traseiro)
#define M4_PWM_PIN 12 // Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define M4_IN1_PIN 27 // Pino de direção 1, conectado ao IN1 ou IN3
#define M4_IN2_PIN 14 // Pino de direção 2, conectado ao IN2 ou IN4
#define M4_ENCODER_A_PIN 39 // Pino do encoder A do motor 4 
#define M4_ENCODER_B_PIN 36 // Pino do encoder B do motor 4
#define M4_PCNT_UNIT PCNT_UNIT_3

// --- Configurações do PWM (LEDC) ---
#define PWM_FREQUENCY 10000 // Frequência do PWM em Hz

const int M1_PWM_CHANNEL = 0;      // Canal LEDC a ser usado (0-15)
const int M2_PWM_CHANNEL = 1;      // Canal LEDC a ser usado (0-15)
const int M3_PWM_CHANNEL = 2;      // Canal LEDC a ser usado (0-15)
const int M4_PWM_CHANNEL = 3;      // Canal LEDC a ser usado (0-15)

// PID 
#define KP 1.2
#define KI 0.001
#define KD 0.025

#endif