#include <Wire.h>
#include <U8g2lib.h>

// Configuração do display (exemplo: OLED I2C 128x64)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/14, /*data=*/12, /*reset=*/U8X8_PIN_NONE);

void setup() {
  // Inicializa o display
  u8g2.begin();
  // Inicializa a comunicação serial
  Serial.begin(115200);
  Serial.println("Aguardando dados...");
}

void loop() {
  // Verifica se há dados disponíveis para ler na serial
  if (Serial.available() > 0) {
    // Lê os dados da serial até uma nova linha
    String input = "";
    while (Serial.available() > 0) {
      input += (char)Serial.read();
      delay(10); // Atraso para garantir que os dados completos sejam lidos
    }

    // Remove espaços extras e quebras de linha
    input.trim();
    
    // Extrai o UID e o STATUS
    String card = "";
    String status = "";

    // Verifica se a entrada contém o formato esperado
    if (input.startsWith("CARD: ") && input.indexOf(" | STATUS: ") != -1) {
      card = input.substring(6, input.indexOf(" | STATUS: "));  // Extrai o UID
      status = input.substring(input.indexOf(" | STATUS: ") + 11);  // Extrai o status
    }
    
    // Exibe a mensagem no display com U8g2
    u8g2.clearBuffer();  // Limpa o buffer do display
    u8g2.setFont(u8g2_font_ncenB08_tr);  // Define a fonte (pode escolher outra)
    
    // Exibe o UID
    u8g2.drawStr(0, 20, card.c_str());  // Convertendo String para const char* com c_str()
    u8g2.drawStr(0, 40, status.c_str());  // Convertendo String para const char* com c_str()
    
    // Verifica o status e desenha o ícone correspondente
    if (status == "Rejeitado") {
      u8g2.drawStr(100, 40, "X");  // Exibe "X" se o status for Rejeitado
    } else if (status == "Permitido") {
      u8g2.drawStr(100, 40, "✔️");  // Exibe "✔️" se o status for Permitido
    }

    u8g2.sendBuffer();  // Atualiza o display
  }
}
