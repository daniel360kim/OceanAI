
#include "Sensors/Sensors.h"
#include "navigation/Orientation.h"

#include "pins.h"

#include "Data/SD/SD.h"

#include "indication/OutputFuncs.h"

#include <Arduino.h>
#include <vector>
#include <numeric>

#include "Navigation/SensorFusion/Fusion.h"
#include "Navigation/Postioning.h"
#include "Sensors/GPS.h"
#include "Sensors/Camera/OV2640.h"

#include "debug.h"
#include "time/Time.h"
#include "core/OS.h"

Fusion SFori;

Optics::Camera camera(CS_VD);
GPS gps(9600);

Orientation ori;

LED signal(SIGNAL);

unsigned long long previous_time;

Velocity nav_v;
Position nav_p;

Data data;
GPSdata gps_data;

SD_Logger logger;

bool rfInit = true;
bool warning = false;

void setup()
{
    output.startupSequence();
    Serial.begin(2000000);

    if (!UnifiedSensors::getInstance().initNavSensors())
    {
        output.indicateError();
    }

    UnifiedSensors::getInstance().initVoltmeter(v_div);
    UnifiedSensors::getInstance().initTDS(TDS);

    UnifiedSensors::getInstance().setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int, TX_RF);

    UnifiedSensors::getInstance().setGyroBias();
/*
    if (!rf.init())
    {
        warning = true;
        rfInit = false;
    }
*/
#if OPTICS_ON == true
    if (!camera.begin())
    {
        warning = true;
    }
#endif

    if (!logger.init())
    {
        output.indicateError();
    }

  
    output.indicateCompleteStartup();
    Serial.println("Done initializing");
    previous_time = micros();
    
}

bool logged = false;

void loop()
{   
    data.time_us = micros();
    data.dt = (data.time_us - previous_time) / 1000000.0;
    previous_time = data.time_us;

    data.loop_time = 1.0 / data.dt;

    output.loopIndication();
    UnifiedSensors::getInstance().logToStruct(data);
    OS::getInstance().log_cpu_state(data);

    gps.updateData(gps_data);

    nav_v.updateVelocity(data);
    nav_p.updatePosition(data);
    SFori.update(data);

    data.relative = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z);
    ori.convertAccelFrame(data.relative, data.facc.x, data.facc.y, data.facc.z, &data.wfacc.x, &data.wfacc.y, &data.wfacc.z);

#if OPTICS_ON == true
    camera.capture(1000000, &data.optical_data.capture_time, &data.optical_data.save_time, &data.optical_data.FIFO_length, logger.closeFile, logger.reopenFile, logger.data_filename);
#endif

    if (!logger.logData(data))
    {
        output.indicateError();
    }

    if (!warning)
    {
        LEDb.displaySpectrum();
    }
    else
    {
        LEDb.blink(255, 0, 0, 500);
    }

    if(data.time_us >= 1e+7)
    {
      LEDa.setColor(255,255,255);
      unsigned long start = micros();
      if(!logger.rewindPrint())
      {
        while(1)
        {
            Serial.println("Rewind failed");
            delay(10);
        }
      }
    
      unsigned long finish = micros();
      LEDa.setColor(0,255,0);
      Serial.print("Rewind took: "); Serial.println(finish - start);
      LEDa.LEDoff();
      while(1);
    

    
}