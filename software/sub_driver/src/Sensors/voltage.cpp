#include "voltage.h"

Sensors::Voltage::Voltage(const uint8_t pin, const double cutoff, const long interval_ns, double r1, double r2)
{
    pinMode(pin, INPUT);
    m_pin = pin;
    m_interval = interval_ns;

    m_r1 = r1;
    m_r2 = r2;

    m_filter.setCutoff(cutoff);
}

/**
 * @brief reads raw voltage value
 * 
 * @return double raw_voltage value
 */
double Sensors::Voltage::readRaw()
{
    double voltage = analogRead(m_pin) * ANALOG_TO_VOLTAGE;

    const double resistor_sum  = m_r1 + m_r2;

    voltage *= resistor_sum / m_r2; // voltage divider

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
double Sensors::Voltage::readFiltered(const double delta_time)
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

/**
 * @brief logs data to the data struct
 * 
 * @param data data struct
 * @param log_location_raw location where to log (needed because we have multiple voltmeters)
 * @param log_location_filtered location where to log (needed because we have multiple voltmeters)
 */
void Sensors::Voltage::logData(LoggedData &data, double &log_location_raw, double &log_location_filtered)
{
    int64_t current_time = scoped_timer.elapsed();
    if(current_time - m_prev_log_ns >= m_interval)
    {
        double delta_time = (current_time - m_prev_log_ns) / 1e9;
        log_location_raw = readRaw();
        log_location_filtered = readFiltered(delta_time);

        m_prev_log_ns = current_time;
    }
}


