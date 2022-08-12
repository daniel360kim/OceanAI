/**
 * @file limit.cpp
 * @author Daniel Kim   
 * @brief Reads the limit switches
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022 Daniel Kim https://github.com/daniel360kim/OceanAI
 * 
 */
#include <Arduino.h>

#include "limit.h"

void Limit::begin(const uint8_t limit_pin)
{
    this->limit_pin = limit_pin;
    pinMode(limit_pin, INPUT_PULLUP);
    button_state = !digitalRead(limit_pin);
}

bool Limit::state()
{
    button_state = digitalRead(limit_pin);
    return !button_state;
}
