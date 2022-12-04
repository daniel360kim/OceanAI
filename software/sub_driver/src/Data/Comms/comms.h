/**
 * @file comms.h
 * @author Daniel Kim
 * @brief Controls communications between the main board and the ccmmunication board
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#ifndef comms_h
#define comms_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

#include "../data_struct.h"

#include "../../core/Timer.h"
#include "../../core/debug.h"

namespace Telemetry
{
    class Telemetry
    {
    public:
        Telemetry() {}

        void init(const int baudrate, const long fast_interval_ns, const long slow_interval_ns);

        void sendTelemetry(FastData &fast_data, SlowData &slow_data);
        void receiveCommand();

    private:
        long m_fast_interval_ns, m_slow_interval_ns;
        int64_t m_previous_fast_send_time, m_previous_slow_send_time;
    };
};

#endif