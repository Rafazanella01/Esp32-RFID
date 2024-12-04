#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

enum EstadoSistema {
  REPOUSO,
  CADASTRO,
  LEITURA
};
EstadoSistema estadoAtual = REPOUSO;  //Estado inicial

String uidLido;

String serverPath = "https://congenial-space-acorn-4jq9qqx76597fqppw-5000.app.github.dev";

//const char *ssid = "ESP32";
//const char *password = "esp32wifi";
//const char *ssid = "VERA";
//const char *password = "vera19901";
//const char *ssid = "Patrícia_PontoCom";
//const char *password = "pati19901";
const char *ssid = "Redmi 9C";
const char *password = "123456789";

unsigned int leituraAnteriorBotao = 0;
unsigned int ledAmareloCont = 0;

// Pinos RC522
#define SS_PIN 21
#define RST_PIN 22
#define buzzer 13
#define ledVerde 25
#define ledAmarelo 26
#define ledVermelho 32
#define botao 35

#define sdaLcd 5
#define sclLcd 4


MFRC522 rfid(SS_PIN, RST_PIN);

//Inicializando o LCD (endereço 0x27, 16 colunas e 2 linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void sendLog(String uid, String stateAcess) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postJSON = "{\"id\":\"" + uid + "\", \"status\": \"" + stateAcess + "\"}";  //Cria o JSON POST

    http.begin(serverPath + "/logs");

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(postJSON);
    Serial.print("Response code '\logs': ");
    Serial.println(httpResponseCode);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

bool verifyNewRegister() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverPath + "/register");

    int httpResponseCode = http.GET();
    Serial.print("Response code /register: ");
    Serial.println(httpResponseCode);

    http.end();

    if (httpResponseCode == 200) {
      return true;
    }

  } else {
    Serial.println("WiFi Disconnected");
    return false;
  }
  return false;
}

bool verifyUID(String uid) {

  if (uid == "") {
    return false;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverPath + "/verify?uid=" + uid);

    int httpResponseCode = http.GET();
    Serial.print("Response code: ");
    Serial.println(httpResponseCode);

    http.end();

    if (httpResponseCode == 200) {
      return true;
    }
  } else {
    Serial.println("WiFi Disconnected");
    return false;
  }
  return false;
}

void notificaAcessoPermitido() {
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

void notificaAcessoNegado() {
  tone(buzzer, 300);

  digitalWrite(ledVermelho, HIGH);
  delay(600);
  digitalWrite(ledVermelho, LOW);

  noTone(buzzer);
  delay(100);
}

void ledAmareloCadastro() {
  ledAmareloCont++;

  if (ledAmareloCont == 20){
    digitalWrite(ledAmarelo, HIGH);
  }

  if (ledAmareloCont == 40){
    digitalWrite(ledAmarelo, LOW);
    ledAmareloCont = 0;
  }
}

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado.");

  Wire.begin(sdaLcd, sclLcd);

  //Inicializando o LCD
  lcd.begin(16, 2);
  lcd.backlight();  //Liga a luz de fundo do LCD
  lcd.clear();      //Limpa o display no início

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("ESP32: Pronto para ler cartões RFID");
}

void verificarBotao() {
  int leituraBotao = digitalRead(botao);

  if (leituraBotao) {
    leituraBotao = 1;
    if (leituraAnteriorBotao != leituraBotao) {
      estadoAtual = CADASTRO;
    }
    leituraAnteriorBotao = leituraBotao;
    delay(500);
  } else {
    leituraBotao = 0;
    leituraAnteriorBotao = leituraBotao;
  }
}

String lerCartao() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return "";
  }

  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    cardID += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.println("CARD: " + cardID);

  delay(1000);
  return cardID;
}

