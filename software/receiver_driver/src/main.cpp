/**
 * @file main.cpp
 * @author Daniel Kim
 * @brief Receiver for the submarine that connects to the GUI application
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
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

RF24 radio(10, 11);             // CE, CSN
RF24Network network(radio);     // Network uses that radio
const uint16_t this_node = 00;  // Address of our node in Octal format (04, 031, etc)
const uint16_t other_node = 01; // Address of the other node in Octal format

void setup()
{
    Serial.begin(2000000);
    if (!radio.begin())
    {
        while (1)
        {
            Serial.println("Rad. not responding");
        }
    }

    radio.setChannel(90);
    network.begin(this_node);
}

unsigned long previousLog;
CombinedData datacopy;

/**
 * @brief converts angle wrap [0,360] to [-180,180] since thats what our GUI application wants
 *
 * @param x angle
 * @return double output
 */
double tohalf(double x)
{
    x = fmod(x + 180, 360);
    if (x < 0)
    {
        x += 360;
    }

    return x - 180;
}

void loop()
{
    network.update();
    // put your main code here, to run repeatedly:
    unsigned long currentLog = micros();

    if (network.available())
    {

        RF24NetworkHeader header;                              // If so, grab it and print it out
        network.read(header, &datacopy, sizeof(CombinedData)); // Get the data

        previousLog = currentLog;

        char comma[] = " ";

        unsigned long time = datacopy.d.time_us;
        Serial.print(time);
        Serial.print(F(comma));
        Serial.print(datacopy.d.bmp_rpres, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.bmp_rtemp, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.wfacc.x, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.wfacc.y, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.wfacc.z, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.fgyr.x, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.fgyr.y, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.fgyr.z, 10);
        Serial.print(F(comma));

        datacopy.d.rel_ori.x = fmod(datacopy.d.rel_ori.x, 360);
        if (datacopy.d.rel_ori.x < 0)
        {
            datacopy.d.rel_ori.x += 360;
        }

        Serial.print(datacopy.d.rel_ori.x);
        Serial.print(" ");
        Serial.print(tohalf(datacopy.d.rel_ori.y));
        Serial.print(" ");

        Serial.print(tohalf(datacopy.d.rel_ori.z));

        Serial.print(F(comma));
        Serial.print(datacopy.d.bmi_temp, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.mag.x, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.mag.y, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.mag.z, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.voltage, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.d.internal_temp, 10);
        Serial.print(F(comma));

        ///////////GPS//////////////
        Serial.print(datacopy.g.metadata.charsProcessed);

        Serial.print(F(comma));
        Serial.print(datacopy.g.location.latitude.coordinate, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.g.location.longitude.coordinate, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.g.speed.mps, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.g.course.deg, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.g.altitude.meters, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.g.satellites.value);
        Serial.print(F(comma));
        Serial.print(datacopy.g.hdop.hdop, 10);
        Serial.print(F(comma));
        Serial.print(datacopy.g.metadata.passedChecksum);
        Serial.print(F(comma));
        Serial.print(datacopy.g.metadata.failedChecksum);
        Serial.print(F(comma));
        Serial.print(datacopy.d.dt,20);
        Serial.print(F(comma));
        Serial.print(datacopy.d.sd_capacity);
        Serial.print(F(comma));
        Serial.print(datacopy.d.system_state);
        Serial.print("\n");
    }
}
