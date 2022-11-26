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


void Telemetry::init(const int baudrate, const long interval_ns)
{
    Serial5.begin(baudrate);
    m_interval_ns = interval_ns;
}

void Telemetry::sendTelemetry(StaticJsonDocument<STATIC_JSON_DOC_SIZE> &doc)
{
    int64_t current_time = scoped_timer.elapsed();
    
    if(current_time - m_previous_send_time >= m_interval_ns)
    {
        m_previous_send_time = current_time;
        serializeMsgPack(doc, Serial5);
        Serial5.println();
    }

}