bool cadastrarCartao(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postJSON = "{\"uid\":\"" + uid + "\"}";  //Cria o JSON com o UID

    http.begin(serverPath + "/register");  //Endpoint para cadastro

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(postJSON);  //Envia o POST com o UID
    Serial.print("Response code (cadastro): ");
    Serial.println(httpResponseCode);  //Imprime o código de resposta do servidor
    http.end();                        //Finaliza a requisição

    if (httpResponseCode == 201) {  //Código de sucesso para cadastro
      return true;                  //Cadastro bem-sucedido
    } else {
      Serial.print("Falha no cadastro. Código: ");
      Serial.println(httpResponseCode);
    }
  } else {
    Serial.println("WiFi Desconectado");
  }
  return false;  //Caso haja erro ou falha no cadastro
}

void loop() {
  EstadoSistema ultimoEstado = estadoAtual;

  verificarBotao();

  static String ultimoUID = "";  //Para evitar leituras duplicadas do mesmo cartão
  String uid = lerCartao();      //Lê o UID do cartão

  switch (estadoAtual) {
    case REPOUSO:
      lcd.setCursor(0, 0);
      lcd.print("Aguardando CARD:");
      if (uid != "" && uid != ultimoUID) {
        uidLido = uid;          //Atualiza o UID lido
        ultimoUID = uid;        //Atualiza o último UID lido
        estadoAtual = LEITURA;  //Muda o estado para LEITURA
      }
      break;

    case LEITURA:
      lcd.clear();
      lcd.print("Analisando CARD!");
      if (uidLido != "") {
        String status = "Rejeitado";  //Valor padrão de status

        if (verifyUID(uidLido)) {
          status = "Permitido";       //Permitir acesso
          notificaAcessoPermitido();  //Notificação de acesso permitido
        } else {
          notificaAcessoNegado();  //Notificação de acesso negado
        }

        //Exibe o resultado na Serial
        Serial.println("CARD: " + uidLido + " | STATUS: " + status);

        //Envia o log para o servidor
        sendLog(uidLido, status);

        //Exibe no LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CARD: " + uidLido);  //Exibe o UID no LCD
        lcd.setCursor(0, 1);
        lcd.print("Status: " + status);  //Exibe o status no LCD

        delay(1000);
        // Reseta o UID lido e volta ao estado de repouso
        uidLido = "";
        estadoAtual = REPOUSO;
      }
      lcd.clear();
      break;

    case CADASTRO:
      if (ultimoEstado != estadoAtual) {
        lcd.clear();
        lcd.print("Verificando...");
        delay(1000);
        if (!verifyNewRegister()) {
          lcd.setCursor(0, 0);
          lcd.print("Sem Pendentes!");
          notificaAcessoNegado();
          lcd.clear();
          estadoAtual = REPOUSO;
          break;
        }
      }

      lcd.setCursor(0, 0);
      lcd.print("Aproxime o CARD:");
      lcd.setCursor(0, 1);
      lcd.print("Para cadastro!");
      ledAmareloCadastro();
      if (uid != "") {
        lcd.clear();
        lcd.print("CARD: " + uid);
        Serial.println("Cadastrando cartão: " + uid);
        lcd.setCursor(0, 1);
        if (cadastrarCartao(uid)) {
          lcd.print("Cadastrado!");
          Serial.println("Cartão cadastrado com sucesso.");
          estadoAtual = REPOUSO;  // Retorna ao estado de repouso
          digitalWrite(ledAmarelo, LOW);
          notificaAcessoPermitido();
          sendLog(uid, "Cadastrado");
        } else {
          lcd.print("Falha no Cadastro!");
          Serial.println("Falha no cadastro. Tentando novamente.");
          sendLog(uid, "Falha no cadastro");
        }
        delay(500);
        lcd.clear();
      }
      break;

    default:
      estadoAtual = REPOUSO;  // Se nenhum estado válido, retorna ao repouso
      break;
  }

  // Se não houver cartão presente, reseta o último UID
  if (uid == "") {
    ultimoUID = "";  // Limpa o último UID lido
  }
}