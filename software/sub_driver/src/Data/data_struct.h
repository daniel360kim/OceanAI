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


//To do: organize into different structs for optimization and organization
class Data
{
public:
    int64_t time_ns;
    int loop_time;
    int system_state;
    double delta_time;
    uint32_t sd_capacity;

    double raw_voltage;
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

    Angles_4D relative;

    Angles_3D rmag;
    Angles_3D fmag;

    double raw_TDS;
    double filt_TDS;
    
    double raw_ext_pres;
    double filt_ext_pres;

    StepperData dive_stepper;
    StepperData pitch_stepper;
    
    OpticalData optical_data;

    /**
     * @brief Prints out all the data to a printing object
     * 
     * @param p printer
     * @param delim how the data should be delimited
     * @param data data class to be printed
     */
    static void printData(Print &p, const char* delim, const Data &data)
    {
        p.print(data.time_ns); p.print(delim);
        p.print(data.loop_time); p.print(delim);
        p.print(data.system_state); p.print(delim);
        p.print(data.delta_time); p.print(delim);
        p.print(data.sd_capacity); p.print(delim);
        p.print(data.raw_voltage); p.print(delim);
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
        p.print(data.relative.w); p.print(delim);
        p.print(data.relative.x); p.print(delim);
        p.print(data.relative.y); p.print(delim);
        p.print(data.relative.z); p.print(delim);
        p.print(data.rmag.x); p.print(delim);
        p.print(data.rmag.y); p.print(delim);
        p.print(data.rmag.z); p.print(delim);
        p.print(data.fmag.x); p.print(delim);
        p.print(data.fmag.y); p.print(delim);
        p.print(data.fmag.z); p.print(delim);
        p.print(data.raw_TDS); p.print(delim);
        p.print(data.filt_TDS); p.print(delim);
        p.print(data.raw_ext_pres); p.print(delim);
        p.print(data.filt_ext_pres); p.print(delim);
        p.print(data.dive_stepper.limit_state); p.print(delim);
        p.print(data.dive_stepper.homed); p.print(delim);
        p.print(data.dive_stepper.current_position); p.print(delim);
        p.print(data.dive_stepper.current_position_mm); p.print(delim);
        p.print(data.dive_stepper.target_position); p.print(delim);
        p.print(data.dive_stepper.target_position_mm); p.print(delim);
        p.print(data.dive_stepper.speed); p.print(delim);
        p.print(data.dive_stepper.acceleration); p.print(delim);
        p.print(data.dive_stepper.max_speed); p.print(delim);
        p.print(data.pitch_stepper.limit_state); p.print(delim);
        p.print(data.pitch_stepper.homed); p.print(delim);
        p.print(data.pitch_stepper.current_position); p.print(delim);
        p.print(data.pitch_stepper.current_position_mm); p.print(delim);
        p.print(data.pitch_stepper.target_position); p.print(delim);
        p.print(data.pitch_stepper.target_position_mm); p.print(delim);
        p.print(data.pitch_stepper.speed); p.print(delim);
        p.print(data.pitch_stepper.acceleration); p.print(delim);
        p.print(data.pitch_stepper.max_speed); p.print(delim);
        p.print(data.optical_data.capture_time); p.print(delim);
        p.print(data.optical_data.save_time); p.print(delim);
        p.print(data.optical_data.FIFO_length); p.print("\n");
    }
    
    //65 total elements
};





#endif