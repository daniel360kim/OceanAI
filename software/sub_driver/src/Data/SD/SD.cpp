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
#include <string>
#include <stddef.h>
#include <numeric>

#include "SD.h"
#include "DataFile.h"
#include "../data_struct.h"
#include "queue.hpp"

#include "../../core/debug.h"
#include "../../core/config.h"
#include "../StartInfo.h"
#include "../../core/Timer.h"
#include "../../core/StateAutomation.h"

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
SD_Logger::SD_Logger(const int64_t duration, int log_interval_ns) 
{
    //Calculate log file size based on interval so we can preallocate
    m_log_file_size = sizeof(Data) * (duration / 1e+9) * 1.0 / (log_interval_ns / 1e+9);
    Serial.print("Log file size: "); Serial.println(m_log_file_size);
    m_log_interval = log_interval_ns;

    //Flush files every 30 seconds to ensure data saves
    flusher.setInterval(3e+10);
    flusher.setFunction(SD_Logger::flush);

    //Calculating sd card capacity is a very expensive function so we call it every once in a while
    capacity_updater.setInterval(360000000000);
    capacity_updater.setFunction(SD_Logger::getCapacity);
}

/**
 * @brief Logs a crash report to the SD card
 * 
 * @return true crash report created 
 * @return false crash report not created
 */
bool SD_Logger::log_crash_report()
{
    //Create a folder to hold all crash logs
    //createFolder() accounts for if the folder exists already
    if(!DataFile::createFolder("logs"))
    {
        return false;
    }

    DataFile crash_file("logs/crash", DataFile::TXT);

    //Create the file
    if(!crash_file.createFile())
    {
        ERROR_LOG(Debug::Fatal, "Failed to create crash log file");
        return false;
    }

    //Open the file
    if(!file.open(crash_file.getFileName(), O_WRITE | O_CREAT))
    {
        ERROR_LOG(Debug::Fatal, "Failed to open crash log file");
        return false;
    }

    CrashReport.printTo(file); //Print all the crash data to the file
    if(!file.close())
    {
        ERROR_LOG(Debug::Fatal, "Failed to close crash log file");
        return false;
    }

    SUCCESS_LOG("Crash Log Saved Successfully");
    return true;
}

/**
 * @brief create initial headers and start file
 * 
 * @return true creation sudataesful  
 * @return false creation unsudataesful
 */
