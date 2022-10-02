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

#include <ADC.h>
#include <ADC_util.h>
#include <Wire.h>
#include <stdint.h>
#include <array>

#include "../core/config.h"
#include "../core/debug.h"
#include "LowPass.h"
#include "../core/timed_function.h"
#include "../Data/StartInfo.h"
#include "../core/Timer.h"

UnifiedSensors UnifiedSensors::instance;

BMP388_DEV baro;
Bmi088Accel accel(Wire, 0x18);
Bmi088Gyro gyro(Wire, 0x68);

LIS3MDL mag;

ADC adc;

volatile bool UnifiedSensors::bar_flag = false;
volatile bool UnifiedSensors::accel_flag = false;
volatile bool UnifiedSensors::gyro_flag = false;
volatile bool UnifiedSensors::mag_flag = true;

bool UnifiedSensors::pressure_sensor_connected = false;

uint8_t UnifiedSensors::TDS_pin = 0;
uint8_t UnifiedSensors::voltage_pin = 0;
uint8_t UnifiedSensors::pressure_pin = 0;

double UnifiedSensors::temp = 0;

void UnifiedSensors::scanAddresses()
{
    uint8_t error = 0;
    uint8_t address = 0;
    int nDevices = 0;

    for(address = 1; address < 127; address++ )
    {
        
        Wire.beginTransmission(address);

        Time::NamedTimer i2c("I2C end Transmission");
        error = Wire.endTransmission();
        Time::TimerManager::getInstance().addTimer(i2c.getTimeInfo());
    
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

    bmp_pres.setCutoff(10);
    bmp_temp.setCutoff(5);

    configs.BMP_os_p = (char *)"Pressure: X4";
    configs.BMP_os_t = (char *)"Temperature: X16";
    configs.BMP_ODR = (char *)"Standby: 80 milliseconds";

    status[1] = accel.begin();
    accel.setOdr(Bmi088Accel::ODR_800HZ_BW_140HZ);
    accel.pinModeInt1(Bmi088Accel::PUSH_PULL, Bmi088Accel::ACTIVE_HIGH);
    accel.mapDrdyInt1(true);
    accel.setRange(Bmi088Accel::RANGE_3G);

    acc_x.setCutoff(30);
    acc_y.setCutoff(30);
    acc_z.setCutoff(30);

    configs.accel_range = (char *)"Accel: 3G";
    configs.accel_ODR = (char *)"Accel ODR: 800Hz";

    status[2] = gyro.begin();
    gyro.setOdr(Bmi088Gyro::ODR_1000HZ_BW_116HZ);
    gyro.pinModeInt3(Bmi088Gyro::PUSH_PULL, Bmi088Gyro::ACTIVE_HIGH);
    gyro.mapDrdyInt3(true);
    gyro.setRange(Bmi088Gyro::RANGE_250DPS);

    gyr_x.setCutoff(30);
    gyr_y.setCutoff(30);
    gyr_z.setCutoff(30);

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
    configs.mag_bias = { HARD_IRON_BIAS[0], HARD_IRON_BIAS[1], HARD_IRON_BIAS[2] };

    for (uint8_t i = 0; i < status.size(); i++)
    {
        if (status[i] < 0)
        {
            switch (i)
            {
            case 0:
            {
                #if DEBUG_ON == true
                                char* bar_message = (char *)"Sensors: Barometer initialization error";
                                Debug::error.addToBuffer(scoped_timer.elapsed(), Debug::Critical_Error, bar_message);

                #if LIVE_DEBUG == true
                                Serial.println(F(bar_message));
                #endif

                #endif

                return false;
                break;
            }
            case 1:
            {
                #if DEBUG_ON == true
                                char* acc_message = (char *)"Sensors: Accelerometer initialization error";
                                Debug::error.addToBuffer(scoped_timer.elapsed(), Debug::Critical_Error, acc_message);

                #if LIVE_DEBUG == true
                                Serial.println(F(acc_message));
                #endif

                #endif

                return false;
                break;
            }
            case 2:
            {
                #if DEBUG_ON == true
                                char* gyr_message = (char *)"Sensors: Gyroscope initialization error";
                                Debug::error.addToBuffer(scoped_timer.elapsed(), Debug::Critical_Error, gyr_message);

                #if LIVE_DEBUG == true
                                Serial.println(F(gyr_message));
                #endif

                #endif

                return false;
                break;
            }

            case 3:
            {
                #if DEBUG_ON == true
                                char* mag_message = (char *)"Sensors: Magentometer initialization error";
                                Debug::error.addToBuffer(scoped_timer.elapsed(), Debug::Critical_Error, mag_message);

                #if LIVE_DEBUG == true
                                Serial.println(F(mag_message));
                #endif

                #endif

                return false;
                break;
            }
            default:
            {
                #if DEBUG_ON == true
                                char* unk_message = (char *)"Sensors: Unknown sensor error initialization error";
                                Debug::error.addToBuffer(scoped_timer.elapsed(), Debug::Critical_Error, unk_message);

                #if LIVE_DEBUG == true
                                Serial.println(F(unk_message));
                #endif

                #endif
                return false;
            }
            }
        }
    }

#if DEBUG_ON == true
    char *message = (char *)"Sensors: All sensors successfully initialized";
    Debug::success.addToBuffer(scoped_timer.elapsed(), Debug::Success, message);

#if LIVE_DEBUG == true
    Serial.println(F(message));
#endif

#endif
    return true;
}

void UnifiedSensors::initADC()
{
    adc.adc0->setAveraging(1);                                       // set number of averages
    adc.adc0->setResolution(12);                                     // set bits of resolution
    adc.adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);  // change the conversion speed
    adc.adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed

////// ADC1 /////
#ifdef ADC_DUAL_ADCS
    adc.adc1->setAveraging(1);                                       // set number of averages
    adc.adc1->setResolution(12);                                     // set bits of resolution
    adc.adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);  // change the conversion speed
    adc.adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
