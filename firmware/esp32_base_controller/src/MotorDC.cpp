#include "MotorDC.h"
#include "config.h" 
#include "driver/pcnt.h" // Biblioteca para o Pulse Counter (PCNT)
#include "driver/ledc.h" // Biblioteca para o PWM Controller (LEDC)

// --- 1. Construtor ---
// Apenas inicializa as variáveis. A configuração do hardware será feita no init().
MotorDC::MotorDC(int input_1_pin, int input_2_pin, int pwm_pin, ledc_channel_t pwm_channel,
                 int pin_enca, int pin_encb, pcnt_unit_t pcnt_unit,
                 float m_f, float c_f, float m_b, float c_b) :
    // Lista de inicialização para as constantes
    INPUT_1_PIN(input_1_pin),
    INPUT_2_PIN(input_2_pin),
    PWM_PIN(pwm_pin),
    PWM_CHANNEL(pwm_channel),
    PIN_ENCA(pin_enca),
    PIN_ENCB(pin_encb),
    PCNT_UNIT(pcnt_unit),
    M_FRONT(m_f), C_FRONT(c_f), M_BACK(m_b), C_BACK(c_b)
{
    // Zera todas as variáveis de estado
    totalPulseCount = 0;
    rpm = 0.0;
    lastHardwareCount = 0;
    target_speed_rps = 0.0;
    current_speed_rps = 0.0;
    prev_error = 0.0;
    integral_error = 0.0;
    prev_time = 0;
}

float MotorDC::getTargetSpeedRPS() {
    return this->target_speed_rps;
}

// --- 2. Inicializador de Hardware ---
// Esta função DEVE ser chamada uma vez dentro do setup() principal do seu programa.
void MotorDC::init() {
    // Configura os pinos de controle de direção
    pinMode(INPUT_1_PIN, OUTPUT);
    pinMode(INPUT_2_PIN, OUTPUT);

    // Configura o hardware de PWM (LEDC)
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);

    // Configura o hardware do Encoder (PCNT)
    setupEncoderPCNT();

    // Inicia o temporizador para o cálculo do PID
    this->prev_time = micros();
}

// --- 3. Configuração do Encoder (PCNT) ---
// Função privada, chamada apenas por init().
void MotorDC::setupEncoderPCNT() {
    pcnt_config_t pcnt_config = {}; // Zera a estrutura de configuração
    
    // Associa os pinos do encoder a esta unidade de contagem
    pcnt_config.pulse_gpio_num = this->PIN_ENCA;   // Pino A (principal)
    pcnt_config.ctrl_gpio_num = this->PIN_ENCB;    // Pino B (controle de direção)
    pcnt_config.unit = this->PCNT_UNIT;
    pcnt_config.channel = PCNT_CHANNEL_0;
    
    // Define a lógica de decodificação de quadratura em hardware
    // Esta é a configuração padrão para a maioria dos encoders
    pcnt_config.pos_mode = PCNT_COUNT_DEC;      // Quando o pino A sobe, decrementa
    pcnt_config.neg_mode = PCNT_COUNT_INC;      // Quando o pino A desce, incrementa
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; // Se o pino B estiver LOW, inverte a lógica acima
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Se o pino B estiver HIGH, mantém a lógica acima

    // Aplica a configuração
    pcnt_unit_config(&pcnt_config);
    
    // Ativa um filtro de hardware para ignorar ruídos (glitches) de até ~12.7us
    pcnt_set_filter_value(this->PCNT_UNIT, 1023);
    pcnt_filter_enable(this->PCNT_UNIT);
    
    // Limpa, reinicia e começa a contar os pulsos
    pcnt_counter_pause(this->PCNT_UNIT);
    pcnt_counter_clear(this->PCNT_UNIT);
    pcnt_counter_resume(this->PCNT_UNIT);
}

// --- 4. Definir Velocidade Alvo ---
// Função pública para o seu código principal (ex: MecanumPlatform) usar.
void MotorDC::setTargetSpeed(float rads_per_sec) {
    this->target_speed_rps = rads_per_sec;
}

