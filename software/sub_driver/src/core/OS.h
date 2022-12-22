/**
 * @file OS.h
 * @author Daniel Kim
 * @brief operating the cpu on the teensy 4.1
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#ifndef OS_H
#define OS_H

#include <Arduino.h>
#include <InternalTemperature.h>

#include "../Data/logged_data.h"

extern "C" uint32_t set_arm_clock(uint32_t frequency); // required prototype

class OS
{
public:
    static OS &getInstance()
    {
        return instance;
    }
    void log_cpu_state(Data &data);

private:
    OS();
    static OS instance;
    static volatile bool high_temp_alarm;
    static volatile bool low_temp_alarm;

    static void HighAlarmISR();
    static void LowAlarmISR();

    unsigned long previous_millis;
};

#endif
