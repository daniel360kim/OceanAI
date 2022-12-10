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

    bool Telemetry::receive(TransmissionData &send_data)
    {
        if (Serial1.available())
        {
            StaticJsonDocument<TELEM_STATIC_JSON_DOC_SIZE> doc;
            DeserializationError error = deserializeMsgPack(doc, Serial1);
            if (error == DeserializationError::Ok)
            {
               send_data.loop_time = doc[0];
               send_data.system_state = doc[1];
               send_data.delta_time = doc[2];
               send_data.internal_temp = doc[3];
               send_data.wfacc.x = doc[4]; send_data.wfacc.y = doc[5]; send_data.wfacc.z = doc[6];
               send_data.rgyr.x = doc[7]; send_data.rgyr.y = doc[8]; send_data.rgyr.z = doc[9];
               send_data.mag.x = doc[10]; send_data.mag.y = doc[11]; send_data.mag.z = doc[12];
               send_data.rel_ori.x = doc[13]; send_data.rel_ori.y = doc[14]; send_data.rel_ori.z = doc[15];

               send_data.filt_ext_pres = doc[16];
               send_data.filt_ext_temp = doc[17];

               send_data.dive_stepper.limit_state = doc[18];
               send_data.dive_stepper.current_position = doc[19];
               send_data.dive_stepper.target_position = doc[20];
               send_data.dive_stepper.speed = doc[21];
               send_data.dive_stepper.acceleration = doc[22];
               send_data.dive_stepper.max_speed = doc[23];

               prev_data = send_data;
               return true;
            }
            else
            {
                while (Serial1.available() > 0)
                {
                    Serial1.read();
                }
                send_data = prev_data;
                return false;
            }
        }
        else
        {
            send_data = prev_data;
            return false;
        }
    }
};