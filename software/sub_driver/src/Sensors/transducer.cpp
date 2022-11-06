#include "transducer.h"


/**
 * @brief Construct a new Transducer:: Transducer object
 * 
 * @param pin pin the transducer is connected to 
 * @param cutoff cutoff frequency for low pass filter
 * @param interval_ns how long to wait between readings (nanoseconds)
 */
Sensors::Transducer::Transducer(const uint8_t pin, const double cutoff, const long interval_ns)
{
    m_pin = pin;
    m_interval = interval_ns;
    m_filter.setCutoff(cutoff);

    pinMode(m_pin, INPUT);
}

/**
 * @brief Reads raw voltage from GPIO
 * 
 * @return double measured voltage
 */
FASTRUN inline double Sensors::Transducer::readVoltage()
{
    return analogRead(m_pin) * ANALOG_TO_VOLTAGE;
}

/**
 * @brief Reads raw pressure from transducer
 * 
 * @return double raw pressure in atm 
 */
FASTRUN double Sensors::Transducer::readRaw()
{
    double voltage = readVoltage();
    double psi = (100.0 / 3.0) * voltage + (50.0 - ((100.0 / 3.0) * (3.3 / 2.0))); //pressure = 25psi * voltage - 12.5psi (linear)
    m_raw_pressure = psi / 14.695948775510204081632653061224; //convert psi to atm 
    m_pres_updated = true;
    return m_raw_pressure;
}

/**
 * @brief Returns a filtered pressure value
 * 
 * @param delta_time change in time (sec)
 * @return double filtered pressure in atm
 */
FASTRUN double Sensors::Transducer::readFiltered(const double delta_time)
{
    double filtered_pressure = 0.0;

    //Make sure that the filtered data and raw data are updated at the same time
    if(m_pres_updated)
    {
        filtered_pressure = m_filter.filt(m_raw_pressure, delta_time);
        m_pres_updated = false;
    }
    else
    {
        filtered_pressure = m_filter.filt(readRaw(), delta_time);
        m_pres_updated = false;
    }

    return filtered_pressure;
}

/**
 * @brief logs the raw and filtered pressure to the data struct
 * 
 * @param data reference to struct where the data is logged
 */
FASTRUN void Sensors::Transducer::logToStruct(Data &data)
{
    int64_t current_time = scoped_timer.elapsed();
    if(current_time >= m_prev_log_ns + m_interval)
    {
        double delta_time = data.delta_time;
        data.raw_ext_pres = readRaw();
        data.filt_ext_pres = readFiltered(delta_time);

        m_prev_log_ns = current_time;
    }
}