#endif
}

void UnifiedSensors::initTDS(uint8_t TDS_pin, uint32_t interval, double filt_cutoff)
{
    pinMode(TDS_pin, INPUT);
    this->TDS_pin = TDS_pin;

    //Setting interval and function pointer for reading from the sensor
    tds_function.setInterval(interval);
    tds_function.setFunction(UnifiedSensors::readTDS);

    tds_filter.setCutoff(filt_cutoff);
}

void UnifiedSensors::initVoltmeter(uint8_t input_pin, uint32_t interval, double filt_cutoff)
{
    pinMode(input_pin, INPUT);
    voltage_pin = input_pin;
    
    //Setting interval and function pointer for reading from the sensor
    voltage_function.setInterval(interval);
    voltage_function.setFunction(UnifiedSensors::readVoltage);

    voltage_filter.setCutoff(filt_cutoff);
}

void UnifiedSensors::initPressureSensor(uint8_t input_pin, uint32_t interval, double filt_cutoff)
{
    pressure_pin = input_pin;
    pinMode(pressure_pin, INPUT);

    double pressure_voltage = 0.0;
    readExternalPressure_v(pressure_voltage);

    /*Pressure sensor outputs at least 0.1V, so it is malfunctional or disconnected if it outputs less than 0.1V*/
    if(pressure_voltage <= 0.1)
    {
        pressure_sensor_connected = false;
        return;
    }
    else
    {
        pressure_sensor_connected = true;
        pressure_function.setInterval(interval);
        pressure_function.setFunction(UnifiedSensors::readExternalPressure);
    }

    ext_pres.setCutoff(filt_cutoff);
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
        double gx, gy, gz;
        returnRawGyro(&gx, &gy, &gz);
        gxIntegration += gx;
        gyIntegration += gy;
        gzIntegration += gz;
        delayMicroseconds(100);
    }

    gx_bias = gxIntegration / 100;
    gy_bias = gyIntegration / 100;
    gz_bias = gzIntegration / 100;

    configs.gyro_bias.x = gx_bias;
    configs.gyro_bias.y = gy_bias;
    configs.gyro_bias.z = gz_bias;
}
void UnifiedSensors::returnRawBaro(double *pres, double *temp)
{
    double temperature = 0;
    double pres_hpa = 0;
    baro.getTempPres(temperature, pres_hpa);

    *pres = pres_hpa * 0.0009869233; //convert to atm
    *temp = temperature;
}

