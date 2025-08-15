#include "MotorDC.h"
#include "config.h"

// O construtor agora é responsável apenas por configurar o hardware do PWM
MotorDC::MotorDC(int input_1_pin, int input_2_pin, int pwm_pin, ledc_channel_t pwm_channel, int pin_enca, int pin_encb) {
    // Guarda os pinos e canais internamente
    this->INPUT_1_PIN = input_1_pin;    
    this->INPUT_2_PIN = input_2_pin;
    this->PWM_CHANNEL = pwm_channel;
    this->PIN_ENCA = pin_enca;
    this->PIN_ENCB = pin_encb;

    // Configura os canais PWM
    ledcSetup(this->PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

    // Anexa os pinos aos canais configurados
    ledcAttachPin(this->PWM_PIN, this->PWM_CHANNEL);
}

void MotorDC::setupEncoder(void (*isr_a)(), void (*isr_b)()) {
    pinMode(this->PIN_ENCA, INPUT_PULLUP);
    pinMode(this->PIN_ENCB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(this->PIN_ENCA), isr_a, CHANGE);
    attachInterrupt(digitalPinToInterrupt(this->PIN_ENCB), isr_b, CHANGE);
    this->prev_time = micros();
}

void MotorDC::setTargetSpeed(float rads_per_sec) {
    this->target_speed_rps = rads_per_sec;
}

void MotorDC::updatePID() {
    // Calcula o tempo decorrido desde a última atualização
    unsigned long current_time = micros();
    float delta_t = (current_time - this->prev_time) / 1.0e6;
    this->prev_time = current_time;

    // Evita divisão por zero se o tempo for muito curto
    if (delta_t == 0) return;

    // Calcula a velocidade atual em rad/s
    long current_pos = this->position;
    float delta_pos = current_pos - this->prev_position;
    float current_speed_rps = (delta_pos / TICKS_PER_REVOLUTION) * 2 * PI / delta_t;
    this->prev_position = current_pos;

    // Calcula o erro
    float error = this->target_speed_rps - current_speed_rps;

    // Termo Proporcional
    float p_term = KP * error;

    // Termo Integral 
    this->integral_error += error * delta_t;
    this->integral_error = constrain(this->integral_error, -100, 100); // Limita o integral
    float i_term = KI * this->integral_error;

    // Termo Derivativo
    float derivative = (error - this->prev_error) / delta_t;
    this->prev_error = error;
    float d_term = KD * derivative;

    // Saída do PID
    float pid_output = p_term + i_term + d_term;

    // Mapeia a saída para a porcentagem do motor
    int motor_speed_percent = constrain(pid_output * 20, -100, 100); // Fator para ajuste

    setSpeedPercent(motor_speed_percent);
}

// Método de baixo nível para correção de velociadde
void MotorDC::setSpeedPercent(int speedPercent) {
    speedPercent = constrain(speedPercent, -100, 100);
    uint32_t pwmValue = map(abs(speedPercent), 0, 100, 0, 255);

    if (speedPercent > 0) {
        digitalWrite(this->INPUT_1_PIN, HIGH);
        digitalWrite(this->INPUT_2_PIN, LOW);
        ledcWrite(this->PWM_CHANNEL, pwmValue);
    } else if (speedPercent < 0) {
        digitalWrite(this->INPUT_1_PIN, LOW);
        digitalWrite(this->INPUT_2_PIN, HIGH);
        ledcWrite(this->PWM_CHANNEL, pwmValue);
    } else {
        ledcWrite(this->PWM_CHANNEL, 0);
    }
}