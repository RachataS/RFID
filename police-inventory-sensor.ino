#include <SPI.h>
#include "RFID.h"

#define SCK_PIN 12
#define MISO_PIN 11
#define MOSI_PIN 13
#define SS_PIN 41
#define RST_PIN 9
#define RELAY_PIN 39

RFID rfid(SS_PIN, RST_PIN);

int buzzPin = 3;

// Setup variables:
int serNum[13] = { 0xB4, 0xDA, 0x4E, 0xD3, 0xF3, 0x37, 0x08, 0x80, 0xF0, 0x9E, 0xFC, 0x3F };

unsigned long tapDelayTime = 20000; // Delay time in milliseconds
unsigned long tapStartTime = 0; // Variable to store the start time of the delay
bool tapDetected = false; // Flag to indicate if a tap is detected

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.init();
  Serial.println("Init RFID...");
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      if (compareSerial()) {
        digitalWrite(RELAY_PIN, HIGH);

        if (!tapDetected) {
          // First tap, start the delay
          tapStartTime = millis();
          tapDetected = true;
        } else {
          // Second tap, reset the delay time
          tapStartTime = millis();
        }

        while (millis() - tapStartTime < tapDelayTime) {
          if (rfid.isCard()) {
            if (rfid.readCardSerial()) {
              if (compareSerial()) {
                // Reset the delay start time
                tapStartTime = millis();
              }
            }
          }

          unsigned long elapsedTime = millis() - tapStartTime;
          Serial.println(elapsedTime);
          delay(1);
        }

        digitalWrite(RELAY_PIN, LOW);
        tapDetected = false;

        showID();
      } else {
        Serial.println("Card does not match");
      }
    }
  }

  rfid.halt();
}

bool compareSerial() {
  bool result = true;
  Serial.println("Comparing card serial numbers...");

  for (int i = 0; i < 5; i++) {
    if (rfid.serNum[i] != serNum[i]) {
      result = false;
      break;
    }
  }
  delay(300);
  return result;
}

void showID() {
  Serial.println("\nCard found");

  Serial.print("Hex: ");
  for (int i = 0; i < 12; i++) {
    Serial.print(rfid.serNum[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  Serial.println("--------------------------------------");
  delay(700);
}
