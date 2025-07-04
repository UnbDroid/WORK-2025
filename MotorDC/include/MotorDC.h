#ifndef _MOTORDC_H
#define _MOTORDC_H

#include "Arduino.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#define WHEEL_RADIUS_METERS 0.04

class MotorDC {
public:
  MotorDC(const int ENCA, const int ENCB, const int L_PWM, const int R_PWM, ledc_channel_t LEDC_CHANNEL_L, ledc_channel_t LEDC_CHANNEL_R); // Construtor da Classe MotorDC.
  void stop_motor();
  void configure_motor(int ticks_per_turn, float kp, float ki,float kd); // Função para Configurar o Motor.
  void set_motor(int direction, double pwmVal);
  void IRAM_ATTR read_encoder(void *arg);
  void set_encoder();
  void reset_encoder();
  void go_forward(int desired_speed_rpm);
  int32_t return_posi();
  double return_speed();
  double wheel_lenght = 2 * 3.1415 * WHEEL_RADIUS_METERS; // TODO: Medir o Raio da Roda Real.
  float return_kp();
  float return_ki();
  float return_kd();
  void tweak_pid(int variable, float diff);

private:
  int ENCA; // Fio Amarelo
  int ENCB; // Fio Branco
  int ENABLE;   // PWM Channel
  int L_PWM;  // Enable Esquerdo
  int R_PWM;  // Enable Direito
  ledc_channel_t LEDC_CHANNEL_L;
  ledc_channel_t LEDC_CHANNEL_R;
  int ticks_per_turn; // Valor de encoder referente a uma volta completa da roda
  float kp;           // Valor de kp para o PID
  float ki;           // Valor de ki para o PID
  float kd;           // Valor de kd para o PID
  double dt = 0;

  volatile double current_speed_rpm = 0;
  volatile double last_error = 0;        // Erro anterior para o PID
  volatile double accumulated_error = 0; // Erro acumulado para o PID
  volatile int64_t posi = 0;             // Posição do motor em ticks do encoder
  int64_t last_posi = 0;                 // Posição do motor em ticks do encoder
  volatile double current_time = 0;
  volatile double last_time = 0;
};

#endif