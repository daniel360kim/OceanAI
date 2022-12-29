#include "voltage.h"

Sensors::Voltage::Voltage(const uint8_t pin, const double cutoff, const long interval_ns)
{
    pinMode(pin, INPUT);
    m_pin = pin;
    m_interval = interval_ns;

    m_filter.setCutoff(cutoff);
}

/**
 * @brief reads raw voltage value
 * 
 * @return double raw_voltage value
 */
FASTRUN double Sensors::Voltage::readRaw()
{
    double voltage = analogRead(m_pin) * ANALOG_TO_VOLTAGE;
    voltage *= (9.95 + 1.992) / 1.992; // voltage divider

    if(voltage <= 0.1)
    {
        voltage = 0.0;
    }

    m_raw_voltage = voltage;
    m_voltage_updated = true;

    return voltage;
}

/**
 * @brief reads filtered voltage value
 * 
 * @param delta_time change in time (seconds)
 */
FASTRUN double Sensors::Voltage::readFiltered(const double delta_time)
{
    double filtered_voltage = 0.0;

    //Make sure that raw and filtered values are updated at the same time
    if(m_voltage_updated)
    {
        filtered_voltage = m_filter.filt(m_raw_voltage, delta_time);
    }
    else
    {
        filtered_voltage = m_filter.filt(readRaw(), delta_time);
    }

    m_voltage_updated = false;
    return filtered_voltage;
}

FASTRUN void Sensors::Voltage::logToStruct(LoggedData &data)
{
    int64_t current_time = scoped_timer.elapsed();
    if(current_time >= m_interval + m_prev_log_ns)
    {
        double delta_time = data.delta_time;
        data.raw_voltage = readRaw();
        data.filt_voltage = readFiltered(delta_time);
        m_prev_log_ns = current_time;
    }
}

