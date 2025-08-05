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
void IRAM_ATTR isr_m1a() { if(motor1_ptr) motor1_ptr->position += (digitalRead(M1_ENCA_PIN) == digitalRead(M1_ENCB_PIN)) ? 1 : -1; }
void IRAM_ATTR isr_m1b() { if(motor1_ptr) motor1_ptr->position += (digitalRead(M1_ENCA_PIN) != digitalRead(M1_ENCB_PIN)) ? 1 : -1; }

void IRAM_ATTR isr_m2a() { if(motor2_ptr) motor2_ptr->position += (digitalRead(M2_ENCA_PIN) == digitalRead(M2_ENCB_PIN)) ? 1 : -1; }
void IRAM_ATTR isr_m2b() { if(motor2_ptr) motor2_ptr->position += (digitalRead(M2_ENCA_PIN) != digitalRead(M2_ENCB_PIN)) ? 1 : -1; }

void IRAM_ATTR isr_m3a() { if(motor3_ptr) motor3_ptr->position += (digitalRead(M3_ENCA_PIN) == digitalRead(M3_ENCB_PIN)) ? 1 : -1; }
void IRAM_ATTR isr_m3b() { if(motor3_ptr) motor3_ptr->position += (digitalRead(M3_ENCA_PIN) != digitalRead(M3_ENCB_PIN)) ? 1 : -1; }

void IRAM_ATTR isr_m4a() { if(motor4_ptr) motor4_ptr->position += (digitalRead(M4_ENCA_PIN) == digitalRead(M4_ENCB_PIN)) ? 1 : -1; }
void IRAM_ATTR isr_m4b() { if(motor4_ptr) motor4_ptr->position += (digitalRead(M4_ENCA_PIN) != digitalRead(M4_ENCB_PIN)) ? 1 : -1; }


// O construtor inicializa cada MotorDC
MecanumPlatform::MecanumPlatform() :
    motor1(M1_RPWM_PIN, M1_LPWM_PIN, M1_R_CHANNEL, M1_L_CHANNEL, M1_ENCA_PIN, M1_ENCB_PIN),
    motor2(M2_RPWM_PIN, M2_LPWM_PIN, M2_R_CHANNEL, M2_L_CHANNEL, M2_ENCA_PIN, M2_ENCB_PIN),
    motor3(M3_RPWM_PIN, M3_LPWM_PIN, M3_R_CHANNEL, M3_L_CHANNEL, M3_ENCA_PIN, M3_ENCB_PIN),
    motor4(M4_RPWM_PIN, M4_LPWM_PIN, M4_R_CHANNEL, M4_L_CHANNEL, M4_ENCA_PIN, M4_ENCB_PIN)
{}

void MecanumPlatform::setup() {
    // Ponteiros globais
    motor1_ptr = &motor1;
    motor2_ptr = &motor2;
    motor3_ptr = &motor3;
    motor4_ptr = &motor4;

    // Configuração do encoder para cada MotorDC
    motor1.setupEncoder(isr_m1a, isr_m1b);
    motor2.setupEncoder(isr_m2a, isr_m2b);
    motor3.setupEncoder(isr_m3a, isr_m3b);
    motor4.setupEncoder(isr_m4a, isr_m4b);
}

// Cinemática inversa
void MecanumPlatform::setSpeed(float linear_x, float linear_y, float angular_z) {
    // Calcula a velocidade alvo para cada roda em rad/s
    float w1 = (1 / WHEEL_RADIUS) * (linear_x - linear_y - (LX + LY) * angular_z);
    float w2 = (1 / WHEEL_RADIUS) * (linear_x + linear_y + (LX + LY) * angular_z);
    float w3 = (1 / WHEEL_RADIUS) * (linear_x + linear_y - (LX + LY) * angular_z);
    float w4 = (1 / WHEEL_RADIUS) * (linear_x - linear_y + (LX + LY) * angular_z);

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