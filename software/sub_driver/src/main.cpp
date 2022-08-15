
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

#include "Data/RF/radio.h"
#include "debug.h"
#include "time/Time.h"

#include "data/transmit.h"

//Transmit transmit(0x1B, 100000, TX_GPS, RX_GPS);

Fusion SFori;

Optics::Camera camera(CS_VD);
GPS gps(9600);

UnifiedSensors sensor;
Orientation ori;

LED signal(SIGNAL);

unsigned long long previous_time;

Velocity nav_v;
Position nav_p;

Radio rf(50000);
Data data;
GPSdata gps_data;

CombinedData cdata;

SD_Logger logger;

bool rfInit = true;
bool warning = false;



void setup()
{
    
    output.startupSequence();
    Serial.begin(2000000);

    if (!sensor.initNavSensors())
    {
        output.indicateError();
    }

    sensor.initVoltmeter(v_div);
    sensor.initTDS(TDS);
    
    sensor.setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int, TX_RF);
    
    sensor.setGyroBias();
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

    output.loopIndication();
    sensor.logToStruct(data);

    gps.updateData(gps_data);

    nav_v.updateVelocity(data);
    nav_p.updatePosition(data);
    SFori.update(data);

    data.relative = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z);
    ori.convertAccelFrame(data.relative, data.facc.x, data.facc.y, data.facc.z, &data.wfacc.x, &data.wfacc.y, &data.wfacc.z);

#if OPTICS_ON == true
    camera.capture(1000000, &data.optical_data.capture_time, &data.optical_data.save_time, &data.optical_data.FIFO_length);
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


    Serial.println(1.0 / data.dt);
/*
    Serial.print(data.external.loop_time); Serial.print("\t");
    Serial.print(data.external.raw_temp); Serial.print("\t");
    Serial.print(data.external.raw_pres); Serial.print("\n");
    */
    cdata.d = data;
    cdata.g = gps_data;


    //transmit.transmit(data);
    
}
