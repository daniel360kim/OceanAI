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
#include "../Data/logged_data.h"

#if HITL_ON

#include <cstdint>

namespace HITL
{
    class HITLNavigation
    {
    public:
        HITLNavigation() {}
        
        void setInitialCoordinate(double latitude, double longitude, int64_t timestamp);

        double getTotalDistanceTraveled(Location<double> location);

        double getTotalDistanceTraveled(Location<double> location, Location<double> prevLocation);
        double getAverageSpeed(Location<double> location, int64_t timestamp);

        double getSpeedX(Location<double> location, int64_t timestamp);

        void logData(LoggedData &data);

    private:
        double m_initialLatitude; //very first latitude
        double m_initialLongitude; //very first longitude

        Location<double> m_prevLocation;

        double m_previousVelocity; //velocity from previous iteration

        int64_t m_initialTimestamp; //very first timestamp

        int64_t m_prevTimestamp; //timestamp from previous iteration
    };
}
#endif