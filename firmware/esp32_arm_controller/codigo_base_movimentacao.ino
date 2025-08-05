/*
 * =================================================================================
 * CÓDIGO DE SEQUÊNCIA DE MOVIMENTO PARA ALVO ESPECÍFICO
 * =================================================================================
 * Autor: Gemini AI
 * Versão: 1.5 (Correção de movimento do Cotovelo)
 *
 * Descrição:
 * V1.5 tenta corrigir o problema do cotovelo não se mover, invertendo o ângulo
 * enviado para o servo. Se a montagem física do servo estiver ao contrário do
 * que a matemática espera, esta alteração deve corrigir o movimento.
 *
 * Como usar:
 * 1. VERIFIQUE SE O FIO DE SINAL DO SERVO DO COTOVELO ESTÁ LIGADO AO PINO 22.
 * 2. Carregue este código.
 * 3. Se o cotovelo se mover na direção oposta, remova o "180 - " da linha
 * de comando do servoCotovelo na função mover_braco_suave.
 * =================================================================================
*/

// --- BIBLIOTECAS ---
#include <ESP32Servo.h>
#include <math.h>

// --- PINOS DE CONEXÃO (Ajuste conforme a sua montagem) ---
const int pinoOmbro1    = 13;
const int pinoOmbro2    = 18;
// ATENÇÃO: Verifique se o servo do cotovelo está mesmo ligado ao pino 22.
const int pinoCotovelo  = 22;
const int pinoGarra     = 21;
const int pinoBase      = 12;

// --- OBJETOS DOS SERVOS ---
Servo servoOmbro1;
Servo servoOmbro2;
Servo servoCotovelo;
Servo servoGarra;
Servo servoBase;

// --- PARÂMETROS DO ROBÔ (CALIBRAÇÃO) ---
const float L1 = 19.0; // Comprimento do link do "ombro"
const float L2 = 24.0; // Comprimento do link do "cotovelo"

// --- PARÂMETROS DA SEQUÊNCIA (AJUSTE AQUI) ---
// CORREÇÃO: Utilizar uma posição de alvo com Z positivo (acima do chão).
// Um Z negativo é fisicamente impossível e causa erros de cálculo.
const float POSICAO_ALVO[3]    = {0.1, 20.0, 39.0};
// Posição segura de descanso/início
const float POSICAO_DESCANSO[3] = {20.0, 0.0, 25.0};
// Altura segura acima do alvo para evitar colisões
const float ALTURA_SEGURANCA_CM = 10.0;


// Ângulos da garra
const int GARRA_ABERTA = 70;
const int GARRA_FECHADA = 0;

// Velocidade do movimento
const int VELOCIDADE = 15; // Delay em ms. Valores maiores = movimento mais lento.

// --- ESTRUTURA PARA ARMAZENAR ÂNGULOS ---
struct AngulosBraco {
  int base;
  int ombro;
  int cotovelo;
};

// --- VARIÁVEIS GLOBAIS DE POSIÇÃO ATUAL ---
int anguloAtualBase = 90;
int anguloAtualOmbro = 90;
int anguloAtualCotovelo = 90;
int anguloAtualGarra = GARRA_ABERTA;

// --- PROTÓTIPOS DAS FUNÇÕES ---
AngulosBraco calcular_cinematica_inversa(float x, float y, float z);
void mover_braco_suave(int anguloBaseFinal, int anguloOmbroFinal, int anguloCotoveloFinal, int anguloGarraFinal, int velocidade);

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== INICIANDO SEQUÊNCIA DE MOVIMENTO PARA ALVO (v1.5) ===");

  // Anexa todos os servos
  servoBase.attach(pinoBase);
  servoOmbro1.attach(pinoOmbro1);
  servoOmbro2.attach(pinoOmbro2);
  servoCotovelo.attach(pinoCotovelo);
  servoGarra.attach(pinoGarra);

  // Coloca o braço na posição de descanso
  Serial.println("A posicionar o braço na posição de descanso...");
  AngulosBraco angulosIniciais = calcular_cinematica_inversa(POSICAO_DESCANSO[0], POSICAO_DESCANSO[1], POSICAO_DESCANSO[2]);

  // Define a posição atual e move instantaneamente para lá
  anguloAtualBase = angulosIniciais.base;
  anguloAtualOmbro = angulosIniciais.ombro;
  anguloAtualCotovelo = angulosIniciais.cotovelo;
  servoBase.write(anguloAtualBase);
  servoOmbro1.write(180 - anguloAtualOmbro); // Comando invertido para o ombro
  servoOmbro2.write(anguloAtualOmbro);      // Comando invertido para o ombro
  servoCotovelo.write(180 - anguloAtualCotovelo); // Comando do cotovelo também invertido
  servoGarra.write(anguloAtualGarra); // Escreve o ângulo inicial da garra (aberta)

  Serial.println("A sequência começará em 3 segundos...");
  delay(3000);
}

