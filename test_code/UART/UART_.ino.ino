/*
  Giao tiep UART1 tren ESP32-S3 qua IC chon kenh 74HC4052
  - Ban go noi dung tren Serial Monitor (qua USB)
  - ESP32 se gui noi dung do ra UART1 (IO17 = TX)
  - Neu ban dang noi TX-RX voi nhau (loopback) thi se doc lai
    duoc chinh du lieu vua gui tren UART1 (IO18 = RX)
*/

#include <Arduino.h>

// Chan UART1 cua ESP32-S3 (noi vao chan common 1Z/2Z cua 74HC4052)
#define UART1_RX_PIN   18   // IO18 -> 1Z (RX chung)
#define UART1_TX_PIN   17   // IO17 -> 2Z (TX chung)

// Chan chon kenh (select) cua 74HC4052
#define MUX_S0_PIN     5    // IO5 -> S0
#define MUX_S1_PIN     4    // IO4 -> S1

#define UART_BAUD     115200

// port = 1 -> chon cong UART1 (RXD1/TXD1)
// port = 2 -> chon cong UART2 (RXD2/TXD2)
void selectUartPort(uint8_t port) {
  if (port == 1) {
    digitalWrite(MUX_S0_PIN, LOW);
    digitalWrite(MUX_S1_PIN, LOW);
  } else {
    digitalWrite(MUX_S0_PIN, HIGH);
    digitalWrite(MUX_S1_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);        // Serial debug qua USB (ban go vao day)
  delay(1000);

  pinMode(MUX_S0_PIN, OUTPUT);
  pinMode(MUX_S1_PIN, OUTPUT);

  selectUartPort(2);            // Chon cong UART1

  Serial1.begin(UART_BAUD, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);

  Serial.println("San sang! Go noi dung roi nhan Enter de gui qua UART.");
}

void loop() {
  // Doc du lieu ban go tu ban phim (Serial Monitor) va gui ra UART1
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      Serial1.println(input);
      Serial.print("-> Da gui qua UAR: ");
      Serial.println(input);
    }
  }

  // Doc du lieu nhan duoc tu UART1 va in ra man hinh
  while (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();
    if (data.length() > 0) {
      Serial.print("<- Nhan duoc tu UART: ");
      Serial.println(data);
    }
  }
}