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


class Quaternion
{
public:
    double w;
    double x;
    double y;
    double z;

    struct Norm
    {
        double norm;
        double theta;
    };
    
    Quaternion() { w = 1, x = 0, y = 0, z = 0; }
    Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}

    void toAxis(double gx, double gy, double gz, double dt);
    static Quaternion hamiltonProduct(Quaternion A, Quaternion B);
    static Norm findNorm(double gx, double gy, double gz, double dt);

};
namespace GPS_data
{
    struct Coordinates
    {
        double degrees, coordinate;
    }; 
    struct Location
    {
        double age;
        Coordinates latitude;
        Coordinates longitude; 
    };

    struct Date
    {
        uint32_t age;
        double year;
        double month;
        double day;
    };

    struct Time
    {
        uint32_t age;
        double hour;
        double minute;
        double second;
        double centisecond;
    };

    struct Speed
    {
        uint32_t age;
        double knots;
        double mph;
        double mps;
    };

    struct Course
    {
        uint32_t age;
        double deg;
    };

    struct Altitude
    {
        uint32_t age;
        double meters;
    };

    struct Satellites
    {
        uint32_t age;
        uint32_t value;
    };

    struct HDOP
    {
        uint32_t age;
        double hdop;
    };

    struct Metadata
    {
        uint32_t charsProcessed;
        uint32_t sentencesWithFix;
        uint32_t failedChecksum;
        uint32_t passedChecksum;
    };

    struct Navigation
    {
        double dest_latitude, dest_longitude;
        double distance_between, course_to;
        bool course_set;
        bool data_valid;
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
    uint64_t time_us;
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
    unsigned long long time_us;
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
    GPS_data::Navigation navigation;
};

#endif