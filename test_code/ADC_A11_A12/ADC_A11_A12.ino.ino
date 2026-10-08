/*
  Test ADC - chi doc 2 kenh AI1 (A11) va AI2 (A12) - dong 4-20mA
  ------------------------------------------------------------------
  ESP32-S3 IO9  (SDA2) -> ADS1015 SDA
  ESP32-S3 IO10 (SCL2) -> ADS1015 SCL
  Dia chi I2C: 0x48

  A12 -> AI2 -> ADS1015 CH0 (4-20mA, shunt 150R - R60)
  A11 -> AI1 -> ADS1015 CH1 (4-20mA, shunt 150R - R59)

  Cong thuc: I(mA) = V(doc duoc) / R_shunt * 1000
  Dai ap ky vong: 0.6V - 3V tuong ung 4-20mA (voi shunt = 150R)
*/

#include <Wire.h>

#define ADS1015_ADDR   0x48
#define REG_CONVERT    0x00
#define REG_CONFIG     0x01

#define SDA_PIN 9
#define SCL_PIN 10

// Gia tri dien tro shunt thuc te - DO LAI bang dong ho de chinh xac
// (tai lieu ghi R59/R60 dao dong quanh 150R)
#define R_SHUNT_AI1   150.0
#define R_SHUNT_AI2   150.0

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

// channel: 0 = AIN0 (AI2/A12), 1 = AIN1 (AI1/A11)
float readChannelVoltage(uint8_t channel) {
  uint16_t mux = 0x4 + channel;   // 100 = AIN0, 101 = AIN1

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

  Serial.println("San sang doc A11 (AI1) va A12 (AI2)...");
}

void loop() {
  float vAI2 = readChannelVoltage(0);   // CH0 - A12
  float vAI1 = readChannelVoltage(1);   // CH1 - A11

  float iAI2_mA = (vAI2 / R_SHUNT_AI2) * 1000.0;
  float iAI1_mA = (vAI1 / R_SHUNT_AI1) * 1000.0;

  Serial.println("-------------------------------");
  Serial.print("A12 (AI2): ");
  Serial.print(vAI2, 3);
  Serial.print(" V -> ");
  Serial.print(iAI2_mA, 2);
  Serial.println(" mA");

  Serial.print("A11 (AI1): ");
  Serial.print(vAI1, 3);
  Serial.print(" V -> ");
  Serial.print(iAI1_mA, 2);
  Serial.println(" mA");

  delay(500);
}