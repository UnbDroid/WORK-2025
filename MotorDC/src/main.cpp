#include <Arduino.h>
#include "MotorDC.h"
#include "PinConfig.h"

// Código utilizado para a leitura de Encoder Magnético do Motor DC utilizando o Driver BTS7960.

#define ENCA 25 // Fio Amarelo.
#define ENCB 26 // Fio Branco.
#define READ_ENCA ((GPIO.in >> ENCA) & 0x1)
#define READ_ENCB ((GPIO.in >> ENCB) & 0x1)
#define RPWM_PIN 22
#define LPWM_PIN 23

// Canal PWM para cada saída.
#define RPWM_CHANNEL 0
#define LPWM_CHANNEL 1

// Configurações PWM.
#define PWM_FREQ 10000      // 10 kHz, ideal para motores.
#define PWM_RESOLUTION 16    // 16 bits → 0–65.535.

volatile int64_t position = 0;

// Constantes PID.
const float kp = 1.2;
const float kd = 0.025;
const float ki = 0.001;

// Valores PID.
uint64_t prevTime = 0;
float ePrev = 0;
float eIntegral = 0;

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleChannelA(){
    bool a = READ_ENCA; // Leitura Direta do Pino GPIO - É possível usar também digitalRead().
    bool b = READ_ENCB;

    portENTER_CRITICAL_ISR(&mux);
    if (a == b) {
        position++;
    }
    else {
        position--;
    }
    portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handleChannelB(){
    bool a = READ_ENCA;
    bool b = READ_ENCB;

    portENTER_CRITICAL_ISR(&mux);
    if (a != b) {
        position++;
    }
    else {
        position--;
    }
    portEXIT_CRITICAL_ISR(&mux);
}

void MotorDC(int speedPercent) {
    speedPercent = constrain(speedPercent, -100, 100);
    int pwmValue = map(abs(speedPercent), 0, 100, 0, 65535);

    if (speedPercent > 0) {
        // Move para Frente (Horário - Testar).
        ledcWrite(RPWM_CHANNEL, pwmValue);
        ledcWrite(LPWM_CHANNEL, 0);
    } 
    else if (speedPercent < 0) {
        // Move para Trás (Anti-Horário - Testar).
        ledcWrite(RPWM_CHANNEL, 0);
        ledcWrite(LPWM_CHANNEL, pwmValue);
    } 
    else {
        // Parada: Não envia PWM para o Motor.
        ledcWrite(RPWM_CHANNEL, 0);
        ledcWrite(LPWM_CHANNEL, 0);
    }
}

void setup() {
    Serial.begin(115200); // Se atentar ao BAUD RATE: 115200 para Depuração.
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    
    attachInterrupt(digitalPinToInterrupt(ENCA), handleChannelA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCB), handleChannelB, CHANGE);

    // Configura os canais PWM.
    ledcSetup(RPWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(LPWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

    // Anexa os pinos aos canais PWM.
    ledcAttachPin(RPWM_PIN, RPWM_CHANNEL);
    ledcAttachPin(LPWM_PIN, LPWM_CHANNEL);

    // Operação das portas da ponte H estarão sempre em HIGH (Pinos Soldados).
}

void loop() {
    float t = micros() / 1e6;
    float frequency = 0.5; // Increase this to make it oscillate faster
    float target = 2500.0 * sin(2 * PI * frequency * t);
    
    // Obtenção da Variação do Tempo Transcorrido.
    uint64_t currentTime = micros();
    float deltaTime = ((float) (currentTime - prevTime))/( 1.0e6 );
    prevTime = currentTime;
    
    int64_t encoderReading = 0;

    portENTER_CRITICAL(&mux);
        encoderReading = position;
    portEXIT_CRITICAL(&mux);

    int64_t targetInt = (int64_t) target;
    int e = encoderReading - targetInt;
    float eDerivative = (e-ePrev)/(deltaTime);
    eIntegral = eIntegral + e*deltaTime;
  
    // Sinal de Controle.
    float controlSignal = kp*e + kd*eDerivative + ki*eIntegral;
    
    controlSignal = constrain(controlSignal, -100, 100);
    MotorDC(controlSignal);
    ePrev = e;

    Serial.printf("Target: %d, Encoder: %ld, Control: %.2f\n", target, encoderReading, controlSignal);
    delay(10);
}