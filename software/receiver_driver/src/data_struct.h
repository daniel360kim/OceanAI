#ifndef data_struct_h
#define data_struct_h

#include <Arduino.h>
namespace GPS_data
{
    struct Coordinates
    {
        double degrees, billionths, coordinate;
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
        uint32_t value;
        double year;
        double month;
        double day;
    };

    struct Time
    {
        uint32_t age;
        uint32_t value;
        double hour;
        double minute;
        double second;
        double centisecond;
    };

    struct Speed
    {
        uint32_t age;
        uint32_t value;
        double knots;
        double mph;
        double mps;
    };

    struct Course
    {
        uint32_t age;
        uint32_t value;
        double deg;
    };

    struct Altitude
    {
        uint32_t age;
        uint32_t value;
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
        uint32_t value;
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

struct PID_values
{
    double setpoint;
    double output;
};

struct StepperData
{
    bool limit_state;
    bool homed;
    bool sleep;
    double predicted_position;
};

struct OpticalData
{
    unsigned long capture_time;
    unsigned long save_time;
    uint8_t FIFO_length;
};

struct Vec3
{
    double x, y, z;
};

struct Vec4
{
    double w, x, y, z;
};

// To do: organize into different structs for optimization and organization
struct Data
{
    unsigned long long time_us;
    uint8_t system_state;

    double dt;
    double bmp_rpres, bmp_rtemp, bmp_fpres, bmp_ftemp; // from bmp388

    Vec3 racc;
    Vec3 facc;
    Vec3 wfacc;
    Vec3 vel;
    Vec3 pos;

    Vec3 rgyr;
    Vec3 fgyr;
    Vec3 rel_ori;
    Vec3 abs_ori;

    Vec4 relative;
    Vec4 absolute;

    double bmi_temp;

    Vec3 mag;

    double ext_rtemp, ext_rpres, ext_ftemp, ext_fpres;

    double TDS;
    double voltage;

    unsigned int clock_speed;
    double internal_temp;

    PID_values dive_pid;
    PID_values pitch_pid;

    StepperData dive_stepper;
    StepperData pitch_stepper;

    OpticalData optical_data;

    unsigned long sd_capacity;
};
struct GPSdata
{
    unsigned long long time_us;
    bool GPS_activated;

    // GPS Data
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

struct CombinedData
{
    Data d;
    GPSdata g;
};



#endif