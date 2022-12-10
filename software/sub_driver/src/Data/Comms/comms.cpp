/**
 * @file comms.cpp
 * @author Daniel Kim
 * @brief Controls communications between the main board and the ccmmunication board
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#include "comms.h"

namespace Telemetry
{
    void Telemetry::init(const int baudrate, const long interval_ns)
    {
        Serial5.begin(baudrate);
        m_interval_ns = interval_ns;
    }

    void Telemetry::sendTelemetry(TransmissionData &send_data)
    {
        int64_t current_time = scoped_timer.elapsed();
        if(current_time - m_previous_send_time <= m_interval_ns)
        {
            return;
        }
        else
        {
            m_previous_send_time = current_time;
            StaticJsonDocument<TELEM_STATIC_JSON_DOC_SIZE> doc;
            send_data.to_json(doc);

            serializeMsgPack(doc, Serial5);
        }
    }
}
