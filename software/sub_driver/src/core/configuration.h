/**
 * @file configuration.h
 * @author Daniel Kim
 * @brief configurations for the system
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cmath>

#include "timer.h"
#include "../Data/logged_data.h"

#define DEBUG_ON false
#define LIVE_DEBUG false

#define HITL_ON true
#define HITL_LOOP true

#define UI_ON true

#if UI_ON && !HITL_ON
#warning UI requires HITL to be enabled.
#endif

#define PRINT_DATA false //prints out logged data as JSON

#if UI_ON && LIVE_DEBUG 
#warning UI and Debug outputs will collide. Disable one of them.
#endif

#if UI_ON && PRINT_DATA
#warning UI and Data outputs will collide. Disable one of them.
#endif

#define SD_ON false

#define OPTICS_ON false

namespace MissionDuration
{
    constexpr uint16_t days = 0;
    constexpr uint16_t hours = 0;
    constexpr uint32_t minutes = 0;
    constexpr uint32_t seconds = 5000;

    constexpr uint64_t mission_time = ((uint64_t)(days * (uint64_t)8.64e+13) + (uint64_t)(hours * (uint64_t)3.6e+12) + (uint64_t)(minutes * (uint64_t)6e+10) + (uint64_t)(seconds * (uint64_t)1e+9));
}

namespace CPU
{
    /**
     * @brief thresholds for throttling in degrees Celsius
     * Lowers clock speed when temperature is above OVERTEMP_THRESHOLD
     * Raises clock speed when temperature is below UNDERTEMP_THRESHOLD
     */
    constexpr int OVERTEMP_THRESHOLD = 80;
    constexpr int UNDERTEMP_THRESHOLD = 70;

    /**
     * @brief clock speeds in Hz
     * Raises clock speed to HIGH_CLOCKSPEED when temperature is below UNDERTEMP_THRESHOLD
     * Lowers clock speed to LOW_CLOCKSPEED when temperature is above OVERTEMP_THRESHOLD
     */
    constexpr int HIGH_CLOCKSPEED = 528000000;
    constexpr int LOW_CLOCKSPEED = 350000000;

    /**
     * @brief interval for logging CPU info
     */
    constexpr int CPU_INFO_LOG_INTERVAL = HZ_TO_NS(1);
};

namespace Sensors
{
    constexpr Angles_3D<double> mag_bias = {0.36, 0.39, 0.49}; // in uT: set in mag calibration script
}

namespace TransportManager
{
    constexpr int SERIAL_BAUDRATE = 115200;
    constexpr int SEND_INTERVAL = HZ_TO_NS(100);
}

namespace Logging
{
    constexpr int LOG_INTERVAL = HZ_TO_NS(30);
    constexpr int BYTES_PER_LOG = 1536; // bytes per log
    constexpr unsigned long long FLUSH_INTERVAL = SEC_TO_NS(30ULL); // flush every 30 seconds
    constexpr unsigned long long CAPACITY_UPDATE_INTERVAL = SEC_TO_NS(360);// update capacity every 6 minutes
}

constexpr int MIN_PULSE_WIDTH = 1;

#endif