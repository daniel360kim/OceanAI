/**
 * @file StartInfo.h
 * @author Daniel Kim
 * @brief Start info configurations to be saved
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */
#ifndef StartInfo_h
#define StartInfo_h

#include <Arduino.h>
#include <vector>
#include <stdint.h>

#include "data_struct.h"

struct StartFile_d
{
    // File names
  char *bin_file;
  char *ascii_file;

// Conditional Compilation Configurations
#if DEBUG_ON
    static constexpr bool debug = true;

#else
    static constexpr bool debug = false;
#endif

#if OPTICS_ON == true
        static constexpr bool optics = true;
#else
    static constexpr bool optics = false;
#endif

    // Camera
    uint8_t img_size;
    uint8_t img_format;

    //SD
    unsigned long sd_cap;

    //I2C Data
    std::vector<uint8_t> addresses;
    std::vector<uint8_t> errors;
    int num_devices;

    /////////////Sensors////////////
    //BMP388 Oversampling
    char* BMP_os_t;
    char* BMP_os_p;

    char* BMP_ODR;

    char* accel_range;
    char* gyro_range;
    char* mag_range;

    char* accel_ODR;
    char* gyro_ODR;
    char* mag_ODR;

    Vec3 gyro_bias;
    Vec3 mag_bias;
};

extern StartFile_d configs;


#endif
