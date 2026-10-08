/*
  Bat lan luot LED IO3 -> IO4 -> IO5 qua MCP23008 (I2C)
  --------------------------------------------------------
  ESP32-S3 IO9  (SDA2) -> MCP23008 SDA
  ESP32-S3 IO10 (SCL2) -> MCP23008 SCL
  Dia chi I2C: 0x20

  Phan cong (theo so do):
    GP5 -> IO3 -> LED RED
    GP6 -> IO4 -> LED YELLOW
    GP7 -> IO5 -> LED GREEN
*/

#include <Wire.h>

#define MCP23008_ADDR 0x20

#define IODIR 0x00
#define GPPU  0x06
#define OLAT  0x0A

#define SDA_PIN 9
#define SCL_PIN 10

void mcpWriteReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MCP23008_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN, 100000);

  Wire.beginTransmission(MCP23008_ADDR);
  uint8_t err = Wire.endTransmission();
  if (err == 0) {
    Serial.println("Da tim thay MCP23008 tai dia chi 0x20");
  } else {
    Serial.print("KHONG tim thay MCP23008! Loi I2C: ");
    Serial.println(err);
  }

  // GP0, GP1 = input | GP2..GP7 = output (chi can GP5,6,7 cho LED)
  mcpWriteReg(IODIR, 0b00000011);
  mcpWriteReg(GPPU, 0b00000011);

  // Tat het LED ban dau
  mcpWriteReg(OLAT, 0b00000000);
}

void loop() {
  // Bat IO3 (LED RED)
  Serial.println("Bat IO3 - LED RED");
  mcpWriteReg(OLAT, 0b00100000);  // GP5 = 1
  delay(1000);

  // Bat IO4 (LED YELLOW)
  Serial.println("Bat IO4 - LED YELLOW");
  mcpWriteReg(OLAT, 0b01000000);  // GP6 = 1
  delay(1000);

  // Bat IO5 (LED GREEN)
  Serial.println("Bat IO5 - LED GREEN");
  mcpWriteReg(OLAT, 0b10000000);  // GP7 = 1
  delay(1000);

  // Tat het truoc khi lap lai
  mcpWriteReg(OLAT, 0b00000000);
  delay(500);
}