void UnifiedSensors::returnRawAccel(double *x, double *y, double *z, double *tempC)
{
    accel.readSensor();
    *x = accel.getAccelX_mss();
    *y = accel.getAccelY_mss();
    *z = accel.getAccelZ_mss();
    *tempC = accel.getTemperature_C();
}
void UnifiedSensors::returnRawGyro(double *x, double *y, double *z)
{
    gyro.readSensor();
    *x = gyro.getGyroX_rads() - gx_bias;
    *y = gyro.getGyroY_rads() - gy_bias;
    *z = gyro.getGyroZ_rads() - gz_bias;
}

void UnifiedSensors::returnRawMag(double *x, double *y, double *z)
{
    mag.read();

    // Convert to mTesla. Rangle is +/-4 so we divide by 6842 to get gauss, then mult. by 100 to get mtesla
    *x = mag.m.x / 68.42 - HARD_IRON_BIAS[0];
    *y = mag.m.y / 68.42 - HARD_IRON_BIAS[1];
    *z = mag.m.z / 68.42 - HARD_IRON_BIAS[2];
}

void UnifiedSensors::readTDS(double& tds_reading)
{
    int tds = adc.adc0->analogRead(TDS_pin);

    double averageVoltage = tds * (double)VREF / 1024.0;
    double compensationCoefficient = 1.0 + 0.02 * (temp - 25.0);                                                                                                                            // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    double compensationVolatge = averageVoltage / compensationCoefficient;                                                                                                                  // temperature compensation
    tds_reading = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; // convert voltage value to tds value
}

void UnifiedSensors::readVoltage(double& voltage)
{
    voltage = adc.adc1->analogRead(voltage_pin) * (double)VREF / 1024.0;
    voltage = voltage * (9.95 + 1.992) / 1.992;

    if (voltage <= 0.1)
        voltage = 0.0;
}

void UnifiedSensors::readExternalPressure_v(double& voltage)
{
    int tds = analogRead(pressure_pin);
    voltage = tds * (double)VREF / 1024.0;
}

void UnifiedSensors::readExternalPressure(double& pressure)
{
    double voltage = 0.0;
    readExternalPressure_v(voltage);
    double psi = (100.0 / 3.0) * voltage + (50.0 - ((100.0 / 3.0) * (3.3 / 2.0))); //pressure = 25psi * voltage - 12.5psi (linear)
    pressure = psi / 14.6959488;
}
void UnifiedSensors::logToStruct(Data &data)
{
    if (UnifiedSensors::mag_flag)
    {
        returnRawMag(&data.mag.x, &data.mag.y, &data.mag.z);

        UnifiedSensors::mag_flag = false;
    }

    if (UnifiedSensors::accel_flag && UnifiedSensors::gyro_flag)
    {

        returnRawAccel(&data.racc.x, &data.racc.y, &data.racc.z, &data.bmi_temp);

        data.facc.x = acc_x.filt(data.racc.x, data.dt);
        data.facc.y = acc_y.filt(data.racc.y, data.dt);
        data.facc.z = acc_z.filt(data.racc.z, data.dt);

        temp_measurements[1] = data.bmi_temp;

        UnifiedSensors::accel_flag = false;

        returnRawGyro(&data.rgyr.x, &data.rgyr.y, &data.rgyr.z);

        data.fgyr.x = gyr_x.filt(data.rgyr.x, data.dt);
        data.fgyr.y = gyr_y.filt(data.rgyr.y, data.dt);
        data.fgyr.z = gyr_z.filt(data.rgyr.z, data.dt);

        UnifiedSensors::gyro_flag = false;
    }

    if (UnifiedSensors::bar_flag)
    {
        returnRawBaro(&data.bmp_rpres, &data.bmp_rtemp);
        temp_measurements[0] = data.bmp_rtemp;

        data.bmp_fpres = bmp_pres.filt(data.bmp_rpres, data.dt);
        data.bmp_ftemp = bmp_temp.filt(data.bmp_rtemp, data.dt);

        UnifiedSensors::bar_flag = false;
    }

    tds_function.void_tick(data.r_TDS);
    voltage_function.void_tick(data.r_voltage);
    pressure_function.void_tick(data.external.raw_pres);

    data.f_TDS = tds_filter.filt(data.r_TDS, data.dt);
    data.f_voltage = voltage_filter.filt(data.r_voltage, data.dt);
    data.external.filt_pres = ext_pres.filt(data.external.raw_pres, data.dt); 
    
}
