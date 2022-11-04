/**
 * @file OS.cpp
 * @author Daniel Kim
 * @brief operating the cpu on the teensy 4.1
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "OS.h"

#include <InternalTemperature.h>

OS OS::instance;

volatile bool OS::high_temp_alarm = false;
volatile bool OS::low_temp_alarm = false;

OS::OS()
{
    InternalTemperature.attachHighTempInterruptCelsius(OS_Settings::HIGH_TEMP, &HighAlarmISR);
}

/**
 * @brief logs data to the data struct
 * 
 * @param data 
 */
FASTRUN void OS::log_cpu_state(Data &data)
{
    unsigned long current_millis = millis();
    if(current_millis - previous_millis >= OS_Settings::log_millis)
    {
        data.internal_temp = InternalTemperature.readTemperatureC();
        data.clock_speed = F_CPU_ACTUAL;

        previous_millis = current_millis;
    }
}

/**
 * @brief ISR for high temp alarm
 * 
 */
void OS::HighAlarmISR()
{
    set_arm_clock(350000000);
    high_temp_alarm = true;
    InternalTemperature.attachLowTempInterruptCelsius (OS_Settings::LOW_TEMP, &LowAlarmISR);
}

/**
 * @brief ISR for low temp alarm
 * 
 */
void OS::LowAlarmISR()
{
    set_arm_clock(528000000);
    low_temp_alarm = false;
    InternalTemperature.attachHighTempInterruptCelsius (OS_Settings::HIGH_TEMP, &HighAlarmISR);
}
