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

#include "SD.h"
#include "DataFile.h"
#include "../data_struct.h"
#include "../../time/Time.h"
#include "../../debug.h"
#include "../../config.h"
#include "../StartInfo.h"
#include "StaticQueue.h"

/**
 * @brief Settings for SD card
 * 
 */
namespace SD_Settings
{
    constexpr unsigned long DURATION = 6e+8;
    constexpr unsigned int LOG_INTERVAL_US = 25000u;
    constexpr unsigned int LOG_RATE = 100u; //log rate in hertz 
    constexpr unsigned int DATA_SIZE = sizeof(Data);
    constexpr unsigned int BUF_SIZE = 400u; //400 sections of 512 byte structs in our buffer
    constexpr unsigned long LOG_FILE_SIZE = DURATION * DATA_SIZE * LOG_RATE; //3 days (in seconds) * size of struct * data rate in hertz
    constexpr unsigned long FLUSH_INTERVAL_US = 60000000ul; //flush every 60 seconds
};

extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;

volatile bool SD_Logger::cap_update_int = false;

SdFs sd;
FsFile file;
cid_t cid;

//Stack allocated queue to allow for buffered prints
StaticCircularBuffer<Data, SD_Settings::BUF_SIZE> buf;

//Flush and update sd capacity at certain intervals
Timer<1, micros> flusher;
Timer<1, micros> cap_update;

SD_Logger::SD_Logger() {}
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
        return false;
    }
    csv_filename = csvFile.file_name;
    configs.ascii_file = csv_filename;

    if(!file.open(csv_filename, O_WRITE | O_CREAT))
    {
        return false;
    }

    //Creating header for CSV file
    file.print(("Time us,loop_time,sys_state,dt,"));
    file.print(("bmp_rpres, bmp_rtemp, bmp_fpres, bmp_ftemp,"));
    file.print(("racc_x,racc_y,racc_z,facc_x,facc_y,facc_z,"));
    file.print(("wfax,wfay,wfaz,"));
    file.print(("vx,vy,vz,px,py,pz,"));
    file.print(("rgx,rgy,rgz,fgx,fgy,fgz,"));
    file.print(("rel_x,rel_y,rel_z,"));
    file.print(("rel_w,rel_x,rel_y,rel_z,"));
    file.print(("bmi_temp,"));
    file.print(("rmx,rmy,rmz,"));
    file.print(("ext_loop_time,ext_rtemp,ext_rpres,ext_ftemp,ext_fpres,"));
    file.print(("TDS,voltage,clk_speed,int_temp,"));
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
        return false;
    }

    //Creating directory for our images
    if(!sd.exists("Images"))
    {
        if(!sd.mkdir("Images"))
        {
            #if DEBUG_ON == true
                char* message = (char*)"Failed to make directory";
                Debug::error.addToBuffer(micros(), Debug::Fatal, message);
                
                #if LIVE_DEBUG == true
                    Serial.println(F(message));
                #endif

            #endif
            return false;
        }
    }

    DataFile start("start", DataFile::ENDING::TXT);
    if(!start.createFile())
    {
        return false;
    }

    if(!file.open(start.file_name, O_WRITE | O_CREAT))
    {
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
    file.print("Live Debug: "); file.println(configs.live_debug);
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

    file.close();


    if(!file.open(data_filename, O_WRITE | O_CREAT))
    {
        return false;
    }
    
    //file must be preallocated to prevent spending lots of time searching for free clusters
    if(!file.preAllocate(SD_Settings::LOG_FILE_SIZE))
    {
        file.close();
        return false;
    }

    //Flushing our data every several seconds
    flusher.every(SD_Settings::FLUSH_INTERVAL_US, SD_Logger::flush);
    cap_update.every(3600000000, SD_Logger::getCapacity); //update our sd capacity every once in a while (expensive function)

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
    //Updating capacity every once in a while   
    if(SD_Logger::cap_update_int == true)
    {
        data.sd_capacity = sd.freeClusterCount();
        SD_Logger::cap_update_int = false;
    }

    //Logging at a certain interval
    unsigned long long current_time = micros();
    if(current_time - previous_time >= SD_Settings::LOG_INTERVAL_US)
    {
        //If our sd card is busy we add to our buffer
        if(file.isBusy())
        {
            buf.insert(data);
            Serial.println("File busy!");
        }
        else
        {
            //If nothing is in the buffer we directly write to the sd card
            if(buf.size() == 0)
            {
                file.write((const uint8_t*)&data, sizeof(data));
                iterations++;
            }
            else
            {
                //If there is data in the buffer we move it into the buffer and write the oldest data to the sd card
                buf.insert(data);
                data = buf.get();
                file.write((const uint8_t*)&data, sizeof(data));
                iterations++;
            }
        }
        previous_time = micros();
    }
    //For debugging. Should conditional compile this but too lazy lol
    if(buf.is_full())
    {
        Serial.println("Buff full!");
        return false;
    }

    cap_update.tick();
    flusher.tick();
    
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
    //finding a buffer size based on available memory and how much data we logged
    //this allows us to create the largest buffer possible for less opens and closed of files
    //pretty nifty tbh
    long buf_size = findFactors(); 

    long write_iterator = 0;
    std::queue<Data> read_buf;
    //Since the findFactors() function automatically makes sure we have the greatest common factor of the iterations, we can just divide the iterations by buf_size
    for(unsigned long long j = 0; j < iterations / buf_size; j++)
    {
        file.open(data_filename, FILE_READ);

        Data datacopy; //creating a copy of our data to read into
        for(long i = 0; i < buf_size + (write_iterator * buf_size); i++)
        {
            file.read((uint8_t *)&datacopy, sizeof(datacopy));
            if(i >= buf_size + ((write_iterator - 1) * buf_size))
            {
                read_buf.push(datacopy); //inserting data into our buffer
            }
        }

        file.close();


        if(!file.open(csv_filename, O_CREAT | O_APPEND | O_WRITE))
        {
            Serial.println(F("File open failed"));
            return false;
        }

        //Printing the copied data into the csv fie
        for(long i = 0; i < buf_size; i++)
        {
            char* comma = (char*)","; //We use comma so much, better to use as a variable
            Data cc;
            cc = read_buf.front();
            read_buf.pop();
            file.print(cc.time_us); file.print(comma); file.print(cc.loop_time); file.print(comma);
            file.print(cc.system_state); file.print(comma);
            file.print(cc.dt,15); file.print(comma);
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
            file.print(cc.TDS); file.print(comma); file.print(cc.voltage); file.print(comma);
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
        file.close();
        write_iterator++;

        std::queue<Data>().swap(read_buf); //clearing the buffer
    }
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

bool SD_Logger::flush(void*)
{
    file.flush();
    return true;
}

bool SD_Logger::getCapacity(void*)
{
    SD_Logger::cap_update_int = true;
    return true;
}

bool SD_Logger::reopenFile(const char* filename)
{
    return file.open(filename, O_WRITE | O_CREAT | O_APPEND);
}