/**
 * @file external_sensor.cpp
 * @author Daniel Kim
 * @brief Communication with external sensor
 * @version 0.1
 * @date 2022-08-12
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <Arduino.h>
#include <Wire.h>

#include "external_sensor.h"
#include "../../data/data_struct.h"

ExternalSensor::ExternalSensor(uint8_t i2c_address)
{
    this->i2c_address = i2c_address;

}

ExternalSensor::RawData ExternalSensor::getData()
{
    constexpr int count = 12;
    uint8_t data[count];

    float result[3];

    readRegisters(SubAddress::DATA, count, data);
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            flt.fbytes[j] = data[(i * 4) + j];
        }
        result[i] = flt.fvalue;
    }

    RawData raw_data;

    raw_data.loop_time = result[0];
    raw_data.temperature = result[1];
    raw_data.pressure = result[2];

    return raw_data;
}


void ExternalSensor::writeRegister(uint8_t subAddress, uint8_t data)
{
    Wire.beginTransmission(i2c_address);
    Wire.write(subAddress);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t ExternalSensor::readRegister(uint8_t subAddress)
{
    uint8_t data = 0x00;
    Wire.beginTransmission(i2c_address);
    Wire.write(subAddress);
    Wire.endTransmission(false);
    Wire.requestFrom(i2c_address, (uint8_t)1);
    data = Wire.read();

    return data;
}
void ExternalSensor::writeRegisters(uint8_t subAddress, uint8_t count, const uint8_t *data)
{
    Wire.beginTransmission(i2c_address);

    for (int i = 0; i < count; i++)
    {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

void ExternalSensor::readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest)
{
    Wire.beginTransmission(i2c_address);
 
    Wire.endTransmission(false);
    Wire.requestFrom(i2c_address, count);

    
    for(int i = 0; i < count; i++)
    {
        dest[i] = Wire.read();
    }
    Wire.endTransmission();
 
    
}