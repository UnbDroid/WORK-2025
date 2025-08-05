#include <Arduino.h>

// Código para Controle PWM Provisório dos Motores DC pelo Driver L298N.

#define ENCA 14 // Fio Amarelo.
#define ENCB 12 // Fio Branco.
#define READ_ENCA ((GPIO.in >> ENCA) & 0x1)
#define READ_ENCB ((GPIO.in >> ENCB) & 0x1)
#define ENA 27
#define IN1 26
#define IN2 25
#define ENB 17
#define IN3 18
#define IN4 19

// Configurações PWM.
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1
#define PWM_FREQ 10000    // Frequência de 10 kHz.
#define PWM_RESOLUTION 8  // Resolução de 8 bits (0, 255).

volatile int32_t position = 0;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleChannelA(){
    bool a = READ_ENCA;
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

void setup() {
  // Se atentar ao BAUD RATE: 115200 para Depuração.
  Serial.begin(115200);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
    
  attachInterrupt(digitalPinToInterrupt(ENCA), handleChannelA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCB), handleChannelB, CHANGE);

  // Configura pinos de direção.
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configura PWM.
  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL_A);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENB, PWM_CHANNEL_B);
}

void MotorDC(int speedPercentage, const int pwmChannel, const int in1, const int in2) {
  speedPercentage = constrain(speedPercentage, -100, 100);
  int pwmValue = map(abs(speedPercentage), 0, 100, 0, 255);

      if (speedPercentage > 0) {
        // Move para Frente (Horário - Testar).
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, pwmValue);
    } 
    else if (speedPercentage < 0) {
        // Move para Trás (Anti-Horário - Testar).
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        ledcWrite(pwmChannel, pwmValue);
    } 
    else {
        // Parada por Freio Passivo (Desliga o Motor).
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, 0);

        /*
        // Parada por Freio Ativo (Força Motor a Parar).
        digitalWrite(in1, HIGH);
        digitalWrite(in2, HIGH);
        ledcWrite(pwmChannel, 0);
        */
    }
}

void loop() {
  int32_t reading = 0;

  portENTER_CRITICAL(&mux);
    reading = position;
  portEXIT_CRITICAL(&mux);
  
  Serial.println(reading);
  delay(100);

  MotorDC(20, PWM_CHANNEL_B, IN3, IN4);
  delay(1000);
  MotorDC(-30, PWM_CHANNEL_B, IN3, IN4);
  delay(2000);
  MotorDC(50, PWM_CHANNEL_B, IN3, IN4);
  delay(2000);
  MotorDC(10, PWM_CHANNEL_B, IN3, IN4);
  delay(2000);
  MotorDC(0, PWM_CHANNEL_B, IN3, IN4);
  delay(5000);
}
