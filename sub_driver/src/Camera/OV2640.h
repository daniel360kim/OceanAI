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

#ifndef OV2640_h
#define OV2640_h

#include <Wire.h>
#include <SPI.h>
#include <ArduCAM.h>
#include <Arduino.h>
#include <memorysaver.h>

#include "../Data/SD/SD.h"
#include "../Data/SD/DataFile.h"

namespace Optics
{
    constexpr uint8_t ONE_PHOTO = 0x00;
    constexpr uint8_t TWO_PHOTOS = 0x01;
    constexpr uint8_t THREE_PHOTOS = 0x02;
    constexpr uint8_t FOUR_PHOTOS = 0X03;
    constexpr uint8_t FIVE_PHOTOS = 0X04;
    constexpr uint8_t SIX_PHOTOS = 0X05;
    constexpr uint8_t SEVEN_PHOTOS = 0X06;
    constexpr uint8_t CONTINUOUS = 0XFF; //continue shooting till FIFO is full

    class Camera
    {
    public:

        //How many pictures to take
       
        
        

        Camera(TwoWire &bus, SPIClass &spiBus, const uint8_t cs_pin);

        bool begin();
        void setFrameCount(uint8_t frame_num) { this->frame_num = frame_num; }
        void capture(unsigned long delay_micros);

    private:
        ArduCAM *camera;
        TwoWire *_i2c;
        SPIClass *_spi;

        uint8_t cs_pin;
        uint8_t frame_num = ONE_PHOTO;
        bool is_header = false;
        char* image_file_name;
        uint8_t read_fifo_burst();


        unsigned long long previous_log;
    };
};














#endif