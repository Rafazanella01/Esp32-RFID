Modulo de RFID: RC522

pinos conectados:

SDA → D4: Serve para selecionar o chip (Chip Select), permitindo que o ESP8266 se comunique com o RC522.

SCK → D5: É o clock que sincroniza a troca de dados entre o ESP8266 e o módulo.

MOSI → D7: Envia dados do ESP8266 para o RC522 (por exemplo, comandos).

MISO → D6: Recebe dados do RC522 para o ESP8266 (como o ID do cartão).

RST → D3: Usado para resetar o módulo RC522, útil em caso de falhas.

VCC  → 3.3V

GND  → GND

====================================================================

saída de dados: ou vamos usar um display lcd 16x2 com um modulo I2C acoplado ou o display oled SSD-1306 que já é acoplado no esp8266