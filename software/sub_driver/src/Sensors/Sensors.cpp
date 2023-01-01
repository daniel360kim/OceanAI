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
#include "../Data/StartInfo.h"
#include "../core/debug.h"
#include "../core/pins.h"

namespace Sensors
{
    static Angles_3D<double> gyro_bias;
    static volatile bool bar_flag;
    static volatile bool accel_flag;
    static volatile bool gyro_flag;
    static volatile bool mag_flag;
 
    static Bmi088Accel accel;
    static Bmi088Gyro gyro;
 
    static LIS3MDL mag;

    static BMP388_DEV baro;

    /*
    BMI088 comes with built in low pass filter
    LIS3MDL does not come with built in low pass filter so we use our own
    */
    Filter::LowPass<1> mag_x;
    Filter::LowPass<1> mag_y;
    Filter::LowPass<1> mag_z;

    /**
     * @brief Initializes the accelerometer on the BMI088
     *
     * @return true accelerometer initialized successfully
     * @return false accelerometer failed to initialize
     */
    bool initAccel()
    {
        int status = accel.begin();
        accel.pinModeInt1(Bmi088Accel::PUSH_PULL, Bmi088Accel::ACTIVE_HIGH);
        accel.mapDrdyInt1(true);
        accel.setRange(Bmi088Accel::RANGE_3G);

        // Saving the configurations to the StartInfo struct
        configs.accel_range = (char *)"Accel: 3G";
        configs.accel_ODR = (char *)"Accel ODR: 800Hz";

        if (status < 0)
        {
            ERROR_LOGf(Severity::ERROR, "Accel: Failed to initialize. Error code: %d", status);
            return false;
        }
        else
        {
            SUCCESS_LOG("Accel: Initialized");
            return true;
        }
    }

    /**
     * @brief Initializes the gyroscope on the BMI088
     *
     * @return true gyroscope initialized successfully
     * @return false gyroscope failed to initialize
     */
    bool initGyro()
    {
        int status = gyro.begin();
        gyro.setOdr(Bmi088Gyro::ODR_1000HZ_BW_116HZ);
        gyro.pinModeInt3(Bmi088Gyro::PUSH_PULL, Bmi088Gyro::ACTIVE_HIGH);
        gyro.mapDrdyInt3(true);
        gyro.setRange(Bmi088Gyro::RANGE_250DPS);

        // Saving the configurations to the StartInfo struct
        configs.gyro_range = (char *)"Gyro: 2000dps";
        configs.gyro_ODR = (char *)"Gyro ODR: 3200Hz";

        if (status < 0)
        {
            ERROR_LOGf(Severity::ERROR, "Gyro: Failed to initialize. Error code: %d", status);
            return false;
        }
        else
        {
            SUCCESS_LOG("Gyro: Initialized");
            return true;
        }
    }

    /**
     * @brief Initializes the magnetometer on the LIS3MDL
     *
     * @return true magnetometer initialized successfully
     * @return false magnetometer failed to initialize
     */
    bool initMag()
    {
        if (!mag.init())
        {
            ERROR_LOG(Severity::ERROR, "Mag: Failed to initialize");
            return false;
        }
        else
        {
            SUCCESS_LOG("Mag: Initialized");
        }

        mag.enableDefault();

        // Saving the configurations to the StartInfo struct
        configs.mag_range = (char *)"Mag: +/- 4 Gauss";
        configs.mag_ODR = (char *)"Mag ODR: 1000Hz";
        configs.mag_bias = {mag_bias.x, mag_bias.y, mag_bias.z};

        // Setting the cutoff frequency for the mag low pass filter
        mag_x.setCutoff(30);
        mag_y.setCutoff(30);
        mag_z.setCutoff(30);

        return true;
    }

