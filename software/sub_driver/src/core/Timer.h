/**
 * @file Timer.h
 * @author Daniel Kim
 * @brief Timer implementation to measure execution times
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef Timer_h
#define Timer_h

#include <teensy_clock/teensy_clock.h>
#include <chrono>
#include <map>

/**
 * @brief Timing executions and saving the info
 * 
 */
namespace Time
{
    class Timer
    {
    public:
        Timer()
        {
            reset();
        }

        void reset()
        {
            start_time = teensy_clock::now();
        }

        void reset(std::chrono::time_point<teensy_clock, teensy_clock::duration> epoch)
        {
            start_time = epoch;
        }

        int64_t elapsed()
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(teensy_clock::now() - start_time).count();
        }

        int64_t elapsedMillis()
        {
            return elapsed() * 1000;
        }

    protected:
        std::chrono::time_point<teensy_clock, teensy_clock::duration> start_time;
    };

    

    struct TimerData
    {
        const char* name;
        int64_t time;
    };

    /**
     * @brief When debugging, we can use this to measure the execution time of each function and sort by the name
     * 
     */
    class NamedTimer : public Timer
    {
    public:
        NamedTimer(const char* name) : Timer()
        {
            this->name = name;
        }

        TimerData getTimeInfo()
        {
            TimerData data;
            data.name = name;
            data.time = elapsed();
            return data;
        }

    protected:
        NamedTimer() {}
    private:
        const char* name;
    };


    /**
     * @brief Singleton since one class manages all the timers
     * 
     */
    class TimerManager : public NamedTimer
    {
    public:
        static TimerManager& getInstance()
        {
            static TimerManager instance;
            return instance;
        }

        void addTimer(TimerData timer)
        {
            timers[timer.name] = timer;
        }

        void printTimers()
        {
            for (auto it = timers.begin(); it != timers.end(); it++)
            {
                Serial.print(it->first);
                Serial.print(": ");
                Serial.print(it->second.time);
                Serial.print(" ns ");
                Serial.print(it->second.time / 1000000);
                Serial.println(" ms");

            }
        }

        void clearMap()
        {
            timers.clear();
        }

    private:
        TimerManager() {}

        std::map<const char*, TimerData> timers;

    };

    /**
     * @brief Sets mission duration
     * 
     */
    class Mission
    {
    public:
        Mission() {}
        static constexpr uint16_t reset_interval_hours = 3; //interval between resets
        //resets are when the sub will resurface, save all the data, and restart the mission

        static constexpr uint16_t days = 0;
        static constexpr uint16_t hours = 0;
        static constexpr uint32_t minutes = 0;
        static constexpr uint32_t seconds = 20;

        static constexpr uint64_t mission_time = ((uint64_t)(days * (uint64_t)8.64e+13) + (uint64_t)(hours * (uint64_t)3.6e+12) + (uint64_t)(minutes * (uint64_t)6e+10) + (uint64_t)(seconds * (uint64_t)1e+9));

        //Making this signed so that inaccurate comparators work
        
        static int64_t time_remaining_mission(int64_t elapsed_nanos) { return mission_time - elapsed_nanos; }

    };  

    
};

extern Time::Timer scoped_timer;





#endif