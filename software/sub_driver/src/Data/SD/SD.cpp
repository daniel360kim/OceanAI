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

#include "SD.h"
#include "DataFile.h"
#include "../data_struct.h"
#include "RingBuffer.h"
#include "../../time/Time.h"
#include "../../debug.h"
#include "../../config.h"
#include "../StartInfo.h"

namespace SD_Settings
{
    constexpr unsigned long DURATION = 6e+8;
    constexpr unsigned int LOG_INTERVAL_US = 10000u;
    constexpr unsigned int LOG_RATE = 100u; //log rate in hertz 
    constexpr unsigned int DATA_SIZE = sizeof(Data);
    constexpr unsigned int BUF_SIZE = 200u; //400 sections of 512 byte structs in our buffer
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

CircularBuffer<Data> buf(SD_Settings::BUF_SIZE);

Timer<1, micros> flusher;
Timer<1, micros> cap_update;

SD_Logger::SD_Logger() {}

bool SD_Logger::init()
{
    DataFile data("data", DataFile::ENDING::DAT);
    data_filename = data.file_name;
    configs.bin_file = data_filename;

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

    file.print(("Time us,sys_state,dt,"));
    file.print(("bmp_rpres, bmp_rtemp, bmp_fpres, bmp_ftemp,"));
    file.print(("racc_x,racc_y,racc_z,facc_x,facc_y,facc_z,"));
    file.print(("wfax,wfay,wfaz,"));
    file.print(("vx,vy,vz,px,py,pz,"));
    file.print(("rgx,rgy,rgz,fgx,fgy,fgz,"));
    file.print(("rel_x,rel_y,rel_z,"));
    file.print(("abs_x,abs_y,abs_z,"));
    file.print(("rel_w,rel_x,rel_y,rel_z,"));
    file.print(("abs_w,abs_x,abs_y,abs_z,"));
    file.print(("bmi_temp,"));
    file.print(("rmx,rmy,rmz,"));
    file.print(("ext_rtemp,ext_rpres,ext_ftemp,ext_fpres,"));
    file.print(("TDS,voltage,clk_speed,int_temp,"));
    file.print(("dive_sp,dive_output,pitch_sp,pitch_output,"));
    file.print(("dive_limit,dive_homed,dive_sleep,dive_pp,"));
    file.print(("pitch_limit,pitch_homed,pitch_sleep,pitch_pp,"));
    file.print(("cap_time,save_time,fifo_length,"));
    file.print(("sd_capacity\n"));

    file.close();

    configs.sd_cap = sd.freeClusterCount();

    if(!data.createFile())
    {
        return false;
    }

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

    flusher.every(SD_Settings::FLUSH_INTERVAL_US, SD_Logger::flush);
    cap_update.every(3600000000, SD_Logger::getCapacity);

    return true;
}

bool SD_Logger::logData(Data data)
{
    
    if(SD_Logger::cap_update_int == true)
    {
        data.sd_capacity = sd.freeClusterCount();
        SD_Logger::cap_update_int = false;
    }

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
    if(buf.full())
    {
        Serial.println("Buff full!");
        return false;
    }

    cap_update.tick();
    flusher.tick();
    
    return true;
    
}

bool SD_Logger::rewindPrint()
{
    long buf_size = findFactors();

    long write_iterator = 0;
    CircularBuffer<Data> *read_buf = new CircularBuffer<Data>(buf_size);

    for(unsigned long long j = 0; j < iterations / buf_size; j++)
    {
        file.open(data_filename, FILE_READ);

        Data datacopy;
        for(long i = 0; i < buf_size + (write_iterator * buf_size); i++)
        {
            file.read((uint8_t *)&datacopy, sizeof(datacopy));
            if(i >= buf_size + ((write_iterator - 1) * buf_size))
            {
                read_buf->insert(datacopy);
            }
        }

        file.close();


        if(!file.open(csv_filename, O_CREAT | O_APPEND | O_WRITE))
        {
            Serial.println(F("File open failed"));
            return false;
        }

        for(long i = 0; i < buf_size; i++)
        {
            char* comma = (char*)",";
            Data cc;
            cc = read_buf->get();
            file.print(cc.time_us); file.print(F(comma));
            file.print(cc.system_state); file.print(F(comma));
            file.print(cc.dt,15); file.print(F(comma));
            file.print(cc.bmp_rpres); file.print(F(comma)); file.print(cc.bmp_rtemp); file.print(F(comma)); file.print(cc.bmp_fpres); file.print(F(comma)); file.print(cc.bmp_ftemp);file.print(F(comma));
            file.print(cc.racc.x); file.print(F(comma)); file.print(cc.racc.y); file.print(F(comma)); file.print(cc.racc.z); file.print(F(comma));
            file.print(cc.facc.x); file.print(F(comma)); file.print(cc.facc.y); file.print(F(comma)); file.print(cc.facc.z); file.print(F(comma));
            file.print(cc.wfacc.x); file.print(F(comma)); file.print(cc.wfacc.y); file.print(F(comma)); file.print(cc.wfacc.z); file.print(F(comma));
            file.print(cc.vel.x); file.print(F(comma)); file.print(cc.vel.y); file.print(F(comma)); file.print(cc.vel.z); file.print(F(comma));
            file.print(cc.pos.x); file.print(F(comma)); file.print(cc.pos.y); file.print(F(comma)); file.print(cc.pos.z); file.print(F(comma));
            file.print(cc.rgyr.x); file.print(F(comma)); file.print(cc.rgyr.y); file.print(F(comma)); file.print(cc.rgyr.z); file.print(F(comma));
            file.print(cc.fgyr.x); file.print(F(comma)); file.print(cc.fgyr.y); file.print(F(comma)); file.print(cc.fgyr.z); file.print(F(comma));
            file.print(cc.rel_ori.x); file.print(F(comma)); file.print(cc.rel_ori.y); file.print(F(comma)); file.print(cc.rel_ori.z); file.print(F(comma));
            file.print(cc.abs_ori.x); file.print(F(comma)); file.print(cc.abs_ori.y); file.print(F(comma)); file.print(cc.abs_ori.z); file.print(F(comma));
            file.print(cc.relative.w); file.print(F(comma)); file.print(cc.relative.x); file.print(F(comma)); file.print(cc.relative.y); file.print(F(comma)); file.print(cc.relative.z); file.print(F(comma));
            file.print(cc.absolute.w); file.print(F(comma)); file.print(cc.absolute.x); file.print(F(comma)); file.print(cc.absolute.y); file.print(F(comma)); file.print(cc.absolute.z); file.print(F(comma));
            file.print(cc.bmi_temp); file.print(F(comma));
            file.print(cc.mag.x); file.print(F(comma)); file.print(cc.mag.y); file.print(F(comma)); file.print(cc.mag.z); file.print(F(comma));
            file.print(cc.ext_rtemp); file.print(F(comma)); file.print(cc.ext_rpres); file.print(F(comma)); file.print(cc.ext_ftemp); file.print(F(comma)); file.print(cc.ext_fpres); file.print(F(comma));
            file.print(cc.TDS); file.print(F(comma)); file.print(cc.voltage); file.print(F(comma));
            file.print(cc.clock_speed); file.print(F(comma)); file.print(cc.internal_temp); file.print(F(comma));
            file.print(cc.dive_pid.setpoint); file.print(F(comma)); file.print(cc.dive_pid.output); file.print(F(comma));
            file.print(cc.pitch_pid.setpoint); file.print(F(comma)); file.print(cc.pitch_pid.output); file.print(F(comma)); 
            file.print(cc.dive_stepper.limit_state); file.print(F(comma)); file.print(cc.dive_stepper.homed); file.print(F(comma)); file.print(cc.dive_stepper.sleep); file.print(F(comma)); file.print(cc.dive_stepper.predicted_position); file.print(F(comma));
            file.print(cc.pitch_stepper.limit_state); file.print(F(comma)); file.print(cc.pitch_stepper.homed); file.print(F(comma)); file.print(cc.pitch_stepper.sleep); file.print(F(comma)); file.print(cc.pitch_stepper.predicted_position); file.print(F(comma));
            file.print(cc.optical_data.capture_time); file.print(F(comma)); file.print(cc.optical_data.save_time); file.print(F(comma)); file.print(cc.optical_data.FIFO_length); file.print(F(comma));
            file.println(cc.sd_capacity);
        }   

        file.close();
        write_iterator++;
    }
    delete read_buf;
    return true;
}

unsigned int SD_Logger::freeMemory()
{
    return (char *)&_heap_end - __brkval;
}

unsigned int SD_Logger::findFactors()
{
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
        if(*i * sizeof(Data) <= free_mem - 1000) //leave 1000 bytes free to ensure we dont over do it
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