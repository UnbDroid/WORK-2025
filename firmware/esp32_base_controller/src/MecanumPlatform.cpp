#include "MecanumPlatform.h"
#include "config.h"

// Ponteiros globais para as funções de interrupção acessarem os motores
namespace {
    MotorDC* motor1_ptr = nullptr;
    MotorDC* motor2_ptr = nullptr;
    MotorDC* motor3_ptr = nullptr;
    MotorDC* motor4_ptr = nullptr;
}

// O construtor inicializa cada MotorDC

MecanumPlatform::MecanumPlatform() :
    motor1(M1_IN1_PIN, M1_IN2_PIN, M1_PWM_PIN, LEDC_CHANNEL_0, M1_ENCODER_A_PIN, M1_ENCODER_B_PIN, M1_PCNT_UNIT, M1_M_FRENTE, M1_C_FRENTE, M1_M_TRAS, M1_C_TRAS),
    motor2(M2_IN1_PIN, M2_IN2_PIN, M2_PWM_PIN, LEDC_CHANNEL_1, M2_ENCODER_A_PIN, M2_ENCODER_B_PIN, M2_PCNT_UNIT, M2_M_FRENTE, M2_C_FRENTE, M2_M_TRAS, M2_C_TRAS),
    motor3(M3_IN1_PIN, M3_IN2_PIN, M3_PWM_PIN, LEDC_CHANNEL_2, M3_ENCODER_A_PIN, M3_ENCODER_B_PIN, M3_PCNT_UNIT, M3_M_FRENTE, M3_C_FRENTE, M3_M_TRAS, M3_C_TRAS),
    motor4(M4_IN1_PIN, M4_IN2_PIN, M4_PWM_PIN, LEDC_CHANNEL_3, M4_ENCODER_A_PIN, M4_ENCODER_B_PIN, M4_PCNT_UNIT, M4_M_FRENTE, M4_C_FRENTE, M4_M_TRAS, M4_C_TRAS)
{ }

void MecanumPlatform::setup() {
    // Ponteiros globais
    motor1_ptr = &motor1;
    motor2_ptr = &motor2;
    motor3_ptr = &motor3;
    motor4_ptr = &motor4;
}


void MecanumPlatform::init() {
    motor1.init();
    motor2.init();
    motor3.init();
    motor4.init();
}

// Cinemática inversa
void MecanumPlatform::setSpeed(float linear_x, float linear_y, float angular_z) {
    // Calcula a velocidade alvo para cada roda em rad/s
    float w1 = (1.0f / WHEEL_RADIUS) * (linear_x - linear_y - ((LX + LY) * angular_z));
    float w2 = (1.0f / WHEEL_RADIUS) * (linear_x + linear_y + ((LX + LY) * angular_z));
    float w3 = (1.0f / WHEEL_RADIUS) * (linear_x - linear_y + ((LX + LY) * angular_z));
    float w4 = (1.0f / WHEEL_RADIUS) * (linear_x + linear_y - ((LX + LY) * angular_z));

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