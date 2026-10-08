#include <SPI.h>
#include <Ethernet.h>

#define W5500_MOSI  1
#define W5500_MISO  16
#define W5500_SCLK  2
#define W5500_CS    42
#define W5500_RST   15
#define W5500_INT   41

byte mac[] = {
  0x02, 0x12, 0x34, 0x56, 0x78, 0x90
};

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("==========================");
  Serial.println(" ESP32-S3 + W5500 TEST");
  Serial.println("==========================");

  // CS
  pinMode(W5500_CS, OUTPUT);
  digitalWrite(W5500_CS, HIGH);

  // Reset W5500
  pinMode(W5500_RST, OUTPUT);

  Serial.println("Reset W5500...");
  digitalWrite(W5500_RST, LOW);
  delay(100);
  digitalWrite(W5500_RST, HIGH);
  delay(500);

  // SPI
  Serial.println("Init SPI...");
  Serial.printf("SCLK = GPIO%d\n", W5500_SCLK);
  Serial.printf("MISO = GPIO%d\n", W5500_MISO);
  Serial.printf("MOSI = GPIO%d\n", W5500_MOSI);
  Serial.printf("CS   = GPIO%d\n", W5500_CS);

  SPI.begin(
    W5500_SCLK,
    W5500_MISO,
    W5500_MOSI,
    W5500_CS
  );

  Ethernet.init(W5500_CS);

  Serial.println();
  Serial.println("Checking W5500...");

  // Thử DHCP
  Ethernet.begin(mac);

  delay(1000);

  // Hardware status
  Serial.print("Hardware: ");

  switch (Ethernet.hardwareStatus()) {

    case EthernetW5500:
      Serial.println("W5500 FOUND!");
      break;

    case EthernetW5200:
      Serial.println("W5200");
      break;

    case EthernetW5100:
      Serial.println("W5100");
      break;

    default:
      Serial.println("NOT FOUND!");
      break;
  }

  // Link
  Serial.print("Cable: ");

  switch (Ethernet.linkStatus()) {

    case LinkON:
      Serial.println("CONNECTED");
      break;

    case LinkOFF:
      Serial.println("DISCONNECTED");
      break;

    default:
      Serial.println("UNKNOWN");
      break;
  }

  Serial.println();

  Serial.print("IP      : ");
  Serial.println(Ethernet.localIP());

  Serial.print("Gateway : ");
  Serial.println(Ethernet.gatewayIP());

  Serial.print("Subnet  : ");
  Serial.println(Ethernet.subnetMask());

  Serial.print("DNS     : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("==========================");
}

void loop() {

  static unsigned long previous = 0;

  if (millis() - previous >= 2000) {
    previous = millis();

    Serial.print("W5500: ");

    if (Ethernet.hardwareStatus() == EthernetW5500)
      Serial.print("FOUND");
    else
      Serial.print("NOT FOUND");

    Serial.print(" | Cable: ");

    if (Ethernet.linkStatus() == LinkON)
      Serial.print("CONNECTED");
    else
      Serial.print("DISCONNECTED");

    Serial.print(" | IP: ");
    Serial.println(Ethernet.localIP());
  }
}