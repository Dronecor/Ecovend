#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include <WiFi.h>
#include <WiFiClient.h>
#include "config.h"

class NetworkHandler {
private:
    bool wifiConnected;
    WiFiClient client;

    bool connectToWiFi() {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("\nWiFi connection failed!");
            return false;
        }

        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }

public:
    NetworkHandler() : wifiConnected(false) {}

    bool begin() {
        wifiConnected = connectToWiFi();
        return wifiConnected;
    }

    bool sendDataToServer(const String &userId, int bottleCount) {
        if (!wifiConnected) {
            Serial.println("WiFi not connected!");
            return false;
        }

        // Connect to Raspberry Pi server
        if (!client.connect(SERVER_IP, SERVER_PORT)) {
            Serial.println("Connection to server failed!");
            return false;
        }

        // Format data as JSON
        String jsonData = "{\"user_id\":\"" + userId + "\",\"bottle_count\":" + String(bottleCount) + "}";
        
        // Send HTTP POST request
        client.println("POST /deposit HTTP/1.1");
        client.println("Host: " + String(SERVER_IP));
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(jsonData.length());
        client.println();
        client.println(jsonData);

        // Wait for response
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                Serial.println("Server response timeout!");
                client.stop();
                return false;
            }
        }

        // Read response
        while (client.available()) {
            String line = client.readStringUntil('\r');
            Serial.print(line);
        }

        client.stop();
        return true;
    }

    bool isConnected() {
        return wifiConnected && (WiFi.status() == WL_CONNECTED);
    }

    void reconnect() {
        if (!isConnected()) {
            wifiConnected = connectToWiFi();
        }
    }
};

#endif 