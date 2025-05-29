#include <Arduino.h>

#define ENCA 14 // Fio Amarelo.
#define ENCB 12 // Fio Branco.
#define READ_ENCA ((GPIO.in >> ENCA) & 0x1)
#define READ_ENCB ((GPIO.in >> ENCB) & 0x1)

volatile int32_t position = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

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

void setup() {
    Serial.begin(115200); // Se atentar ao BAUD RATE: 115200 para Depuração.
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    
    attachInterrupt(digitalPinToInterrupt(ENCA), handleChannelA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCB), handleChannelB, CHANGE);
}

void loop() {
    int32_t reading = 0;

    portENTER_CRITICAL(&mux);
        reading = position;
    portEXIT_CRITICAL(&mux);

    Serial.println(position);
    delay(100);  // Delay para Evitar Overflow da Porta Serial.
}