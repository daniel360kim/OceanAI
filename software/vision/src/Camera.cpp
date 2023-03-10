#include "Camera.h"

#include <Arduino.h>

#define CAMERA_MODEL_ESP_EYE // Has PSRAM

namespace Camera
{
    constexpr int PWDN_GPIO_NUM = 32;
    constexpr int RESET_GPIO_NUM = -1;
    constexpr int XCLK_GPIO_NUM = 0;
    constexpr int SIOD_GPIO_NUM = 26;
    constexpr int SIOC_GPIO_NUM = 27;
    constexpr int Y9_GPIO_NUM = 35;
    constexpr int Y8_GPIO_NUM = 34;
    constexpr int Y7_GPIO_NUM = 39;
    constexpr int Y6_GPIO_NUM = 36;
    constexpr int Y5_GPIO_NUM = 21;
    constexpr int Y4_GPIO_NUM = 19;
    constexpr int Y3_GPIO_NUM = 18;
    constexpr int Y2_GPIO_NUM = 5;
    constexpr int VSYNC_GPIO_NUM = 25;
    constexpr int HREF_GPIO_NUM = 23;
    constexpr int PCLK_GPIO_NUM = 22;

    bool init()
    {
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
        config.pixel_format = PIXFORMAT_JPEG;

        if (psramFound())
        {
            config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
            config.jpeg_quality = 10;
            config.fb_count = 2;
        }
        else
        {
            config.frame_size = FRAMESIZE_SVGA;
            config.jpeg_quality = 12;
            config.fb_count = 1;
        }

        // Init Camera
        esp_err_t err = esp_camera_init(&config);
        if (err != ESP_OK)
        {
            Serial.printf("Camera init failed with error 0x%x", err);
            return false;
        }

        // Camera quality adjustments
        sensor_t *s = esp_camera_sensor_get();

        // BRIGHTNESS (-2 to 2)
        s->set_brightness(s, 0);
        // CONTRAST (-2 to 2)
        s->set_contrast(s, 0);
        // SATURATION (-2 to 2)
        s->set_saturation(s, 0);
        // SPECIAL EFFECTS (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
        s->set_special_effect(s, 0);
        // WHITE BALANCE (0 = Disable , 1 = Enable)
        s->set_whitebal(s, 1);
        // AWB GAIN (0 = Disable , 1 = Enable)
        s->set_awb_gain(s, 1);
        // WB MODES (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        s->set_wb_mode(s, 0);
        // EXPOSURE CONTROLS (0 = Disable , 1 = Enable)
        s->set_exposure_ctrl(s, 1);
        // AEC2 (0 = Disable , 1 = Enable)
        s->set_aec2(s, 0);
        // AE LEVELS (-2 to 2)
        s->set_ae_level(s, 0);
        // AEC VALUES (0 to 1200)
        s->set_aec_value(s, 300);
        // GAIN CONTROLS (0 = Disable , 1 = Enable)
        s->set_gain_ctrl(s, 1);
        // AGC GAIN (0 to 30)
        s->set_agc_gain(s, 0);
        // GAIN CEILING (0 to 6)
        s->set_gainceiling(s, (gainceiling_t)0);
        // BPC (0 = Disable , 1 = Enable)
        s->set_bpc(s, 0);
        // WPC (0 = Disable , 1 = Enable)
        s->set_wpc(s, 1);
        // RAW GMA (0 = Disable , 1 = Enable)
        s->set_raw_gma(s, 1);
        // LENC (0 = Disable , 1 = Enable)
        s->set_lenc(s, 1);
        // HORIZ MIRROR (0 = Disable , 1 = Enable)
        s->set_hmirror(s, 0);
        // VERT FLIP (0 = Disable , 1 = Enable)
        s->set_vflip(s, 0);
        // DCW (0 = Disable , 1 = Enable)
        s->set_dcw(s, 1);
        // COLOR BAR PATTERN (0 = Disable , 1 = Enable)
        s->set_colorbar(s, 0);

        return true;
    }

    camera_fb_t capture()
    {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb)
        {
            Serial.println("Camera capture failed");
            return {};
        }

        return *fb;
    }

    void returnFrameBuffer(camera_fb_t &fb)
    {
        esp_camera_fb_return(&fb);
    }

}