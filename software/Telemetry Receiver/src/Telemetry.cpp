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

namespace Telemetry
{

    void Telemetry::init(int baud_rate)
    {
        Serial1.begin(baud_rate);
    }

    bool Telemetry::receive(FastData &fast_data, SlowData &slow_data)
    {
        if (Serial1.available())
        {
            StaticJsonDocument<1536> doc;
            DeserializationError error = deserializeMsgPack(doc, Serial1);
            if (error == DeserializationError::Ok)
            {
                // If we received a slow packet
                if (doc["telem"][0] == 0x02)
                {
                    SlowData slow_data;
                    slow_data.loop_time = doc["telem"][1];
                    slow_data.sys_state = doc["telem"][2];
                    slow_data.filt_voltage = doc["telem"][3];
                    slow_data.stepper_data.current_position = doc["telem"][4];
                    slow_data.stepper_data.target_position = doc["telem"][5];
                    slow_data.stepper_data.speed = doc["telem"][6];
                    slow_data.stepper_data.acceleration = doc["telem"][7];
                    slow_data.stepper_data.max_speed = doc["telem"][8];
                }
                else if(doc["telem"][0] == 0x00)
                {
                    FastData fast_data;
                    fast_data.wfacc.x = doc["telem"][1];
                    fast_data.wfacc.y = doc["telem"][2];
                    fast_data.wfacc.z = doc["telem"][3];
                    fast_data.gyro.x = doc["telem"][4];
                    fast_data.gyro.y = doc["telem"][5];
                    fast_data.gyro.z = doc["telem"][6];
                    fast_data.mag.x = doc["telem"][7];
                    fast_data.mag.y = doc["telem"][8];
                    fast_data.mag.z = doc["telem"][9];
                    fast_data.ori.x = doc["telem"][10];
                    fast_data.ori.y = doc["telem"][11];
                    fast_data.ori.z = doc["telem"][12];
                    Serial.println(fast_data.ori.x);
                }
                else
                {
                    return false;
                }
            }
            else
            {

                while (Serial1.available() > 0)
                {
                    Serial1.read();
                }
                Serial.println("Serial error");
                return false;
            }
        }
    }
};