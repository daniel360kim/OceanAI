/**
 * @file thermistor.h
 * @author Daniel Kim   
 * @brief thermistor control
 * @version 0.1
 * @date 2022-07-29
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>

#include "data_struct.h"

class Thermistor
{
public:
    Thermistor(uint8_t pin, int seriesResistor);
    float getTemperature();

    void log_to_struct(Data &data);
    
    


private:
    inline uint32_t readADC();
    inline float c_to_f(float celsius);

    uint8_t pin;
    int seriesResistor;
};

#endif