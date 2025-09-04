#include <Arduino.h>
#include "ServoLEDC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// --- Definições para os 4 servos ---
#define SERVO1_PIN 13 // Cotovelo
#define SERVO2_PIN 12 // Ombro 1
#define SERVO3_PIN 14 // Ombro 2
#define SERVO4_PIN 27 // Garra

// --- Definições para o Motor de Passo com A4988 ---
#define STEPPER_DIR_PIN 26
#define STEPPER_STEP_PIN 25
#define PASSOS_POR_ROTACAO 200

// --- Definições de Velocidade ---
#define VELOCIDADE_LENTA 2
#define VELOCIDADE_NORMAL 50
#define VELOCIDADE_RAPIDA 90

// <<< NOVO: OFFSETS DE CALIBRAÇÃO PARA OS OMBROS >>>
const int OFFSET_OMBRO_1 = 1; // Correção para o Servo 2
const int OFFSET_OMBRO_2 = 0; // Correção para o Servo 3


// Crie um objeto da nossa classe para cada servo
ServoLEDC servo1cotovelo;
ServoLEDC servo2ombro1;
ServoLEDC servo3ombro2;
ServoLEDC servo4garra;


// <<< NOVA FUNÇÃO: Move os dois servos do ombro de forma sincronizada >>>
void moverOmbrosSincronizados(int angulo, int velocidade) {
    // Aplica a correção (offset) para cada servo
    int anguloFinalOmbro1 = constrain(angulo + OFFSET_OMBRO_1, 0, 180);
    int anguloFinalOmbro2 = constrain(angulo + OFFSET_OMBRO_2, 0, 180);

    // Manda o comando para os dois servos ao mesmo tempo
    servo2ombro1.writeSlow(anguloFinalOmbro1, velocidade);
    servo3ombro2.writeSlow(anguloFinalOmbro2, velocidade);
}

// Função para mover o motor de passo em um ângulo específico.
void moverAngulo(float angulo) {
    long passosParaMover = (PASSOS_POR_ROTACAO / 360.0) * angulo;

    if (angulo >= 0) {
        digitalWrite(STEPPER_DIR_PIN, HIGH); 
        Serial.print("Motor: Movendo ");
        Serial.print(angulo);
        Serial.println(" graus no sentido HORÁRIO...");
    } else {
        digitalWrite(STEPPER_DIR_PIN, LOW);
        Serial.print("Motor: Movendo ");
        Serial.print(abs(angulo));
        Serial.println(" graus no sentido ANTI-HORÁRIO...");
    }

    for (long i = 0; i < abs(passosParaMover); i++) {
        digitalWrite(STEPPER_STEP_PIN, HIGH);
        delayMicroseconds(800);
        digitalWrite(STEPPER_STEP_PIN, LOW);
        delayMicroseconds(800);
    }
    Serial.println("Motor: Movimento concluído.");
}


// Função auxiliar para imprimir o status de todos os servos
void printServoStatus() {
    Serial.print("Angulos Atuais -> ");
    Serial.print("S1 (Cotovelo): ");
    Serial.print(servo1cotovelo.read());
    Serial.print(" | S2 (Ombro1): ");
    Serial.print(servo2ombro1.read());
    Serial.print(" | S3 (Ombro2): ");
    Serial.print(servo3ombro2.read());
    Serial.print(" | S4 (Garra): ");
    Serial.println(servo4garra.read());
    Serial.println("-------------------------------------------------");
}

// Função para testar o motor de passo 
void testaMotorDePasso() {
    Serial.println("\n>>> Iniciando teste do Motor de Passo...");

    // Gira uma volta completa no sentido horário
    Serial.println("Movendo 1 volta no sentido horário...");
    digitalWrite(STEPPER_DIR_PIN, HIGH); // Define a direção

    for (int i = 0; i < PASSOS_POR_ROTACAO; i++) {
        digitalWrite(STEPPER_STEP_PIN, HIGH);
        delayMicroseconds(1000); // Pulso
        digitalWrite(STEPPER_STEP_PIN, LOW);
        delayMicroseconds(1000); // Pausa entre pulsos (controla a velocidade)
    }

    delay(1000); 

    // Gira uma volta completa no sentido anti-horário
    Serial.println("Movendo 1 volta no sentido ANTI-horário...");
    digitalWrite(STEPPER_DIR_PIN, LOW); // Inverte a direção

    for (int i = 0; i < PASSOS_POR_ROTACAO; i++) {
        digitalWrite(STEPPER_STEP_PIN, HIGH);
        delayMicroseconds(1000);
        digitalWrite(STEPPER_STEP_PIN, LOW);
        delayMicroseconds(1000);
    }
    
    Serial.println("<<< Fim do teste do Motor de Passo.");
    delay(2000); 
}


