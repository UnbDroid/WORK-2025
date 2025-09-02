#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "driver/pcnt.h" // Incluído aqui para o tipo pcnt_unit_t

#define WHEEL_RADIUS 0.04;
#define LX 0.125;
#define LY 0.215;


const int M1_PWM_CHANNEL = 0;      // Canal LEDC a ser usado (0-15)
const int M2_PWM_CHANNEL = 1;      // Canal LEDC a ser usado (0-15)
const int M3_PWM_CHANNEL = 2;      // Canal LEDC a ser usado (0-15)
const int M4_PWM_CHANNEL = 3;      // Canal LEDC a ser usado (0-15)

// ===================================================================
// PARÂMETROS GERAIS
// ===================================================================
#define RPM_CALC_INTERVAL 200      // Intervalo em ms para calcular e exibir o RPM
#define NUM_LEITURAS_MEDIA 10      // Quantidade de leituras para a média móvel
#define PWM_FREQUENCY 10000        // Frequência do PWM em Hz
#define TICKS_PER_REVOLUTION 1440.0 // Pulsos do encoder para uma volta completa da RODA

// ===================================================================
// PINOS E CANAIS DOS MOTORES
// ===================================================================

// Motor 1 (Direito Frontal)
#define M1_IN1_PIN 5
#define M1_IN2_PIN 18
#define M1_PWM_PIN 15
#define M1_ENCODER_A_PIN 32
#define M1_ENCODER_B_PIN 33
#define M1_PCNT_UNIT PCNT_UNIT_0

// Motor 2 (Esquerdo Frontal)
#define M2_IN1_PIN 22
#define M2_IN2_PIN 23
#define M2_PWM_PIN 13
#define M2_ENCODER_A_PIN 25
#define M2_ENCODER_B_PIN 26
#define M2_PCNT_UNIT PCNT_UNIT_1

// Motor 3 (Direito Traseiro)
#define M3_IN1_PIN 16
#define M3_IN2_PIN 17
#define M3_PWM_PIN 2
#define M3_ENCODER_A_PIN 34
#define M3_ENCODER_B_PIN 35
#define M3_PCNT_UNIT PCNT_UNIT_2

// Motor 4 (Esquerdo Traseiro)
#define M4_IN1_PIN 27
#define M4_IN2_PIN 14
#define M4_PWM_PIN 12
#define M4_ENCODER_A_PIN 39
#define M4_ENCODER_B_PIN 36
#define M4_PCNT_UNIT PCNT_UNIT_3

// ===================================================================
// EQUAÇÕES DOS MOTORES (PWM = m * RPM + c)
// ===================================================================

// M1 - Equações
#define M1_M_FRENTE 0.583
#define M1_C_FRENTE 24.5
#define M1_M_TRAS 0.598
#define M1_C_TRAS -20.7

// M2 - Equações
#define M2_M_FRENTE 0.578
#define M2_C_FRENTE 24
#define M2_M_TRAS 0.611
#define M2_C_TRAS -19.7

// M3 - Equações
#define M3_M_FRENTE 0.602
#define M3_C_FRENTE 25.5
#define M3_M_TRAS 0.633
#define M3_C_TRAS -21.5

// M4 - Equações
#define M4_M_FRENTE 0.65
#define M4_C_FRENTE 19
#define M4_M_TRAS 0.576
#define M4_C_TRAS -24.7

// --- Configurações do PWM (LEDC) ---
#define PWM_FREQUENCY 1500 // Frequência do PWM em Hz
#define PWM_RESOLUTION 8

// PID 
#define KP 0.1
#define KI 0.001
#define KD 0.025

#endif