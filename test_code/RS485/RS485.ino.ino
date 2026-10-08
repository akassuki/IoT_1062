/*
  Test RS485 qua MAX3485 tren ESP32-S3 - co in ca HEX va ASCII
  de doi chieu voi script Python (COM18, baud 9600)
  --------------------------------------------------------------
  ESP32-S3 GPIO37 (TX) -> DI cua MAX3485 (tu dong dieu khien DE/RE)
  ESP32-S3 GPIO36 (RX) <- RO cua MAX3485
*/

#include <Arduino.h>

#define RS485_TX_PIN   37
#define RS485_RX_PIN   36
#define RS485_BAUD     9600   // phai khop voi BAUDRATE ben Python

void printHex(const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);        // Serial debug qua USB
  delay(1000);

  Serial1.begin(RS485_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

  Serial.println("San sang! Go noi dung roi Enter de gui qua RS485.");
}

void loop() {
  // Gui du lieu tu ban phim (Serial Monitor) ra RS485
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      String toSend = input + "\r\n";   // khop dinh dang voi script Python
      Serial1.print(toSend);

      Serial.print("TX ASCII -> ");
      Serial.println(input);
      Serial.print("TX HEX   -> ");
      printHex((const uint8_t*)toSend.c_str(), toSend.length());
    }
  }

  // Nhan du lieu tu RS485 (RO -> GPIO36), in ca ASCII va HEX
  if (Serial1.available()) {
    uint8_t buf[128];
    size_t len = 0;
    delay(20);   // cho du lieu ve du (giong timeout ben Python)
    while (Serial1.available() && len < sizeof(buf)) {
      buf[len++] = Serial1.read();
    }

    Serial.print("RX HEX   <- ");
    printHex(buf, len);

    Serial.print("RX ASCII <- ");
    for (size_t i = 0; i < len; i++) Serial.print((char)buf[i]);
    Serial.println();
  }
}