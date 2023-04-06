/**
 * @file thermistor.cpp
 * @author Daniel Kim
 * @brief Reads data from a thermistor
 * @version 0.1
 * @date 2022-11-05
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "thermistor.h"

/**
 * @brief Construct a new Thermistor:: Thermistor object
 * 
 * @param pin pin the thermistor is connected to
 * @param RT0 resistance at 0 degrees C
 * @param B B constant
 * @param T0 T0 constant in in Celsius
 * @param cutoff cutoff frequency for low pass filter
 * @param interval_ns interval in nanoseconds to read from sensor
 */
Sensors::Thermistor::Thermistor(const uint8_t pin, const double RT0, const double B, const double T0, const double cutoff, const long interval_ns)
{
    m_pin = pin;
    m_RT0 = RT0;
    m_B = B;
    m_T0 = T0 + 273.15; // convert to Kelvin
    m_interval = interval_ns;

    pinMode(m_pin, INPUT);

    m_filter.setCutoff(cutoff);
}

/**
 * @brief Reads the temperature in Celsius
 * 
 * @return double temperature in Celsius
 */ 
double Sensors::Thermistor::readRaw()
{
    constexpr double VREF = 3.3;
    double RT = 0.0;
    double VR = 0.0;
    double ln = 0.0;
    double VRT = 0.0;

    VRT = analogRead(m_pin) * (VREF / 1023.0);
    VR = VREF - VRT;
    RT = VRT / (VR / m_RT0);
    ln = std::log(RT / m_RT0);
    raw_reading = 1.0 / ((ln / m_B) + (1.0 / m_T0));
    raw_reading = raw_reading - 273.15;

    m_temp_updated = true;

    return raw_reading;
}

/**
 * @brief Reads the temperature in Celsius
 * 
 * @param delta_time change in time
 * @return double temperature in Celsius
 */
double Sensors::Thermistor::readFiltered(const double delta_time)
{
    //Ensure that filtered temp and raw temp are updated at the same time
    if(m_temp_updated)
    {
        filtered_reading = m_filter.filt(raw_reading, delta_time);
        m_temp_updated = false;
    }
    else
    {
        filtered_reading = m_filter.filt(readRaw(), delta_time);
        m_temp_updated = false;
    }

    return filtered_reading;
}

/**
 * @brief Logs the raw and filtered temperature to the data struct
 * 
 * @param data reference to the struct to log to
 */
void Sensors::Thermistor::logToStruct(LoggedData &data)
{
    int64_t current_time = scoped_timer.elapsed();
    if(current_time >= m_prev_log_ns + m_interval)
    {
        double delta_time = (current_time - m_prev_log_ns) / 1e9;

        data.raw_ext_temp = readRaw();
        data.filt_ext_temp = readFiltered(delta_time);

        m_prev_log_ns = current_time;
    }
}