bool SD_Logger::init()
{
    //Creating binary file 
    DataFile bin("data", DataFile::DAT);
    if(!bin.createFile())
    {
        ERROR_LOG(Debug::Fatal, "Failed to create binary file");
        return false;
    }
    m_data_filename = bin.getFileName(); //Get the file name and save to the class
    configs.bin_file = bin.getFileName(); //Save the file name to configs so we can read it in the start file


    //Creating CSV file
    DataFile csv("ASCII", DataFile::CSV);
    if(!csv.createFile())
    {
        ERROR_LOG(Debug::Fatal, "Failed to create CSV file");
        return false;
    }
    m_csv_filename = csv.getFileName();
    configs.ascii_file = csv.getFileName();

    if(!file.open(m_csv_filename, O_WRITE | O_CREAT))
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to open CSV file");
        return false;
    }

    //Creating header for CSV file
    file.print(("Time ns,loop_time(hz),sys_state,delta_time(sec),"));
    file.print(("bmp_raw_pres(atm), bmp_raw_temp(°C),"));
    file.print(("radata_x(m/s/s),racc_y(m/s/s),racc_z(m/s/s),"));
    file.print(("wfax(m/s/s),wfay(m/s/s),wfaz(m/s/s),"));
    file.print(("vx(m/s),vy(m/s),vz(m/s),px(m),py(m),pz(m),"));
    file.print(("rgx(rad/s),rgy(rad/s),rgz(rad/s),"));
    file.print(("rel_x(deg),rel_y(deg),rel_z(deg),"));
    file.print(("rel_w,rel_x,rel_y,rel_z,"));
    file.print(("bmi_temp(°C),"));
    file.print(("rmx(T),rmy(T),rmz(T),"));
    file.print(("fmx(T),fmy(T),fmz(T),"));
    file.print(("raw_TDS,filt_TDS,raw_ext_pres(atm),filt_ext_pres(atm),raw_voltage,filt_voltage,clk_speed,int_temp(°C),"));
    file.print(("dive_limit,dive_homed,dive_current_pos,dive_current_pos_mm,dive_target_pos,dive_target_pos_mm,"));
    file.print(("dive_speed,dive_accel,dive_max_speed,"));
    file.print(("pitch_limit,pitch_homed,pitch_current_pos,pitch_current_pos_mm,dive_target_pos,dive_target_pos_mm,"));
    file.print(("pitch_speed,pitch_accel,pitch_max_speed,"));
    file.print(("cap_time,save_time,fifo_length,"));
    file.print(("sd_capacity\n"));

    if(!file.close())
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to close CSV file");
        return false;
    }

    //Updating SD Capacity
    configs.sd_cap = sd.freeClusterCount();

    if(!bin.createFile())
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to create binary file");
        return false;
    }

    #if OPTICS_ON //We only need an Images directory if we are taking photos
        DataFile::createFolder("Images"); //Create a folder to hold all the images 
    #endif


    /*Startfile includes lots of information that we only need one
    *For instance: Gyro bias values, detected sensors, etc. 
    */
    DataFile start("start", DataFile::TXT);
    if(!start.createFile())
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to create start file");
        return false;
    }

    if(!file.open(start.getFileName(), O_WRITE | O_CREAT))
    {
        ERROR_LOG(Debug::Fatal, "Failed to open start file");
        return false;
    }

    //Creating start file
    file.println("OceanAI Start Configurations");
    for(int i = 0; i < 255; i++)
    {
        file.print("*");
    }
    
    file.print("\n");
    file.println("////////// Printing to: //////////");
    file.print("Binary File: "); file.println(m_data_filename);
    file.print("ASCII File: "); file.println(m_csv_filename);
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
    file.println("IMU Package - Adatael: "); file.println(configs.accel_range); file.println(configs.accel_ODR);
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

    file.close();

    if(!file.open(m_data_filename, O_WRITE | O_CREAT))
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to open binary file");
        return false;
    }
    
    //file must be preallocated to prevent spending lots of time searching for free clusters
    if(!file.preAllocate(m_log_file_size + 10000))
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to preallocate binary file");
        file.close();
        return false;
    }
     
    return true; //everything went well! SD card is ready to go
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
    //Update the capacity from the capacity we calculated in initialization
    if(!m_inital_cap_updated)
    {
        data.sd_capacity = configs.sd_cap;
        m_inital_cap_updated = true;
    }
    //Logging at a certain interval set by the constructor
    int64_t current_time = scoped_timer.elapsed();
    if(current_time - m_previous_log_time >= m_log_interval)
    {
        //If our sd card is busy we add to our buffer
        if(file.isBusy())
        {
            INFO_LOG("SD Card is busy, adding to buffer");
            write_buf.push(data); //adding buffer to queue data structure (FIFO)
        }
        else
        {
            //If nothing is in the buffer we directly write to the sd card
            if(write_buf.size() == 0)
            {
                file.write((const uint8_t*)&data, sizeof(data));
                //Counting how many times we write to the sd card.
                //This is used to calculate how many times to rewind when converting the binary 
                //to ascci after the mission is over
                m_write_iterations++; 

            }
            else
            {
                //If there is data in the buffer we move it into the buffer and write the oldest data to the sd card
                write_buf.push(data);
                data = write_buf.front();
                write_buf.pop();
                file.write((const uint8_t*)&data, sizeof(data));
                m_write_iterations++;
            }
        }
        m_previous_log_time = scoped_timer.elapsed(); //update the last time we logged
    }
    /*
    * We dont want to cause any buffer overflows so we limit the buffer size to 100
    * If the buffer reaches this threshold, we probably are logging data to fast anyways
    * So we slow down the data logging rate
    */
    if(write_buf.size() > 500) 
    {
        INFO_LOG("Buffer is full, slowing down data logging rate");
        //Clear the old data if it gets too large by swapping with an empty queue
        std::queue<Data> empty;
        std::swap(write_buf, empty);

        m_log_interval+= 10000000; //If the buffer is too large we increase the log interval to prevent overflow
    }

    //Update our timers continuously so they can update/flush when needed
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
 * Since the SD files can get really large, we use large datatypes just to be safe
 * @return true conversion successful
 * @return false conversion unsuccessful
 */
