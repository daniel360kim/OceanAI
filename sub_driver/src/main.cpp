
#include "Sensors/Sensors.h"
#include "Navigation/Orientation.h"

#include "pins.h"

#include "Data/SD/SD.h"

#include "indication/LED.h"
#include "indication/Buzzer.h"

#include <Arduino.h>
#include <Entropy.h>

#include "Navigation/SensorFusion/Fusion.h"
#include "Navigation/Postioning.h"
#include "Sensors/GPS.h"
#include "Camera/OV2640.h"

#include "Data/RF/radio.h"
#include "debug.h"
#include "time/Time.h"

#include <vector>
#include <numeric>

Fusion SFori;

Optics::Camera camera(Wire, SPI, CS_VD);
GPS gps(9600);

UnifiedSensors sensor;
Orientation ori;
RGBLED LEDa(RED_a, GRN_a, BLU_a);
RGBLED LEDb(RED_b, GRN_b, BLU_b);

LED signal(SIGNAL);

Piezo buzzer(BUZZ);

unsigned long long previous_time;
Velocity nav_v;
Position nav_p;

Radio rf(50000);

Data data;
GPSdata gps_data;

CombinedData cdata;

Timer<1,micros, double> print;
SD_Logger logger;
void setup()
{
  
  buzzer.startupSounds();
  Serial.begin(2000000);
  sensor.initNavSensors();
  sensor.initVoltmeter(v_div);
  sensor.initTDS(TDS);
  sensor.setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);
  sensor.setGyroBias();
  Entropy.Initialize();

  rf.init();
  buzzer.indicateCompeteStartup();
  #if OPTICS_ON == true
    camera.begin();
  #endif
  previous_time = micros();
  
  if(!logger.init())
  {
    Serial.println("File init failed");
  }

}

bool logged = false;


void loop()
{ 
  
  data.time_us = micros();
  data.dt = (data.time_us - previous_time) / 1000000.0;
  previous_time = data.time_us;


  LEDa.displaySpectrum();
  LEDb.displaySpectrum();
  sensor.logToStruct(data);
  
  gps.updateData(gps_data);

 


  ori.update(data.fgyr.x, data.fgyr.y, data.fgyr.z, data.dt);


  nav_v.updateVelocity(data);
  nav_p.updatePosition(data);
  SFori.update(data);

  
  Quaternion sf_orientation = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z);
  ori.convertAccelFrame(sf_orientation, data.facc.x, data.facc.y, data.facc.z, &data.wfacc.x, &data.wfacc.y, &data.wfacc.z);
  nav_v.updateVelocity(data);
  
  //Serial.println(data.fgyr_x);
  /*

  Serial.print(data.SF_x);
  Serial.print("\t");
  Serial.print(data.SF_y);
  Serial.print("\t");
  Serial.println(data.SF_z);
  
  Serial.print("\t\t");
  Serial.print(data.location.age);
  Serial.print("\t");
  Serial.print(data.Py);
  Serial.print("\t");
  Serial.print(data.Pz);
  Serial.print("\n");
  
  
  //Serial.println(data.GPS_activated);
*/
  //start.logGPSdata(gps_data);

  #if OPTICS_ON == true
    camera.capture(1000000);
  #endif
  //cdata.d = data;
  //cdata.g = gps_data;
  
  //rf.writeData(cdata);

  if(!logger.logData(data))
  {
    Serial.println("Logging failed");
  }

  //Serial.println(data.voltage);
  if(data.time_us >= 10000000ull && logged == false)
  {
      logged = true;
      if(!logger.rewindPrint())
      {
        Serial.println("Rewind failed");
      }

      Serial.println("rewind finished");

      while(1);
   
  
   /*
    LEDa.setColor(255,0,0);
    //start.rewindGPSPrint();
    LEDb.setColor(0,0,255);
    LEDb.setColor(0,0,255);
    //delay(10000);
    LEDb.setColor(0,255,255);
    LEDb.setColor(0,255,255);
    start.rewindPrint();
    LEDa.setColor(0,255,0);
    
*/
  }




}
