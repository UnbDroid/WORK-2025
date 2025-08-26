#include <Arduino.h>
#include "driver/pcnt.h" // <-- ESSENCIAL: Biblioteca para o Pulse Counter (PCNT)
#include "driver/ledc.h" // <-- BOA PRÁTICA: Biblioteca para o PWM Controller (LEDC)

// --- Configurações Gerais ---
#define QTD_MOTORES 4
#define PPR_CAIXA_SAIDA 1440.0 // Contagens por revolução do eixo de SAÍDA (ajuste se for diferente)
#define RPM_CALC_INTERVAL 200 // Intervalo em ms para calcular e exibir o RPM

// Estrutura para organizar os dados de cada motor
struct Motor {
  const char* nome;
  // Pinos de Direção (L298N IN1, IN2)
  const int pin_in1;
  const int pin_in2;
  // Pino de Velocidade (L298N Enable)
  const int pin_pwm;
  // Pinos do Encoder (Canal A, Canal B)
  const int pin_enc_a;
  const int pin_enc_b;
  // Hardware da ESP32
  const int pwm_channel;
  const pcnt_unit_t pcnt_unit;
  // Variáveis de estado
  volatile int64_t contagemPulsosTotal; // Acumula o total de pulsos
  double rpm;
  int16_t ultimaContagemHardware; // Para calcular o delta
};

// --- Mapeamento de Pinos ---
// Instanciando os 4 motores com suas respectivas configurações
Motor motores[QTD_MOTORES] = {
  // Motor 1
  {"M1", 27, 14, 15, 39, 36, 0, PCNT_UNIT_0, 0, 0.0, 0},
  // Motor 2
  {"M2", 22, 23, 2,  34, 35, 1, PCNT_UNIT_1, 0, 0.0, 0},
  // Motor 3 (ASSUMINDO pino 33, e não 32)
  {"M3", 5, 18,  12, 33, 32, 2, PCNT_UNIT_2, 0, 0.0, 0},
  // Motor 4
  {"M4", 16, 17, 13, 25, 26, 3, PCNT_UNIT_3, 0, 0.0, 0}
};

unsigned long ultimaAtualizacaoRpm = 0;

// Função para configurar o hardware PCNT para um motor
void setupEncoderPCNT(uint8_t index) {
  pcnt_config_t pcnt_config = {}; // Zera a estrutura de configuração
  
  // Configura os pinos e canais do PCNT
  pcnt_config.pulse_gpio_num = motores[index].pin_enc_a;
  pcnt_config.ctrl_gpio_num = motores[index].pin_enc_b;
  pcnt_config.unit = motores[index].pcnt_unit;
  pcnt_config.channel = PCNT_CHANNEL_0;
  
  // Define como o PCNT deve contar nos eventos de borda
  pcnt_config.pos_mode = PCNT_COUNT_DEC; // Decrementa na borda de subida de A
  pcnt_config.neg_mode = PCNT_COUNT_INC; // Incrementa na borda de descida de A
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; // Inverte a contagem baseado no pino B (HIGH)
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Mantém a contagem baseado no pino B (LOW)

  // Inicializa a unidade PCNT
  pcnt_unit_config(&pcnt_config);
  
  // (Opcional) Configura um filtro para ignorar ruídos/glitches de até 10us
  pcnt_set_filter_value(motores[index].pcnt_unit, 1023);
  pcnt_filter_enable(motores[index].pcnt_unit);
  
  // Zera e inicia o contador
  pcnt_counter_pause(motores[index].pcnt_unit);
  pcnt_counter_clear(motores[index].pcnt_unit);
  pcnt_counter_resume(motores[index].pcnt_unit);
}

// Função para definir a direção e velocidade de um motor
void moverMotor(uint8_t index, int velocidade) {
  bool paraFrente = velocidade < 0;
  
  if (velocidade == 0) {
    digitalWrite(motores[index].pin_in1, LOW);
    digitalWrite(motores[index].pin_in2, LOW);
  } else {
    digitalWrite(motores[index].pin_in1, paraFrente ? HIGH : LOW);
    digitalWrite(motores[index].pin_in2, paraFrente ? LOW : HIGH);
  }
  
  int pwmValue = abs(velocidade);
  if (pwmValue > 255) pwmValue = 255;
  
  ledcWrite(motores[index].pwm_channel, pwmValue);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sistema de 4 motores...");

  for (int i = 0; i < QTD_MOTORES; i++) {
    // --- Configuração dos Pinos de Controle do Motor ---
    pinMode(motores[i].pin_in1, OUTPUT);
    pinMode(motores[i].pin_in2, OUTPUT);
    
    // --- Configuração do PWM (LEDC) ---
    ledcSetup(motores[i].pwm_channel, 5000, 8); // Canal, Frequência de 5kHz, Resolução de 8 bits (0-255)
    ledcAttachPin(motores[i].pin_pwm, motores[i].pwm_channel);
    
    // --- Configuração do Encoder (PCNT) ---
    setupEncoderPCNT(i);
    
    Serial.printf("Motor %s configurado.\n", motores[i].nome);
  }

  // --- Exemplo de Uso: Ligar todos os motores para frente com velocidade ~70% ---
  int velocidadeExemplo = -180; // De -255 a 255
  for (int i = 0; i < QTD_MOTORES; i++) {
    moverMotor(i, velocidadeExemplo);
  }
}

void loop() {
  unsigned long tempoAtual = millis();
  
  if (tempoAtual - ultimaAtualizacaoRpm >= RPM_CALC_INTERVAL) {
    unsigned long intervalo = tempoAtual - ultimaAtualizacaoRpm;
    ultimaAtualizacaoRpm = tempoAtual;

    // Limpa a tela do monitor serial (funciona na maioria dos terminais, como o da IDE do Arduino 2.x)
    Serial.print("\033[2J\033[H");
    Serial.println("--- Leitura dos Motores ---");
    Serial.println("Motor | Contagem Total   | RPM");
    Serial.println("------------------------------------");

    for (int i = 0; i < QTD_MOTORES; i++) {
      // Lê o valor atual do contador de hardware
      int16_t contagemHardwareAtual;
      pcnt_get_counter_value(motores[i].pcnt_unit, &contagemHardwareAtual);
      
      // Calcula a diferença desde a última leitura
      int16_t deltaPulsos = contagemHardwareAtual - motores[i].ultimaContagemHardware;
      
      // Atualiza a contagem total e a última contagem de hardware
      motores[i].contagemPulsosTotal += deltaPulsos;
      motores[i].ultimaContagemHardware = contagemHardwareAtual;

      // Calcula o RPM
      motores[i].rpm = ((double)deltaPulsos / PPR_CAIXA_SAIDA) * (60000.0 / (double)intervalo);
      
      // Imprime os dados formatados
      char buffer[100];
      sprintf(buffer, "%-5s | %-16lld | %.2f", 
              motores[i].nome, 
              motores[i].contagemPulsosTotal, 
              motores[i].rpm);
      Serial.println(buffer);
    }
    Serial.println("------------------------------------");
  }
}