/**
 * @file external_sensor.h
 * @author Daniel Kim
 * @brief Communication with external sensor
 * @version 0.1
 * @date 2022-08-12
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef external_sensor_h
#define external_sensor_h

#include "../../data/data_struct.h"

class ExternalSensor
{
public:
    struct RawData
    {
        unsigned long long time_us;
        float loop_time;
        float temperature;
        float pressure;
    };
    
    ExternalSensor(uint8_t i2c_address) : i2c_address(i2c_address) {}
    RawData getSensorData();

private:
    uint8_t i2c_address;

};




#endif