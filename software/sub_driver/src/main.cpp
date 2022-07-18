
#include "Sensors/Sensors.h"
#include "Navigation/Orientation.h"

#include "pins.h"

#include "Data/SD/SD.h"

#include "indication/OutputFuncs.h"

#include <Arduino.h>

#include "Navigation/SensorFusion/Fusion.h"
#include "Navigation/Postioning.h"
#include "Sensors/GPS.h"
#include "Camera/OV2640.h"

#include "Data/RF/radio.h"
#include "debug.h"
#include "time/Time.h"

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
    sensor.setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);
    sensor.setGyroBias();

    if (!rf.init())
    {
        warning = true;
        rfInit = false;
    }

#if OPTICS_ON == true
    if (!camera.begin())
    {
        warning = true;
    }
#endif

    delay(500); //Consistent SD initializations need the delay
    if (!logger.init())
    {
        output.indicateError();
    }

    output.indicateCompleteStartup();

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

    /*

    if(data.time_us >= 10000000)
    {
      logger.rewindPrint();
      while(1);
    }

    */

    cdata.d = data;
    cdata.g = gps_data;

    if (rfInit)
    {
        rf.writeData(cdata);
    }
}