bool SD_Logger::rewindPrint()
{
    if(!file.close())
    {
        ERROR_LOG(Debug::Critical_Error, "Failed to close binary file before translation");
        return false;
    }

    //Freeing memory from write_buffer
    std::queue<Data> empty;
    std::swap(write_buf, empty);

    //finding a buffer size based on available memory and how much data we logged
    //this allows us to create the largest buffer possible for less opens and closes of files
    int buf_size = findFactors(); 
    unsigned long long write_iterator = 0;

    //Creating a circular buffer with a size based on the available memory
    CircularBuffer<Data> read_buf(buf_size);

    //How many times we need to reopen and close files
    for(unsigned long long j = 0; j < m_write_iterations / buf_size; j++)
    {
        if(!file.open(m_data_filename, FILE_READ))
        {
            std::string bin_file_err = "Failed to open binary file at iteration: " + j;
            ERROR_LOG(Debug::Critical_Error, bin_file_err.c_str());
            return false;
        }

        Data datacopy; //creating a copy of our data to read into
        //This is really ineffiient 
        //TODO make it better
        //MAYBE used file.seek or position to go to specific lines
        //Right now, everytime we read, we start from the beginning
        for(unsigned long i = 0; i < buf_size + (write_iterator * buf_size); i++)
        {
            if(!file.read((uint8_t *)&datacopy, sizeof(datacopy)))
            {
                ERROR_LOG(Debug::Critical_Error, "Failed to read from binary file during translation");
            }

            if(i >= buf_size + ((write_iterator - 1) * buf_size))
            {
                read_buf.insert(datacopy); //inserting data into our buffer
            }
        }


        if(!file.close())
        {
            ERROR_LOG(Debug::Critical_Error, "Failed to close binary file during translation");
            return false;
        }

        if(!file.open(m_csv_filename, O_CREAT | O_APPEND | O_WRITE))
        {
            ERROR_LOG(Debug::Critical_Error, "Failed to open csv file during translation");
            return false;
        }

        Data data;
        
        //Printing the copied data into the csv fie
        for(int i = 0; i < buf_size; i++)
        {
            read_buf.get(data);
            printData(file, data);
        }   
        if(!file.close())
        {
            ERROR_LOG(Debug::Critical_Error, "Failed to close csv file during translation");
        }
        write_iterator++;

        read_buf.reset();
    }

    SUCCESS_LOG("Finished converting binary file to csv file");
    return true;
}

