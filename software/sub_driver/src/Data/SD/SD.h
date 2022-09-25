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
#include <tuple>
#include <stdint.h>
#include <vector>
#include <CrashReport.h>

#include "DataFile.h"
#include "../data_struct.h"
#include "../../config.h"
#include "StaticQueue.h"
#include "../../mission.h"

extern SdFs sd;
extern FsFile file;

class SD_Logger
{
public:
    SD_Logger() {}
    SD_Logger(Duration mission, uint32_t log_interval);

    bool log_crash_report();
    bool init();
    bool logData(Data data);

    /**
     * @brief closes current file
     * 
     * @return true file close successful
     * @return false file close unsuccessful
     */
    static bool closeFile() { return file.close(); }

    static bool reopenFile(const char* filename);

    bool rewindPrint();

    char* data_filename;

private:
    char* csv_filename;
    uint64_t previous_time;

    unsigned long pt_cap_update;

    uint64_t iterations;

    static volatile bool cap_update_int;

    static unsigned int freeMemory();
    unsigned int findFactors();
    static bool flush(void*);
    static bool getCapacity(void*);


    StaticCircularBuffer<Data, 10> buf;

    uint64_t m_log_file_size;
    uint32_t log_interval;


};





#endif

