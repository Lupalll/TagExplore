#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SS_PIN  5  // ESP32 pin GPIO5 
#define RST_PIN 27 // ESP32 pin GPIO27 

MFRC522 rfid(SS_PIN, RST_PIN);

const char* ssid = "The Fairphone";
const char* password = "abcdefg23";
const char* serverUrl = "http://172.28.215.186/tagexplore/set_uid.php";

void setup() {
  Serial.begin(9600);
  SPI.begin();      // init SPI bus
  rfid.PCD_Init();  // init MFRC522
  
  delay(4000);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi ;)");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
      if (rfid.PICC_ReadCardSerial()) { // NUID has been read
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        Serial.print("RFID/NFC Tag Type: ");
        Serial.println(rfid.PICC_GetTypeName(piccType));

        // Clear description for each new tag read
        String description = "";
        
        for (int block = 4; block < 128; block++) {
            byte buffer[18];
            byte size = sizeof(buffer);
            MFRC522::StatusCode status = rfid.MIFARE_Read(block, buffer, &size);
            if (status == MFRC522::STATUS_OK) {
              Serial.print("Block ");
              Serial.print(block);
              Serial.println(":");
              // Check if ';' is set in any block
              bool foundSemicolon = false;
              for (byte i = 0; i < size; i++) {
                  char character = buffer[i];
                  if (buffer[i] == ';') {
                      foundSemicolon = true;
                      break;
                  }
                  if (isPrintable(character)) {
                      Serial.print(character);
                      description += character;
                  }
              }
              Serial.println();
              if (foundSemicolon) {
                  Serial.println("Found ';' in block. Stopping reading.");
                  break; // Exit the loop if ';' is found
              }

            } else {
              Serial.println("Reading block failed...");
              break; // Exit the loop if reading fails
            }
        }

        // UID
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String postData = "uid=";
        for (int i = 0; i < rfid.uid.size; i++) {
          if (rfid.uid.uidByte[i] < 0x10) postData += "0";
          postData += String(rfid.uid.uidByte[i], HEX);
        }
        postData += "&description=";
        postData += description;
        Serial.println(postData);
        int httpResponseCode;
        for (int i = 0; i < 3; i++) {
          if (i > 0) delay(1000);
          httpResponseCode = http.POST(postData);
          Serial.print("HTTP Response Code: ");
          Serial.println(httpResponseCode);
          if (httpResponseCode == 200) break;
        }
        http.end();

        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
      }
    }
  }
}
