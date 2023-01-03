/**
 * @file transducer.h
 * @author Daniel Kim
 * @brief read from pressure transducer
 * @version 0.1
 * @date 2022-11-05
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef Transducer_h
#define Transducer_h

#include <Arduino.h>
#include <cstdint>
#include <cmath>

#include "constants.h"
#include "LowPass.h"
#include "read_functions.h"
#include "../core/Timer.h"

namespace Sensors
{
    class Transducer
    {
    public:
        Transducer(const uint8_t pin, const double cutoff, const long interval_ns);
        
        inline double readVoltage();

        double readRaw();
        double readFiltered(const double delta_time);

        void logToStruct(LoggedData &data);

    private:
        uint8_t m_pin;
        long m_interval;

        double m_raw_pressure;

        bool m_pres_updated = false;
        int64_t m_prev_log_ns = 0;

        Filter::LowPass<1> m_filter;
    };
}

#endif