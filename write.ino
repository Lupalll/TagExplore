#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600); // baud 9600
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Scan an NFC tag to write text...");
}

int calculateBlocksNeeded(int stringLength) {
    const int blockSize = 4;
    int blocksNeeded = floor(stringLength / blockSize);
    if (stringLength % blockSize != 0) {
        blocksNeeded += 1;
    }
    return blocksNeeded;
}

void clearPreviousData() {
  // Clear previous data by writing blank data to all blocks
  for (int i = 4; i < 20; i++) { // Assuming you have 16 blocks for data storage
    byte blankData[4] = {0x00, 0x00, 0x00, 0x00}; // Blank data
    rfid.MIFARE_Ultralight_Write(i, blankData, 4);
  }
}

String data = "Default"; // Data to write on NFC tag

void loop() {
  // Check for input from Serial Monitor
  if (Serial.available() > 0) {
    // Read the input
    data = Serial.readString();
    
    // Print the updated data String
    Serial.print("Updated data: '");
    Serial.print(data);
    Serial.print("' => ");
    Serial.println(data.length());
  }
  
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      // Print card UID
      Serial.print("Card UID:");
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();
    
      // Clear previous data
      clearPreviousData();
      
      // Prepare data to write
      MFRC522::StatusCode status;

      int dataLength = data.length();
      int numBlocks = calculateBlocksNeeded(dataLength);

      for (int i = 0; i < numBlocks; i++) {
        String blockString = data.substring(i*4, (i*4)+4);
        
        // Ignore newline characters ('\n') and equal sign ('=')
        blockString.replace("\n", "");
        
        Serial.print(data.substring(i*4, (i*4)+4));
        Serial.println();
        
        if (i == calculateBlocksNeeded(dataLength) - 1) {
          if (blockString.length() == 4) {
              String endData = ";";
              String endString = endData.substring(0, 4);
              status = rfid.MIFARE_Ultralight_Write(5+i, (byte *)endString.c_str(), 4);
          } else {
              blockString += ";";
          }
        }
        
        status = rfid.MIFARE_Ultralight_Write(4+i, (byte *)blockString.c_str(), 4);
      }
    
      if (status == MFRC522::STATUS_OK) {
        Serial.println("Write successful!");
      } else {
        Serial.println("Write failed...");
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}
