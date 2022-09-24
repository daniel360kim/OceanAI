
#include "Sensors/Sensors.h"
#include "navigation/Orientation.h"

#include "pins.h"

#include "Data/SD/SD.h"

#include "indication/OutputFuncs.h"
#include "indication/LED.h"

#include <Arduino.h>

#include "Navigation/SensorFusion/Fusion.h"
#include "Navigation/Postioning.h"
#include "Sensors/Camera/OV2640.h"

#include "debug.h"
#include "time/Time.h"
#include "core/OS.h"

#include "module/stepper.h"

#include <EasyTransferI2C.h>


EasyTransferI2C ET;

Fusion SFori;

Optics::Camera camera(CS_VD);

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

StepperPins pins_p{
    STP_p,
    DIR_p,
    MS1_p,
    MS2_p,
    ERR_p,
    STOP_p};

StepperPins pins_b{
    STP_b,
    DIR_b,
    MS1_b,
    MS2_b,
    ERR_b,
    STOP_b};

Buoyancy buoyancy(pins_b, Stepper::Resolution::HALF, StepperProperties(81.0, 52670));
// Stepper pitch(pins_p, Stepper::Resolution::HALF, StepperProperties(81.0, 52670)); no pitch for now


void receive(int byteCount)
{
}
TransmissionPacket packet;
void setup()
{
    output.startupSequence();
    Serial.begin(2000000);

    UnifiedSensors::getInstance().scanAddresses();

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

    // Indicate that the stepper is homing
    LEDb.blink(255, 0, 0, 1000);
    LEDa.blink(255, 0, 0, 1000);

    // buoyancy.calibrate();

    data.dive_stepper.homed = true;

    buoyancy.setMaxSpeed(6000);
    buoyancy.setSpeed(6000);
    buoyancy.setAcceleration(6000);
    buoyancy.setResolution(Stepper::Resolution::HALF);

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

    nav_v.updateVelocity(data);
    nav_p.updatePosition(data);
    SFori.update(data);

    data.relative = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z);
    ori.convertAccelFrame(data.relative, data.facc.x, data.facc.y, data.facc.z, &data.wfacc.x, &data.wfacc.y, &data.wfacc.z);

#if OPTICS_ON == true
    camera.capture(1000000, &data.optical_data.capture_time, &data.optical_data.save_time, &data.optical_data.FIFO_length, logger.closeFile, logger.reopenFile, logger.data_filename);
#endif

    signal.blink(80);
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

    if (data.time_us >= 1e+10)
    {
        signal.on();
        LEDa.setColor(255, 255, 255);
        unsigned long start = micros();
        if (!logger.rewindPrint())
        {
            while (1)
            {
                Serial.println("Rewind failed");
                delay(10);
            }
        }

        unsigned long finish = micros();
        LEDa.setColor(0, 255, 0);
        Serial.print("Rewind took: ");
        Serial.println(finish - start);
        LEDa.LEDoff();
        while (1);
    }

    
    buoyancy.forward();
    buoyancy.logToStruct(data);

}