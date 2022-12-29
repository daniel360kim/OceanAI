/**
 * @file voltage.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-06
 * 
* @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef voltage_h
#define voltage_h

#include <Arduino.h>
#include <stdint.h>
#include <cmath>

#include "constants.h"
#include "LowPass.h"
#include "read_functions.h"
#include "../core/Timer.h"

namespace Sensors
{
    class Voltage : public ReadFunctions
    {
    public:
        Voltage(const uint8_t pin, const double cutoff, const long interval_ns);

        double readRaw();
        double readFiltered(const double delta_time);

        void logToStruct(LoggedData &data);
    private:
        uint8_t m_pin;
        long m_interval;

        double m_raw_voltage;

        bool m_voltage_updated = false;
        int64_t m_prev_log_ns = 0;

        Filter::LowPass<1> m_filter;
    };
}

#endif