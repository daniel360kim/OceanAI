/**
 * @file transmission.h
 * @author Daniel Kim
 * @brief Communication with external sensor
 * @version 0.1
 * @date 2022-08-12
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef transmission_h
#define transmission_h

#include <Arduino.h>
#include <Wire.h>

constexpr uint8_t ext_CHIPID = 0x50;

struct RawData
{
    double data[3];
};

class ExternalSensor
{
public:
    ExternalSensor(uint8_t i2c_address) : i2c_address(i2c_address) {}
    bool initialize();

    void sendData(RawData data);
    static volatile uint8_t lastMasterCommand;

private:
    uint8_t i2c_address;

    static volatile uint8_t* arrayPointer;
    

    static double array[3];

    static void receiveCommand(int howMany);
    static void requestEvent();
};




#endif