/**
 * @file mission.h
 * @author Daniel Kim
 * @brief setting timings for classes
 * @version 0.1
 * @date 2022-09-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef Mission_h
#define Mission_h

#include <stdint.h>
#include <Arduino.h>

class Duration
{
public:
    Duration() {}
    static constexpr uint8_t days = 0;
    static constexpr uint16_t hours = 0;
    static constexpr uint32_t minutes = 0;
    static constexpr uint32_t seconds = 10;

    static constexpr uint16_t reset_interval_hours = 3; //interval between resets
    //resets are when the sub will resurface, save all the data, and restart the mission

    static constexpr uint64_t mission_time = (days * 24 * 60 * 60 * 1000 * 1000) + (hours * 60 * 60 * 1000 * 1000) + (minutes * 60 * 1000 * 1000) + (seconds * 1000 * 1000);

    //Making this signed so that inaccurate comparators work
    
    static int64_t time_remaining_mission() { return mission_time - micros(); }
private:
    
};




#endif