    /**
     * @brief Initializes the barometer on the BMP388
     *
     * @return true barometer initialized successfully
     * @return false barometer failed to initialize
     */
    bool initBaro()
    {
        if (!baro.begin(BMP388_I2C_ALT_ADDR))
        {
            ERROR_LOG(Severity::ERROR, "Baro: Failed to initialize");
            return false;
        }
        else
        {
            SUCCESS_LOG("Baro: Initialized");
        }

        baro.setPresOversampling(OVERSAMPLING_X4);
        baro.setTempOversampling(OVERSAMPLING_X16);
        baro.enableInterrupt();
        baro.setTimeStandby(TIME_STANDBY_80MS);
        baro.startNormalConversion();

        // Saving the configurations to the StartInfo struct
        configs.BMP_os_p = (char *)"Pressure: X4";
        configs.BMP_os_t = (char *)"Temperature: X16";
        configs.BMP_ODR = (char *)"Standby: 80 milliseconds";
    }

    /**
     * @brief Initializes all the sensors on the sensor array
     * 
     * @return true all sensors initialized successfully
     * @return false a sensor failed to initialize
     */
    bool initAll()
    { 
        //Individual functions handle logging to debugger
        if (!initAccel())
        {
            return false;
        }
        if (!initGyro())
        {
            return false;
        }
        if (!initMag())
        {
            return false;
        }
        if (!initBaro())
        {
            return false;
        }

        return true;
    }

    /**
     * @brief Scans all the I2C addresses and returns the addresses of the devices found
     *
     * @return std::vector<uint8_t> vector of addresses of the devices found
     */
    std::vector<uint8_t> scanI2C()
    {
        uint8_t error = 0;
        uint8_t address = 0;
        int nDevices = 0;

        for (address = 1; address < 127; address++)
        {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();

            if (error == 0)
            {
                INFO_LOGf("I2C device found at address 0x%d", address);
                configs.addresses.push_back(address); // Saving the address to the StartInfo struct
                nDevices++;
            }
            else if (error == 4)
            {
                INFO_LOGf("Unknow error at address 0x%d", address)
            }
        }

        configs.num_devices = nDevices; // How many devices were found
        return configs.addresses;       // Returning the addresses of the devices found
    }

    /**
     * @brief Configures interrupt ISRS from specific sensors
     *
     * @param bar_int pin that the barometer interrupt is connected to
     * @param accel_int pin that the accelerometer interrupt is connected to
     * @param gyro_int pin that the gyroscope interrupt is connected to
     * @param mag_int pin that the magnetometer interrupt is connected to
     */
    void setInterrupts(const uint8_t bar_int, const uint8_t accel_int, const uint8_t gyro_int, const uint8_t mag_int)
    {
        // Setting the interrupt pins as inputs
        pinMode(bar_int, INPUT);
        pinMode(accel_int, INPUT);
        pinMode(gyro_int, INPUT);
        pinMode(mag_int, INPUT);

        // Attaching the interrupt ISRs
        attachInterrupt(digitalPinToInterrupt(bar_int), bar_drdy, RISING);
        attachInterrupt(digitalPinToInterrupt(accel_int), accel_drdy, RISING);
        attachInterrupt(digitalPinToInterrupt(gyro_int), gyro_drdy, RISING);
        attachInterrupt(digitalPinToInterrupt(mag_int), mag_drdy, RISING);
    }

