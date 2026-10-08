#include "Arduino.h"

#define M0_PIN   45
#define M1_PIN   38
#define AUX_PIN  11
#define RX_PIN   12
#define TX_PIN   13

HardwareSerial mySerial(1);

// ==== SỬA CÁC GIÁ TRỊ NÀY THEO Ý MUỐN TRƯỚC KHI NẠP ====
const uint8_t NEW_ADDH   = 0x02;   // Byte cao của địa chỉ
const uint8_t NEW_ADDL   = 0x00;   // Byte thấp của địa chỉ
const uint8_t NEW_CHAN   = 21;     // Kênh (Channel)
const uint8_t NEW_SPED   = 0x1A;   // UART 9600bps, 8N1, AirRate 2.4kbps
const uint8_t NEW_OPTION = 0xC4;   // Fixed=Enable, FEC=Enable, Power=20dBm, IO=PushPull, Wakeup=250ms
// =========================================================

void waitAuxHigh(uint32_t timeoutMs = 3000) {
  uint32_t start = millis();
  while (digitalRead(AUX_PIN) == LOW) {
    if (millis() - start > timeoutMs) break;
    delay(5);
  }
  delay(50);
}

void enterConfigMode() {
  digitalWrite(M0_PIN, HIGH);
  digitalWrite(M1_PIN, HIGH); // Config mode (theo datasheet EBYTE)
  waitAuxHigh();
  delay(200);
  while (mySerial.available()) mySerial.read(); // xả buffer cũ
}

void enterNormalMode() {
  digitalWrite(M0_PIN, LOW);
  digitalWrite(M1_PIN, LOW);
  waitAuxHigh();
  delay(200);
}

void printConfig(const char* label, uint8_t* buf, int n) {
  Serial.print("== "); Serial.print(label); Serial.println(" ==");
  Serial.print("RAW: ");
  for (int i = 0; i < n; i++) {
    Serial.print("0x"); Serial.print(buf[i], HEX); Serial.print(" ");
  }
  Serial.println();
  if (n >= 6) {
    Serial.print("ADDH: 0x");   Serial.println(buf[1], HEX);
    Serial.print("ADDL: 0x");   Serial.println(buf[2], HEX);
    Serial.print("SPED: 0x");   Serial.println(buf[3], HEX);
    Serial.print("CHAN: ");     Serial.println(buf[4], DEC);
    Serial.print("OPTION: 0x"); Serial.println(buf[5], HEX);
  } else {
    Serial.println("!!! Khong nhan duoc phan hoi hop le !!!");
  }
  Serial.println("=====================");
}

void readConfig(const char* label) {
  enterConfigMode();

  mySerial.write((uint8_t)0xC1);
  mySerial.write((uint8_t)0xC1);
  mySerial.write((uint8_t)0xC1);
  delay(300);

  uint8_t buf[8]; int n = 0;
  while (mySerial.available() && n < 8) buf[n++] = mySerial.read();
  printConfig(label, buf, n);

  enterNormalMode();
}

void writeConfig() {
  enterConfigMode();

  mySerial.write((uint8_t)0xC0);
  mySerial.write(NEW_ADDH);
  mySerial.write(NEW_ADDL);
  mySerial.write(NEW_SPED);
  mySerial.write(NEW_CHAN);
  mySerial.write(NEW_OPTION);
  delay(300);

  Serial.print("Phan hoi ghi cau hinh: ");
  uint8_t buf[8]; int n = 0;
  while (mySerial.available() && n < 8) buf[n++] = mySerial.read();
  for (int i = 0; i < n; i++) {
    Serial.print("0x"); Serial.print(buf[i], HEX); Serial.print(" ");
  }
  Serial.println();
  if (n == 0) {
    Serial.println("!!! Khong co phan hoi - kiem tra lai wiring !!!");
  }

  enterNormalMode();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(M0_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);
  pinMode(AUX_PIN, INPUT);

  mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  enterNormalMode(); // đảm bảo về Normal trước khi bắt đầu

  Serial.println("\n########## BAT DAU CAU HINH MODULE ##########\n");

  readConfig("TRUOC KHI SUA");
  writeConfig();
  readConfig("SAU KHI SUA (xac minh)");

  Serial.println("\n########## HOAN TAT ##########");
  Serial.println("Neu SAU KHI SUA hien thi dung nhu NEW_ADDH/ADDL/CHAN/OPTION ban da dat,");
  Serial.println("thi cau hinh da duoc luu thanh cong. Ban co the nap sketch gui/nhan gio.");
}

void loop() {
  // Không làm gì thêm - chỉ chạy 1 lần trong setup()
}