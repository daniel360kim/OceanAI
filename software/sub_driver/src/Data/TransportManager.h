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
    struct StepperCommands
    {
        int16_t speed = 0;
        int16_t acceleration = 0;
        uint8_t direction = 0;
    };

    struct Commands
    {
        uint8_t system_state = 0;
        
        StepperCommands buoyancy;
        StepperCommands pitch;

        uint8_t recalibrate_pitch = false;
        uint8_t auto_pitch = false;

        float hitl_scale = 0;

        uint8_t sd_log_enable = 1;
        uint16_t sd_log_interval_hz = 30;
    };

    struct StepperInfo
    {
        int16_t current_position = 0;
        int16_t target_position = 0;
        int16_t speed = 0;
        int16_t acceleration = 0;
    };
    
    class Packet
    {
    public:
        uint16_t loop_time = 0;
        float voltage = 0.f;
        float regulator = 0.f;
        uint8_t system_state = 0;
        float internal_temp = 0.f;

        #if HITL_ON
            HITLData hitl_data;
        #endif

        float hitl_sensor_data[4] = { 0.f };

        float hitl_rate = 0;
        float hitl_progress = 0.f;

        uint16_t sd_log_interval_hz;

        Angles_3D<float> rel_ori = { 0.f };
        float acc[3] = { 0.f };
        float gyr[3] = { 0.f };
        float mag[3] = { 0.f };

        float x;
        float y;
        float z;

        StepperInfo buoyancy;
        StepperInfo pitch;

        Commands commands = {};

        /**
         * @brief Converts the data within LoggedData to the format needed for transmission by the GUI
         * 32 bit float used instead of 64 bit double to save bandwidth
         * Size of types is set (uint8_t, uint16_t, uint32_t, float) to ensure cross platform compatibility
         * @param data LoggedData instance with the data to convert
         */
        void convert(LoggedData &data)
        {
            //Convert to set size since data is transmitted to different devices
            loop_time = static_cast<uint16_t>(data.loop_time);
            voltage = static_cast<float>(data.raw_voltage);
            regulator = static_cast<float>(data.raw_regulator);
            system_state = static_cast<uint8_t>(data.system_state);
            internal_temp = static_cast<float>(data.bmi_temp);

            hitl_data = data.HITL;

            //seconds to hours
            hitl_data.timestamp /= 3600;

            hitl_sensor_data[0] = static_cast<float>(hitl_data.depth);
            hitl_sensor_data[1] = static_cast<float>(hitl_data.pressure);
            hitl_sensor_data[2] = static_cast<float>(hitl_data.salinity);
            hitl_sensor_data[3] = static_cast<float>(hitl_data.temperature);

            hitl_rate = data.hitl_rate;
            hitl_progress = static_cast<float>(data.hitl_progress);

            sd_log_interval_hz = data.sd_log_rate_hz;

            rel_ori.x = static_cast<float>(data.rel_ori.x);
            rel_ori.y = static_cast<float>(data.rel_ori.y);
            rel_ori.z = static_cast<float>(data.rel_ori.z);

            gyr[0] = static_cast<float>(data.rgyr.x);
            gyr[1] = static_cast<float>(data.rgyr.y);
            gyr[2] = static_cast<float>(data.rgyr.z);

            acc[0] = static_cast<float>(data.racc.x);
            acc[1] = static_cast<float>(data.racc.y);
            acc[2] = static_cast<float>(data.racc.z);

            mag[0] = static_cast<float>(data.rmag.x);
            mag[1] = static_cast<float>(data.rmag.y);
            mag[2] = static_cast<float>(data.rmag.z);

            //Translated orientation values for 3D model 
            x = static_cast<float>(data.rel_ori.x * DEG_TO_RAD);
            y = static_cast<float>(data.rel_ori.y * DEG_TO_RAD);
            z = static_cast<float>(data.rel_ori.z * DEG_TO_RAD);

            buoyancy.acceleration = static_cast<int16_t>(data.dive_stepper.acceleration);
            buoyancy.current_position = static_cast<int16_t>(data.dive_stepper.current_position);
            buoyancy.target_position = static_cast<int16_t>(data.dive_stepper.target_position);
            buoyancy.speed = static_cast<int16_t>(data.dive_stepper.speed);

            pitch.acceleration = static_cast<int16_t>(data.pitch_stepper.acceleration);
            pitch.current_position = static_cast<int16_t>(data.pitch_stepper.current_position) * -1;
            pitch.target_position = static_cast<int16_t>(data.pitch_stepper.target_position);
            pitch.speed = static_cast<int16_t>(data.pitch_stepper.speed);

            buoyancy.acceleration = std::abs<int16_t>(buoyancy.acceleration);
            buoyancy.current_position = std::abs<int16_t>(buoyancy.current_position);
            buoyancy.target_position = std::abs<int16_t>(buoyancy.target_position);
            buoyancy.speed = std::abs<int16_t>(buoyancy.speed);

            pitch.acceleration = std::abs<int16_t>(pitch.acceleration);
            pitch.current_position = std::abs<int16_t>(pitch.current_position);
            pitch.target_position = std::abs<int16_t>(pitch.target_position);
            pitch.speed = std::abs<int16_t>(pitch.speed);

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