void loop() {
  // Define as coordenadas de trabalho
  float pos_alvo[3] = {POSICAO_ALVO[0], POSICAO_ALVO[1], POSICAO_ALVO[2]};
  float pos_acima_alvo[3] = {POSICAO_ALVO[0], POSICAO_ALVO[1], POSICAO_ALVO[2] + ALTURA_SEGURANCA_CM};
  float pos_descanso[3] = {POSICAO_DESCANSO[0], POSICAO_DESCANSO[1], POSICAO_DESCANSO[2]};

  // Calcula todos os ângulos necessários no início do ciclo
  AngulosBraco angulosAlvo = calcular_cinematica_inversa(pos_alvo[0], pos_alvo[1], pos_alvo[2]);
  AngulosBraco angulosAcimaAlvo = calcular_cinematica_inversa(pos_acima_alvo[0], pos_acima_alvo[1], pos_acima_alvo[2]);
  AngulosBraco angulosDescanso = calcular_cinematica_inversa(pos_descanso[0], pos_descanso[1], pos_descanso[2]);


  // === NOVA SEQUÊNCIA DE "PEGAR E SOLTAR" ===

  // --- ETAPA 1: IR PARA UM PONTO SEGURO ACIMA DO ALVO ---
  Serial.println("1. A mover para cima do alvo...");
  mover_braco_suave(angulosAcimaAlvo.base, angulosAcimaAlvo.ombro, angulosAcimaAlvo.cotovelo, GARRA_ABERTA, VELOCIDADE);

  // --- ETAPA 2: DESCER ATÉ AO ALVO ---
  Serial.println("2. A descer para o alvo...");
  mover_braco_suave(angulosAlvo.base, angulosAlvo.ombro, angulosAlvo.cotovelo, GARRA_ABERTA, VELOCIDADE);

  // --- ETAPA 3: FECHAR A GARRA PARA PEGAR ---
  Serial.println("3. A fechar a garra...");
  mover_braco_suave(anguloAtualBase, anguloAtualOmbro, anguloAtualCotovelo, GARRA_FECHADA, VELOCIDADE);
  delay(500); // Pausa para firmar a garra

  // --- ETAPA 4: SUBIR COM O OBJETO ---
  Serial.println("4. A subir com o objeto...");
  mover_braco_suave(angulosAcimaAlvo.base, angulosAcimaAlvo.ombro, angulosAcimaAlvo.cotovelo, GARRA_FECHADA, VELOCIDADE);

  // --- ETAPA 5: VOLTAR À POSIÇÃO DE DESCANSO ---
  Serial.println("5. A voltar para a posição de descanso...");
  mover_braco_suave(angulosDescanso.base, angulosDescanso.ombro, angulosDescanso.cotovelo, GARRA_FECHADA, VELOCIDADE);

  // --- ETAPA 6: ABRIR A GARRA PARA SOLTAR ---
  Serial.println("6. A abrir a garra...");
  mover_braco_suave(anguloAtualBase, anguloAtualOmbro, anguloAtualCotovelo, GARRA_ABERTA, VELOCIDADE);


  Serial.println("\n=== CICLO COMPLETO. A REINICIAR EM 5 SEGUNDOS. ===");
  delay(5000);
}


// --- FUNÇÕES AUXILIARES (Cinemática e Movimento) ---

AngulosBraco calcular_cinematica_inversa(float x, float y, float z) {
  AngulosBraco angulos;
  Serial.printf("\nA calcular ângulos para X:%.1f Y:%.1f Z:%.1f\n", x, y, z);
  angulos.base = atan2(y, x) * 180.0 / M_PI;
  float r = sqrt(pow(x, 2) + pow(y, 2));
  float d = sqrt(pow(r, 2) + pow(z, 2));
  if (d > (L1 + L2)) {
      Serial.println("AVISO: Ponto fora de alcance!");
      d = L1 + L2 - 0.1;
  }
  float alfa = acos((pow(L1, 2) + pow(d, 2) - pow(L2, 2)) / (2 * L1 * d)) * 180.0 / M_PI;
  float beta = acos((pow(L1, 2) + pow(L2, 2) - pow(d, 2)) / (2 * L1 * L2)) * 180.0 / M_PI;
  angulos.ombro = (atan2(z, r) * 180.0 / M_PI) + alfa;
  angulos.cotovelo = 180.0 - beta;
  angulos.base = 90 - angulos.base;
  angulos.base = constrain(angulos.base, 0, 180);
  angulos.ombro = constrain(angulos.ombro, 0, 180);
  angulos.cotovelo = constrain(angulos.cotovelo, 0, 180);
  
  // DIAGNÓSTICO: Imprime os ângulos finais calculados
  Serial.printf("  -> Ângulos Calculados: Base=%d, Ombro=%d, Cotovelo=%d\n", angulos.base, angulos.ombro, angulos.cotovelo);
  return angulos;
}

void mover_braco_suave(int anguloBaseFinal, int anguloOmbroFinal, int anguloCotoveloFinal, int anguloGarraFinal, int velocidade) {
  bool movimentoCompleto = false;
  while (!movimentoCompleto) {
      movimentoCompleto = true;
      if (anguloAtualBase != anguloBaseFinal) { anguloAtualBase += (anguloBaseFinal > anguloAtualBase) ? 1 : -1; movimentoCompleto = false; }
      if (anguloAtualOmbro != anguloOmbroFinal) { anguloAtualOmbro += (anguloOmbroFinal > anguloAtualOmbro) ? 1 : -1; movimentoCompleto = false; }
      if (anguloAtualCotovelo != anguloCotoveloFinal) { anguloAtualCotovelo += (anguloCotoveloFinal > anguloAtualCotovelo) ? 1 : -1; movimentoCompleto = false; }
      if (anguloAtualGarra != anguloGarraFinal) { anguloAtualGarra += (anguloGarraFinal > anguloAtualGarra) ? 1 : -1; movimentoCompleto = false; }

      servoBase.write(anguloAtualBase);
      // CORREÇÃO: Lógica dos servos do ombro invertida para corrigir a direção do movimento vertical.
      servoOmbro1.write(180 - anguloAtualOmbro);
      servoOmbro2.write(anguloAtualOmbro);
      // CORREÇÃO: Invertendo o ângulo do cotovelo. Se ele se mover na direção errada, remova o "180 - ".
      servoCotovelo.write(180 - anguloAtualCotovelo);
      servoGarra.write(anguloAtualGarra);
      delay(velocidade);
  }
}
