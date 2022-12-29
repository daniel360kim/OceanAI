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

#include <vector>
#include <cstdint>

#include "BMI088/BMI088.h"
#include "LIS3MDL/LIS3MDL.h"
#include "BMP388/BMP388_DEV.h"
#include "LowPass.h"
#include "../Data/logged_data.h"

namespace Sensors
{
    bool initAccel();
    bool initGyro();
    bool initMag();

    bool initBaro();

    bool initAll();

    std::vector<uint8_t> scanI2C();

    void setInterrupts(const uint8_t bar_int, const uint8_t accel_int, const uint8_t gyro_int, const uint8_t mag_int);
    void setInterrupts();

    void bar_drdy();
    void accel_drdy();
    void gyro_drdy();
    void mag_drdy();

    Angles_3D<double> setGyroBias();

    BMP388Data returnRawBaro();
    Angles_3D<double> returnRawAccel();
    Angles_3D<double> returnRawGyro();
    Angles_3D<double> returnRawMag();
    double returnAccelTemp();

    void logData(LoggedData &data);
}


#endif