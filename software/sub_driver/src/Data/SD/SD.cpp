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

namespace SD_Settings
{
    constexpr unsigned int LOG_RATE = 100u; //log rate in hertz
    constexpr unsigned int LOG_INTERVAL_US = 1000u; 
    constexpr unsigned int DATA_SIZE = sizeof(Data);
    constexpr unsigned int BUF_SIZE = 200u; //400 sections of 512 byte structs in our buffer
    constexpr unsigned long LOG_FILE_SIZE = 259200u * DATA_SIZE * LOG_RATE; //3 days (in seconds) * size of struct * data rate in hertz
    constexpr unsigned int READ_BUF_SIZE = 600u;
    constexpr unsigned long FLUSH_INTERVAL_US = 60000000ul; //flush every 60 seconds
};

extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;

volatile bool SD_Logger::cap_update_int = false;

SdFs sd;
FsFile file;

CircularBuffer<Data> buf(SD_Settings::BUF_SIZE);

Timer<1, micros> flusher;
Timer<1, micros> cap_update;

SD_Logger::SD_Logger() {}

bool SD_Logger::init()
{
    DataFile data("data", DataFile::ENDING::TXT);
    data_filename = data.file_name;

    DataFile csvFile("ASCII", DataFile::ENDING::CSV);
    if(!csvFile.createFile())
    {
        return false;
    }

    csv_filename = csvFile.file_name;

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
    for(unsigned long long j = 0; j < iterations / buf_size; j++)
    {
        CircularBuffer<Data> *read_buf = new CircularBuffer<Data>(buf_size);
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
        if(read_buf->full())
        {
            Serial.println("Readbuf full!");
            Serial.print("Freemem "); Serial.println(freeMemory());
        }
        
        file.close();


        if(!file.open(csv_filename, O_CREAT | O_APPEND | O_WRITE))
        {
            Serial.println("File open failed");
            return false;
        }

        //Print our massive header just once
        if(!ASCII_header_made)
        {
            file.print(F("Time us,sys_state,dt,"));
            file.print(F("bmp_rpres, bmp_rtemp, bmp_fpres, bmp_ftemp,"));
            file.print(F("racc_x,racc_y,racc_z,facc_x,facc_y,facc_z,"));
            file.print(F("wfax,wfay,wfaz,"));
            file.print(F("vx,vy,vz,px,py,pz,"));
            file.print(F("rgx,rgy,rgz,fgx,fgy,fgz,"));
            file.print(F("rel_x,rel_y,rel_z,"));
            file.print(F("abs_x,abs_y,abs_z,"));
            file.print(F("rel_w,rel_x,rel_y,rel_z,"));
            file.print(F("abs_w,abs_x,abs_y,abs_z,"));
            file.print(F("bmi_temp,"));
            file.print(F("rmx,rmy,rmz,"));
            file.print(F("ext_rtemp,ext_rpres,ext_ftemp,ext_fpres,"));
            file.print(F("TDS,voltage,clk_speed,int_temp,"));
            file.print(F("dive_sp,dive_output,pitch_sp,pitch_output,"));
            file.print(F("dive_limit,dive_homed,dive_sleep,dive_pp,"));
            file.print(F("pitch_limit,pitch_homed,pitch_sleep,pitch_pp,"));
            file.print(F("cap_time,save_time,fifo_length,"));
            file.print(F("sd_capacity\n"));

            ASCII_header_made = true;
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
        delete read_buf;
        Serial.print("Freemem new: "); Serial.println(freeMemory());
    
    }
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