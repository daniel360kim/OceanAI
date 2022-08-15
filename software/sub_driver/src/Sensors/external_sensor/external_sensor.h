/**
 * @file external_sensor.h
 * @author Daniel Kim
 * @brief Communication with external sensor
 * @version 0.1
 * @date 2022-08-12
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef external_sensor_h
#define external_sensor_h

#include <Arduino.h>
#include <Wire.h>

#include "../../data/data_struct.h"

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

    struct RawData
    {
        float loop_time;
        float temperature;
        float pressure;
    };

    union float_to_byte
    {
        float floatData[3];
        uint8_t rawData[3 * sizeof(float)];
    };
    
    ExternalSensor(uint8_t i2c_address);

    RawData getData();
    bool reset();
    bool chipID();
    
private:
    uint8_t i2c_address;

    void writeRegister(uint8_t subAddress, uint8_t data);
    uint8_t readRegister(uint8_t subAddress);
    void writeRegisters(uint8_t subAddress, uint8_t count, const uint8_t *data);
    void readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest);

};




#endif