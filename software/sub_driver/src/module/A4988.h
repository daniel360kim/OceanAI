/**
 * @file A4988.h
 * @author Daniel Kim       
 * @brief A4988 stepper motor driver 
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022 Daniel Kim https://github.com/daniel360kim/OceanAI
 * 
 */
#ifndef A4988_h
#define A4988_h

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>

#include "../pins.h"
#include "module_settings.h"
#include "limit.h"
#include "../data/data_struct.h"

class A4988
{
public:
    enum Resolution
    {
        Full = 0,
        Half = 1,
        Quarter = 2,
        Eighth = 3,
        Sixteenth = 4,
    };
    
    A4988(const uint8_t step_pin, const uint8_t dir_pin, const uint8_t ms1_pin, const uint8_t ms2_pin, const uint8_t ms3_pin, const uint8_t slp, const uint8_t rst, const uint8_t limit_pin);
    void begin();
    void setResolution(Resolution resolution);
    void setState(bool slp_state);
    void setDirection(bool dir_state); //sets the direction of the motor
    void step(); //steps the motor

    bool calibrate(); //calibrate the motor (blocking)

    void toPosition(uint8_t position); //goes to a specific position (blocking)

    void logToStruct(Data &data);

private:
    bool is_running = false;
    bool direction;
    //control pins
    uint8_t step_pin;
    uint8_t dir_pin; 
    //microstep pins
    uint8_t ms1_pin; 
    uint8_t ms2_pin; 
    uint8_t ms3_pin;
    //power pins
    uint8_t slp;
    uint8_t rst;

    Resolution resolution;
    int steps_per_revolution;
    Limit limit;

    int current_position;
};












#endif