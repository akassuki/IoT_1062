#include "Arduino.h"

#define M0_PIN   45
#define M1_PIN   38
#define AUX_PIN  11
#define RX_PIN   12
#define TX_PIN   13

HardwareSerial mySerial(1);

const uint8_t PEER_ADDH = 0x00;
const uint8_t PEER_ADDL = 0x00;
const uint8_t PEER_CHAN = 21;

String rxBuffer = "";

void sendToPeer(const String &text) {
  mySerial.write(PEER_ADDH);
  mySerial.write(PEER_ADDL);
  mySerial.write(PEER_CHAN);
  mySerial.write((const uint8_t*)text.c_str(), text.length());
  mySerial.write('\n');
  Serial.print("[Da gui] "); Serial.println(text);
}

void handleIncoming() {
  while (mySerial.available()) {
    int b = mySerial.read();
    if (b == '\n') {
      Serial.print("[Nhan duoc] ");
      Serial.println(rxBuffer);
      rxBuffer = "";
    } else if (b >= 0 && b != '\r') {
      rxBuffer += (char)b;
    }
  }
}

void handleKeyboardInput() {
  static String inputLine = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputLine.length() > 0) {
        sendToPeer(inputLine);
        inputLine = "";
      }
    } else {
      inputLine += c;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(M0_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);
  pinMode(AUX_PIN, INPUT);

  digitalWrite(M0_PIN, LOW);
  digitalWrite(M1_PIN, LOW); // Normal mode - không đụng gì tới cấu hình nữa
  delay(300);

  mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  Serial.println("=== San sang! Go tin nhan va Enter de gui ===");
}

void loop() {
  handleIncoming();
  handleKeyboardInput();
}