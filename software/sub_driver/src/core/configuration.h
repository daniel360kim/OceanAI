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

#include "timer.h"
#include "../Data/logged_data.h"

#define DEBUG_ON true
#define LIVE_DEBUG true

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
    constexpr int CPU_INFO_LOG_INTERVAL = HZ_TO_NS(5);
};


namespace Sensors
{
    constexpr Angles_3D<double> mag_bias = { 0.36, 0.39, 0.49 }; // in uT: set in mag calibration script

}


#endif