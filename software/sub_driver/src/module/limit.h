/**
 * @file limit.h
 * @author Daniel Kim   
 * @brief Reads the limit switches
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022 Daniel Kim https://github.com/daniel360kim/OceanAI
 * 
 */

#ifndef LIMIT_H
#define LIMIT_H

#include <Arduino.h>

class Limit
{
public:
    Limit() {}
    void begin(const uint8_t limit_pin);
    bool state();
    

private:
    uint8_t limit_pin;
    bool button_state;
};




#endif