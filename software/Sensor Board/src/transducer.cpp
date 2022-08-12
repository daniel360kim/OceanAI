/**
 * @file transducer.cpp
 * @author Daniel Kim   
 * @brief transducer driver
 * @version 0.1
 * @date 2022-07-29
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "transducer.h"

#include <Arduino.h>

Transducer::Transducer(uint8_t pin)
{
    this->pin = pin;
}

float Transducer::getPressure()
{
    uint32_t adc = analogRead(pin);
    float pressure = ((float)adc * 3.3) / 1023.0;
    pressure = (3.0 * (pressure - 0.475)) * 10000.0;
    return pressure;
}