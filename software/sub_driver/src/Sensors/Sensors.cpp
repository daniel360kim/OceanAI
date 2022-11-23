/**
 * @file Sensors.cpp
 * @author Daniel Kim
 * @brief Reads sensor from our 9dof sensor array
 * @version 0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#include "Sensors.h"

#include <stdint.h>
#include <array>

#include "../core/debug.h"
#include "LowPass.h"
#include "../core/timed_function.h"
#include "../Data/StartInfo.h"
#include "../core/Timer.h"

UnifiedSensors UnifiedSensors::instance;

volatile bool UnifiedSensors::bar_flag = false;
volatile bool UnifiedSensors::accel_flag = false;
volatile bool UnifiedSensors::gyro_flag = false;
volatile bool UnifiedSensors::mag_flag = true;

void UnifiedSensors::scanAddresses()
{
    uint8_t error = 0;
    uint8_t address = 0;
    int nDevices = 0;

    for(address = 1; address < 127; address++ )
    {
        
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
    
        if (error == 0)
        {
            configs.addresses.push_back(address);
            nDevices++;
        }
        else if (error == 4)
        {
            configs.errors.push_back(address);
        }
    }

    configs.num_devices = nDevices;
}

bool UnifiedSensors::initNavSensors()
{
    std::array<int, 3> status;

    int baro_stat = baro.begin(BMP388_I2C_ALT_ADDR); // returns 0 for error 1 for success
    if (baro_stat == 0)
    {
        status[0] = -1;
    }
    else
    {
        status[0] = 1;
    }
    baro.setPresOversampling(OVERSAMPLING_X4);
    baro.setTempOversampling(OVERSAMPLING_X16);
    baro.enableInterrupt();
    baro.setTimeStandby(TIME_STANDBY_80MS);
    baro.startNormalConversion();

    configs.BMP_os_p = (char *)"Pressure: X4";
    configs.BMP_os_t = (char *)"Temperature: X16";
    configs.BMP_ODR = (char *)"Standby: 80 milliseconds";

    status[1] = accel.begin();
    accel.setOdr(Bmi088Accel::ODR_800HZ_BW_140HZ);
    accel.pinModeInt1(Bmi088Accel::PUSH_PULL, Bmi088Accel::ACTIVE_HIGH);
    accel.mapDrdyInt1(true);
    accel.setRange(Bmi088Accel::RANGE_3G);

    configs.accel_range = (char *)"Accel: 3G";
    configs.accel_ODR = (char *)"Accel ODR: 800Hz";

    status[2] = gyro.begin();
    gyro.setOdr(Bmi088Gyro::ODR_1000HZ_BW_116HZ);
    gyro.pinModeInt3(Bmi088Gyro::PUSH_PULL, Bmi088Gyro::ACTIVE_HIGH);
    gyro.mapDrdyInt3(true);
    gyro.setRange(Bmi088Gyro::RANGE_250DPS);

    configs.gyro_range = (char *)"Gyro: 250 degrees per second";
    configs.gyro_ODR = (char *)"Gyro ODR: 1000Hz";

    if (!mag.init())
    {
        status[3] = -1;
    }
    else
    {
        status[3] = 1;
    }

    mag.enableDefault();

    configs.mag_range = (char *)"Mag: +/- 4 Gauss";
    configs.mag_ODR = (char *)"Mag ODR: 1000Hz";
    configs.mag_bias = { mag_bias.x, mag_bias.y, mag_bias.z };

    mag_x.setCutoff(30);
    mag_y.setCutoff(30);
    mag_z.setCutoff(30);

    for (uint8_t i = 0; i < status.size(); i++)
    {
        if (status[i] < 0)
        {
            switch (i)
            {
            case 0:
            {
                ERROR_LOG(Debug::Critical_Error, "BMP388 initialization failed");
                return false;
                break;
            }
            case 1:
            {
                ERROR_LOG(Debug::Critical_Error, "Bmi088 Accel initialization failed");
                return false;
                break;
            }
            case 2:
            {
                ERROR_LOG(Debug::Critical_Error, "Bmi088 Gyro initialization failed");
                return false;
                break;
            }

            case 3:
            {
                ERROR_LOG(Debug::Critical_Error, "LIS3MDL Mag initialization failed");
                return false;
                break;
            }
            default:
            {
                ERROR_LOG(Debug::Critical_Error, "Unknown sensor initialization error");
                return false;
            }
            }
        }
    }

    SUCCESS_LOG("All sensors initialized successfully");

    return true;
}


void UnifiedSensors::setInterrupts(const uint8_t bar_int, const uint8_t accel_int, const uint8_t gyro_int, const uint8_t mag_int)
{
    pinMode(bar_int, INPUT);
    pinMode(accel_int, INPUT);
    pinMode(gyro_int, INPUT);
    pinMode(mag_int, INPUT);

    attachInterrupt(digitalPinToInterrupt(bar_int), bar_drdy, RISING);
    attachInterrupt(digitalPinToInterrupt(accel_int), accel_drdy, RISING);
    attachInterrupt(digitalPinToInterrupt(gyro_int), gyro_drdy, RISING);
    attachInterrupt(digitalPinToInterrupt(mag_int), mag_drdy, RISING);
}

void UnifiedSensors::setGyroBias()
{
    double gxIntegration = 0.0;
    double gyIntegration = 0.0;
    double gzIntegration = 0.0;

    for (uint8_t i = 0; i < 100; i++)
    {
        Angles_3D gyro = returnRawGyro();
        gxIntegration += gyro.x;
        gyIntegration += gyro.y;
        gzIntegration += gyro.z;
        delayMicroseconds(100);
    }

    gyro_bias.x = gxIntegration / 100.0;
    gyro_bias.y= gyIntegration / 100.0;
    gyro_bias.z = gzIntegration / 100.0;

    configs.gyro_bias.x = gyro_bias.x;
    configs.gyro_bias.y = gyro_bias.y;
    configs.gyro_bias.z = gyro_bias.z;
}
FASTRUN BMP388Data UnifiedSensors::returnRawBaro()
{
    BMP388Data data;
    double pres_hpa = 0;
    baro.getTempPres(data.temperature, pres_hpa);

    data.pressure = pres_hpa * 0.0009869233; //convert to atm

    return data;
}

FASTRUN Angles_3D UnifiedSensors::returnRawAccel()
{
    Angles_3D accel_data;

    accel.readSensor();
    accel_data.x = accel.getAccelX_mss();
    accel_data.y = accel.getAccelY_mss();
    accel_data.z = accel.getAccelZ_mss();

    return accel_data;
}

double UnifiedSensors::returnAccelTempC()
{
    return accel.getTemperature_C();
}

FASTRUN Angles_3D UnifiedSensors::returnRawGyro()
{
    Angles_3D gyro_data;

    gyro.readSensor();
    gyro_data.x = gyro.getGyroX_rads() - gyro_bias.x;
    gyro_data.y = gyro.getGyroY_rads() - gyro_bias.y;
    gyro_data.z = gyro.getGyroZ_rads() - gyro_bias.z;
    
    return gyro_data;
}

FASTRUN Angles_3D UnifiedSensors::returnRawMag()
{
    Angles_3D mag_data;

    mag.read();
    // Convert to mTesla. Rangle is +/-4 so we divide by 6842 to get gauss, then mult. by 100 to get mtesla
    mag_data.x = mag.m.x / 68.42 - mag_bias.x;
    mag_data.y = mag.m.y / 68.42 - mag_bias.y;
    mag_data.z = mag.m.z / 68.42 - mag_bias.z;

    return mag_data;
}

FASTRUN void UnifiedSensors::logIMUToStruct(Data &data)
{
    //Flags are triggered by interrupts set by the sensors
    //Mag, Accel, Gyro, and baro all have their own interrupt pins
    if (UnifiedSensors::mag_flag)
    {
        data.rmag = returnRawMag();

        data.fmag.x = mag_x.filt(data.rmag.x, data.delta_time);
        data.fmag.y = mag_y.filt(data.rmag.y, data.delta_time);
        data.fmag.z = mag_z.filt(data.rmag.z, data.delta_time);

        UnifiedSensors::mag_flag = false;
    }

    if (UnifiedSensors::accel_flag && UnifiedSensors::gyro_flag)
    {
        data.racc = returnRawAccel();
        data.bmi_temp = returnAccelTempC();

        UnifiedSensors::accel_flag = false;

        data.rgyr = returnRawGyro();

        UnifiedSensors::gyro_flag = false;
    }

    if (UnifiedSensors::bar_flag)
    {
        data.raw_bmp = returnRawBaro();

        UnifiedSensors::bar_flag = false;
    }

}
