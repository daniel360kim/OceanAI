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

double ExternalSensor::array[3] = {0.0, 0.0, 0.0};

volatile uint8_t* ExternalSensor::arrayPointer = NULL;
volatile uint8_t ExternalSensor::lastMasterCommand = 99;


bool ExternalSensor::initialize()
{
    Wire.begin(i2c_address);
    Wire.onReceive(receiveCommand);
    Wire.onRequest(requestEvent);
    return true;
}

void ExternalSensor::sendData(RawData data)
{
    array[0] = data.data[0];
    array[1] = data.data[1];
    array[2] = data.data[2];
}

void ExternalSensor::receiveCommand(int howMany)
{
    lastMasterCommand = Wire.read();
}


void ExternalSensor::requestEvent()
{
    constexpr int bufferSize = sizeof(double) * 3;
    uint8_t buffer[bufferSize];
    arrayPointer = (uint8_t*) &array;
    for(uint8_t i = 0; i < bufferSize; i++)
    {
        buffer[i] = arrayPointer[i];
    }
    Wire.write(buffer, bufferSize);


}