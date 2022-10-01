/**
 * @file data_struct.h
 * @author Daniel Kim
 * @brief the data struct where data to be logged is saved
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#ifndef Data_struct_h
#define Data_struct_h

#include <Arduino.h>
#include <chrono>

#include "navigation/Quaternion.h"
namespace GPS_data
{
    struct Coordinates
    {
        float degrees, coordinate;
    }; 
    struct Location
    {
        Coordinates latitude;
        Coordinates longitude; 
    };

    struct Date
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
    };

    struct Time
    {
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t centisecond;
    };

    struct Speed
    {
        float knots;
        float mph;
        float mps;
    };

    struct Course
    {
        float deg;
    };

    struct Altitude
    {
        float meters;
    };

    struct Satellites
    {
        uint32_t value;
    };

    struct HDOP
    {
        float hdop;
    };

    struct Metadata
    {
        uint32_t charsProcessed;
        uint32_t sentencesWithFix;
        uint32_t failedChecksum;
        uint32_t passedChecksum;
    };
};

struct StepperData
{
    bool limit_state;
    bool homed;
    double current_position;
    double current_position_mm;

    double target_position;
    double target_position_mm;

    double speed;
    double acceleration;
    double max_speed;
};

struct OpticalData
{
    uint32_t capture_time;
    uint32_t save_time;
    uint32_t FIFO_length;
};

struct Vec3
{
    double x, y, z;
};

struct ExternalData
{
    float loop_time;
    float raw_temp;
    float raw_pres;

    double filt_temp;
    double filt_pres;
};

//To do: organize into different structs for optimization and organization
struct Data
{
    int64_t time_ns;
    uint32_t loop_time;

    uint8_t system_state;

    double dt;
    double bmp_rpres, bmp_rtemp, bmp_fpres, bmp_ftemp; //from bmp388

    Vec3 racc;
    Vec3 facc;
    Vec3 wfacc;
    Vec3 vel;
    Vec3 pos;

    Vec3 rgyr;
    Vec3 fgyr;
    Vec3 rel_ori;

    Quaternion relative;

    double bmi_temp;

    Vec3 mag;

    ExternalData external;

    double TDS;
    double voltage;

    uint16_t clock_speed;
    double internal_temp;

    StepperData dive_stepper;
    StepperData pitch_stepper;

    bool sinking;
    bool rising;

    OpticalData optical_data;

    uint32_t sd_capacity;

};

//int size = sizeof(Data);

struct GPSdata
{   
    bool GPS_activated;
    //GPS Data
    GPS_data::Location location;    
    GPS_data::Date date;
    GPS_data::Time time;
    GPS_data::Speed speed;
    GPS_data::Course course;
    GPS_data::Altitude altitude;
    GPS_data::Satellites satellites;
    GPS_data::HDOP hdop;
    GPS_data::Metadata metadata;
};

struct TransmissionPacket
{
    float ax; 
    float ay;
    float az;

    float x;
    float y;
    float z;

    bool limit_state_p;
    bool homed_p;
    float current_position_p;
    float current_position_mm_p;

    float target_position_p;
    float target_position_mm_p;

    float speed_p;
    float acceleration_p;
    float max_speed_p;

    bool limit_state_b;
    bool homed_b;
    float current_position_b;
    float current_position_mm_b;

    float target_position_b;
    float target_position_mm_b;

    float speed_b;
    float acceleration_b;
    float max_speed_b;

    bool sinking;
    bool rising;

    //converts data to the transmission packet
    void set(Data data)
    {
        ax = data.wfacc.x;
        ay = data.wfacc.y;
        az = data.wfacc.z;

        x = data.rel_ori.x;
        y = data.rel_ori.y;
        z = data.rel_ori.z;

        limit_state_b = data.dive_stepper.limit_state;
        homed_b = data.dive_stepper.homed;
        current_position_b = data.dive_stepper.current_position;
        current_position_mm_b = data.dive_stepper.current_position_mm;
        target_position_b = data.dive_stepper.target_position;
        target_position_mm_b = data.dive_stepper.target_position_mm;
        speed_b = data.dive_stepper.speed;
        acceleration_b = data.dive_stepper.acceleration;
        max_speed_b = data.dive_stepper.max_speed;

        limit_state_p = data.pitch_stepper.limit_state;
        homed_p = data.pitch_stepper.homed;
        current_position_p = data.pitch_stepper.current_position;
        current_position_mm_p = data.pitch_stepper.current_position_mm;
        target_position_p = data.pitch_stepper.target_position;
        target_position_mm_p = data.pitch_stepper.target_position_mm;
        speed_p = data.pitch_stepper.speed;
        acceleration_p = data.pitch_stepper.acceleration;
        max_speed_p = data.pitch_stepper.max_speed;

        sinking = data.sinking;
        rising = data.rising;
    }
};



#endif