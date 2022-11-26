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
#include <stdint.h>
#include <ArduinoJson.h>

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

struct Angles_3D
{
    double x, y, z;
};

struct Angles_4D
{
    double w, x, y, z;
};

struct BMP388Data
{
    double pressure;
    double temperature;
};


static constexpr int STATIC_JSON_DOC_SIZE = 1536;
//To do: organize into different structs for optimization and organization
class Data
{
public:
    int64_t time_ns;
    int loop_time;
    int system_state;

    double filt_voltage;
    int clock_speed;
    double internal_temp;

    BMP388Data raw_bmp;

    double bmi_temp;
    Angles_3D racc;
    Angles_3D wfacc;
    Angles_3D vel;
    Angles_3D pos;

    Angles_3D rgyr;
    Angles_3D rel_ori;

    Angles_3D fmag;

    double filt_TDS;
    
    double filt_ext_pres;

    double filt_ext_temp;

    StepperData dive_stepper;
    
    static void printData(Print &p, const char* delim, const Data &data)
    {
        p.print(data.time_ns); p.print(delim);
        p.print(data.loop_time); p.print(delim);
        p.print(data.system_state); p.print(delim);
        p.print(data.filt_voltage); p.print(delim);
        p.print(data.clock_speed); p.print(delim);
        p.print(data.internal_temp); p.print(delim);

        p.print(data.raw_bmp.pressure); p.print(delim);
        p.print(data.raw_bmp.temperature); p.print(delim);

        p.print(data.bmi_temp); p.print(delim);
        p.print(data.racc.x); p.print(delim);
        p.print(data.racc.y); p.print(delim);
        p.print(data.racc.z); p.print(delim);
        p.print(data.wfacc.x); p.print(delim);
        p.print(data.wfacc.y); p.print(delim);
        p.print(data.wfacc.z); p.print(delim);
        p.print(data.vel.x); p.print(delim);
        p.print(data.vel.y); p.print(delim);
        p.print(data.vel.z); p.print(delim);
        p.print(data.pos.x); p.print(delim);
        p.print(data.pos.y); p.print(delim);
        p.print(data.pos.z); p.print(delim);
        p.print(data.rgyr.x); p.print(delim);
        p.print(data.rgyr.y); p.print(delim);
        p.print(data.rgyr.z); p.print(delim);
        p.print(data.rel_ori.x); p.print(delim);
        p.print(data.rel_ori.y); p.print(delim);
        p.print(data.rel_ori.z); p.print(delim);
        p.print(data.fmag.x); p.print(delim);
        p.print(data.fmag.y); p.print(delim);
        p.print(data.fmag.z); p.print(delim);

        p.print(data.filt_TDS); p.print(delim);
        p.print(data.filt_ext_pres); p.print(delim);
        p.print(data.filt_ext_temp); p.print(delim);

        p.print(data.dive_stepper.limit_state); p.print(delim);
        p.print(data.dive_stepper.homed); p.print(delim);
        p.print(data.dive_stepper.current_position); p.print(delim);
        p.print(data.dive_stepper.current_position_mm); p.print(delim);
        p.print(data.dive_stepper.target_position); p.print(delim);
        p.print(data.dive_stepper.target_position_mm); p.print(delim);
        p.print(data.dive_stepper.speed); p.print(delim);
        p.print(data.dive_stepper.acceleration); p.print(delim);
        p.print(data.dive_stepper.max_speed); p.print(delim);

        p.println();

    }

    static void json_to_data(Data &data, StaticJsonDocument<1536> &doc)
    {
        data.time_ns = doc["time"];
        data.loop_time = doc["sys_data"][0];
        data.system_state = doc["sys_data"][1];
        data.filt_voltage = doc["sys_data"][2];
        data.clock_speed = doc["sys_data"][3];
        data.internal_temp = doc["sys_data"][4];

        data.raw_bmp.pressure = doc["baro_data"][0];
        data.raw_bmp.temperature = doc["baro_data"][1];

        data.bmi_temp = doc["IMU_data"][0];
        data.racc.x = doc["IMU_data"][1]; data.racc.y = doc["IMU_data"][2]; data.racc.z = doc["IMU_data"][3];
        data.wfacc.x = doc["IMU_data"][4]; data.wfacc.y = doc["IMU_data"][5]; data.wfacc.z = doc["IMU_data"][6];
        data.vel.x = doc["IMU_data"][7]; data.vel.y = doc["IMU_data"][8]; data.vel.z = doc["IMU_data"][9];
        data.pos.x = doc["IMU_data"][10]; data.pos.y = doc["IMU_data"][11]; data.pos.z = doc["IMU_data"][12];
        data.rgyr.x = doc["IMU_data"][13]; data.rgyr.y = doc["IMU_data"][14]; data.rgyr.z = doc["IMU_data"][15];
        data.rel_ori.x = doc["IMU_data"][16]; data.rel_ori.y = doc["IMU_data"][17]; data.rel_ori.z = doc["IMU_data"][18];
        data.fmag.x = doc["IMU_data"][19]; data.fmag.y = doc["IMU_data"][20]; data.fmag.z = doc["IMU_data"][21];

        data.filt_TDS = doc["external_data"][0];
        data.filt_ext_pres = doc["external_data"][1];
        data.filt_ext_temp = doc["external_data"][2];

        data.dive_stepper.limit_state = doc["step_data"][0];
        data.dive_stepper.homed = doc["step_data"][1];
        data.dive_stepper.current_position = doc["step_data"][2];
        data.dive_stepper.current_position_mm = doc["step_data"][3];
        data.dive_stepper.target_position = doc["step_data"][4];
        data.dive_stepper.target_position_mm = doc["step_data"][5];
        data.dive_stepper.speed = doc["step_data"][6];
        data.dive_stepper.acceleration = doc["step_data"][7];
        data.dive_stepper.max_speed = doc["step_data"][8];
    }

};





#endif