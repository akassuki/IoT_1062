/*
  Test ADC - chi doc 2 kenh AI3 (A13) va AI4 (A14)
  --------------------------------------------------
  ESP32-S3 IO9  (SDA2) -> ADS1015 SDA
  ESP32-S3 IO10 (SCL2) -> ADS1015 SCL
  Dia chi I2C: 0x48

  A13 -> AI3 -> ADS1015 CH2 (dau vao 0-10V, chia ap /2)
  A14 -> AI4 -> ADS1015 CH3 (dau vao 0-10V, chia ap /2)
*/

#include <Wire.h>

#define ADS1015_ADDR   0x48
#define REG_CONVERT    0x00
#define REG_CONFIG     0x01

#define SDA_PIN 9
#define SCL_PIN 10

void writeConfig(uint16_t config) {
  Wire.beginTransmission(ADS1015_ADDR);
  Wire.write(REG_CONFIG);
  Wire.write((config >> 8) & 0xFF);
  Wire.write(config & 0xFF);
  Wire.endTransmission();
}

int16_t readConversion() {
  Wire.beginTransmission(ADS1015_ADDR);
  Wire.write(REG_CONVERT);
  Wire.endTransmission(false);
  Wire.requestFrom(ADS1015_ADDR, 2);
  if (Wire.available() < 2) return 0;
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  int16_t raw = (int16_t)((hi << 8) | lo);
  return raw >> 4;   // ADS1015 = 12 bit, dich phai 4 bit
}

// channel: 2 = AIN2 (AI3/A13), 3 = AIN3 (AI4/A14)
float readChannelVoltage(uint8_t channel) {
  uint16_t mux = 0x4 + channel;   // 110 = AIN2, 111 = AIN3

  uint16_t config = 0x8000            // bat dau chuyen doi
                   | (mux << 12)      // chon kenh single-ended
                   | 0x0000           // PGA = 000 -> FSR = +-6.144V
                   | 0x0100           // MODE = single-shot
                   | 0x0080           // DR = 1600 SPS
                   | 0x0003;          // tat comparator

  writeConfig(config);
  delay(3);   // cho chuyen doi xong

  int16_t raw = readConversion();
  float voltage = raw * (6.144 / 2048.0);
  return voltage;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN, 100000);

  Wire.beginTransmission(ADS1015_ADDR);
  uint8_t err = Wire.endTransmission();
  Serial.print("I2C check ADS1015, err = ");
  Serial.println(err);   // 0 = OK

  Serial.println("San sang doc A13 (AI3) va A14 (AI4)...");
}

void loop() {
  float vAI3 = readChannelVoltage(2);   // CH2 - A13
  float vAI4 = readChannelVoltage(3);   // CH3 - A14

  float vAI3_real = vAI3 * 2.0;   // bu he so chia ap /2
  float vAI4_real = vAI4 * 2.0;

  Serial.println("-------------------------------");
  Serial.print("A13 (AI3): ");
  Serial.print(vAI3, 3);
  Serial.print(" V (tai ADS1015) -> ");
  Serial.print(vAI3_real, 2);
  Serial.println(" V (thuc te dau vao)");

  Serial.print("A14 (AI4): ");
  Serial.print(vAI4, 3);
  Serial.print(" V (tai ADS1015) -> ");
  Serial.print(vAI4_real, 2);
  Serial.println(" V (thuc te dau vao)");

  delay(500);
}