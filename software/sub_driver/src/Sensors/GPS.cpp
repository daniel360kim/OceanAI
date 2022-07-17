/**
 * @file GPS.cpp
 * @author Daniel Kim
 * @brief GPS Utilization
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#include <Arduino.h>

#include "GPS.h"

GPS::GPS(const uint32_t GPSBaud)
{
    Serial5.begin(GPSBaud);
}

void GPS::updateData(GPSdata &data)
{
    data.time_us = micros();
    if(Serial5.available() > 0)
    {
        gps.encode(Serial5.read());
    }
    else
    {
        data.GPS_activated = false;
    }

    if(gps.location.isUpdated())
    {
        updateLocation(data.location);
    }

    if(gps.date.isUpdated())
    {
        updateDate(data.date);
    }

    if(gps.time.isUpdated())
    {
        updateTime(data.time);
    }

    if(gps.speed.isUpdated())
    {
        updateSpeed(data.speed);
    }

    if(gps.course.isUpdated())
    {
        updateCourse(data.course);
    }

    if(gps.altitude.isUpdated())
    {
        updateAltitude(data.altitude);
    }

    if(gps.satellites.isUpdated())
    {
        updateSatellites(data.satellites);
    }

    if(gps.hdop.isUpdated())
    {
        updateHDOP(data.hdop);
    }
    
    updateMetadata(data.metadata);

    data.navigation.course_set = course_set;
    data.navigation.data_valid = gps.location.isValid();
    if(course_set == true)
    {
        data.navigation.distance_between = getDistance();
        data.navigation.course_to = getCourse();
        data.navigation.dest_latitude = dest_lat;
        data.navigation.dest_longitude = dest_lon;
    }
    else
    {
        data.navigation.distance_between = 0.0;
        data.navigation.course_to = 0.0;
        data.navigation.dest_latitude = 0.0;
        data.navigation.dest_longitude = 0.0; 
    }

    if(data.metadata.charsProcessed < 10)
    {
        data.GPS_activated = false;
    }
    else
    {
        data.GPS_activated = true;
    }

}

void GPS::setDestination(double lat, double lon)
{
    dest_lat = lat;
    dest_lon = lon;

    course_set = true;
}

double GPS::getDistance()
{
    return TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), dest_lat, dest_lon);
}

double GPS::getCourse()
{
    return TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), dest_lat, dest_lon);
}


void GPS::updateLocation(GPS_data::Location &location)
{
   location.age = gps.location.age();

   location.latitude.degrees = gps.location.rawLat().deg;
   location.latitude.coordinate = gps.location.lat();

   location.longitude.degrees = gps.location.rawLng().deg;
   location.longitude.coordinate = gps.location.lng();
}
void GPS::updateDate(GPS_data::Date &date)
{
    date.age = gps.date.age();
    date.year = gps.date.year();
    date.month = gps.date.month();
    date.day = gps.date.day();
}
void GPS::updateTime(GPS_data::Time &time)
{
    time.age = gps.time.age();
    time.hour = gps.time.hour();
    time.minute = gps.time.minute();
    time.second = gps.time.second();
    time.centisecond = gps.time.centisecond();
}
void GPS::updateSpeed(GPS_data::Speed &speed)
{
    speed.age = gps.speed.age();
    speed.knots = gps.speed.knots();
    speed.mph = gps.speed.mph();
    speed.mps = gps.speed.mps();
}
void GPS::updateCourse(GPS_data::Course &course)
{
    course.age = gps.course.age();
    course.deg = gps.course.deg();
}
void GPS::updateAltitude(GPS_data::Altitude &altitude)
{
    altitude.age = gps.altitude.age();
    altitude.meters = gps.altitude.meters();
}
void GPS::updateSatellites(GPS_data::Satellites &satellites)
{
    satellites.age = gps.altitude.age();
    satellites.value = gps.altitude.value();
}
void GPS::updateHDOP(GPS_data::HDOP &hdop)
{
    hdop.age = gps.hdop.age();
    hdop.hdop = gps.hdop.hdop();
}
void GPS::updateMetadata(GPS_data::Metadata &metadata)
{
    metadata.charsProcessed = gps.charsProcessed();
    metadata.sentencesWithFix = gps.sentencesWithFix();
    metadata.failedChecksum = gps.failedChecksum();
    metadata.passedChecksum = gps.passedChecksum();

    
}