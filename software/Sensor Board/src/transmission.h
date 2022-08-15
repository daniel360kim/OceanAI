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

    void writeRegister(uint8_t subAddress, uint8_t data);
    uint8_t readRegister(uint8_t subAddress);
    void readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest);

private:
    uint8_t i2c_address;

    static volatile uint8_t* arrayPointer;
    static volatile uint8_t lastMasterCommand;

    static double array[3];

    static void receiveCommand(int howMany);
    static void requestEvent();
};




#endif