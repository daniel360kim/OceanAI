/**
 * @file thermistor.h
 * @author Daniel Kim
 * @brief Reads data from a thermistor
 * @version 0.1
 * @date 2022-11-05
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef thermistor_h
#define thermistor_h

#include <Arduino.h>
#include <stdint.h>
#include <cmath>

#include "constants.h"
#include "LowPass.h"
#include "read_functions.h"
#include "../core/Timer.h"


namespace Sensors
{
    class Thermistor : public ReadFunctions
    {
    public:
        Thermistor(const uint8_t pin, const double RT0, const double B, const double T0, const double cutoff, const long interval_ns);
        double readRaw();
        double readFiltered(const double delta_time);

        void logToStruct(LoggedData &data);

    private:
        uint8_t m_pin;
        double m_RT0;
        double m_B;
        double m_T0;
        long m_interval;
        
        bool m_temp_updated = false;

        int64_t m_prev_log_ns = 0;

        Filter::LowPass<1> m_filter;
    };
}

#endif