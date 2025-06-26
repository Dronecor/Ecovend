#ifndef RFID_HANDLER_H
#define RFID_HANDLER_H

#include <MFRC522.h>
#include <SPI.h>
#include "config.h"

class RFIDHandler {
private:
    MFRC522 rfid;
    bool isCardPresent;
    String currentUserID;
    
    // Registered RFID tags (you should implement proper storage)
    const String AUTHORIZED_TAGS[3] = {
        "AA BB CC DD",  // Replace with your actual tag IDs
        "11 22 33 44",
        "55 66 77 88"
    };

public:
    RFIDHandler() : rfid(RFID_SS_PIN, RFID_RST_PIN), isCardPresent(false) {
        SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN);
        rfid.PCD_Init();
        pinMode(BUILTIN_LED_PIN, OUTPUT);
        digitalWrite(BUILTIN_LED_PIN, LOW);
    }

    void begin() {
        rfid.PCD_Init();
    }

    bool checkCard() {
        if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
            return false;
        }

        String tagID = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            tagID += (rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
            tagID += String(rfid.uid.uidByte[i], HEX);
        }
        tagID.toUpperCase();
        tagID = tagID.substring(1);

        bool isAuthorized = false;
        for (const String& authorizedTag : AUTHORIZED_TAGS) {
            if (tagID.equals(authorizedTag)) {
                isAuthorized = true;
                currentUserID = tagID;
                break;
            }
        }

        if (isAuthorized) {
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            isCardPresent = true;
        } else {
            blinkError();
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();

        return isAuthorized;
    }

    void blinkError() {
        for (int i = 0; i < 3; i++) {
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            delay(200);
            digitalWrite(BUILTIN_LED_PIN, LOW);
            delay(200);
        }
    }

    String getCurrentUserID() {
        return currentUserID;
    }

    void resetCard() {
        isCardPresent = false;
        currentUserID = "";
        digitalWrite(BUILTIN_LED_PIN, LOW);
    }

    bool isAuthenticated() {
        return isCardPresent;
    }
};

#endif 