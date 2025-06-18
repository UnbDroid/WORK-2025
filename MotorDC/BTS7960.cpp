#include <Arduino.h>

// Código utilizado para a leitura de Encoder Magnético do Motor DC utilizando o Driver BTS7960 - (Criar classe para Motor DC em breve).

#define ENCA 14 // Fio Amarelo.
#define ENCB 12 // Fio Branco.
#define READ_ENCA ((GPIO.in >> ENCA) & 0x1)
#define READ_ENCB ((GPIO.in >> ENCB) & 0x1)
#define RPWM_PIN 17
#define LPWM_PIN 16
#define R_EN_PIN 19
#define L_EN_PIN 18

// Canal PWM para cada saída.
#define RPWM_CHANNEL 0
#define LPWM_CHANNEL 1

// Configurações PWM.
#define PWM_FREQ 20000      // 20 kHz, ideal para motores.
#define PWM_RESOLUTION 8    // 8 bits → 0–255.

volatile int32_t position = 0;
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
    int pwmValue = map(abs(speedPercent), 0, 100, 0, 255);

    if (speedPercent > 0) {
        // Move para Frente (Horário - Testar).
        digitalWrite(R_EN_PIN, HIGH);
        digitalWrite(L_EN_PIN, LOW);
        ledcWrite(RPWM_CHANNEL, pwmValue);
        ledcWrite(LPWM_CHANNEL, 0);
    } 
    else if (speedPercent < 0) {
        // Move para Trás (Anti-Horário - Testar).
        digitalWrite(R_EN_PIN, LOW);
        digitalWrite(L_EN_PIN, HIGH);
        ledcWrite(RPWM_CHANNEL, 0);
        ledcWrite(LPWM_CHANNEL, pwmValue);
    } 
    else {
        // Parada por Freio Passivo (Desliga o Motor).
        digitalWrite(R_EN_PIN, LOW);
        digitalWrite(L_EN_PIN, LOW);
        ledcWrite(RPWM_CHANNEL, 0);
        ledcWrite(LPWM_CHANNEL, 0);

        /*
        // Parada por Freio Ativo (Força Motor a Parar).
        digitalWrite(R_EN_PIN, HIGH);
        digitalWrite(L_EN_PIN, HIGH);
        ledcWrite(RPWM_CHANNEL, 0);
        ledcWrite(LPWM_CHANNEL, 0);
        */
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

    // Operação das portas da ponte H.
    pinMode(R_EN_PIN, OUTPUT);
    pinMode(L_EN_PIN, OUTPUT);
}

void loop() {
    int32_t reading = 0;

    portENTER_CRITICAL(&mux);
        reading = position;
    portEXIT_CRITICAL(&mux);

    Serial.println(reading);
    delay(100);
    
    MotorDC(20);
    delay(2000);
    MotorDC(15);
    delay(2000);
    MotorDC(-20);
    delay(1000);
    MotorDC(0);
    delay(4000);
}