/*
 * =================================================================================
 * CÓDIGO DE TESTE ESPECÍFICO PARA A GARRA (VERSÃO DE DIAGNÓSTICO)
 * =================================================================================
 * Autor: Gemini AI
 * Versão: 1.1
 *
 * Descrição:
 * Esta versão de diagnóstico serve para identificar problemas com o servo da garra.
 * Em vez de saltar entre as posições, o código faz um movimento de "varrimento"
 * suave, grau a grau, de totalmente aberto para totalmente fechado.
 *
 * Se o servo se mover um pouco e depois parar, tremer ou reiniciar o ESP32,
 * o problema é quase de certeza a ALIMENTAÇÃO. Servos consomem muita energia
 * e a porta USB do computador geralmente não é suficiente.
 *
 * >> SOLUÇÃO: Use uma fonte de alimentação EXTERNA de 5V para alimentar os seus servos. <<
 * - Ligue o pino VCC (vermelho) do servo aos 5V da fonte externa.
 * - Ligue o pino GND (preto/castanho) do servo ao GND da fonte externa E ao GND do ESP32.
 * - Ligue o pino de SINAL (laranja/amarelo) ao pino 15 do ESP32.
 *
 * Como usar:
 * 1. Carregue este código no seu ESP32.
 * 2. Abra o Monitor Serial (velocidade 115200).
 * 3. Observe o movimento da garra e as mensagens no monitor.
 * =================================================================================
*/

// --- BIBLIOTECAS ---
#include <ESP32Servo.h>

// --- PINOS DE CONEXÃO ---
// Certifique-se de que este é o pino correto para o servo da sua garra.
const int pinoGarra = 21;

// --- OBJETO DO SERVO ---
Servo servoGarra;

// --- PARÂMETROS DE TESTE (AJUSTE AQUI) ---
const int ANGULO_ABERTO = 0;    // Ângulo que representa a garra totalmente aberta
const int ANGULO_FECHADO = 80;  // Ângulo que representa a garra totalmente fechada
const int VELOCIDADE_SWEEP = 20; // Delay em ms entre cada grau. Valores maiores = movimento mais lento.

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== INICIANDO TESTE DA GARRA (v1.1 - Diagnóstico) ===");

  // Anexa o servo da garra ao pino correspondente
  servoGarra.attach(pinoGarra, 500, 2400);

  Serial.println("O teste começará em 3 segundos...");
  delay(3000);
}

void loop() {
  // --- ETAPA 1: FECHAR A GARRA (MOVIMENTO DE VARRIMENTO) ---
  Serial.println("Movendo do ângulo aberto para o fechado...");

  for (int pos = ANGULO_ABERTO; pos <= ANGULO_FECHADO; pos++) {
    servoGarra.write(pos); // Envia o ângulo atual para o servo
    Serial.print("Ângulo enviado: ");
    Serial.println(pos);
    delay(VELOCIDADE_SWEEP); // Pequena pausa para criar um movimento suave
  }

  Serial.println("Posição final atingida. A aguardar 2 segundos.");
  delay(2000);


  // --- ETAPA 2: ABRIR A GARRA (MOVIMENTO DE VARRIMENTO) ---
  Serial.println("Movendo do ângulo fechado para o aberto...");

  for (int pos = ANGULO_FECHADO; pos >= ANGULO_ABERTO; pos--) {
    servoGarra.write(pos); // Envia o ângulo atual para o servo
    Serial.print("Ângulo enviado: ");
    Serial.println(pos);
    delay(VELOCIDADE_SWEEP); // Pequena pausa para criar um movimento suave
  }
  
  Serial.println("Posição inicial atingida. A aguardar 2 segundos.");
  delay(2000);
}
