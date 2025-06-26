#include "../camera_handler.h"
#include "../config.h"

// Create instance of camera handler
CameraHandler cameraHandler;

// Global variables
unsigned long lastDetectionTime = 0;
bool isProcessing = false;
int detectionCount = 0;

void setup() {
    Serial.begin(115200);  // Using direct value since SERIAL_BAUD_RATE might not be defined
    while (!Serial) {
        ; // Wait for serial port to connect
    }
    
    Serial.println("Initializing system...");

    // Initialize camera
    if (!cameraHandler.begin()) {
        Serial.println("Camera initialization failed!");
        while (1) {
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            delay(100);
            digitalWrite(BUILTIN_LED_PIN, LOW);
            delay(100);
        }
    }

    Serial.println("Camera initialized successfully!");
    Serial.println("Starting bottle detection...");
}

void loop() {
    if (!isProcessing) {
        float confidence = 0.0;
        bool bottleDetected = cameraHandler.detectBottle(confidence);
        
        if (bottleDetected) {
            Serial.printf("Bottle detected! Confidence: %.2f%%\n", confidence * 100);
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            detectionCount++;
            lastDetectionTime = millis();
            isProcessing = true;
        } else {
            digitalWrite(BUILTIN_LED_PIN, LOW);
            if (millis() - lastDetectionTime > 5000) {  // Reset after 5 seconds of no detection
                detectionCount = 0;
                isProcessing = false;
            }
        }
    } else {
        // Add a small delay to prevent too rapid detection
        delay(1000);
        isProcessing = false;
    }
} 