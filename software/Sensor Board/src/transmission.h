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
    float data[3];
};

class ExternalSensor
{
public:
    enum SubAddress
    {
        COMMAND = 0x00,
        COMMAND_ACK = 0x01,
        CHIPID = 0x02,
        DATA = 0x03
    };

    enum Command
    {
        RESET = 0x15
    };

    enum Acknowledge
    {
        SUCCESS = 0x17,
        FAIL = 0x18
    };

    union Send
    {
        uint8_t fbytes[sizeof(float)];
        float fvalue;
    } flt;
    
    ExternalSensor(uint8_t i2c_address) : i2c_address(i2c_address) {}
    bool initialize();

    void writeRegister(uint8_t subAddress, uint8_t data);
    uint8_t readRegister(uint8_t subAddress);
    void writeRegisters(uint8_t subAddress, RawData data);
    void readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest);

private:
    uint8_t i2c_address;

};




#endif