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

#include "SD.h"
#include "DataFile.h"
#include "../data_struct.h"
#include "RingBuffer.h"

namespace SD_Settings
{
    constexpr unsigned int LOG_RATE = 100u; //log rate in hertz
    constexpr unsigned int LOG_INTERVAL_US = 10000u; 
    constexpr unsigned int DATA_SIZE = sizeof(Data);
    constexpr unsigned int BUF_SIZE = 200u; //400 sections of 512 byte structs in our buffer
    constexpr unsigned long LOG_FILE_SIZE = 259200u * DATA_SIZE * LOG_RATE; //3 days (in seconds) * size of struct * data rate in hertz
    constexpr unsigned int READ_BUF_SIZE = 600u;
};

SdFs sd;
FsFile file;

CircularBuffer<Data> buf(SD_Settings::BUF_SIZE);

SD_Logger::SD_Logger()
{
}

bool SD_Logger::init()
{
    DataFile data("data", DataFile::ENDING::TXT);
    data_filename = data.file_name;

    if(!data.createFile())
    {
        return false;
    }
    if(!file.open(data_filename, O_RDWR | O_CREAT | O_TRUNC))
    {
        return false;
    }
    
    //file must be preallocated to prevent spending lots of time searching for free clusters
    if(!file.preAllocate(SD_Settings::LOG_FILE_SIZE))
    {
        file.close();
        return false;
    }

    return true;
}

bool SD_Logger::logData(Data data)
{
    //Open and truncate existing file


    unsigned long long current_time = micros();
    if(current_time - previous_time >= SD_Settings::LOG_INTERVAL_US)
    {
        //If our sd card is busy we add to our buffer
        if(file.isBusy())
        {
            buf.insert(data);
        }
        else
        {
            //If nothing is in the buffer we directly write to the sd card
            if(buf.size() == 0)
            {
                file.write((const uint8_t*)&data, sizeof(data));
                iterations++;
                file.flush();
            }
            else
            {
                //If there is data in the buffer we move it into the buffer and write the oldest data to the sd card
                buf.insert(data);
                data = buf.get();
                file.write((const uint8_t*)&data, sizeof(data));
                iterations++;
                file.flush();
            }
        }
        previous_time = micros();
    }
    if(buf.full())
    {
        return false;
    }


    return true;
    
}

bool SD_Logger::rewindPrint()
{
    for(unsigned long long j = 0; j < 1 + ((iterations - 1) / SD_Settings::READ_BUF_SIZE); j++)
    {
        CircularBuffer<Data> *read_buf = new CircularBuffer<Data>(SD_Settings::READ_BUF_SIZE);
        file.open(data_filename, FILE_READ);

        Data datacopy;
        for(unsigned int i = 0; i < SD_Settings::READ_BUF_SIZE; i++)
        {
            file.read((uint8_t *)&datacopy, sizeof(datacopy));
            read_buf->insert(datacopy);

        }
        if(read_buf->full())
        {
            Serial.println("Readbuf full!");
        }
        
        file.close();

        DataFile csvFile("ASCII", DataFile::ENDING::CSV);
        if(!csvFile.createFile())
        {
            Serial.println("ASCII filed create failed");
            return false;
        }

        if(!file.open(csvFile.file_name, O_RDWR | O_CREAT | O_TRUNC))
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

        for(unsigned int i = 0; i < SD_Settings::READ_BUF_SIZE; i++)
        {
            char* comma = ",";
            Data cc;
            cc = read_buf->get();
            file.print(cc.time_us); file.print(F(comma));
            file.print(cc.system_state); file.print(F(comma));
            file.print(cc.dt); file.print(F(comma));
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

        if(!read_buf->empty())
        {
            Serial.println("Read buf aint empty");
            return false;
        }

        delete read_buf;
    
    }
    return true;
}
