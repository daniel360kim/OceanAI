/**
 * @file OV2640.h
 * @author Daniel Kim
 * @brief Utilization of the OV2640 2MP camera
 * @version 0.1
 * @date 2022-07-05
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#ifndef camera_h
#define camera_h

#include <Wire.h>
#include <SPI.h>
#include <ArduCAM.h>
#include <Arduino.h>
#include <memorysaver.h>
#include <EEPROM.h>

#include <string>
#include <stdint.h>

#include "../../core/debug.h"
#include "../../Data/SD/SD.h"


    enum Resolution
    {
        RESOLUTION_160x120,
        RESOLUTION_176x144,
        RESOLUTION_320x240,
        RESOLUTION_352x288,
        RESOLUTION_640x480,
        RESOLUTION_800x600,
        RESOLUTION_1024x768,
        RESOLUTION_1280x1024,
        RESOLUTION_1600x1200
    };

    enum Frame_Number
    {
        ONE_PHOTO = 0x00,
        TWO_PHOTOS = 0x01,
        THREE_PHOTOS = 0x02,
        FOUR_PHOTOS = 0x03,
        FIVE_PHOTOS = 0x04,
        SIX_PHOTOS = 0x05,
        SEVEN_PHOTOS = 0x06,
        CONTINUOUS = 0x07
    };

    enum Saturation
    {
        SATURATION_0,
        SATURATION_1,
        SATURATION_2,
        SATURATION_3,
        SATURATION_4,
        SATURATION_5,
        SATURATION_6,
        SATURATION_7,
        SATURATION_8,
    };

    enum Brightness
    {
        BRIGHTNESS_0,
        BRIGHTNESS_1,
        BRIGHTNESS_2,
        BRIGHTNESS_3,
        BRIGHTNESS_4,
        BRIGHTNESS_5,
        BRIGHTNESS_6,
        BRIGHTNESS_7,
        BRIGHTNESS_8,
    };

    enum Contrast
    {
        CONTRAST_0,
        CONTRAST_1,
        CONTRAST_2,
        CONTRAST_3,
        CONTRAST_4,
        CONTRAST_5,
        CONTRAST_6,
        CONTRAST_7,
        CONTRAST_8,
    };

    enum Color
    {
        ANTIQUE,
        BLUE,
        GREEN,
        RED,
        BLACK_AND_WHITE,
        NEGATIVE,
        BW_NEGATIVE,
        NORMAL,
        SEPIA,
        BRIGHT,
        SOLARIZE,
        BLUEISH,
        YELLOWISH,
        NONE,
    };

    enum White_Balance
    {
        AUTO,
        SUNNY,
        CLOUDY,
        OFFICE,
        HOME
    };

    enum Format
    {
        BMP = 0,
        JPEG = 1,
        RAW = 2,
    };

    class OV2640_Mini
    {
    public:
        OV2640_Mini() {}
        OV2640_Mini(const uint8_t cs_pin, Resolution resolution, Frame_Number frame_number, bool use_eeprom,
                    Saturation saturation = Saturation::SATURATION_5,
                    Brightness brightness = Brightness::BRIGHTNESS_5,
                    Contrast contrast = Contrast::CONTRAST_5,
                    Color color = Color::NONE,
                    White_Balance white_balance = White_Balance::AUTO,
                    Format format = Format::JPEG);

        ArduCAM *getCamera() { return m_camera; }

        void setResolution(Resolution resolution);
        void setFrameNumber(Frame_Number frame_number);
        void setSaturation(Saturation saturation);
        void setBrightness(Brightness brightness);
        void setContrast(Contrast contrast);
        void setColor(Color color);
        void setWhiteBalance(White_Balance white_balance);
        void setFormat(Format format);

        void saveCurrentResolution() { EEPROM.write(0, m_resolution); }
        void saveCurrentFrameNumber() { EEPROM.write(1, m_frame_number); }
        void saveCurrentSaturation() { EEPROM.write(2, m_saturation); }
        void saveCurrentBrightness() { EEPROM.write(3, m_brightness); }
        void saveCurrentContrast() { EEPROM.write(4, m_contrast); }
        void saveCurrentColor() { EEPROM.write(5, m_color); }
        void saveCurrentWhiteBalance() { EEPROM.write(6, m_white_balance); }
        void saveCurrentFormat() { EEPROM.write(7, m_format); }

        void saveSettings();
        void saveSettings(Resolution resolution, Frame_Number frame_number, Saturation saturation, Brightness brightness, Contrast contrast, Color color, White_Balance white_balance, Format format);

        Resolution getResolution() { return m_resolution; }
        Frame_Number getFrameNumber() { return m_frame_number; }
        Saturation getSaturation() { return m_saturation; }
        Brightness getBrightness() { return m_brightness; }
        Contrast getContrast() { return m_contrast; }
        Color getColor() { return m_color; }
        White_Balance getWhiteBalance() { return m_white_balance; }

        void retrieveSettings();
        void retrieveSettings(Resolution &resolution, Frame_Number &frame_number, Saturation &saturation, Brightness &brightness, Contrast &contrast, Color &color, White_Balance &white_balance, Format &format);

        bool initialize();

        inline bool is_header() { return m_is_header; }
        inline void set_header(bool is_header) { m_is_header = is_header; }

        void capture();

    private:
        ArduCAM *m_camera;

        Resolution m_resolution;
        Frame_Number m_frame_number;
        Saturation m_saturation;
        Brightness m_brightness;
        Contrast m_contrast;
        Color m_color;
        White_Balance m_white_balance;
        Format m_format;

        uint8_t m_cs_pin;

        unsigned int m_addr = 0;

        bool m_is_header = false;
    };

#endif