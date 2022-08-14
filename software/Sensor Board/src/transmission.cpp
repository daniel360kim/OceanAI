/**
 * @file transmission.cpp
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

#include "transmission.h"


bool ExternalSensor::initialize()
{
    Wire.begin();
    Wire.setClock(400000);

    return true;
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
void ExternalSensor::writeRegisters(uint8_t subAddress, RawData data)
{
    Wire.beginTransmission(i2c_address);

    uint8_t send_data[12];
    for(int i = 0; i < 3; i++)
    {
        flt.fvalue = data.data[i];
        for(int j = 0; j < sizeof(float); j++)
        {
            send_data[i * sizeof(float) + j] = flt.fbytes[j];
        }
    }
    for(int i = 0; i < 12; i++)
    {
        Wire.write(send_data[i]);
        Serial.println(send_data[i], HEX);
    }
    Serial.println();

    Wire.endTransmission();
}

void ExternalSensor::readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest)
{
    Wire.beginTransmission(i2c_address);
    Wire.write(subAddress);
    Wire.endTransmission(false);
    uint8_t i = 0;
    Wire.requestFrom(i2c_address, (uint8_t)count);
    while (Wire.available())
    {
        dest[i++] = Wire.read();
    }
}