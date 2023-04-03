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


