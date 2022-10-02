/**
 * @file States.cpp
 * @author Daniel Kim
 * @brief finite state automation driver - states classes
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "States.h"

#include "Sensors/Sensors.h"
#include "navigation/Orientation.h"

#include "pins.h"

#include "Data/SD/SD.h"

#include "indication/OutputFuncs.h"
#include "indication/LED.h"

#include <Arduino.h>
#include <teensy_clock/teensy_clock.h>
#include <stdint.h>
#include <chrono>

#include "Navigation/SensorFusion/Fusion.h"
#include "Navigation/Postioning.h"
#include "Sensors/Camera/OV2640.h"


#include "debug.h"
#include "Time.h"
#include "core/OS.h"
#include "core/Timer.h"
#include "module/stepper.h"

Time::Mission mission_duration;
Fusion SFori;

Optics::Camera camera(CS_VD);

Orientation ori;

LED signal(SIGNAL);

int64_t previous_time;
teensy_clock::time_point start_time;

Velocity nav_v;
Position nav_p;

Data data;
GPSdata gps_data;

SD_Logger logger(mission_duration, 33333333);

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

CurrentState currentState;

/**
 * @brief Functions that run no matter the state
 * 
 */
void continuousFunctions()
{
    data.time_ns = scoped_timer.elapsed(); //scoped timer is a global object to measure time since program epoch
    data.dt = (scoped_timer.elapsed() - previous_time) / 1000000000.0;
    previous_time = scoped_timer.elapsed();

    data.system_state = static_cast<uint8_t>(currentState);

    data.loop_time = 1.0 / data.dt;

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

    if (!warning)
    {
        LEDa.displaySpectrum();
        LEDb.displaySpectrum();
    }
    else
    {
        LEDb.blink(255, 0, 0, 500);
    }

    buoyancy.logToStruct(data);

    //Serial.println(data.external.raw_pres);
}

/**
 * Singleton return instance functions
 */

//Instantiates the singletons
Initialization Initialization::instance;
ErrorIndication ErrorIndication::instance;
IdleMode IdleMode::instance;
Diving Diving::instance;
Resurfacing Resurfacing::instance;
Surfaced Surfaced::instance;
SD_translate SD_translate::instance;
SD_reinitialize SD_reinitialize::instance;

Initialization& Initialization::getInstance()
{
    return instance;
}

ErrorIndication& ErrorIndication::getInstance()
{
    return instance;
}

IdleMode& IdleMode::getInstance()
{
    return instance;
}

Diving& Diving::getInstance()
{
    return instance;
}

Resurfacing& Resurfacing::getInstance()
{
    return instance;
}

Surfaced& Surfaced::getInstance()
{
    return instance;
}

SD_translate& SD_translate::getInstance()
{
    return instance;
}

SD_reinitialize& SD_reinitialize::getInstance()
{
    return instance;
}
///****************///

/**
 * Initialization state functions
 */

void Initialization::enter(StateAutomation* state)
{
    start_time = teensy_clock::now();
    if(CrashReport)
    {
        if(!logger.log_crash_report())
        {
            warning = true;
        }
    }

    output.startupSequence();
    Serial.begin(2000000);

    UnifiedSensors::getInstance().scanAddresses();

    if(!UnifiedSensors::getInstance().initNavSensors())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    UnifiedSensors::getInstance().initVoltmeter(v_div);
    UnifiedSensors::getInstance().initTDS(TDS);
    UnifiedSensors::getInstance().initPressureSensor(TX_GPS);

    UnifiedSensors::getInstance().setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);

    UnifiedSensors::getInstance().setGyroBias();

    #if OPTICS_ON == true
        if (!camera.begin())
        {
            warning = true;
        }
    #endif

    if (!logger.init())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    LEDb.blink(255, 0, 0, 1000);
    LEDa.blink(255, 0, 0, 1000);

    currentState = CurrentState::INITIALIZATION;

    //buoyancy.calibrate();
}

void Initialization::run(StateAutomation* state)
{
    state->setState(Diving::getInstance());
}

void Initialization::exit(StateAutomation* state)
{
    output.indicateCompleteStartup();
    previous_time = scoped_timer.elapsed();
}


void ErrorIndication::enter(StateAutomation* state)
{
    currentState = CurrentState::ERROR_INDICATION;
}

void ErrorIndication::run(StateAutomation* state)
{
    output.indicateError();
}

void ErrorIndication::exit(StateAutomation* state)
{
}

void Diving::enter(StateAutomation* state)
{
    currentState = CurrentState::DIVING_MODE;
    buoyancy.setMaxSpeed(6000);
    buoyancy.setSpeed(6000);
    buoyancy.setAcceleration(6000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
}

void Diving::run(StateAutomation* state)
{
    buoyancy.forward();
    if(buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Resurfacing::getInstance());
    }

    continuousFunctions();

    if(!logger.logData(data))
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    if(mission_duration.time_remaining_mission(scoped_timer.elapsed()) <= 0)
    {
        state->setState(SD_translate::getInstance());
        return;
    }

    //ADD POWER CHECKING
}

void Diving::exit(StateAutomation* state)
{
}

void Resurfacing::enter(StateAutomation* state)
{
    currentState = CurrentState::RESURFACING;
    buoyancy.setMaxSpeed(6000);
    buoyancy.setSpeed(6000);
    buoyancy.setAcceleration(6000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
}

void Resurfacing::run(StateAutomation* state)
{
    buoyancy.forward();
    //Just filling and refilling syringe at this point. need to check accelerometers and pressure
    //sensors to check minimums and maximums of our dive
    if(buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Surfaced::getInstance());
    }

    continuousFunctions();

    if(!logger.logData(data))
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    if(mission_duration.time_remaining_mission(scoped_timer.elapsed()) <= 0)
    {
        state->setState(SD_translate::getInstance());
        return;
    }

    //ADD POWER CHECKING
}

void Resurfacing::exit(StateAutomation* state)
{
}

void Surfaced::enter(StateAutomation* state)
{
    currentState = CurrentState::SURFACED;
    buoyancy.setMaxSpeed(6000);
    buoyancy.setSpeed(6000);
    buoyancy.setAcceleration(6000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
}

void Surfaced::run(StateAutomation* state)
{
    continuousFunctions();

    if(!logger.logData(data))
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    if(mission_duration.time_remaining_mission(scoped_timer.elapsed()) <= 0)
    {
        state->setState(SD_translate::getInstance());
        return;
    }

    state->setState(Diving::getInstance());
}

void Surfaced::exit(StateAutomation* state)
{
}

void SD_translate::enter(StateAutomation* state)
{
    currentState = CurrentState::SD_TRANSLATE;
    signal.on();
    LEDa.setColor(255, 255, 255);
}

void SD_translate::run(StateAutomation* state)
{
    Serial.println("Running SD_translate state");
    Time::Timer rewind_timer;
    if(!logger.rewindPrint())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    int64_t elapsed = rewind_timer.elapsed();
    LEDa.setColor(0, 255, 0);
    Serial.print(F("Rewind time(ns): ")); Serial.println(elapsed);
    while(1)
    {
        //just stop the program for now. add reinitialization state later
        delay(100);
    }
    
}

void SD_translate::exit(StateAutomation* state)
{
}

