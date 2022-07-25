/**
 * @file OV2640.cpp
 * @author Daniel Kim
 * @brief Utilization of the OV2640 2MP camera
 * @version 0.1
 * @date 2022-07-05
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <Wire.h>
#include <SPI.h>
#include <ArduCAM.h>
#include <memorysaver.h>
#include <Arduino.h>

#include "OV2640.h"
#include "../Data/SD/SD.h"
#include "../Data/SD/DataFile.h"
#include "../Data/StartInfo.h"
#include "../debug.h"
#include "../config.h"
#include "../pins.h"

FsFile image_file;


namespace Optics
{
    namespace Settings
    {
        constexpr uint8_t IMAGE_SIZE = OV2640_640x480;
        constexpr uint8_t IMAGE_FORMAT = JPEG;
    };
    ArduCAM camera(OV2640, CS_VD);

    Camera::Camera(const uint8_t cs_pin)
    {
        this->cs_pin = cs_pin;
    }

    bool Camera::begin()
    {
        //set chip select as an output
        pinMode(cs_pin, OUTPUT);
        digitalWrite(cs_pin, HIGH);

        Wire.begin();
        SPI.begin(); //initialize SPI bus

        camera.write_reg(0x07, 0x80); //Reset the CPLD
        delay(100);
        camera.write_reg(0x08, 0x80);
        delay(100);

        uint8_t temp = 0;
        while(1)
        {
            //Check the SPI bus is good to go
            camera.write_reg(ARDUCHIP_TEST1, 0x55);
            temp = camera.read_reg(ARDUCHIP_TEST1);
            if(temp != 0x55)
            {
                #if DEBUG_ON == true
                    char* message = (char*)"OV2640: SPI bus initialization failed";
                    Debug::error.addToBuffer(micros(), Debug::Fatal, message);

                    #if LIVE_DEBUG == true
                        Serial.println(F(message));
                    #endif

                #endif
                
                return false;
            }
            else
            {
                #if DEBUG_ON == true
                    char* message = (char*)"OV2640: SPI bus initialization successful";
                    Debug::success.addToBuffer(micros(), Debug::Success, message);

                    #if LIVE_DEBUG == true
                        Serial.println(F(message));
                    #endif

                #endif
                break;
            }
        }

        uint8_t vid, pid;
        while(1)
        {
            //x2 check that the camera connected is the OV2640 
            camera.wrSensorReg8_8(0xff, 0x01);
            camera.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
            camera.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);

            if((vid != 0x26) && ((pid != 0x41) || (pid != 0x42)))
            {
                #if DEBUG_ON == true
                char* message = (char*)"OV2640: Could not find/detect camera";
                Debug::error.addToBuffer(micros(), Debug::Fatal, message);

                    #if LIVE_DEBUG == true
                        Serial.println(F(message));
                    #endif

                #endif
                return false;
            }
            else
            {
                #if DEBUG_ON == true
                char* message = (char*)"OV2640: Found the camera";
                Debug::success.addToBuffer(micros(), Debug::Success, message);

                    #if LIVE_DEBUG == true
                        Serial.println(F(message));
                    #endif

                #endif
                break;
                
            }
        }

        camera.set_format(Settings::IMAGE_FORMAT);
        camera.InitCAM();
        camera.clear_fifo_flag();
        camera.write_reg(0x01, frame_num);

        configs.img_size = Settings::IMAGE_SIZE;
        configs.img_format = Settings::IMAGE_FORMAT;

        return true;
    }
    
    void Camera::capture(unsigned long delay_micros, unsigned long *capture_time, unsigned long *save_time, uint8_t *FIFO_length)
    {
        unsigned long long current_micros = micros();

        if(current_micros - previous_log >= delay_micros)
        {
            camera.flush_fifo();
            camera.clear_fifo_flag();

            camera.OV2640_set_JPEG_size(Settings::IMAGE_SIZE);

            camera.start_capture();
            
            unsigned long long total_time = micros();
            while ( !camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));

            *capture_time = total_time - micros();
            #if LIVE_DEBUG == true
                Serial.print(F("capture total_time used (in miliseconds):"));
                Serial.println(*capture_time / 1000, DEC);
            #endif
            total_time = micros();

            read_fifo_burst(FIFO_length);
            total_time = micros() - total_time;
            camera.clear_fifo_flag();

            *save_time = total_time;
            
            #if LIVE_DEBUG == true
                Serial.print(F("save capture total_time used (in miliseconds):"));
                Serial.println(*save_time / 1000, DEC);
            #endif

            camera.clear_fifo_flag();

            previous_log = current_micros;
        };
    }
    
    uint8_t Camera::read_fifo_burst(uint8_t *fifolength)
    {
        uint8_t temp = 0, temp_last = 0;
        uint32_t length = 0;
        static int i = 0;
        static int k = 0;
        char str[16];
        byte buf[256];

        length = camera.read_fifo_length();
        *fifolength = length;
        if(length >= MAX_FIFO_SIZE) //8M
        {
             #if DEBUG_ON == true
                char* message = (char*)"OV2640: FIFO Buffer Oversized";
                Debug::error.addToBuffer(micros(), Debug::Fatal, message);

                    #if LIVE_DEBUG == true
                        Serial.println(F(message));
                    #endif

             #endif
        }

        camera.CS_LOW();
        camera.set_fifo_burst();
        i = 0;

        while(length--)
        {
            temp_last = temp;
            temp =  SPI.transfer(0x00);
            //Read JPEG data from FIFO
            if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
            {
                buf[i++] = temp;  //save the last  0XD9
                //Write the remain bytes in the buffer
                camera.CS_HIGH();
                image_file.write(buf, i);
                //Close the file
                image_file.close();
                is_header = false;
                camera.CS_LOW();
                camera.set_fifo_burst();
                i = 0;
            
            }
             if (is_header == true)
            {
            //Write image data to buffer if not full
                if (i < 256)
                    buf[i++] = temp;
                else
                {
                    //Write 256 bytes image data to file
                    camera.CS_HIGH();
                    image_file.write(buf, 256);
                    i = 0;
                    buf[i++] = temp;
                    camera.CS_LOW();
                    camera.set_fifo_burst();
                }
            }
            else if ((temp == 0xD8) && (temp_last == 0xFF))
            {
                is_header = true;
                camera.CS_HIGH();
                //Create a avi file
                k = k + 1;

                itoa(k, str, 10);
                strcat(str, ".jpg");
                char* dir = (char*)"Images/";
                String str_s(str);
                String dir_s(dir);

                String path = dir_s + str_s;
                char c_path[50];
                path.toCharArray(c_path,50);
                //Open the new file
                if(!image_file.open(c_path, O_WRITE | O_CREAT))
                {
                    #if DEBUG_ON == true
                        char* message = (char*)"Failed to open image file";
                        Debug::error.addToBuffer(micros(), Debug::Warning, message);
                        
                        #if LIVE_DEBUG == true
                            Serial.println(F(message));
                        #endif

                    #endif
                }

                if(!image_file.preAllocate(11000u))
                {
                    file.close();
                    #if DEBUG_ON == true
                        char* message = (char*)"Failed to preallocate image file";
                        Debug::error.addToBuffer(micros(), Debug::Warning, message);
                        
                        #if LIVE_DEBUG == true
                            Serial.println(F(message));
                        #endif

                    #endif
                }
                
                //error check here
                camera.CS_LOW();
                camera.set_fifo_burst();
                buf[i++] = temp_last;
                buf[i++] = temp;
            }
        }

        camera.CS_HIGH();
        return 1;
    }
};