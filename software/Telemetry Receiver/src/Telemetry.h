/**
 * @file Telemetry.h
 * @author Daniel Kim
 * @brief Receives data from the main board through Serial
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <Arduino.h>

#include "data_struct.h"

namespace Telemetry
{

    class Telemetry
    {
    public:
        Telemetry() {}
        void init(int baud_rate);

        bool receive(FastData &fast_data, SlowData &slow_data);

    private:
        Data previous_data;
    };

}

#endif