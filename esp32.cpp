#include <SPI.h>
#include <MFRC522.h>
//#include <WiFi.h>
#include <HTTPClient.h> 

//const char* ssid = "Redmi 9C";
//const char* password = "123456789";

//Pinos RC522
#define SS_PIN 21
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  //WiFi.begin(ssid, password); //Conecta à rede Wi-Fi
  //while (WiFi.status() != WL_CONNECTED)
  /*{ //Espera até que a conexão seja estabelecida
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado.");
  */
  
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("ESP32: Pronto para ler cartões RFID");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  {
    return;
  }

  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    cardID += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.println("CARD:" + cardID);
  delay(1000);
}
