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
struct Data
{
    int64_t time_ns;
    int loop_time;

    int system_state;

    double delta_time;

    BMP388Data raw_bmp;

    Angles_3D racc;
    Angles_3D wfacc;
    Angles_3D vel;
    Angles_3D pos;

    Angles_3D rgyr;
    Angles_3D rel_ori;

    Angles_4D relative;

    double bmi_temp;

    Angles_3D rmag;
    Angles_3D fmag;

    double raw_TDS;
    double filt_TDS;
    
    double raw_ext_pres;
    double filt_ext_pres;

    double raw_voltage;
    double filt_voltage;

    int clock_speed;
    double internal_temp;

    StepperData dive_stepper;
    StepperData pitch_stepper;
    
    OpticalData optical_data;

    uint32_t sd_capacity;

};





#endif