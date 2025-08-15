#include <Arduino.h>
#include "MecanumPlatform.h" 

// --- Pinos de Controle do Motor no Driver L298N ---
// Renomeei os pinos para maior clareza com o padrão do L298N 

//M1
#define ENA_PIN 4// Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define IN1_PIN 17 // Pino de direção 1, conectado ao IN1 ou IN3
#define IN2_PIN 16 // Pino de direção 2, conectado ao IN2 ou IN4

//M4
#define ENA_PIN1 13// Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define IN1_PIN1 14 // Pino de direção 1, conectado ao IN1 ou IN3
#define IN2_PIN1 12 // Pino de direção 2, conectado ao IN2 ou IN4

// M3
#define ENA_PIN2 15// Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define IN1_PIN2 26 // Pino de direção 1, conectado ao IN1 ou IN3
#define IN2_PIN2 27 // Pino de direção 2, conectado ao IN2 ou IN4

// M2
#define ENA_PIN3 2// Pino de velocidade (PWM), deve ser conectado ao ENA ou ENB do L298N
#define IN1_PIN3 25 // Pino de direção 1, conectado ao IN1 ou IN3
#define IN2_PIN3 33 // Pino de direção 2, conectado ao IN2 ou IN4

// --- Configurações do PWM (LEDC) ---
#define PWM_FREQUENCY 10000 // Frequência do PWM em Hz
#define PWM_CHANNEL    0      // Canal LEDC a ser usado (0-15)
#define PWM_CHANNEL1   1     // Canal LEDC a ser usado (0-15)
#define PWM_CHANNEL2   2     // Canal LEDC a ser usado (0-15)
#define PWM_CHANNEL3   3     // Canal LEDC a ser usado (0-15)
#define PWM_RESOLUTION 8   // Resolução em bits (8 bits = 0-255)

void setup() {
  // Configura os pinos de direção como saída
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  pinMode(IN1_PIN1, OUTPUT);
  pinMode(IN2_PIN1, OUTPUT);

  pinMode(IN1_PIN2, OUTPUT);
  pinMode(IN2_PIN2, OUTPUT);

  pinMode(IN1_PIN3, OUTPUT);
  pinMode(IN2_PIN3, OUTPUT);

  // --- Configuração do sistema LEDC para o controle de velocidade (PWM) ---
  // 1. Configura o canal PWM com frequência e resolução
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL1, PWM_FREQUENCY, PWM_RESOLUTION);

  // 2. Anexa o pino ENA ao canal PWM configurado
  ledcAttachPin(ENA_PIN, PWM_CHANNEL);
  ledcAttachPin(ENA_PIN1, PWM_CHANNEL1);

  ledcSetup(PWM_CHANNEL2, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL3, PWM_FREQUENCY, PWM_RESOLUTION);

  // 2. Anexa o pino ENA ao canal PWM configurado
  ledcAttachPin(ENA_PIN2, PWM_CHANNEL2);
  ledcAttachPin(ENA_PIN3, PWM_CHANNEL3);
}

void loop() {
  Serial.begin(115200); // Inicia a comunicação serial para debug (opcional)

  // --- Teste 1: Girar para Frente com Velocidade Máxima ---
  Serial.println("Girando para frente...");
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  ledcWrite(PWM_CHANNEL, 200); // Velocidade máxima (2^8 - 1)

  digitalWrite(IN1_PIN1, HIGH);
  digitalWrite(IN2_PIN1, LOW);
  ledcWrite(PWM_CHANNEL1, 200); // Velocidade máxima (2^8 - 1)


  digitalWrite(IN1_PIN2, HIGH);
  digitalWrite(IN2_PIN2, LOW);
  ledcWrite(PWM_CHANNEL2, 200); // Velocidade máxima (2^8 - 1)
   
  digitalWrite(IN1_PIN3, HIGH);
  digitalWrite(IN2_PIN3, LOW);
  ledcWrite(PWM_CHANNEL3, 200); // Velocidade máxima (2^8 - 1)*/

  delay(3000);

}