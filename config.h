#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Camera Settings
#define CAMERA_MODEL_AI_THINKER // This is for ESP32-CAM
#define FRAME_SIZE FRAMESIZE_96X96
#define PIXEL_FORMAT PIXFORMAT_RGB565
#define FRAME_BUFFER_COUNT 2

// Camera Pins (ESP32-CAM Default)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Built-in LED Pin (ESP32-CAM)
#define BUILTIN_LED_PIN 4  // GPIO 4 is the built-in LED on most ESP32-CAM boards

// Network Configuration
#define WIFI_SSID "Eri1"
#define WIFI_PASSWORD "blackpanter"
#define SERVER_URL "http://your-server-url.com/api"
#define API_KEY "your_api_key"

// Edge Impulse Model Configuration
#define MIN_BOTTLE_CONFIDENCE 0.6  // Minimum confidence threshold for bottle detection
#define MODEL_INPUT_WIDTH 96
#define MODEL_INPUT_HEIGHT 96

// System Configuration
#define SERIAL_BAUD_RATE 115200
#define DETECTION_TIMEOUT 30000  // 30 seconds timeout for bottle detection
#define LED_BLINK_DURATION 100   // milliseconds
#define MEMORY_ALLOCATION_SIZE (96 * 96 * 3) // RGB buffer size

// Debug Configuration
#define DEBUG_MODE 1  // Set to 0 to disable debug prints
#define DEBUG_PRINT(x) if(DEBUG_MODE) Serial.println(x)
#define DEBUG_PRINTF(x, ...) if(DEBUG_MODE) Serial.printf(x, __VA_ARGS__)

#endif 