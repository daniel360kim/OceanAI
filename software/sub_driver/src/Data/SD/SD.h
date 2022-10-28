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
#include <stdint.h>
#include <vector>
#include <CrashReport.h>
#include <queue>
#include <ArduinoJson.h>

#include "DataFile.h"
#include "../data_struct.h"
#include "../../core/config.h"
#include "../../core/Timer.h"
#include "../../core/timed_function.h"

extern SdFs sd;
extern FsFile file;

class SD_Logger
{
public:
    SD_Logger() {}
    SD_Logger(const int64_t duration_ns, int log_interval_ns);

    bool log_crash_report();
    bool init();
    bool logData(Data &data);

    static bool closeFile() { return file.close(); }

    static bool reopenFile(const char* filename);

    template<int N>
    void data_to_json(Data &data, StaticJsonDocument<N> &doc);


private:
    const char* m_data_filename; //name of the binary data
    const char* m_csv_filename; //name of the ascii data
    int64_t m_previous_log_time;
    unsigned long long m_write_iterations;
    
    static void flush(void*);
    static void getCapacity(uint32_t &capacity);

    std::queue<StaticJsonDocument<67>> write_buf;

    Time::Async<void, void*> flusher;
    Time::Async<void, uint32_t&> capacity_updater;

    uint64_t m_log_file_size;
    int m_log_interval;

    bool m_inital_cap_updated = false;

    
};





#endif

