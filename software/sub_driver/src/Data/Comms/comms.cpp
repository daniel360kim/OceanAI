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
    void Telemetry::init(const int baudrate, const long fast_interval_ns, const long slow_interval_ns)
    {
        Serial5.begin(baudrate);
        m_fast_interval_ns = fast_interval_ns;
        m_slow_interval_ns = slow_interval_ns;
    }

    void Telemetry::sendTelemetry(FastData &fast_data, SlowData &slow_data)
    {
        int64_t current_time = scoped_timer.elapsed();
        if(current_time - m_previous_fast_send_time < m_fast_interval_ns && current_time - m_previous_slow_send_time < m_slow_interval_ns)
        {
            return; // Don't send telemetry if it's not time yet
        }
        else if(current_time - m_previous_fast_send_time >= m_fast_interval_ns && current_time - m_previous_slow_send_time >= m_slow_interval_ns)
        {
            //If both fast and slow data need to be sent
            StaticJsonDocument<TELEM_STATIC_JSON_DOC_SIZE> fast_json_doc;
            fast_data.to_json(fast_json_doc);

            StaticJsonDocument<TELEM_STATIC_JSON_DOC_SIZE> slow_json_doc;
            slow_data.to_json(slow_json_doc);

            serializeMsgPack(fast_json_doc, Serial5);
            serializeMsgPack(slow_json_doc, Serial5);

            m_previous_fast_send_time = current_time;
            m_previous_slow_send_time = current_time;
        }
        else if(current_time - m_previous_fast_send_time >= m_fast_interval_ns)
        {
            //If only fast data needs to be sent
            StaticJsonDocument<TELEM_STATIC_JSON_DOC_SIZE> fast_json_doc;
            fast_data.to_json(fast_json_doc);

            serializeMsgPack(fast_json_doc, Serial5);

            m_previous_fast_send_time = current_time;
        }
        else if(current_time - m_previous_slow_send_time >= m_slow_interval_ns)
        {
            //If only slow data needs to be sent
            StaticJsonDocument<TELEM_STATIC_JSON_DOC_SIZE> slow_json_doc;
            slow_data.to_json(slow_json_doc);

            serializeMsgPack(slow_json_doc, Serial5);

            m_previous_slow_send_time = current_time;
        }
        else
        {
            return; // Should never reach here
        }
    }
}
