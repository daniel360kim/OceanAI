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
#include <cstdint>
#include <string>
#include <stddef.h>
#include <numeric>
#include <Arduino.h>


#include "SD.h"
#include "DataFile.h"
#include "../logged_data.h"

#include "../../core/debug.h"
#include "../StartInfo.h"
#include "../../core/Timer.h"
#include "../../core/StateAutomation.h"

#include <string>

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
    m_log_file_size = (1536 * (duration / 1e+9) * 1.0 / (log_interval_ns / 1e+9)) + 10000; // 1536 bytes per log, 10000 bytes extra for safety
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
        //ERROR_LOG(Debug::Fatal, "Failed to create crash log file");
        return false;
    }

    //Open the file
    if(!file.open(crash_file.getFileName(), O_WRITE | O_CREAT))
    {
        //ERROR_LOG(Debug::Fatal, "Failed to open crash log file");
        return false;
    }

    CrashReport.printTo(file); //Print all the crash data to the file
    if(!file.close())
    {
        //ERROR_LOG(Debug::Fatal, "Failed to close crash log file");
        return false;
    }

    //SUCCESS_LOG("Crash Log Saved Successfully");
    return true;
}

/**
 * @brief create initial headers and start file
 * 
 * @return true creation sudataesful  
 * @return false creation unsudataesful
 */
