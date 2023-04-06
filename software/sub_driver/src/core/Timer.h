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

#include <Arduino.h>
#include <teensy_clock/teensy_clock.h>
#include <chrono>
#include <map>

/**
 * Macros to convert time units
 * This project uses nanoseconds as the base unit
 */

#define SEC_TO_NS(sec) (sec * 1000000000ULL)
#define MS_TO_NS(ms) (ms * 1000000)
#define US_TO_NS(us) (us * 1000)

#define HZ_TO_NS(hz) ((1 / hz) * 1000000000)
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

        inline int64_t elapsed()
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(teensy_clock::now() - start_time).count();
        }

        int64_t elapsedMillis()
        {
            return elapsed() * 1000;
        }

        double deltaTime()
        {
            int64_t currentTime = elapsed();
            int64_t deltaTime = currentTime - m_previous_time;
            m_previous_time = currentTime;

            return deltaTime / 1000000000.0;
        }

        void setPreviousTime(int64_t time)
        {
            m_previous_time = time;
        }

    protected:
        int64_t m_previous_time = 0;
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

        void showElapsed()
        {
            Serial.print(name);
            Serial.print(": ");
            Serial.print(elapsed());
            Serial.println(" ns");
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
        static TimerManager instance;

        std::map<const char*, TimerData> timers; //all the running timers

    };

    
};

extern Time::Timer scoped_timer;





#endif