/**
 * @file cpu.h
 * @author Daniel Kim       
 * @brief cpu temperature and throttling
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef CPU_H
#define CPU_H

#include <cstdint>

#include "../data/logged_data.h"

namespace CPU
{
    extern "C" uint32_t set_arm_clock(uint32_t freq); // set clock speed in Hz requireed to read clock speed
    

    void init();
    void HighAlarmISR();
    void LowAlarmISR();

    void log_cpu_info(LoggedData &logged_data);
}


#endif