void calibrarOmbros() {
    int calibOffset1 = 0;
    int calibOffset2 = 0;

    Serial.println("\n\n--- MODO DE CALIBRACAO DOS OMBROS ---");
    Serial.println("Os servos do ombro foram movidos para 90 graus.");
    Serial.println("Ouca o zumbido. O objetivo e faze-lo parar.");
    Serial.println("Envie comandos para ajustar:");
    Serial.println("  'a1' ou 'a-1' -> ajusta o Servo Ombro 1 (pino 12)");
    Serial.println("  'b1' ou 'b-1' -> ajusta o Servo Ombro 2 (pino 14)");
    Serial.println("Quando o zumbido parar, anote os valores finais e coloque-os nas constantes OFFSET_OMBRO_1 e OFFSET_OMBRO_2 no topo do codigo.");
    Serial.println("Depois de anotar, reinicie o ESP32 para continuar.");
    Serial.println("----------------------------------------");

    // Posiciona os servos no centro para a calibração
    servo2ombro1.write(90);
    servo3ombro2.write(90);

    // Loop infinito de calibração
    while(true) {
        if (Serial.available() > 0) {
            String comando = Serial.readStringUntil('\n');
            char servoID = comando.charAt(0);
            int valor = comando.substring(1).toInt();

            if (servoID == 'a') {
                calibOffset1 += valor;
                servo2ombro1.write(90 + calibOffset1);
                Serial.print("Novo Offset Ombro 1 (Servo 2): ");
                Serial.println(calibOffset1);
            } else if (servoID == 'b') {
                calibOffset2 += valor;
                servo3ombro2.write(90 + calibOffset2);
                Serial.print("Novo Offset Ombro 2 (Servo 3): ");
                Serial.println(calibOffset2);
            }
        }
    }
}


void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    Serial.begin(115200);
    Serial.println("--- Sistema do Braco Robotico Iniciado ---");

    // Configuração dos Servos
    servo1cotovelo.attach(SERVO1_PIN, 0);
    servo2ombro1.attach(SERVO2_PIN, 1);
    servo3ombro2.attach(SERVO3_PIN, 2);
    servo4garra.attach(SERVO4_PIN, 3); // Garra desativada 

    // Configura os pinos do motor de passo
    pinMode(STEPPER_DIR_PIN, OUTPUT);
    pinMode(STEPPER_STEP_PIN, OUTPUT);
    delay(1000);

    // <<< MODO DE CALIBRAÇÃO >>> cotovelo
    // calibrarOmbros();
}

void loop() {
    Serial.println("\n--- Iniciando Nova Sequencia ---");
    delay(2000); // Pausa inicial

    // Passo 1: Levanta os ombros
    moverOmbrosSincronizados(80, VELOCIDADE_NORMAL);
    delay(3000);

    // Passo 2: Estica o cotovelo
    servo1cotovelo.writeSlow(90, VELOCIDADE_NORMAL);
    delay(3000);

    // Passo 3: Gira a base (motor de passo) 180 graus no sentido horário
    moverAngulo(180);
    delay(3000);

    // Passo 4: Abre a garra
    servo4garra.writeSlow(90, VELOCIDADE_NORMAL);
    delay(3000);

    // Passo 5: Gira a base (motor de passo) 90 graus no sentido ANTI-horário
    // AQUI você pode escolher outro valor
    moverAngulo(-90);
    delay(3000);

    // Passo 6: Fecha a garra
    servo4garra.writeSlow(0, VELOCIDADE_NORMAL);
    delay(3000);

    Serial.println("\n--- FIM DA SEQUENCIA ---");
}