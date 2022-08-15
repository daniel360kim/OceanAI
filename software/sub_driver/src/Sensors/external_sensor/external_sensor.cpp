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
    Wire.begin();
    this->i2c_address = i2c_address;
}

ExternalSensor::RawData ExternalSensor::getData()
{
    Wire.beginTransmission(i2c_address);
    Wire.write((uint8_t)1);

    float_to_byte ftb;
    if(Wire.requestFrom(i2c_address, sizeof(float_to_byte)) == sizeof(float_to_byte))
    {
        for(byte i = 0; i < 3 * sizeof(float); i++)
        {
            ftb.rawData[i] = Wire.read();
        }
    }

    Wire.endTransmission();

    float data[3];

    for(byte i = 0; i < 3; i++)
    {
        data[i] = ftb.floatData[i];
    }
    return RawData{data[0], data[1], data[2]};
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
    //Wire.write(subAddress);
    Wire.requestFrom(i2c_address, count);

    int i = 0;
    if(Wire.available())
    {
        while(Wire.available())
        {
            dest[i] = Wire.read();
            Serial.print("DEST "); Serial.println(dest[i]);
            i++;
        }
    }
    
    Wire.endTransmission();
}