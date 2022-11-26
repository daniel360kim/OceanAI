/**
 * @file Telemetry.cpp
 * @author Daniel Kim
 * @brief Receives data from the main board through Serial
 * @version 0.1
 * @date 2022-11-25
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "Telemetry.h"

void Telemetry::init(int baud_rate)
{
    Serial1.begin(baud_rate);
}

bool Telemetry::receive(Data &data)
{
    if(Serial1.available())
    {
        StaticJsonDocument<1536> doc;
        DeserializationError error = deserializeMsgPack(doc, Serial1);
        if(error == DeserializationError::Ok)
        {
            Data::json_to_data(data, doc);
        }
        else
        {

            while(Serial1.available() > 0)
            {
                Serial1.read();
            }

            return false;
        }

    }
}
