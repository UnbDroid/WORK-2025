#include "MotorDC.h"
#include "config.h"

void MotorDC::setupEncoderPNCT(MotorDC *motor) {
    pcnt_config_t pcnt_config = {}; // Zera a estrutura de configuração
  
    // Configura os pinos e canais do PCNT
    pcnt_config.pulse_gpio_num = this->PIN_ENCA;
    pcnt_config.ctrl_gpio_num = this->PIN_ENCB;
    pcnt_config.unit = this->pcnt_unit;
    pcnt_config.channel = PCNT_CHANNEL_0;
    
    // Define como o PCNT deve contar nos eventos de borda
    pcnt_config.pos_mode = PCNT_COUNT_DEC; // Decrementa na borda de subida de A
    pcnt_config.neg_mode = PCNT_COUNT_INC; // Incrementa na borda de descida de A
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; // Inverte a contagem baseado no pino B (HIGH)
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Mantém a contagem baseado no pino B (LOW)

    // Inicializa a unidade PCNT
    pcnt_unit_config(&pcnt_config);
    
    // (Opcional) Configura um filtro para ignorar ruídos/glitches de até 10us
    pcnt_set_filter_value(this->pcnt_unit, 1023);
    pcnt_filter_enable(this->pcnt_unit);
    
    // Zera e inicia o contador
    pcnt_counter_pause(this->pcnt_unit);
    pcnt_counter_clear(this->pcnt_unit);
    pcnt_counter_resume(this->pcnt_unit);
}

void MotorDC::setTargetSpeed(float rads_per_sec) {
    this->target_speed_rps = rads_per_sec;
}

void MotorDC::moveMotor(MotorDC* motor, int pwm) {
    bool forward = pwm < 0;

    if(pwm == 0) {
        digitalWrite(this->INPUT_1_PIN, LOW);
        digitalWrite(this->INPUT_2_PIN, LOW);
    } else {
        digitalWrite(this->INPUT_1_PIN, forward ? HIGH : LOW);
        digitalWrite(this->INPUT_2_PIN, forward ? LOW : HIGH);
    }

    int pwmValue = abs(pwm);
    if (pwmValue > 255) pwmValue = 255;

    ledcWrite(this->PWM_CHANNEL, pwmValue);
}

/*void MotorDC::updateEncoder(MotorDC* motor) {
    unsigned long currentTime = millis();

    if()
} */

/*void MotorDC::updatePID() {
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
}*/

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