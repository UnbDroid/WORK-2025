#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__

#include "driver/gpio.h"
#include "driver/ledc.h"

// Pinos GPIO que NÃO DEVEM SER USADOS: 6 a 11 (FLASH) / 1, 3 (TX|RX).
// Utilizar com CUIDADO: 0, 2, 12, 15.
// Só funcionam como ENTRADA: 34, 35, 36, 39.

// Pinos utilizados - Motores DC: 22, 23 , 25, 26, 21, 19, 32, 33, 27, 14, 16, 17, 13, 4, 5, 18.

// Configuração GPIO dos pinos {
// --------------------------------------------------------------------------------

#define ENABLE_H_BRIDGE 2

#define L_PWM_LEFT_FRONT 22
#define R_PWM_LEFT_FRONT 23

#define ENCA_LEFT_FRONT 25
#define ENCB_LEFT_FRONT 26
#define READ_ENCA_LEFT_FRONT  ((GPIO.in >> ENCA_LEFT_FRONT ) & 0x1)
#define READ_ENCB_LEFT_FRONT  ((GPIO.in >> ENCB_LEFT_FRONT ) & 0x1)

#define L_PWM_LEFT_BACK 21
#define R_PWM_LEFT_BACK 19

#define ENCA_LEFT_BACK 32
#define ENCB_LEFT_BACK 33
#define READ_ENCA_LEFT_BACK ((GPIO.in >> ENCA_LEFT_BACK) & 0x1)
#define READ_ENCB_LEFT_BACK ((GPIO.in >> ENCB_LEFT_BACK) & 0x1)

#define L_PWM_RIGHT_FRONT 27
#define R_PWM_RIGHT_FRONT 14

#define ENCA_RIGHT_FRONT 16
#define ENCB_RIGHT_FRONT 17
#define READ_ENCA_RIGHT_FRONT ((GPIO.in >> ENCA_RIGHT_FRONT) & 0x1)
#define READ_ENCB_RIGHT_FRONT ((GPIO.in >> ENCB_RIGHT_FRONT) & 0x1)

#define L_PWM_RIGHT_BACK 13
#define R_PWM_RIGHT_BACK 4

#define ENCA_RIGHT_BACK 5
#define ENCB_RIGHT_BACK 18 
#define READ_ENCA_RIGHT_BACK  ((GPIO.in >> ENCA_RIGHT_BACK ) & 0x1)
#define READ_ENCB_RIGHT_BACK ((GPIO.in >> ENCB_RIGHT_BACK ) & 0x1)

#define TICKS_PER_ROTATIONS 16
#define DISTANCE_BETWEEN_WHEELS_METERS 0.30

// --------------------------------------------------------------------------------
// } Configuração GPIO dos pinos

// Configuração LEDC {
// --------------------------------------------------------------------------------

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_LEFT_FRONT_L_PWM LEDC_CHANNEL_0
#define LEDC_CHANNEL_LEFT_FRONT_R_PWM LEDC_CHANNEL_1
#define LEDC_CHANNEL_LEFT_BACK_L_PWM LEDC_CHANNEL_2
#define LEDC_CHANNEL_LEFT_BACK_R_PWM LEDC_CHANNEL_3
#define LEDC_CHANNEL_RIGHT_FRONT_L_PWM LEDC_CHANNEL_4
#define LEDC_CHANNEL_RIGHT_FRONT_R_PWM LEDC_CHANNEL_5
#define LEDC_CHANNEL_RIGHT_BACK_L_PWM LEDC_CHANNEL_6
#define LEDC_CHANNEL_RIGHT_BACK_R_PWM LEDC_CHANNEL_7
#define LEDC_PWM_RESOLUTION 8  // Set duty resolution to 8 bits
#define LEDC_DUTY (128)        // Set duty to 50%. (2 ** 8) * 50% = 128
#define LEDC_FREQUENCY (10000) // Frequency in Hertz. Set frequency at 10 kHz

// --------------------------------------------------------------------------------
// } Configuração LEDC

#endif