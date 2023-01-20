/**
 * @file hitl.h
 * @author Daniel Kim   
 * @brief Hardware in the loop simulation
 * @version 0.1
 * @date 2023-01-19
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef HITL_H
#define HITL_H

#include <array>

#include "../core/configuration.h"

namespace HITL
{
    /**
     * @brief The hardware in the loop simulation data
     * 7 columns: time, latitude, longitude, depth, pressure, salinity, temperature
     * 4337 rows
     * 
     */
    EXTMEM std::array<std::array<double, 7>, 4337> HITL_data;
}











#endif