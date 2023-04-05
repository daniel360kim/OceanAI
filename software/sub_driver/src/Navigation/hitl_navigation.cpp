/**
 * @file hitl_navigation.cpp
 * @author Daniel Kim
 * @brief Handles navigation with HITL data
 * @version 0.1
 * @date 2023-02-07
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#include "hitl_navigation.h"

#include <cmath>

namespace HITL
{
    #if HITL_ON

    void HITLNavigation::setInitialCoordinate(double latitude, double longitude, int64_t timestamp)
    {
        m_initialLatitude = latitude;
        m_initialLongitude = longitude;
        m_initialTimestamp = timestamp;
    }

    /**
     * @brief Calculates total distance traveled from initial coordinate using the Haversine formula
     * 
     * @see https://en.wikipedia.org/wiki/Haversine_formula
     * @ref https://stackoverflow.com/questions/27928/calculate-distance-between-two-latitude-longitude-points-haversine-formula
     * Requires that initial coordinate is set using setInitialCoordinate()
     * @param latitude current latitude
     * @param longitude current longitude
     * @return double distance in km
     */
    double HITLNavigation::getTotalDistanceTraveled(Location<double> location)
    {
        constexpr double EARTH_RADIUS = 6371000; //meters
        
        double dLat = (location.latitude - m_initialLatitude) * DEG_TO_RAD;
        double dLon = (location.longitude - m_initialLongitude) * DEG_TO_RAD;

        double a = 
            std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
            std::cos(m_initialLatitude * DEG_TO_RAD) * std::cos(location.latitude * DEG_TO_RAD) *
            std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
        
        double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1-a));
        return EARTH_RADIUS * c;
    }

    /**
     * @brief Calculates distance between two coordinates using the Haversine formula
     * 
     * @param latitude current latitude
     * @param longitude current longitude
     * @param prevLatitude previous latitude
     * @param prevLongitude previous longitude
     * @return double distance between two coordinates in km
     */
    double HITLNavigation::getTotalDistanceTraveled(Location<double> location, Location<double> prevLocation)
    {
        constexpr double EARTH_RADIUS = 6371000; //meters

        double dLat = (location.latitude - prevLocation.latitude) * DEG_TO_RAD;
        double dLon = (location.longitude - prevLocation.longitude) * DEG_TO_RAD;

        double a = 
            std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
            std::cos(prevLocation.latitude * DEG_TO_RAD) * std::cos(location.latitude * DEG_TO_RAD) *
            std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
        
        double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        return EARTH_RADIUS * c;
    }

    /**
     * @brief Calculates average speed from initial coordinate
     * 
     * Requires that initial coordinate is set using setInitialCoordinate()
     * @param latitude current latitude
     * @param longitude current longitude
     * @param timestamp current timestamp
     * @return double speed in km/h
     */
    double HITLNavigation::getAverageSpeed(Location<double> location, int64_t timestamp)
    {
        double distance = getTotalDistanceTraveled(location);
        double time = (timestamp - m_initialTimestamp) / 1000.0 / 60.0 / 60.0; //seconds to hours
        return distance / time;
    }

    /**
     * @brief Calculates instantaneous speed in the x direction
     * 
     * @param latitude current latitude
     * @param longitude current longitude
     * @param timestamp current time
     * @return double instantaneous speed in meters/second
     */
    double HITLNavigation::getSpeedX(Location<double> location, int64_t timestamp)
    {
        if(timestamp == m_prevTimestamp)
        {
            return m_previousVelocity;
        }

        double distance = getTotalDistanceTraveled(location, m_prevLocation) * 1000.0; //km to m
        double time = (timestamp - m_prevTimestamp) / 1000000000.0; //ns to s

        m_prevLocation = location;

        m_prevTimestamp = timestamp;

        m_previousVelocity = distance / time;
        return m_previousVelocity;
    }

    void HITLNavigation::logData(LoggedData &data)
    {
        data.HITL.distance = getTotalDistanceTraveled(data.HITL.location);
        data.HITL.averageSpeed = getAverageSpeed(data.HITL.location, data.time_ns);
        data.HITL.currentSpeed = getSpeedX(data.HITL.location, data.HITL.timestamp);
    }

    #endif
}


