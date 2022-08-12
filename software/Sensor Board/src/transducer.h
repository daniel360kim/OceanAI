/**
 * @file transducer.h
 * @author Daniel Kim   
 * @brief transducer driver
 * @version 0.1
 * @date 2022-07-29
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef TRANSDUCER_H
#define TRANSDUCER_H

#include <Arduino.h>

class Transducer
{
public:
    Transducer(uint8_t pin);
    float getPressure();
private:    
    uint8_t pin;
    
};


#endif