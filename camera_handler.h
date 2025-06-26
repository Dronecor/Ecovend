#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include "esp_camera.h"
#include "edge-impulse-model/Erioluwa-project-1_inferencing/src/Erioluwa-project-1_inferencing.h"
#include "config.h"
#include "esp_system.h"
#include "esp_heap_caps.h"

class CameraHandler {
private:
    bool cameraInitialized;
    
    bool initCamera() {
        camera_config_t config;
        config.ledc_channel = LEDC_CHANNEL_0;
        config.ledc_timer = LEDC_TIMER_0;
        config.pin_d0 = Y2_GPIO_NUM;
        config.pin_d1 = Y3_GPIO_NUM;
        config.pin_d2 = Y4_GPIO_NUM;
        config.pin_d3 = Y5_GPIO_NUM;
        config.pin_d4 = Y6_GPIO_NUM;
        config.pin_d5 = Y7_GPIO_NUM;
        config.pin_d6 = Y8_GPIO_NUM;
        config.pin_d7 = Y9_GPIO_NUM;
        config.pin_xclk = XCLK_GPIO_NUM;
        config.pin_pclk = PCLK_GPIO_NUM;
        config.pin_vsync = VSYNC_GPIO_NUM;
        config.pin_href = HREF_GPIO_NUM;
        config.pin_sscb_sda = SIOD_GPIO_NUM;
        config.pin_sscb_scl = SIOC_GPIO_NUM;
        config.pin_pwdn = PWDN_GPIO_NUM;
        config.pin_reset = RESET_GPIO_NUM;
        config.xclk_freq_hz = 20000000;
        config.pixel_format = PIXFORMAT_RGB565;
        config.frame_size = FRAMESIZE_96X96;
        config.jpeg_quality = 12;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;

        esp_err_t err = esp_camera_init(&config);
        if (err != ESP_OK) {
            Serial.printf("Camera init failed with error 0x%x", err);
            return false;
        }

        sensor_t * s = esp_camera_sensor_get();
        if (s) {
            s->set_brightness(s, 1);
            s->set_contrast(s, 1);
            s->set_saturation(s, 1);
            s->set_special_effect(s, 0);
            s->set_whitebal(s, 1);
            s->set_awb_gain(s, 1);
            s->set_wb_mode(s, 0);
            s->set_exposure_ctrl(s, 1);
            s->set_aec2(s, 0);
            s->set_gain_ctrl(s, 1);
            s->set_bpc(s, 0);
            s->set_wpc(s, 1);
            s->set_raw_gma(s, 1);
            s->set_lenc(s, 1);
            s->set_hmirror(s, 0);
            s->set_vflip(s, 0);
            s->set_dcw(s, 1);
            s->set_colorbar(s, 0);
        }
        
        return true;
    }

    bool convertFrameToEIFormat(camera_fb_t *fb, float *image_data) {
        if (!fb || !image_data) return false;
        
        uint16_t *rgb565_data = (uint16_t *)fb->buf;
        const int pixel_count = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
        
        for (int i = 0; i < pixel_count; i++) {
            uint16_t pixel = rgb565_data[i];
            
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = pixel & 0x1F;
            
            r = (r << 3) | (r >> 2);
            g = (g << 2) | (g >> 4);
            b = (b << 3) | (b >> 2);
            
            image_data[i] = static_cast<float>(r) / 255.0f;
        }
        
        return true;
    }

public:
    CameraHandler() : cameraInitialized(false) {
        pinMode(BUILTIN_LED_PIN, OUTPUT);
        digitalWrite(BUILTIN_LED_PIN, LOW);
    }

    bool begin() {
        cameraInitialized = initCamera();
        if (!cameraInitialized) {
            Serial.println("Camera initialization failed");
            return false;
        }
        
        if (psramFound()) {
            Serial.println("PSRAM found and initialized");
        } else {
            Serial.println("Warning: No PSRAM found, performance may be limited");
        }
        
        return true;
    }

    bool detectBottle(float &confidence) {
        if (!cameraInitialized) {
            return false;
        }

        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            return false;
        }

        float *image_data = (float*)heap_caps_malloc(EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT * sizeof(float), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!image_data) {
            Serial.println("Failed to allocate memory for image data");
            esp_camera_fb_return(fb);
            return false;
        }

        if (!convertFrameToEIFormat(fb, image_data)) {
            Serial.println("Failed to convert frame to EI format");
            heap_caps_free(image_data);
            esp_camera_fb_return(fb);
            return false;
        }

        ei_impulse_result_t result = { 0 };
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
        signal.get_data = [image_data](size_t offset, size_t length, float *out_ptr) -> int {
            memcpy(out_ptr, &image_data[offset], length * sizeof(float));
            return 0;
        };

        EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
        
        heap_caps_free(image_data);
        esp_camera_fb_return(fb);

        if (err != EI_IMPULSE_OK) {
            Serial.printf("Edge Impulse classification failed: %d\n", err);
            return false;
        }

        float maxConfidence = 0;
        bool isBottle = false;
        
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            if (result.classification[ix].value > maxConfidence) {
                maxConfidence = result.classification[ix].value;
                isBottle = String(result.classification[ix].label) == "bottle";
            }
        }

        confidence = maxConfidence;
        return isBottle && (confidence >= MIN_BOTTLE_CONFIDENCE);
    }

    void indicateUnknownObject() {
        for (int i = 0; i < 3; i++) {
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            delay(LED_BLINK_DURATION / 2);
            digitalWrite(BUILTIN_LED_PIN, LOW);
            delay(LED_BLINK_DURATION / 2);
        }
    }
};

#endif 