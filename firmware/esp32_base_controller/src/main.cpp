#include <Arduino.h>
#include "driver/pcnt.h" // Biblioteca para o Pulse Counter (PCNT)
#include "driver/ledc.h" // Biblioteca para o PWM Controller (LEDC)
// a4988

// --- Configurações Gerais ---
#define PPR_CAIXA_SAIDA 1440.0 // Contagens por revolução do eixo de SAÍDA
#define RPM_CALC_INTERVAL 200  // Intervalo em ms para calcular e exibir o RPM
#define NUM_LEITURAS_MEDIA 10  // Quantidade de leituras para a média móvel

// --- Pinos do Motor ---
#define MOTOR_NOME "Motor"
#define MOTOR_IN1_PIN 27
#define MOTOR_IN2_PIN 14
#define MOTOR_PWM_PIN 12
#define MOTOR_ENC_A_PIN 39
#define MOTOR_ENC_B_PIN 36
#define MOTOR_PWM_CHANNEL 0
#define MOTOR_PCNT_UNIT PCNT_UNIT_0

// --- Variáveis de Estado ---
int pwmAtivo = 0; // Armazena o último PWM comandado
double rpmAtual = 0.0;
double rpmMedio = 0.0;
double leiturasRpm[NUM_LEITURAS_MEDIA] = {0.0};
int indiceLeituraRpm = 0;
int16_t ultimaContagemHardware = 0;
unsigned long ultimaAtualizacaoRpm = 0;

// Função para definir a direção e velocidade do motor
void moverMotor(int velocidade) {
  bool paraFrente = velocidade > 0;
  
  if (velocidade == 0) {
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, LOW);
  } else {
    digitalWrite(MOTOR_IN1_PIN, paraFrente ? HIGH : LOW);
    digitalWrite(MOTOR_IN2_PIN, paraFrente ? LOW : HIGH);
  }
  
  int pwmValue = abs(velocidade);
  if (pwmValue > 255) pwmValue = 255;
  
  ledcWrite(MOTOR_PWM_CHANNEL, pwmValue);
}

// Função para zerar o histórico da média móvel
void zerarMediaMovel() {
  Serial.println("--- Novo PWM detectado. Zerando a média móvel. ---");
  for (int i = 0; i < NUM_LEITURAS_MEDIA; i++) {
    leiturasRpm[i] = 0.0;
  }
  indiceLeituraRpm = 0;
  rpmMedio = 0.0; // Zera a média imediatamente para a exibição
}

// Função para configurar o hardware PCNT para o motor
void setupEncoderPCNT() {
  pcnt_config_t pcnt_config = {};
  pcnt_config.pulse_gpio_num = MOTOR_ENC_A_PIN;
  pcnt_config.ctrl_gpio_num = MOTOR_ENC_B_PIN;
  pcnt_config.unit = MOTOR_PCNT_UNIT;
  pcnt_config.channel = PCNT_CHANNEL_0;
  pcnt_config.pos_mode = PCNT_COUNT_DEC;
  pcnt_config.neg_mode = PCNT_COUNT_INC;
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
  
  pcnt_unit_config(&pcnt_config);
  pcnt_set_filter_value(MOTOR_PCNT_UNIT, 1023);
  pcnt_filter_enable(MOTOR_PCNT_UNIT);
  pcnt_counter_pause(MOTOR_PCNT_UNIT);
  pcnt_counter_clear(MOTOR_PCNT_UNIT);
  pcnt_counter_resume(MOTOR_PCNT_UNIT);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nIniciando teste de motor por PWM...");

  // Configuração dos Pinos e Hardware
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  ledcSetup(MOTOR_PWM_CHANNEL, 1500, 8);
  ledcAttachPin(MOTOR_PWM_PIN, MOTOR_PWM_CHANNEL);
  setupEncoderPCNT();
  
  // Garante que o motor comece parado
  moverMotor(0); 

  Serial.println("\n--- INSTRUÇÕES ---");
  Serial.println("Digite um valor de PWM (-255 a 255) e aperte Enter.");
  Serial.println("---------------------\n");
}

void loop() {
  // 1. Processa comandos do usuário via terminal
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    int pwmValor = comando.toInt();

    if (pwmValor != pwmAtivo) {
        zerarMediaMovel();
    }

    if (pwmValor >= -255 && pwmValor <= 255) {
      pwmAtivo = pwmValor; // Atualiza o PWM ativo
      
      moverMotor(pwmAtivo);
      Serial.printf("\n>> COMANDO: Ajustando PWM para %d <<\n\n", pwmAtivo);
    } else {
      Serial.println("\n>> ERRO: Valor de PWM inválido. Use de -255 a 255. <<\n");
    }
  }

  // 2. Calcula e exibe RPM em intervalos regulares
  if (millis() - ultimaAtualizacaoRpm >= RPM_CALC_INTERVAL) {
    ultimaAtualizacaoRpm = millis();

    // Lê o hardware
    int16_t contagemHardwareAtual;
    pcnt_get_counter_value(MOTOR_PCNT_UNIT, &contagemHardwareAtual);
    
    // Calcula as variáveis
    int16_t deltaPulsos = contagemHardwareAtual - ultimaContagemHardware;
    ultimaContagemHardware = contagemHardwareAtual;
    rpmAtual = ((double)deltaPulsos / PPR_CAIXA_SAIDA) * (60000.0 / (double)RPM_CALC_INTERVAL);

    // Adiciona leitura atual e calcula a média móvel
    leiturasRpm[indiceLeituraRpm] = rpmAtual;
    indiceLeituraRpm = (indiceLeituraRpm + 1) % NUM_LEITURAS_MEDIA;
    double soma = 0;
    for (int i = 0; i < NUM_LEITURAS_MEDIA; i++) {
      soma += leiturasRpm[i];
    }
    rpmMedio = soma / NUM_LEITURAS_MEDIA;
    
    // Exibe os resultados
    char buffer[100];
    sprintf(buffer, "PWM Ativo: %-4d | RPM Atual: %-7.2f | RPM Médio: %.2f",
            pwmAtivo, rpmAtual, rpmMedio);
    Serial.println(buffer);
    // Adiciona uma linha extra para ser lida facilmente pelo script Python
    // Formato: "DATA:PWM_VALOR,RPM_MEDIO_VALOR"
    Serial.printf("DATA:%d,%.2f\n", pwmAtivo, rpmMedio);
  }
}