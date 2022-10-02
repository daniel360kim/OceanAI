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

#include "BMP388/BMP388_DEV.h"
#include "BMI088/BMI088.h"
#include "LIS3MDL/LIS3MDL.h"
#include "external_sensor/external_sensor.h"

#include "LowPass.h"

#include "../core/timed_function.h"
#include "../core/Timer.h"
#include "../Data/data_struct.h"

constexpr double VREF = 3.3;

class UnifiedSensors
{
public:
    static UnifiedSensors& getInstance()
    {
        return instance;
    }
    void scanAddresses();
    bool initNavSensors();
    void initADC();
    void initTDS(uint8_t TDS_pin, uint32_t interval, double filt_cutoff);
    void initVoltmeter(uint8_t input_pin , uint32_t interval, double filt_cutoff);
    void initPressureSensor(uint8_t input_pin , uint32_t interval, double filt_cutoff);
    void setInterrupts(const uint8_t bar_int, const uint8_t accel_int, const uint8_t gyro_int, const uint8_t mag_int);
    void setGyroBias();

    void returnRawBaro(double *pres, double *temp);
    void returnRawAccel(double *x, double *y, double *z, double *tempC);
    void returnRawGyro(double *x, double *y, double *z);
    void returnRawMag(double *x, double *y, double *z);
    
    void logToStruct(Data &data);

    static void readTDS(double &tds_reading);
    static void readVoltage(double& voltage);
    static void readExternalPressure_v(double& voltage);
    static void readExternalPressure(double& pressure);

    double gx_bias = 0, gy_bias = 0, gz_bias = 0;
    const double HARD_IRON_BIAS[3] = { 0.36, 0.39, 0.49 };

    std::vector<uint8_t> address;

private:
    UnifiedSensors() {}

    static UnifiedSensors instance;
    static uint8_t TDS_pin, voltage_pin, pressure_pin;
    static double temp;

    double temp_measurements[2];

    static bool pressure_sensor_connected;

    //Timers to measure tds and voltage and pressure at certain intervals
    Time::Async<void, double&> tds_function;
    Time::Async<void, double&> voltage_function;
    Time::Async<void, double&> pressure_function;

    static volatile bool bar_flag;
    static volatile bool accel_flag;
    static volatile bool gyro_flag ;
    static volatile bool mag_flag;

    static void bar_drdy()  { bar_flag = true; }
    static void accel_drdy()  { accel_flag = true; }
    static void gyro_drdy()  { gyro_flag = true; }
    static void mag_drdy()  { mag_flag = true; }


    //Low pass filters for all the sensors
    Filter::LowPass<1> gyr_x;
    Filter::LowPass<1> gyr_y;
    Filter::LowPass<1> gyr_z;

    Filter::LowPass<1> acc_x;
    Filter::LowPass<1> acc_y;
    Filter::LowPass<1> acc_z;

    Filter::LowPass<1> bmp_pres;
    Filter::LowPass<1> bmp_temp;

    Filter::LowPass<1> tds_filter;
    Filter::LowPass<1> voltage_filter;
    Filter::LowPass<1> ext_temp;
    Filter::LowPass<1> ext_pres;
};



#endif