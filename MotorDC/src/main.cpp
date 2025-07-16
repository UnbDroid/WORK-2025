#include <Arduino.h>
#include "MotorDC.h"

// --- Pinos ---
#define ENCA 22 // Fio Amarelo do Encoder
#define ENCB 23 // Fio Branco do Encoder
#define RPWM_PIN 14 // PWM para girar para frente
#define LPWM_PIN 25 // PWM para girar para trás

// --- Canais e Configurações do PWM ---
const ledc_channel_t RPWM_CHANNEL = LEDC_CHANNEL_0;
const ledc_channel_t LPWM_CHANNEL = LEDC_CHANNEL_1;
#define PWM_FREQ 10000      // 10 kHz
#define PWM_RESOLUTION 16   // 16 bits (0-65535)

// --- Constantes do Controlador PID (Ajuste estes valores para otimizar) ---
const float kp = 1.2;
const float kd = 0.025;
const float ki = 0.001;

// =================================================================
// --- VARIÁVEIS GLOBAIS ---
// =================================================================

// --- Objeto do Motor ---
// O construtor desta classe já configura os pinos, canais e o PWM para nós.
MotorDC motorDC(RPWM_PIN, LPWM_PIN, RPWM_CHANNEL, LPWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

// --- Variáveis do Encoder ---
volatile int64_t position = 0;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// --- Variáveis do PID ---
uint64_t prevTime = 0;
float ePrev = 0;
float eIntegral = 0;


void IRAM_ATTR handleChannelA(){
    bool a = digitalRead(ENCA);
    bool b = digitalRead(ENCB);
    portENTER_CRITICAL_ISR(&mux);
    if (a == b) { position++; } else { position--; }
    portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handleChannelB(){
    bool a = digitalRead(ENCA);
    bool b = digitalRead(ENCB);
    portENTER_CRITICAL_ISR(&mux);
    if (a != b) { position++; } else { position--; }
    portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
    Serial.begin(115200);

    // Configura os pinos do encoder como entrada
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    
    // Anexa as interrupções aos pinos do encoder
    attachInterrupt(digitalPinToInterrupt(ENCA), handleChannelA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCB), handleChannelB, CHANGE);

    // NÃO PRECISAMOS MAIS CONFIGURAR O PWM AQUI!
    // O construtor do objeto motorDC já fez todo o trabalho pesado.
    
    Serial.println("Setup concluído. Iniciando controle PID de posição...");
    prevTime = micros(); // Inicializa o contador de tempo do PID
}

// =================================================================
// --- LOOP PRINCIPAL ---
// =================================================================

void loop() {
    // --- 1. Definir o Alvo (Target) ---
    // O alvo é uma posição que varia suavemente ao longo do tempo (onda seno)
    float t = micros() / 1e6;
    float frequency = 0.5; // Frequência da oscilação em Hz
    float target = 2500.0 * sin(2 * PI * frequency * t);
    
    // --- 2. Ler a Posição Atual ---
    // Lê a variável 'position' que é atualizada pelas interrupções
    int64_t encoderReading;
    portENTER_CRITICAL(&mux);
    encoderReading = position;
    portEXIT_CRITICAL(&mux);

    // --- 3. Calcular o Sinal de Controle PID ---
    uint64_t currentTime = micros();
    float deltaTime = ((float) (currentTime - prevTime)) / 1.0e6;
    prevTime = currentTime;
    
    int e = encoderReading - (int64_t)target;
    eIntegral += e * deltaTime;
    float eDerivative = (e - ePrev) / deltaTime;
    ePrev = e;
  
    float controlSignal = kp * e + kd * eDerivative + ki * eIntegral;
    
    motorDC.setSpeed(controlSignal);


    Serial.printf("Alvo: %.2f, Encoder: %lld, Controle: %.2f\n", target, encoderReading, controlSignal);
    
    delay(40);
}