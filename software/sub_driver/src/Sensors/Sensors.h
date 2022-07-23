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

#include "BMP388/BMP388_DEV.h"
#include "BMI088/BMI088.h"
#include "LIS3MDL/LIS3MDL.h"

#include "../Data/data_struct.h"

constexpr double VREF = 3.3;

class UnifiedSensors
{

public:
    UnifiedSensors();
    bool initNavSensors();
    void initADC();
    void initTDS(uint8_t TDS_pin);
    void initVoltmeter(uint8_t input_pin);
    void setInterrupts(uint8_t bar_int, uint8_t accel_int, uint8_t gyro_int, uint8_t mag_int);
    void setGyroBias();

    void returnRawBaro(double *pres, double *temp);
    void returnRawAccel(double *x, double *y, double *z, double *tempC);
    void returnRawGyro(double *x, double *y, double *z);
    void returnRawMag(double *x, double *y, double *z);
    
    void logToStruct(Data &data);

    double readTDS();
    double readVoltage();

    static volatile bool bar_flag;
    static volatile bool accel_flag;
    static volatile bool gyro_flag ;
    static volatile bool mag_flag;

    static void bar_drdy()  { bar_flag = true; }
    static void accel_drdy()  { accel_flag = true; }
    static void gyro_drdy()  { gyro_flag = true; }
    static void mag_drdy()  { mag_flag = true; }

    double gx_bias = 0, gy_bias = 0, gz_bias = 0;
    const double HARD_IRON_BIAS[3] = { 0.36, 0.39, 0.49 };

private:
    uint8_t TDS_pin, voltage_pin;
    double temp = 25;

    double temp_measurements[2];
    int getMedian(int bArray[], int iFilterLen);

    //Using a timer we generate a fake interrupt for analog reads
    static volatile bool TDS_flag;
    static volatile bool voltage_flag;

    //Set the flags true for the interrupt
    static inline bool TDS_drdy(void*) { TDS_flag = true; return true; }
    static inline bool voltage_drdy(void*) { voltage_flag = true; return true; }

};



#endif