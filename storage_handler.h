#ifndef STORAGE_HANDLER_H
#define STORAGE_HANDLER_H

#include <SD.h>
#include <SPI.h>
#include "config.h"

class StorageHandler {
private:
    File dataFile;
    bool sdInitialized;

public:
    StorageHandler() : sdInitialized(false) {}

    bool begin() {
        if (!SD.begin()) {
            Serial.println("SD Card initialization failed!");
            return false;
        }
        sdInitialized = true;
        
        // Create CSV file with headers if it doesn't exist
        if (!SD.exists(CSV_FILENAME)) {
            dataFile = SD.open(CSV_FILENAME, FILE_WRITE);
            if (dataFile) {
                dataFile.println("timestamp,user_id,bottles_count");
                dataFile.close();
            } else {
                Serial.println("Error creating CSV file!");
                return false;
            }
        }
        return true;
    }

    bool logDeposit(const String &userId, int bottleCount) {
        if (!sdInitialized) {
            return false;
        }

        dataFile = SD.open(CSV_FILENAME, FILE_APPEND);
        if (!dataFile) {
            Serial.println("Failed to open file for writing");
            return false;
        }

        // Get current timestamp (you might want to add RTC or NTP functionality)
        unsigned long timestamp = millis();

        // Write data to CSV
        String dataString = String(timestamp) + "," + userId + "," + String(bottleCount);
        dataFile.println(dataString);
        dataFile.close();
        
        return true;
    }

    String getLastEntry() {
        if (!sdInitialized) {
            return "";
        }

        dataFile = SD.open(CSV_FILENAME, FILE_READ);
        if (!dataFile) {
            return "";
        }

        String lastLine = "";
        while (dataFile.available()) {
            String line = dataFile.readStringUntil('\n');
            if (line.length() > 0) {
                lastLine = line;
            }
        }
        dataFile.close();
        return lastLine;
    }

    bool isInitialized() {
        return sdInitialized;
    }
};

#endif 