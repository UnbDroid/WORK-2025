#include "MecanumPlatform.h"
#include "config.h"

// Ponteiros globais para as funções de interrupção acessarem os motores
namespace {
    MotorDC* motor1_ptr = nullptr;
    MotorDC* motor2_ptr = nullptr;
    MotorDC* motor3_ptr = nullptr;
    MotorDC* motor4_ptr = nullptr;
}

// Funções de interrupção
void IRAM_ATTR isr_m1a() { if(motor1_ptr) motor1_ptr->readEncoder(); }
void IRAM_ATTR isr_m2a() { if(motor2_ptr) motor2_ptr->readEncoder(); }
void IRAM_ATTR isr_m3a() { if(motor3_ptr) motor3_ptr->readEncoder(); }
void IRAM_ATTR isr_m4a() { if(motor4_ptr) motor4_ptr->readEncoder(); }

// O construtor inicializa cada MotorDC

MecanumPlatform::MecanumPlatform() :
    motor1(M1_IN1_PIN, M1_IN2_PIN, M1_PWM_PIN, static_cast<ledc_channel_t>(M1_PWM_CHANNEL), M1_ENCODER_A_PIN, M1_ENCODER_B_PIN),
    motor2(M2_IN1_PIN, M2_IN2_PIN, M2_PWM_PIN, static_cast<ledc_channel_t>(M2_PWM_CHANNEL), M2_ENCODER_A_PIN, M2_ENCODER_B_PIN),
    motor3(M3_IN1_PIN, M3_IN2_PIN, M3_PWM_PIN, static_cast<ledc_channel_t>(M3_PWM_CHANNEL), M3_ENCODER_A_PIN, M3_ENCODER_B_PIN),
    motor4(M4_IN1_PIN, M4_IN2_PIN, M4_PWM_PIN, static_cast<ledc_channel_t>(M4_PWM_CHANNEL), M4_ENCODER_A_PIN, M4_ENCODER_B_PIN)
{ }

void MecanumPlatform::setup() {
    // Ponteiros globais
    motor1_ptr = &motor1;
    motor2_ptr = &motor2;
    motor3_ptr = &motor3;
    motor4_ptr = &motor4;

    // Configuração do encoder para cada MotorDC
    motor1.setupEncoder(isr_m1a);
    motor2.setupEncoder(isr_m2a);
    motor3.setupEncoder(isr_m3a);
    motor4.setupEncoder(isr_m4a);
}

// Cinemática inversa
void MecanumPlatform::setSpeed(float linear_x, float linear_y, float angular_z) {
    // Calcula a velocidade alvo para cada roda em rad/s
    float w1 = (1 / WHEEL_RADIUS) * (linear_x - linear_y - (LX + LY) * angular_z);
    float w2 = (1 / WHEEL_RADIUS) * (linear_x + linear_y + (LX + LY) * angular_z);
    float w3 = (1 / WHEEL_RADIUS) * (linear_x - linear_y + (LX + LY) * angular_z);
    float w4 = (1 / WHEEL_RADIUS) * (linear_x + linear_y - (LX + LY) * angular_z);

    motor1.setTargetSpeed(w1);
    motor2.setTargetSpeed(w2);
    motor3.setTargetSpeed(w3);
    motor4.setTargetSpeed(w4);
}

// Chama o update de cada motor.
void MecanumPlatform::update() {
    motor1.updatePID();
    motor2.updatePID();
    motor3.updatePID();
    motor4.updatePID();
}