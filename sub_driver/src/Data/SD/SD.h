/**
 * @file SD.h
 * @author Daniel Kim
 * @brief Logs SD data from struct and reads it back
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#ifndef SD_h
#define SD_h

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#include "RingBuffer.h"
#include "DataFile.h"
#include "../data_struct.h"

extern SdFs sd;
extern FsFile file;

class SD_Logger
{
public:
    SD_Logger();

    bool init();
    bool logData(Data data);

    bool rewindPrint();

private:
    char* data_filename;
    char* csv_filename;
    unsigned long long previous_time;
    bool ASCII_header_made = false;

    unsigned long long iterations;

    static unsigned int freeMemory();
    unsigned int findFactors();

    
    
};



#endif