// --- 5. Lógica de Controle PID com Feedforward ---
// Esta função é chamada repetidamente no loop principal.
void MotorDC::updatePID() {
    // --- Medir a Velocidade Atual ---
    unsigned long current_time = micros();
    float delta_t = (current_time - this->prev_time) / 1.0e6;
    this->prev_time = current_time;
    if (delta_t <= 0) return; // Evita divisão por zero

    // Lê o valor atual do contador de hardware do PCNT
    int16_t contagemHardwareAtual;
    pcnt_get_counter_value(this->PCNT_UNIT, &contagemHardwareAtual);
    
    // Calcula a diferença desde a última leitura
    // O contador de 16 bits pode dar a volta (overflow), esta forma de cálculo lida com isso.
    int16_t deltaPulsos = contagemHardwareAtual - this->lastHardwareCount;
    
    // Atualiza a contagem total e a última contagem para o próximo ciclo
    this->totalPulseCount += deltaPulsos;
    this->lastHardwareCount = contagemHardwareAtual;

    // Calcula a velocidade atual em rad/s, agora usando a contagem precisa do PCNT
    this->current_speed_rps = ((float)deltaPulsos / TICKS_PER_REVOLUTION) * (2.0 * PI) / delta_t;

    // Se a velocidade alvo for zero, simplesmente para o motor.
    if (this->target_speed_rps == 0) {
        setSpeedPercent(0);
        this->integral_error = 0; // Zera o integral para evitar "windup"
        return;
    }
    
    // Converte as velocidades de rad/s para RPM para usar nas equações
    float target_rpm = this->target_speed_rps * 9.5492965855; // (60 / 2*PI)
    float current_rpm = this->current_speed_rps * 9.5492965855;

    // --- Calcular o Feedforward (O Chute Inteligente) ---
    float feedforward_pwm = 0.0;
    if (target_rpm > 0) {
        feedforward_pwm = (target_rpm * this->M_FRONT) + this->C_FRONT;
    } else if (target_rpm < 0) {
        // Usa a equação para trás com o valor absoluto do RPM
        feedforward_pwm = (abs(target_rpm) * this->M_BACK) + this->C_BACK;
    }
    
    // --- Calcular o Feedback (O Ajuste Fino - PID) ---
    float error = target_rpm - current_rpm;
    this->integral_error += error * delta_t;
    this->integral_error = constrain(this->integral_error, -100.0, 100.0); // Limita o integral (anti-windup)
    float derivative = (error - this->prev_error) / delta_t;
    this->prev_error = error;
    
    float pid_correction = (KP * error) + (KI * this->integral_error) + (KD * derivative);
    
    // --- Combinar e Aplicar ao Motor ---
    float total_pwm = feedforward_pwm + pid_correction;
    
    // Direção é determinada pelo sinal da velocidade alvo
    int speed_sign = (target_rpm >= 0) ? 1 : -1;

    // Converte a saída PWM para uma porcentagem (-100 a 100) para a função setSpeedPercent
    int motor_speed_percent = (total_pwm / 255.0) * 100.0 * speed_sign;
    
    setSpeedPercent(motor_speed_percent);
}

// --- 6. Controle de Baixo Nível do Motor ---
// Esta função privada traduz uma porcentagem (-100 a 100) para comandos de hardware.
void MotorDC::setSpeedPercent(int speedPercent) {
    speedPercent = constrain(speedPercent, -100, 100);
    
    // Define a direção com base no sinal da porcentagem
    if (speedPercent > 0) { // Para frente
        digitalWrite(this->INPUT_1_PIN, HIGH);
        digitalWrite(this->INPUT_2_PIN, LOW);
    } else if (speedPercent < 0) { // Para trás
        digitalWrite(this->INPUT_1_PIN, LOW);
        digitalWrite(this->INPUT_2_PIN, HIGH);
    } else { // Parado
        digitalWrite(this->INPUT_1_PIN, LOW);
        digitalWrite(this->INPUT_2_PIN, LOW);
    }

    // Calcula o valor do PWM (0-255) a partir do valor absoluto da porcentagem
    uint32_t pwmValue = map(abs(speedPercent), 0, 100, 0, (pow(2, PWM_RESOLUTION) - 1));
    ledcWrite(this->PWM_CHANNEL, pwmValue);
}