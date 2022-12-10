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

        void init(const int baudrate, const long interval_ns);

        void sendTelemetry(TransmissionData &send_data);
        void receiveCommand();

    private:
        long m_interval_ns;
        int64_t m_previous_send_time;
    };
};

#endif