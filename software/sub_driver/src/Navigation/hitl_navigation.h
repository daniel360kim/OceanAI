/**
 * @file hitl_navigation.h
 * @author Daniel Kim
 * @brief Handles navigation with HITL data
 * @version 0.1
 * @date 2023-02-07
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */


#include "../core/configuration.h"

#if HITL_ON

#include <cstdint>

class HITLNavigation
{
public:
    HITLNavigation() {}
    
    void setInitialCoordinate(double latitude, double longitude, int64_t timestamp);

    double getTotalDistanceTraveled(double latitude, double longitude);
    double getTotalDistanceTraveled(double latitude, double longitude, double prevLatitude, double prevLongitude);
    double getAverageSpeed(double latitude, double longitude, int64_t timestamp);

    double getSpeedX(double latitude, double longitude, int64_t timestamp);

private:
    double m_initialLatitude; //very first latitude
    double m_initialLongitude; //very first longitude

    double m_prevLatitude; //latitude from previous iteration
    double m_prevLongitude; //longitude from previous iteration

    double m_previousVelocity; //velocity from previous iteration

    int64_t m_initialTimestamp; //very first timestamp

    int64_t m_prevTimestamp; //timestamp from previous iteration
};

#endif