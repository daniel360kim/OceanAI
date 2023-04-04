/**
 * @file cpu.cpp
 * @author Daniel Kim       
 * @brief cpu temperature and throttling
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <InternalTemperature.h>

#include "cpu.h"
#include "configuration.h"
#include "debug.h"
#include "timed_function.h"

namespace CPU
{
    /**
     * @brief Initialize CPU throttling
     * 
     */
    void init()
    {
        InternalTemperature.attachHighTempInterruptCelsius(OVERTEMP_THRESHOLD, &HighAlarmISR); //set overtemp threshold and attach HighAlarmISR (ISR for when temperature is above OVERTEMP_THRESHOLD)

        INFO_LOGf("Overtemp threshold: %d", OVERTEMP_THRESHOLD);
        INFO_LOGf("Undertemp threshold: %d", UNDERTEMP_THRESHOLD);

        SUCCESS_LOG("CPU throttling initialized");
    }

    /**
     * @brief ISR for when temperature is above OVERTEMP_THRESHOLD
     * 
     */
    void HighAlarmISR()
    {
        set_arm_clock(LOW_CLOCKSPEED); // set clock speed to LOW_CLOCKSPEED
        INFO_LOGf("CPU temperature above %d degrees Celsius. Clock speed lowered to %d Hz", OVERTEMP_THRESHOLD, LOW_CLOCKSPEED);
        
        InternalTemperature.attachLowTempInterruptCelsius(UNDERTEMP_THRESHOLD, &LowAlarmISR); // attach LowAlarmISR when temperature is below UNDERTEMP_THRESHOLD
    }

    /**
     * @brief ISR for when temperature is low enough to raise clock speed
     * 
     */
    void LowAlarmISR()
    {
        set_arm_clock(HIGH_CLOCKSPEED); // set clock speed to HIGH_CLOCKSPEED
        INFO_LOGf("CPU temperature below %d degrees Celsius. Clock speed raised to %d Hz", UNDERTEMP_THRESHOLD, HIGH_CLOCKSPEED);
        
        InternalTemperature.attachHighTempInterruptCelsius(OVERTEMP_THRESHOLD, &HighAlarmISR); // attach HighAlarmISR when temperature is above OVERTEMP_THRESHOLD
    }

    int64_t previous_cpu_log = 0;
    void log_cpu_info(LoggedData &logged_data)
    {
        // log CPU info at a set interval
        int64_t current_time = scoped_timer.elapsed();
        if(current_time - previous_cpu_log < CPU_INFO_LOG_INTERVAL)
        {
            return;
        }
        else
        {
            logged_data.clock_speed = F_CPU_ACTUAL;
            logged_data.internal_temp = InternalTemperature.readTemperatureC();

            logged_data.loop_time = 1.0 / logged_data.delta_time;

            previous_cpu_log = current_time;
        }
    }

}