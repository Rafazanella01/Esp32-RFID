#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

enum EstadoSistema
{
  REPOUSO,
  CADASTRO,
  LEITURA
};
EstadoSistema estadoAtual = REPOUSO; // Estado inicial

String uidLido;

String serverPath = "https://congenial-space-acorn-4jq9qqx76597fqppw-5000.app.github.dev";

const char *ssid = "Redmi 9C";
const char *password = "123456789";

unsigned int leituraAnteriorBotao = 0;

// Pinos RC522
#define SS_PIN 21
#define RST_PIN 22
#define buzzer 13
#define ledVerde 25
#define ledAmarelo 26
#define ledVermelho 32
#define botao 35

MFRC522 rfid(SS_PIN, RST_PIN);

void sendLog(String uid, String stateAcess)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String postJSON = "{\"id\":\"" + uid + "\", \"status\": \"" + stateAcess + "\"}"; // Cria o JSON POST

    http.begin(serverPath + "/logs");

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(postJSON);
    Serial.print("Response code '\logs': ");
    Serial.println(httpResponseCode);
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}

bool verifyUID(String uid)
{

  if (uid == "")
  {
    return false;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(serverPath + "/verify?uid=" + uid);

    int httpResponseCode = http.GET();
    Serial.print("Response code: ");
    Serial.println(httpResponseCode);

    http.end();

    if (httpResponseCode == 200)
    {
      return true;
    }
  }
  else
  {
    Serial.println("WiFi Disconnected");
    return false;
  }
  return false;
}

void notificaAcessoPermitido()
{
  digitalWrite(ledVerde, HIGH);

  tone(buzzer, 5000);
  delay(50);
  noTone(buzzer);
  delay(120);
  tone(buzzer, 5000);
  delay(50);
  noTone(buzzer);

  digitalWrite(ledVerde, LOW);
}

void notificaAcessoNegado()
{
  tone(buzzer, 300);

  digitalWrite(ledVermelho, HIGH);
  delay(600);
  digitalWrite(ledVermelho, LOW);

  noTone(buzzer);
  delay(100);
}

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado.");

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("ESP32: Pronto para ler cartões RFID");
}

String lerCartao()
{
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  {
    return "";
  }

  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    cardID += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.println("CARD: " + cardID);

  delay(1000);
  return cardID;
}

void loop()
{
  int leituraBotao = digitalRead(botao);
  String status = "";

  if (leituraBotao)
  {
    leituraBotao = 1;
    if (leituraAnteriorBotao != leituraBotao)
    {
      estadoAtual = CADASTRO;
    }
    leituraAnteriorBotao = leituraBotao;
    delay(500);
  }
  else
  {
    leituraBotao = 0;
    leituraAnteriorBotao = leituraBotao;
  }

  String uid = lerCartao();

  switch (estadoAtual)
  {

  case REPOUSO:
    if (uid != "")
    {
      Serial.print("Cartao detectado: " + uid);
      uidLido = uid;
      estadoAtual = LEITURA;
      break;
    }
    uidLido = "";
    break;

  case LEITURA:
    Serial.println("leiura :" + uidLido);
    status = "Rejeitado";

    if (verifyUID(uidLido))
    {
      status = "Permitido";
      notificaAcessoPermitido();
    }
    else
    {
      notificaAcessoNegado();
    }

    Serial.print(status);

    sendLog(uidLido, status);
    estadoAtual = REPOUSO;
    break;

  case CADASTRO:
    Serial.print("cadastrando...");

    estadoAtual = REPOUSO;
    break;

  default:
    return;
  }
}
