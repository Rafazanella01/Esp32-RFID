#include <SPI.h>
#include <MFRC522.h>
#include <U8g2lib.h>

#define SS_PIN D4
#define RST_PIN D3

#define BUZZER D8

// Inicializa o display OLED
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/14, /*data=*/12, /*reset=*/U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/D5, /*data=*/D6, /*reset=*/U8X8_PIN_NONE);

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  //Inicia o display OLED
  u8g2.begin();

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Leitor RFID iniciado");

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);

  pinMode(BUZZER, OUTPUT);
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    //Tenta ler o cartão
    tone(BUZZER, 5000); //som de 1kHz
    delay(300);           
    noTone(BUZZER);     //Desliga o buzzer

   /* digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW); */

    if (mfrc522.PICC_ReadCardSerial()) {
      String uid = "";
      Serial.print("novo cartao lido  ");

      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += String(mfrc522.uid.uidByte[i], HEX);
      }
      
    mfrc522.PICC_HaltA();

    //Exibe o UID no Monitor Serial
    Serial.print("UID lido: ");
    Serial.println(uid);

    //Exibe o UID no display OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(0, 20, "UID lido:");
    u8g2.drawStr(0, 40, uid.c_str());
    u8g2.sendBuffer();

    delay(1000); //Aguarda antes de ler o próximo cartão
    }
  }
delay(200);
}