    /**
     * @brief Configures interrupt ISRs from specific sensors using the default pins
     *
     */
    void setInterrupts()
    {
        setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);
    }

    void bar_drdy()
    {
        bar_flag = true;
    }

    void accel_drdy()
    {
        accel_flag = true;
    }

    void gyro_drdy()
    {
        gyro_flag = true;
    }

    void mag_drdy()
    {
        mag_flag = true;
    }

    /**
     * @brief Reads idle bias by averaging 100 gyro readings
     *
     * @return Angles_3D<double> bias values
     */
    Angles_3D<double> setGyroBias()
    {
        // Summation of all the gyro readings
        double gxIntegration = 0.0;
        double gyIntegration = 0.0;
        double gzIntegration = 0.0;

        // Read 100 samples
        for (uint8_t i = 0; i < 100; i++)
        {
            Angles_3D<double> gyro = returnRawGyro();
            gxIntegration += gyro.x;
            gyIntegration += gyro.y;
            gzIntegration += gyro.z;
            delayMicroseconds(100);
        }

        // Calculate the average
        gyro_bias.x = gxIntegration / 100.0;
        gyro_bias.y = gyIntegration / 100.0;
        gyro_bias.z = gzIntegration / 100.0;

        // Saving the configurations to the StartInfo struct
        configs.gyro_bias.x = gyro_bias.x;
        configs.gyro_bias.y = gyro_bias.y;
        configs.gyro_bias.z = gyro_bias.z;

        return gyro_bias;
    }

    /**
     * @brief Reads the raw barometer data
     *
     * @return BMP388Data object storing the temperature and pressure
     */
    BMP388Data returnRawBaro()
    {
        BMP388Data data;

        double pres_hpa = 0.0;
        baro.getTempPres(data.temperature, pres_hpa);

        data.pressure = pres_hpa * 0.0009869233; // convert to atm

        return data;
    }

    /**
     * @brief Reads the raw accelerometer data (m/s^2)
     *
     * @return Angles_3D<double> object storing the x, y, and z acceleration values
     */
    Angles_3D<double> returnRawAccel()
    {
        Angles_3D<double> accel_data;

        accel.readSensor();
        accel_data.x = accel.getAccelX_mss();
        accel_data.y = accel.getAccelY_mss();
        accel_data.z = accel.getAccelZ_mss();

        return accel_data;
    }

    /**
     * @brief Reads the raw gyroscope data (rad/s)
     *
     * @return Angles_3D<double> object storing the x, y, and z angular velocity values
     */
    Angles_3D<double> returnRawGyro()
    {
        Angles_3D<double> gyro_data;

        gyro.readSensor();
        // Read from the gyro and subtract the offset values
        gyro_data.x = gyro.getGyroX_rads() - gyro_bias.x;
        gyro_data.y = gyro.getGyroY_rads() - gyro_bias.y;
        gyro_data.z = gyro.getGyroZ_rads() - gyro_bias.z;

        return gyro_data;
    }

    /**
     * @brief Reads the raw magnetometer data (uTesla)
     *
     * @return Angles_3D<double> object storing the x, y, and z magnetic field values
     */
    Angles_3D<double> returnRawMag()
    {
        Angles_3D<double> mag_data;

        mag.read();
        // Convert to mTesla. Rangle is +/-4 so we divide by 6842 to get gauss, then mult. by 100 to get utesla
        mag_data.x = mag.m.x / 68.42 - mag_bias.x;
        mag_data.y = mag.m.y / 68.42 - mag_bias.y;
        mag_data.z = mag.m.z / 68.42 - mag_bias.z;

        return mag_data;
    }

    /**
     * @brief Reads the raw temperature data (C)
     *
     * @return double temperature value
     */
    double returnAccelTemp()
    {
        return accel.getTemperature_C();
    }

    void logData(LoggedData &data)
    {
        // Flags are triggered by the interrupt ISRs set in setInterrupts()
        // Mag, gyro, accel, and baro all have different interrupt pins
        if (mag_flag) // If the magnetometer has new data
        {
            data.rmag = returnRawMag();

            data.fmag.x = mag_x.filt(data.rmag.x, data.delta_time);
            data.fmag.y = mag_y.filt(data.rmag.y, data.delta_time);
            data.fmag.z = mag_z.filt(data.rmag.z, data.delta_time);
            mag_flag = false;
        }

        if (accel_flag) // If the accelerometer has new data
        {
            data.racc = returnRawAccel();
            data.bmi_temp = returnAccelTemp();

            accel_flag = false;
        }

        if (gyro_flag) // If the gyroscope has new data
        {
            data.rgyr = returnRawGyro();

            gyro_flag = false;
        }

        if (bar_flag) // If the barometer has new data
        {
            data.raw_bmp = returnRawBaro();

            bar_flag = false;
        }
    }

}
