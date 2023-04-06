#include "tds.h"

/**
 * @brief Construct a new Sensors:: Total Dissolved Solids:: Total Dissolved Solids object
 * 
 * @param pin pin the tds sensor is connected to    
 * @param cutoff cutoff frequency for the low pass filter
 * @param interval_ns time inbetween readings (nanoseconds)
 */
Sensors::TotalDissolvedSolids::TotalDissolvedSolids(const uint8_t pin, const double cutoff, const long interval_ns)
{
    pinMode(pin, INPUT);
    m_pin = pin;
    m_interval_ns = interval_ns;

    m_filter.setCutoff(cutoff);
}

/**
 * @brief reads raw tds value
 * 
 * @param temp temperature where the sensor is loated
 * @return double raw_tds value
 */
double Sensors::TotalDissolvedSolids::readRaw(const double temp)
{
    int tds = analogRead(m_pin);
    double averageVoltage = tds * ANALOG_TO_VOLTAGE;

    double compensationCoefficient = 1.0 + 0.02 * (temp - STANDARD_TEMP);
    double compensationVoltage = averageVoltage / compensationCoefficient;

    m_raw_tds_reading = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
    m_tds_updated = true; 

    return m_raw_tds_reading;
}

/**
 * @brief reads filtered tds value
 * 
 * @param delta_time change in time
 * @param temp ambient temperature
 * @return double filtered tds value
 */
double Sensors::TotalDissolvedSolids::readFiltered(const double delta_time, const double temp)
{
    double filtered_reading = 0.0;
    //Make sure that filtered and raw values match up
    if (m_tds_updated)
    {
        filtered_reading = m_filter.filt(m_raw_tds_reading, delta_time);
    }
    else
    {
        filtered_reading = m_filter.filt(readRaw(temp), delta_time);
    }

    m_tds_updated = false;
    return filtered_reading;
}

/**
 * @brief logs tds value to data struct
 * 
 * @param data reference to struct where data is logged
 */
void Sensors::TotalDissolvedSolids::logToStruct(LoggedData &data)
{
    int64_t current_time = scoped_timer.elapsed();
    if(current_time >= m_prev_log_ns + m_interval_ns)
    {
        double delta_time = (current_time - m_prev_log_ns) / 1e9;
        data.raw_TDS = readRaw(data.filt_ext_temp);
        data.filt_TDS = readFiltered(delta_time, data.filt_ext_temp);

        m_prev_log_ns = current_time;
    }
}