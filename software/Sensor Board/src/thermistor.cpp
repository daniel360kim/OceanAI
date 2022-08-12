/**
 * @file thermistor.cpp
 * @author Daniel Kim   
 * @brief thermistor control
 * @version 0.1
 * @date 2022-07-29
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <Arduino.h>

#include "thermistor.h"

Thermistor::Thermistor(uint8_t pin, const int seriesResistor)
{
    this->pin = pin;
    this->seriesResistor = seriesResistor;
}

float Thermistor::getTemperature()
{
    uint32_t reading = readADC();
    float temperature = seriesResistor * (1023.0 / (float)reading - 1.0);
    temperature = log(temperature);
    temperature = (1.0 / (1.009249522e-03 + 2.378405444e-04 * temperature + 2.019202697e-07 * temperature * temperature * temperature)) - 273.15;
    return c_to_f(temperature);
}

inline uint32_t Thermistor::readADC()
{
    uint32_t adc = analogRead(pin);
    return adc;
}

inline float Thermistor::c_to_f(float celsius)
{
    return celsius * 9.0 / 5.0 + 32.0;
}