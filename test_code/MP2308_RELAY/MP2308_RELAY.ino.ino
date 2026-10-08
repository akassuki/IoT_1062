/*
  Test dieu khien Relay qua MCP23008 (I2C)
  --------------------------------------------
  ESP32-S3 IO9  (SDA2) -> MCP23008 SDA
  ESP32-S3 IO10 (SCL2) -> MCP23008 SCL
  Dia chi I2C: 0x20

  Phan cong (theo so do I2C):
    GP2 -> RLY1 (qua Q3, dieu khien Relay 1)
    GP3 -> RLY2 (qua Q4, dieu khien Relay 2)
    GP4 -> RLY3 (qua Q5, dieu khien Relay 3)
*/

#include <Wire.h>

#define MCP23008_ADDR 0x20

#define IODIR    0x00
#define GPPU     0x06
#define GPIO_REG 0x09
#define OLAT     0x0A

#define SDA_PIN 9
#define SCL_PIN 10

void mcpWriteReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MCP23008_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t mcpReadReg(uint8_t reg) {
  Wire.beginTransmission(MCP23008_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MCP23008_ADDR, 1);
  return Wire.available() ? Wire.read() : 0xFF;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN, 100000);

  Wire.beginTransmission(MCP23008_ADDR);
  uint8_t err = Wire.endTransmission();
  Serial.print("I2C check, err = ");
  Serial.println(err);   // 0 = OK

  // GP0,GP1 = input | GP2..GP7 = output
  mcpWriteReg(IODIR, 0b00000011);
  mcpWriteReg(GPPU, 0b00000011);

  // Tat het relay ban dau
  mcpWriteReg(OLAT, 0b00000000);

  Serial.println("San sang test Relay...");
}

void loop() {
  // Bat lan luot RLY1 (GP2), RLY2 (GP3), RLY3 (GP4)
  for (int bit = 2; bit <= 4; bit++) {
    uint8_t val = (1 << bit);
    mcpWriteReg(OLAT, val);

    uint8_t readBack = mcpReadReg(GPIO_REG);

    const char* name = (bit == 2) ? "RLY1" : (bit == 3) ? "RLY2" : "RLY3";
    Serial.print("Bat ");
    Serial.print(name);
    Serial.print(" | Ghi OLAT = 0b");
    Serial.print(val, BIN);
    Serial.print(" | Doc lai GPIO = 0b");
    Serial.println(readBack, BIN);

    delay(1500);
  }

  // Tat het relay truoc khi lap lai
  mcpWriteReg(OLAT, 0b00000000);
  Serial.println("Tat het relay");
  delay(1000);
}