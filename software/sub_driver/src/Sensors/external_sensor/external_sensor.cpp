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

ExternalSensor::RawData ExternalSensor::getSensorData()
{
    RawData data;
    Wire.beginTransmission(i2c_address);
    Wire.readBytes((uint8_t*)&data, sizeof(ExternalData));
    Wire.endTransmission();

    return data;
}