void SD_Logger::printData(Print &printer, Data &data)
{
    char* comma = (char*)","; //We use comma so much, better to use as a variable
    //Translating integer system state into readable characters
    CurrentState state = static_cast<CurrentState>(data.system_state);

    printer.print(data.time_ns); printer.print(comma); printer.print(data.loop_time); printer.print(comma);

    StateAutomation::printState(printer, state); printer.print(comma);

    printer.print(data.delta_time,10); printer.print(comma);

    printer.print(data.raw_bmp.pressure); printer.print(comma); printer.print(data.raw_bmp.temperature); printer.print(comma);
    
    printer.print(data.racc.x); printer.print(comma); printer.print(data.racc.y); printer.print(comma); printer.print(data.racc.z); printer.print(comma);
    printer.print(data.wfacc.x); printer.print(comma); printer.print(data.wfacc.y); printer.print(comma); printer.print(data.wfacc.z); printer.print(comma);
    printer.print(data.vel.x); printer.print(comma); printer.print(data.vel.y); printer.print(comma); printer.print(data.vel.z); printer.print(comma);
    printer.print(data.pos.x); printer.print(comma); printer.print(data.pos.y); printer.print(comma); printer.print(data.pos.z); printer.print(comma);
    
    printer.print(data.rgyr.x); printer.print(comma); printer.print(data.rgyr.y); printer.print(comma); printer.print(data.rgyr.z); printer.print(comma);
    printer.print(data.rel_ori.x); printer.print(comma); printer.print(data.rel_ori.y); printer.print(comma); printer.print(data.rel_ori.z); printer.print(comma);
    
    printer.print(data.relative.w); printer.print(comma); printer.print(data.relative.x); printer.print(comma); printer.print(data.relative.y); printer.print(comma); printer.print(data.relative.z); printer.print(comma);
    
    printer.print(data.bmi_temp); printer.print(comma);
    printer.print(data.rmag.x); printer.print(comma); printer.print(data.rmag.y); printer.print(comma); printer.print(data.rmag.z); printer.print(comma);
    printer.print(data.fmag.x); printer.print(comma); printer.print(data.fmag.y); printer.print(comma); printer.print(data.fmag.z); printer.print(comma);

    printer.print(data.raw_TDS); printer.print(comma); printer.print(data.filt_TDS); printer.print(comma);
    printer.print(data.raw_ext_pres); printer.print(comma); printer.print(data.filt_ext_pres); printer.print(comma);

    printer.print(data.raw_voltage); printer.print(comma); printer.print(data.filt_voltage); printer.print(comma);
    printer.print(data.clock_speed); printer.print(comma); printer.print(data.internal_temp); printer.print(comma);
    printer.print(data.dive_stepper.limit_state); printer.print(comma); printer.print(data.dive_stepper.homed); printer.print(comma); 
    printer.print(data.dive_stepper.current_position); printer.print(comma); printer.print(data.dive_stepper.current_position_mm); printer.print(comma); printer.print(data.dive_stepper.target_position); printer.print(comma); printer.print(data.dive_stepper.target_position_mm); printer.print(comma);
    printer.print(data.dive_stepper.speed); printer.print(comma); printer.print(data.dive_stepper.acceleration); printer.print(comma); printer.print(data.dive_stepper.max_speed); printer.print(comma);
    printer.print(data.pitch_stepper.limit_state); printer.print(comma); printer.print(data.pitch_stepper.homed); printer.print(comma);
    printer.print(data.pitch_stepper.current_position); printer.print(comma); printer.print(data.pitch_stepper.current_position_mm); printer.print(comma); printer.print(data.pitch_stepper.target_position); printer.print(comma); printer.print(data.pitch_stepper.target_position_mm); printer.print(comma);
    printer.print(data.pitch_stepper.speed); printer.print(comma); printer.print(data.pitch_stepper.acceleration); printer.print(comma); printer.print(data.pitch_stepper.max_speed); printer.print(comma);
    printer.print(data.optical_data.capture_time); printer.print(comma); printer.print(data.optical_data.save_time); printer.print(comma); printer.print(data.optical_data.FIFO_length); printer.print(comma);
    printer.println(data.sd_capacity);
}

/**
 * @brief Finds how much heap memory we have
 * 
 * @return unsigned int free memory amount
 */
int SD_Logger::freeMemory()
{
    return (char *)&_heap_end - __brkval; 
}

/**
 * @brief finds the greatest common factor of our iterations based on free memory available
 * 
 * @return unsigned int greates common factor
 */
int SD_Logger::findFactors()
{
    //We save all the factors found into a vector
    //TODO: Make this better :(
    std::vector<unsigned int> factors;
    for(unsigned int i = 1; i <= m_write_iterations; i++)
    {
        if(m_write_iterations % i == 0)
        {
            factors.push_back(i);
        }
    }

    unsigned int free_mem = freeMemory();
    unsigned int factor_to_use = 0;

    //Reverse iterate to find largest factor
    for(auto i = factors.rbegin(); i != factors.rend(); i++)
    {
        //Make sure that the largest factor we find can fit within are free memory
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

