/**
 * @file GPS.h
 * @author Daniel Kim
 * @brief GPS Utilization
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <Arduino.h>

#include "../Data/data_struct.h"

class GPS
{
public:
    TinyGPSPlus gps;
    GPS(const uint32_t GPSBaud);

    void updateData(GPSdata &data);
    
    void setDestination(double lat, double lon);
    void unsetCourse() { course_set = false; }
    double getDistance();
    double getCourse();
    
private:
    void updateLocation(GPS_data::Location &location);
    void updateDate(GPS_data::Date &date);
    void updateTime(GPS_data::Time &time);
    void updateSpeed(GPS_data::Speed &speed);
    void updateCourse(GPS_data::Course &course);
    void updateAltitude(GPS_data::Altitude &altitude);
    void updateSatellites(GPS_data::Satellites &satellites);
    void updateHDOP(GPS_data::HDOP &hdop);
    void updateMetadata(GPS_data::Metadata &metadata);

    double dest_lat, dest_lon;
    bool course_set = false;
    

};


#endif