bool SD_Logger::init(bool format)
{
    if(format)
    {
        removeAllDataFiles();
    }
    //Creating binary file 
    DataFile bin("data", DataFile::JSON);

    if(!bin.createFile())
    {
        //ERROR_LOG(Debug::Fatal, "Failed to create binary file");
        return false;
    }

    m_data_filename = bin.getFileName(); //Get the file name and save to the class
    configs.bin_file = bin.getFileName(); //Save the file name to configs so we can read it in the start file

    if(!DataFile::createFolder("images"))
    {
        //ERROR_LOG(Debug::Fatal, "Failed to create image folder");
        return false;
    }

    //Creating initial image file
    DataFile img("images/img00", DataFile::JPG);
    if(!img.createFile())
    {
        //ERROR_LOG(Debug::Fatal, "Failed to create image file");
        return false;
    }
    m_current_image_filename = img.getFileName(); //Get the file name and save to the class

    //Creating CSV file
    DataFile csv("ASCII", DataFile::CSV);
    if(!csv.createFile())
    {
        //ERROR_LOG(Debug::Fatal, "Failed to create CSV file");
        return false;
    }
    m_csv_filename = csv.getFileName();
    configs.ascii_file = csv.getFileName();

    if(!file.open(m_csv_filename, O_WRITE | O_CREAT))
    {
        //ERROR_LOG(Debug::Critical_Error, "Failed to open CSV file");
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
        //ERROR_LOG(Debug::Critical_Error, "Failed to close CSV file");
        return false;
    }

    //Updating SD Capacity
    configs.sd_cap = sd.freeClusterCount();

    if(!bin.createFile())
    {
        //ERROR_LOG(Debug::Critical_Error, "Failed to create binary file");
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
        //ERROR_LOG(Debug::Critical_Error, "Failed to create start file");
        return false;
    }

    if(!file.open(start.getFileName(), O_WRITE | O_CREAT))
    {
        //ERROR_LOG(Debug::Fatal, "Failed to open start file");
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

    file.println("////////// I2C Device LoggedData //////////");
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
        //ERROR_LOG(Debug::Critical_Error, "Failed to open binary file");
        return false;
    }
    
    //file must be preallocated to prevent spending lots of time searching for free clusters
    if(!file.preAllocate(m_log_file_size + 10000))
    {
        //ERROR_LOG(Debug::Critical_Error, "Failed to preallocate binary file");
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
bool SD_Logger::logData(LoggedData &data)
{
    //Logging at a certain interval set by the constructor
    int64_t current_time = scoped_timer.elapsed();
    if(current_time - m_previous_log_time < m_log_interval)
    {
        return true;
    }
    else
    {
        //If our sd card is busy we add to our buffer
        if(file.isBusy())
        {
            write_buf.push(data); //adding buffer to queue data structure (FIFO)
        }
        else
        {
            //If nothing is in the buffer we directly write to the sd card
            if(write_buf.size() == 0)
            {
                StaticJsonDocument<STATIC_JSON_DOC_SIZE> doc;
                LoggedData::data_to_json(data, doc);
                serializeJson(doc, file);
                file.print("\n");

                #if PRINT_DATA
                    serializeJson(json_data, Serial);
                #endif
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

                StaticJsonDocument<STATIC_JSON_DOC_SIZE> doc;
                LoggedData::data_to_json(data, doc);
                serializeJson(doc, file);
                file.print("\n");
                #if PRINT_DATA
                    serializeJson(json_data, Serial);
                    file.print("\n");
                #endif

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
    if(write_buf.size() > 100) 
    {
        //INFO_LOG("Buffer is full, slowing down data logging rate");
        //Clear the old data if it gets too large by swapping with an empty queue
        std::queue<LoggedData> empty;
        std::swap(write_buf, empty);

        m_log_interval+= 10000000; //If the buffer is too large we increase the log interval to prevent overflow
    }

    //Update our timers continuously so they can update/flush when needed
    flusher.void_tick(this);
    
    return true;
}

void SD_Logger::update_sd_capacity(LoggedData &data)
{
    //Update the capacity from the capacity we calculated in initialization
    if(!m_inital_cap_updated)
    {
        data.sd_capacity = configs.sd_cap;
        m_inital_cap_updated = true;
    }
    capacity_updater.void_tick(data.sd_capacity);
}

bool SD_Logger::removeAllDataFiles()
{
    const char* base_name = "data";
    const char* extension = ".json";

    for(int i = 0; i < 100; i++)
    {
        //ile names are in the format dataXX.json
        char* filename = new char[10];

        if(i < 10)
        {
            sprintf(filename, "%s0%d%s", base_name, i, extension);
        }
        else
        {
            sprintf(filename, "%s%d%s", base_name, i, extension);
        }

        if(!sd.exists(filename))
        {
            delete[] filename;
            return true;
        }
        else
        {
            if(!sd.remove(filename))
            {
                return false;
            }
        }
        delete[] filename;

    }
    return true;
}

void SD_Logger::log_image(OV2640_Mini &camera)
{
    uint32_t length = camera.getCamera()->read_fifo_length();
    uint8_t buf[256] = {0};

    if(length >= MAX_FIFO_SIZE)
    {
        //ERROR_LOG(Debug::Warning, "FIFO length is too large");
        return;
    }
    else if(length == 0)
    {
        //ERROR_LOG(Debug::Warning, "FIFO length is 0");
        return;
    }

    camera.getCamera()->CS_LOW();
    camera.getCamera()->set_fifo_burst();

    int i = 0; //buffer index
    uint8_t temp = 0;
    uint8_t temp_last = 0;

    while(length--)
    {
        temp_last = temp;
        temp = SPI.transfer(0x00);

        //Read JPEG data from FIFO
        if((temp == 0xD9) && (temp_last == 0xFF))
        {
            buf[i++] = temp; //save the last 0XD9
            camera.getCamera()->CS_HIGH();
            //WRITE TO SD!!
            file.write(buf, i);

            DataFile::incrementFileName(m_current_image_filename, 10);
            file.close();

            reopenFile(m_data_filename); //reopen the data file so we can relog
            camera.set_header(false);
            camera.getCamera()->CS_HIGH();
            camera.getCamera()->set_fifo_burst();
            i = 0;
        }
        if(camera.is_header())
        {
            if(i < 256)
            {
                buf[i++] = temp;
            }
            else
            {
                camera.getCamera()->CS_HIGH();
                //FILE WRITE
                i = 0;
                buf[i++] = temp;
                camera.getCamera()->CS_LOW();
                camera.getCamera()->set_fifo_burst();
            }
        }
        else if((temp == 0xD8) & (temp_last == 0xFF))
        {
            camera.set_header(true);
            camera.getCamera()->CS_HIGH();
            
            // Close the current file we were working on
            if(!file.close())
            {
                //ERROR_LOG(Debug::Warning, "Failed to close file");
                return;
            }

            if(!file.open(m_current_image_filename, O_WRITE | O_CREAT))
            {
                //ERROR_LOG(Debug::Warning, "Failed to open file for writing");
                return;
            }
            if(!file.preAllocate(35000))
            {
                file.close();
                //ERROR_LOG(Debug::Warning, "Failed to preallocate file");
            }

            camera.getCamera()->CS_LOW();
            camera.getCamera()->set_fifo_burst();

            buf[i++] = temp_last;
            buf[i++] = temp;
        }
        
    }
}

void SD_Logger::flush(void*)
{
    file.sync();
}

void SD_Logger::getCapacity(uint32_t &capacity)
{
    capacity = sd.freeClusterCount();
}

bool SD_Logger::reopenFile(const char* filename)
{
    return file.open(filename, O_WRITE | O_CREAT | O_APPEND);
}

