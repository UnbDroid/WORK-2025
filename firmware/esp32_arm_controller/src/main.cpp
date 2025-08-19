#include <Arduino.h>
#include "ServoLEDC.h" // Incluímos nossa nova classe!

// --- Defina os pinos para os 4 servos ---
// (Você pode escolher outros pinos GPIO se precisar)
#define SERVO1_PIN 13
#define SERVO2_PIN 12 //ombro 1
#define SERVO3_PIN 14 //ombro 2
#define SERVO4_PIN 27 //garra

// Crie um objeto da nossa classe para cada servo
ServoLEDC servo1;
ServoLEDC servo2;
ServoLEDC servo3;
ServoLEDC servo4;

void setup() {
    Serial.begin(115200);
    Serial.println("--- Testando 4 Servos com a classe ServoLEDC ---");

    // Agora, configuramos cada servo.
    // É ESSENCIAL que cada servo tenha um CANAL PWM ÚNICO (de 0 a 15).
    servo1.attach(SERVO1_PIN, 0); // Pino 13, Canal 0
    servo2.attach(SERVO2_PIN, 1); // Pino 12, Canal 1
    servo3.attach(SERVO3_PIN, 2); // Pino 14, Canal 2
    servo4.attach(SERVO4_PIN, 3); // Pino 27, Canal 3

    Serial.println("Servos prontos. Iniciando movimento...");
    delay(1000);
}

void loop() {

    int angulo_horizontal = 120;
    Serial.println("Movendo todos os servos para a posição inicial (45 graus)");
    servo1.write(45);
    delay(2000);
   // servo4.write(45);
    //delay(2000);


  //  servo2.write(180 - angulo_horizontal);
//    servo3.write(180 - angulo_horizontal);

    //ombro indo para trás
    servo2.write(60);
    servo3.write(60);
    delay(2000);


    Serial.println("Movendo todos para a posição final (135 graus)");
    servo1.write(80);
    delay(2000);
   // servo4.write(135);
//    delay(2000);



    servo2.write(angulo_horizontal);
    servo3.write(angulo_horizontal);
    delay(2000);


/*
    Serial.println("Fazendo um movimento de 'onda'...");
    for (int i = 0; i <= 180; i += 5) {
        servo1.write(i);
        delay(20);
    }
    for (int i = 0; i <= 180; i += 5) {
        servo2.write(i);
        delay(20);
    }
     for (int i = 0; i <= 180; i += 5) {
        servo3.write(i);
        delay(20);
    }
     for (int i = 0; i <= 180; i += 5) {
        servo4.write(i);
        delay(20);
    }*/
//    delay(1000);
}