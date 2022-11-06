/**
 * @file Sensors.h
 * @author Daniel Kim
 * @brief Reads sensor from our 9dof sensor array
 * @version 0.1
 * @date 2022-06-28
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */
#ifndef Sensors_h
#define Sensors_h

#include <Arduino.h>
#include <Wire.h>
#include <InternalTemperature.h>
#include <vector>
#include <stdint.h>
#include <WireIMXRT.h>

#include "BMP388/BMP388_DEV.h"
#include "BMI088/BMI088.h"
#include "LIS3MDL/LIS3MDL.h"

#include "thermistor.h"
#include "tds.h"
#include "transducer.h"

#include "LowPass.h"

#include "../core/timed_function.h"
#include "../core/Timer.h"
#include "../Data/data_struct.h"

class UnifiedSensors
{
public:
    static UnifiedSensors& getInstance()
    {
        return instance;
    }
    void scanAddresses();
    bool initNavSensors();
    void setInterrupts(const uint8_t bar_int, const uint8_t accel_int, const uint8_t gyro_int, const uint8_t mag_int);
    void setGyroBias();

    BMP388Data returnRawBaro();
    Angles_3D returnRawAccel();
    double returnAccelTempC();
    Angles_3D returnRawGyro();
    Angles_3D returnRawMag();

    void logIMUToStruct(Data &data);
    void logExternalSensorToStruct(Data &data);

    Angles_3D gyro_bias;
    Angles_3D mag_bias = { 0.36, 0.39, 0.49 };

    std::vector<uint8_t> address;

private:
    UnifiedSensors() {}
    
    BMP388_DEV baro;
    Bmi088Accel accel;
    Bmi088Gyro gyro;
    LIS3MDL mag;

    static UnifiedSensors instance;

    uint8_t TDS_pin, voltage_pin, pressure_pin;

    bool m_pressure_sensor_connected = false;

    int64_t m_tds_prev_log;
    int64_t m_volt_prev_log;
    int64_t m_ext_pres_prev_log;

    static volatile bool bar_flag;
    static volatile bool accel_flag;
    static volatile bool gyro_flag ;
    static volatile bool mag_flag;

    static void bar_drdy()  { bar_flag = true; }
    static void accel_drdy()  { accel_flag = true; }
    static void gyro_drdy()  { gyro_flag = true; }
    static void mag_drdy()  { mag_flag = true; }

    long m_tds_interval_ns;
    long m_volt_interval_ns;
    long m_ext_pres_interval_ns;

    //Low pass filters for all the sensors
    Filter::LowPass<1> mag_x;
    Filter::LowPass<1> mag_y;
    Filter::LowPass<1> mag_z;
    
};



#endif