/**
 * @file TransportManager.h
 * @author Daniel Kim
 * @brief Communications with the GUI
 * @version 0.1
 * @date 2022-12-29
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#ifndef TRANSPORTMANAGER_H
#define TRANSPORTMANAGER_H

#include <electricui.h>

#include "../core/configuration.h"
#include "logged_data.h"

namespace TransportManager
{
    struct Commands
    {
        uint8_t system_state = 0;
        int16_t stepper_speed = 0;
        int16_t stepper_acceleration = 0;
    };

    class Packet
    {
    public:
        uint16_t loop_time = 0;
        float voltage = 0.f;
        uint8_t system_state = 0;
        float internal_temp = 0.f;

        Angles_3D<float> rel_ori = {0.f};
        float acc[3] = {0.f};
        float gyr[3] = {0.f};

        int16_t stepper_current_position = 0;
        int16_t stepper_target_position = 0;
        int16_t stepper_speed = 0;
        int16_t stepper_acceleration = 0;

        Commands commands = {};

        void convert(LoggedData &data)
        {
            //Convert to set size since data is transmitted to different devices
            loop_time = static_cast<uint16_t>(data.loop_time);
            voltage = static_cast<float>(data.filt_voltage);
            system_state = static_cast<uint8_t>(data.system_state);
            internal_temp = static_cast<float>(data.bmi_temp);

            rel_ori.x = static_cast<float>(data.rel_ori.x);
            rel_ori.y = static_cast<float>(data.rel_ori.y);
            rel_ori.z = static_cast<float>(data.rel_ori.z);

            gyr[0] = static_cast<float>(data.rgyr.x);
            gyr[1] = static_cast<float>(data.rgyr.y);
            gyr[2] = static_cast<float>(data.rgyr.z);

            acc[0] = static_cast<float>(data.racc.x);
            acc[1] = static_cast<float>(data.racc.y);
            acc[2] = static_cast<float>(data.racc.z);

            stepper_acceleration = static_cast<int16_t>(data.dive_stepper.acceleration);
            stepper_current_position = static_cast<int16_t>(data.dive_stepper.current_position);
            stepper_target_position = static_cast<int16_t>(data.dive_stepper.target_position);
            stepper_speed = static_cast<int16_t>(data.dive_stepper.speed);

            stepper_acceleration = std::abs<int16_t>(stepper_acceleration);
            stepper_current_position = std::abs<int16_t>(stepper_current_position);
            stepper_target_position = std::abs<int16_t>(stepper_target_position);
            stepper_speed = std::abs<int16_t>(stepper_speed);
        }
    };

    void serial_write(uint8_t *data, uint16_t len);
    void serial_rx_handler();

    void init();

    bool handleTransport(LoggedData &logged_data);

    Commands getCommands();

    void setIdle(bool command);
}

#endif