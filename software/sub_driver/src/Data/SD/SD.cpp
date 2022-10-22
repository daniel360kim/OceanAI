/**
 * @file SD.cpp
 * @author Daniel Kim
 * @brief Logs SD data from struct and reads it back
 * @version 0.1
 * @date 2022-07-01
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <vector>
#include <queue>
#include <memory>
#include <CrashReport.h>
#include <stdint.h>

#include "SD.h"
#include "DataFile.h"
#include "../data_struct.h"

#include "../../core/debug.h"
#include "../../core/config.h"
#include "../StartInfo.h"
#include "../../core/Timer.h"

extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;

SdFs sd;
FsFile file;
cid_t cid;

/**
 * @brief Construct a new sd logger::sd logger object
 * 
 * @param mission mission information
 * @param log_interval how many nanoseconds between each log 
 */
SD_Logger::SD_Logger(Time::Mission mission, uint32_t log_interval) 
{
    m_log_file_size = sizeof(Data) * (mission.mission_time / 1e+9) * (log_interval / 1e+9);
    this->log_interval = log_interval;

    flusher.setInterval(3e+10);
    flusher.setFunction(SD_Logger::flush);

    capacity_updater.setInterval(360000000000);
    capacity_updater.setFunction(getCapacity);
}

bool SD_Logger::log_crash_report()
{
    if(!sd.begin(SdioConfig(FIFO_SDIO)))
    {
        #if DEBUG_ON
            char* msg = (char*)"SD card initialization failed";
            ERROR_LOG(Debug::Critical_Error, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }

    if(!sd.exists("logs"))
    {
        if(!sd.mkdir("logs"))
        {
            #if DEBUG_ON
                char* msg = (char*)("Failed to create logs directory");
                ERROR_LOG(Debug::Fatal, *msg);
                #if LIVE_DEBUG 
                    Serial.println(*msg);
                #endif
            #endif
            return false;
        }
    }
    DataFile crash_file("logs/crash", DataFile::ENDING::TXT);
    if(!crash_file.createFile())
    {
        #if DEBUG_ON 
            char* msg = (char*)"Failed to create crash file";
            ERROR_LOG(Debug::Fatal, msg);
            #if LIVE_DEBUG 
                Serial.println(msg);
            #endif
        #endif
        return false;
    }

    file.open(crash_file.file_name, O_WRITE | O_CREAT);
    CrashReport.printTo(file);
    file.close();

    #if DEBUG_ON
        char* msg = (char*)("Crash report logged successfully");
        SUCCESS_LOG(*msg);
        #if LIVE_DEBUG 
            Serial.println(*msg);
        #endif
    #endif
    return true;
}
/**
 * @brief create initial headers and start file
 * 
 * @return true creation succesful  
 * @return false creation unsuccesful
 */
bool SD_Logger::init()
{
    //Creating binary file 
    DataFile data("data", DataFile::ENDING::DAT);
    data_filename = data.file_name;
    configs.bin_file = data_filename;

    //Creating CSV file
    DataFile csvFile("ASCII", DataFile::ENDING::CSV);
    if(!csvFile.createFile())
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to create CSV file");
            ERROR_LOG(Debug::Fatal, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }
    csv_filename = csvFile.file_name;
    configs.ascii_file = csv_filename;

    if(!file.open(csv_filename, O_WRITE | O_CREAT))
    {
        #if DEBUG_ON

            ERROR_LOG(Debug::Fatal, "Failed to open CSV file");
        #endif
        return false;
    }

    //Creating header for CSV file
    file.print(("Time us,loop_time(hz),sys_state,dt(sec),"));
    file.print(("bmp_rpres(atm), bmp_rtemp(°C), bmp_fpres(atm), bmp_ftemp(°C),"));
    file.print(("racc_x(m/s/s),racc_y(m/s/s),racc_z(m/s/s),facc_x(m/s/s),facc_y(m/s/s),facc_z(m/s/s),"));
    file.print(("wfax(m/s/s),wfay(m/s/s),wfaz(m/s/s),"));
    file.print(("vx(m/s),vy(m/s),vz(m/s),px(m),py(m),pz(m),"));
    file.print(("rgx(rad/s),rgy(rad/s),rgz(rad/s),fgx(rad/s),fgy(rad/s),fgz(rad/s),"));
    file.print(("rel_x(deg),rel_y(deg),rel_z(deg),"));
    file.print(("rel_w,rel_x,rel_y,rel_z,"));
    file.print(("bmi_temp(°C),"));
    file.print(("rmx(T),rmy(T),rmz(T),"));
    file.print(("ext_loop_time(hz),ext_rtemp(°C),ext_rpres(atm),ext_ftemp(°C),ext_fpres(atm),"));
    file.print(("raw_TDS,filt_TDS,raw_voltage,filt_voltage,clk_speed,int_temp(°C),"));
    file.print(("dive_limit,dive_homed,dive_current_pos,dive_current_pos_mm,dive_target_pos,dive_target_pos_mm,"));
    file.print(("dive_speed,dive_accel,dive_max_speed,"));
    file.print(("pitch_limit,pitch_homed,pitch_current_pos,pitch_current_pos_mm,dive_target_pos,dive_target_pos_mm,"));
    file.print(("pitch_speed,pitch_accel,pitch_max_speed,"));
    file.print(("cap_time,save_time,fifo_length,"));
    file.print(("sd_capacity\n"));

    file.close();

    //Updating SD Capacity
    configs.sd_cap = sd.freeClusterCount();

    if(!data.createFile())
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to create binary file");
            ERROR_LOG(Debug::Fatal, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }

    //Creating directory for our images
    if(!sd.exists("Images"))
    {
        if(!sd.mkdir("Images"))
        {
            #if DEBUG_ON
                char* msg = (char*)("Failed to create Images directory");
                ERROR_LOG(Debug::Fatal, *msg);
                #if LIVE_DEBUG 
                    Serial.println(*msg);
                #endif
            #endif
            return false;
        }
    }

    DataFile start("start", DataFile::ENDING::TXT);
    if(!start.createFile())
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to create start file");
            ERROR_LOG(Debug::Fatal, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }

    if(!file.open(start.file_name, O_WRITE | O_CREAT))
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to open start file");
            ERROR_LOG(Debug::Fatal, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }

    //Creating header file
    file.println("OceanAI Start Configurations");
    for(int i = 0; i < 255; i++)
    {
        file.print("*");
    }
    
    file.print("\n");
    file.println("////////// Printing to: //////////");
    file.print("Binary File: "); file.println(data_filename);
    file.print("ASCII File: "); file.println(csv_filename);
    file.print("SD Capacity: "); file.print(configs.sd_cap); file.println(" bytes");
    file.println("/////////////////////////////////\n");

    file.println("////////// Conditional Compilation Configurations //////////");
    file.println("0 = false | 1 = true");
    file.print("Debug: "); file.println(configs.debug);
    file.print("Optics: "); file.println(configs.optics);
    file.println("/////////////////////////////////\n");

    file.println("////////// I2C Device Data //////////");
    file.println("I2C Addresses Detected:");
    if(configs.num_devices == 0)
    {
        file.print("No I2C devices detected "); file.print("("); file.print(configs.num_devices); file.println(")");
    }
    else
    {           
        for(unsigned int i = 0; i < configs.addresses.size(); i++)
        {
            file.print("0x");
            if(configs.addresses[i] < 16)
            {
                file.print("0");
            }
            file.print(configs.addresses[i], HEX);
            file.print("\n");
        }
        file.print("\n");
        file.println("I2C Errors:");
        if(configs.errors.empty())
        {
            file.println("No I2C errors detected");
        }
        else
        {
            for(unsigned int i = 0; i < configs.errors.size(); i++)
            {
                file.print("0x");
                if(configs.errors[i] < 16)
                {
                    file.print("0");
                }
                file.print(configs.errors[i], HEX);
                file.print("\n");
            }
        }
    }
    file.print("\n");

    file.println("////////// Sensor Configurations /////////");
    file.println("BMP388: "); file.println(configs.BMP_os_p); file.println(configs.BMP_os_t); file.println(configs.BMP_ODR);
    file.print("\n");
    file.println("IMU Package - Accel: "); file.println(configs.accel_range); file.println(configs.accel_ODR);
    file.print("\n");
    file.println("IMU Package - Gyro: "); file.println(configs.gyro_range); file.println(configs.gyro_ODR);
    file.print("X Bias: "); file.print(configs.gyro_bias.x); file.print("   ");
    file.print("Y Bias: "); file.print(configs.gyro_bias.y); file.print("   ");
    file.print("Z Bias: "); file.print(configs.gyro_bias.z); file.print("\n");
    file.print("\n");
    file.println("IMU Package - Mag: "); file.println(configs.mag_range); file.println(configs.mag_ODR);
    file.print("X Bias: "); file.print(configs.mag_bias.x); file.print("   ");
    file.print("Y Bias: "); file.print(configs.mag_bias.y); file.print("   ");
    file.print("Z Bias: "); file.print(configs.mag_bias.z); file.print("\n");
    
    for(int i = 0; i < 255; i++)
    {
        file.print("*");
    }

    if(!file.close())
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to close start file");
            ERROR_LOG(Debug::Fatal, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }


    if(!file.open(data_filename, O_WRITE | O_CREAT))
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to open binary file");
            ERROR_LOG(Debug::Critical_Error, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }
    
    //file must be preallocated to prevent spending lots of time searching for free clusters
    if(!file.preAllocate(m_log_file_size + 10000))
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to preallocate binary file");
            ERROR_LOG(Debug::Critical_Error, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        file.close();
        return false;
    }
    return true;
}

/**
 * @brief logs our data to the SD card
 * 
 * @param data strucutre with our logged data
 * @return true 
 * @return false 
 */
bool SD_Logger::logData(Data data)
{
    if(!m_inital_cap_updated)
    {
        data.sd_capacity = configs.sd_cap;
        m_inital_cap_updated = true;
    }
    //Logging at a certain interval
    uint64_t current_time = scoped_timer.elapsed();
    if(current_time - previous_time >= log_interval)
    {
        //If our sd card is busy we add to our buffer
        if(file.isBusy())
        {
            #if DEBUG_ON
                char* msg = (char*)("Pushed SD data to buffer: SD card is busy");
                ERROR_LOG(Debug::Warning, *msg);
                #if LIVE_DEBUG 
                    Serial.println(*msg); 
                #endif
            #endif
            write_buf.push(data);
        }
        else
        {
            //If nothing is in the buffer we directly write to the sd card
            if(write_buf.size() == 0)
            {
                file.write((const uint8_t*)&data, sizeof(data));
                iterations++;
            }
            else
            {
                //If there is data in the buffer we move it into the buffer and write the oldest data to the sd card
                write_buf.push(data);
                data = write_buf.front();
                write_buf.pop();
                file.write((const uint8_t*)&data, sizeof(data));
                iterations++;
            }
        }
        previous_time = scoped_timer.elapsed();
    }
    if(write_buf.size() > 100)
    {
        #if DEBUG_ON
            char* msg = (char*)("SD Buffer Overflow: clearing and reducing log interval");
            ERROR_LOG(Debug::Warning, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        //Clear the old data if it gets too large
        std::queue<Data> empty;
        std::swap(write_buf, empty);

        log_interval+= 10000000; //If the buffer is too large we increase the log interval to prevent overflow
    }

    flusher.void_tick(this);
    capacity_updater.void_tick(data.sd_capacity);
    
    return true;
}

/**
 * @brief converts binary file to csv file
 * reads a line of the binary file, saves it to a buffer several times
 * closes binary file, opens csv file, writes buffer to csv file
 * keep doing till all data is converted
 * 
 * We do this since writing as binary is wayyyyyyyyyy faster than writing as ASCII
 * So once the mission is complete, we can spend several minutes converting
 * 
 * @return true conversion successful
 * @return false conversion unsuccessful
 */
bool SD_Logger::rewindPrint()
{
    if(!file.close())
    {
        #if DEBUG_ON
            char* msg = (char*)("Failed to close binary file before translation");
            ERROR_LOG(Debug::Critical_Error, *msg);
            #if LIVE_DEBUG 
                Serial.println(*msg);
            #endif
        #endif
        return false;
    }

    //Freeing memory from write_buffer
    std::queue<Data> empty;
    std::swap(write_buf, empty);

    //finding a buffer size based on available memory and how much data we logged
    //this allows us to create the largest buffer possible for less opens and closed of files
    //pretty nifty tbh
    long buf_size = findFactors(); 
    long write_iterator = 0;
    std::queue<Data> read_buf;


    //Since the findFactors() function automatically makes sure we have the greatest common factor of the iterations, we can just divide the iterations by buf_size
    for(uint64_t j = 0; j < iterations / buf_size; j++)
    {
        if(!file.open(data_filename, FILE_READ))
        {
            #if DEBUG_ON
                char* msg = (char*)("Failed to open binary file for translation");
                ERROR_LOG(Debug::Critical_Error, *msg);
                #if LIVE_DEBUG 
                    Serial.println(*msg);
                #endif
            #endif
            return false;
        }

        Data datacopy; //creating a copy of our data to read into
        for(long i = 0; i < buf_size + (write_iterator * buf_size); i++)
        {
            file.read((uint8_t *)&datacopy, sizeof(datacopy));
            if(i >= buf_size + ((write_iterator - 1) * buf_size))
            {
                read_buf.push(datacopy); //inserting data into our buffer
            }
        }

        if(!file.close())
        {
            #if DEBUG_ON
                char* msg = (char*)("Failed to close binary file after reading");
                ERROR_LOG(Debug::Critical_Error, *msg);
                #if LIVE_DEBUG 
                    Serial.println(*msg);
                #endif

            #endif
            return false;
        }


        if(!file.open(csv_filename, O_CREAT | O_APPEND | O_WRITE))
        {
            #if DEBUG_ON
                char* msg = (char*)("Failed to open csv file for writing");
                ERROR_LOG(Debug::Critical_Error, *msg);
                #if LIVE_DEBUG 
                    Serial.println(*msg);
                #endif
            #endif
            return false;
        }

        //Printing the copied data into the csv fie
        for(long i = 0; i < buf_size; i++)
        {
            char* comma = (char*)","; //We use comma so much, better to use as a variable
            Data cc;
            cc = read_buf.front();
            read_buf.pop();

            char* system_state = nullptr;

            //Translating integer system state into readable characters
            switch(cc.system_state)
            {
                case 0:
                    system_state = (char*)"Initialization";
                    break;
                case 1:
                    system_state = (char*)"Error Indication";
                    break;
                case 2:
                    system_state = (char*)"Idle Mode";
                    break;
                case 3:
                    system_state = (char*)"Diving Mode";
                    break;
                case 4:
                    system_state = (char*)"Resurfacing Mode";
                    break;
                case 5:
                    system_state = (char*)"Surfaced";
                    break;
                case 6:
                    system_state = (char*)"SD_translating";
                    break;
                case 7:
                    system_state = (char*)"SD_reinitializing";
                    break;
                default:
                    system_state = (char*)"Unknown";
                    break;
            }
            file.print(cc.time_ns); file.print(comma); file.print(cc.loop_time); file.print(comma);
            file.print(system_state); file.print(comma);
            file.print(cc.dt,10); file.print(comma);
            file.print(cc.bmp_rpres); file.print(comma); file.print(cc.bmp_rtemp); file.print(comma); file.print(cc.bmp_fpres); file.print(comma); file.print(cc.bmp_ftemp);file.print(comma);
            file.print(cc.racc.x); file.print(comma); file.print(cc.racc.y); file.print(comma); file.print(cc.racc.z); file.print(comma);
            file.print(cc.facc.x); file.print(comma); file.print(cc.facc.y); file.print(comma); file.print(cc.facc.z); file.print(comma);
            file.print(cc.wfacc.x); file.print(comma); file.print(cc.wfacc.y); file.print(comma); file.print(cc.wfacc.z); file.print(comma);
            file.print(cc.vel.x); file.print(comma); file.print(cc.vel.y); file.print(comma); file.print(cc.vel.z); file.print(comma);
            file.print(cc.pos.x); file.print(comma); file.print(cc.pos.y); file.print(comma); file.print(cc.pos.z); file.print(comma);
            file.print(cc.rgyr.x); file.print(comma); file.print(cc.rgyr.y); file.print(comma); file.print(cc.rgyr.z); file.print(comma);
            file.print(cc.fgyr.x); file.print(comma); file.print(cc.fgyr.y); file.print(comma); file.print(cc.fgyr.z); file.print(comma);
            file.print(cc.rel_ori.x); file.print(comma); file.print(cc.rel_ori.y); file.print(comma); file.print(cc.rel_ori.z); file.print(comma);
            file.print(cc.relative.w); file.print(comma); file.print(cc.relative.x); file.print(comma); file.print(cc.relative.y); file.print(comma); file.print(cc.relative.z); file.print(comma);
            file.print(cc.bmi_temp); file.print(comma);
            file.print(cc.mag.x); file.print(comma); file.print(cc.mag.y); file.print(comma); file.print(cc.mag.z); file.print(comma);
            file.print(cc.external.loop_time); file.print(comma); file.print(cc.external.raw_temp); file.print(comma); file.print(cc.external.raw_pres); file.print(comma); file.print(cc.external.filt_pres); file.print(comma); file.print(cc.external.filt_temp); file.print(comma);
            file.print(cc.r_TDS); file.print(comma); file.print(cc.f_TDS); file.print(comma); file.print(cc.r_voltage); file.print(comma); file.print(cc.f_voltage); file.print(comma);
            file.print(cc.clock_speed); file.print(comma); file.print(cc.internal_temp); file.print(comma);
            file.print(cc.dive_stepper.limit_state); file.print(comma); file.print(cc.dive_stepper.homed); file.print(comma); 
            file.print(cc.dive_stepper.current_position); file.print(comma); file.print(cc.dive_stepper.current_position_mm); file.print(comma); file.print(cc.dive_stepper.target_position); file.print(comma); file.print(cc.dive_stepper.target_position_mm); file.print(comma);
            file.print(cc.dive_stepper.speed); file.print(comma); file.print(cc.dive_stepper.acceleration); file.print(comma); file.print(cc.dive_stepper.max_speed); file.print(comma);
            file.print(cc.pitch_stepper.limit_state); file.print(comma); file.print(cc.pitch_stepper.homed); file.print(comma);
            file.print(cc.pitch_stepper.current_position); file.print(comma); file.print(cc.pitch_stepper.current_position_mm); file.print(comma); file.print(cc.pitch_stepper.target_position); file.print(comma); file.print(cc.pitch_stepper.target_position_mm); file.print(comma);
            file.print(cc.pitch_stepper.speed); file.print(comma); file.print(cc.pitch_stepper.acceleration); file.print(comma); file.print(cc.pitch_stepper.max_speed); file.print(comma);
            file.print(cc.optical_data.capture_time); file.print(comma); file.print(cc.optical_data.save_time); file.print(comma); file.print(cc.optical_data.FIFO_length); file.print(comma);
            file.println(cc.sd_capacity);
        }   
        if(!file.close())
        {
            #if DEBUG_ON
                char* msg = (char*)("Failed to close csv file");
                ERROR_LOG(Debug::Critical_Error, *msg);
                #if LIVE_DEBUG
                    Serial.println(*msg);
                #endif
            #endif
        }
        write_iterator++;

        std::queue<Data>().swap(read_buf); //clearing the buffer
    }

    #if DEBUG_ON 
        char* msg = (char*)("Finished writing to SD card");
        SUCCESS_LOG(*msg);
        #if LIVE_DEBUG
            Serial.println(*msg);
        #endif
    #endif
    return true;
}

/**
 * @brief Finds how much heap memory we have
 * 
 * @return unsigned int free memory amount
 */
unsigned int SD_Logger::freeMemory()
{
    return (char *)&_heap_end - __brkval; 
}

/**
 * @brief finds the greatest common factor of our iterations based on free memory available
 * 
 * @return unsigned int greates common factor
 */
unsigned int SD_Logger::findFactors()
{
    //We save all the factors found into a vector
    //Honestly could optimize this for larger numbers... Will do!
    std::vector<unsigned int> factors;
    for(unsigned int i = 1; i <= iterations; i++)
    {
        if(iterations % i == 0)
        {
            factors.push_back(i);
        }
    }

    unsigned int free_mem = freeMemory();
    unsigned int factor_to_use = 0;

    //Reverse iterate to find largest factor
    for(auto i = factors.rbegin(); i != factors.rend(); i++)
    {
        if(*i * sizeof(Data) <= free_mem - 200) //leave 200 bytes free to ensure we dont over do it
        {
            factor_to_use = *i;
            break;
        }
    }

    return factor_to_use;
}

void SD_Logger::flush(void*)
{
    file.flush();
}

void SD_Logger::getCapacity(uint32_t &capacity)
{
    capacity = sd.freeClusterCount();
}

bool SD_Logger::reopenFile(const char* filename)
{
    return file.open(filename, O_WRITE | O_CREAT | O_APPEND);
}