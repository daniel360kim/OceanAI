/**
 * @file tds.h
 * @author Daniel Kim
 * @brief salinity sensor driver
 * @version 0.1
 * @date 2022-11-05
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef dss_h
#define dss_h

#include <Arduino.h>
#include <stdint.h>

#include "constants.h"
#include "read_functions.h"
#include "LowPass.h"
#include "../core/Timer.h"

namespace Sensors
{
    class TotalDissolvedSolids
    {
    public:
        TotalDissolvedSolids(const uint8_t pin, const double cutoff, const long interval_ns);

        double readRaw(const double temp);
        double readFiltered(const double delta_time, const double temp);

        void logToStruct(Data &data);

    private:
        uint8_t m_pin;
        long m_interval_ns;

        double m_raw_tds_reading;
        bool m_tds_updated = false;

        int64_t m_prev_log_ns = 0;

        Filter::LowPass<1> m_filter;
    };
}

#endif