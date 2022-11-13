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
#include "Timer.h"

/**
 * @brief Anonymous namespace to avoid name collisions
 * 
 */
namespace
{
    Time::Mission mission_duration;

    Fusion SFori;
    Sensors::Thermistor external_temp(RX_GPS, 10000, 4100, 25, 30, 10000000);
    Sensors::Transducer external_pres(TX_GPS, 30, 10000000);
    Sensors::TotalDissolvedSolids total_dissolved_solids(TDS, 30, 10000000);
    Sensors::Voltage voltmeter(v_div, 30, 10000000);

    Optics::Camera camera(CS_VD);

    Orientation ori;

    LED signal(SIGNAL);

    int64_t previous_time;
    teensy_clock::time_point start_time;

    Velocity nav_v;
    Position nav_p;

    Data data;

    SD_Logger logger(mission_duration.mission_time, 4000000);

    bool warning = false;
/*
    StepperPins pins_p{
        STP_p,
        DIR_p,
        MS1_p,
        MS2_p,
        ERR_p,
        STOP_p};
*/
    StepperPins pins_b{
        STP_b,
        DIR_b,
        MS1_b,
        MS2_b,
        ERR_b,
        STOP_b};

    Buoyancy buoyancy(pins_b, Stepper::Resolution::HALF, StepperProperties(169.0, 76000));

    CurrentState currentState;
}

/**
 * @brief Functions that run in multiple states
 * FASTRUN is a macro where the function data is copied to ITCM in RAM and runs from there
 */
FASTRUN void continuousFunctions()
{
    data.time_ns = scoped_timer.elapsed(); //scoped timer is a global object to measure time since program epoch
    data.delta_time = (scoped_timer.elapsed() - previous_time) / 1000000000.0;
    previous_time = scoped_timer.elapsed();

    data.system_state = static_cast<uint8_t>(currentState);
    
    #if PRINT_STATE
        StateAutomation::printState(Serial, currentState);
    #endif

    data.loop_time = 1.0 / data.delta_time;

    UnifiedSensors::getInstance().logIMUToStruct(data);
    
    external_temp.logToStruct(data);
    external_pres.logToStruct(data);
    total_dissolved_solids.logToStruct(data);
    voltmeter.logToStruct(data);

    OS::getInstance().log_cpu_state(data);

    nav_v.updateVelocity(data);
    nav_p.updatePosition(data);
    SFori.update(data);

    Quaternion relative = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z);
    data.wfacc = ori.convertAccelFrame(relative, data.racc.x, data.racc.y, data.racc.z);

    data.relative = static_cast<Angles_4D>(relative);

    
#if OPTICS_ON == true
    camera.capture(1000000, &data.optical_data.capture_time, &data.optical_data.save_time, &data.optical_data.FIFO_length, logger.closeFile, logger.reopenFile, logger.get_data_filename());
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
    
}
///****************///

/**
 * Initialization state functions
 */

/**
 * @brief All the initialization functions
 * 
 * @param state 
 */
void Initialization::enter(StateAutomation* state)
{
    start_time = teensy_clock::now(); //begin the scoped timer

    //If there was a crash from the last run, report it to SD
    if(CrashReport)
    {
        if(!logger.log_crash_report())
        {
            warning = true;
        }
    }

    // Flashy lights!
    output.startupSequence();

    if(voltmeter.readRaw() <= 11.1 && voltmeter.readRaw() >= 6)
    {
        ERROR_LOG(Debug::Critical_Error, "Low battery voltage");
        state->setState(ErrorIndication::getInstance());
    }

    #if LIVE_DEBUG == true
        while(!Serial); //Wait for serial montior to open
    #endif
    Serial.begin(2000000);

    LEDa.setColor(255, 0, 255);
    LEDb.setColor(255, 0, 255);

    //I2C Scanner
    UnifiedSensors::getInstance().scanAddresses();
    SUCCESS_LOG("I2C Scanner Complete");

    LEDa.setColor(0, 255, 255);
    LEDb.setColor(0, 255, 255);

    //Initialize the navigation sensors (IMU, Barometer, Magnetometer)
    if(!UnifiedSensors::getInstance().initNavSensors())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    UnifiedSensors::getInstance().setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);
    SUCCESS_LOG("Nav Sensor Initialization Complete");

    UnifiedSensors::getInstance().setGyroBias(); //Read readings from gyroscopes and set them as bias

    //Initialize the optical camera
    #if OPTICS_ON == true
        if (!camera.begin())
        {
            warning = true;
        }
    #endif

    //Initialize the SD card
    if (!logger.init())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    SUCCESS_LOG("SD Card Initialization Complete");

    LEDb.blink(255, 0, 0, 1000);
    LEDa.blink(255, 0, 0, 1000);

    currentState = CurrentState::INITIALIZATION;

    buoyancy.setMinPulseWidth(1);
    buoyancy.calibrate(); //Calibrate the stepper motors
}

void Initialization::run(StateAutomation* state)
{
    //initialization happens once and we move on...
    state->setState(Diving::getInstance());
}

void Initialization::exit(StateAutomation* state)
{
    output.indicateCompleteStartup();
    //set previous time before main loop
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
    //Set current state for reading
    currentState = CurrentState::DIVING_MODE;

    //Initialize stepper settings
    buoyancy.setMaxSpeed(7000);
    buoyancy.setSpeed(7000);
    buoyancy.setAcceleration(6000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.setMinPulseWidth(1); //how long to wait between high and low pulses
    buoyancy.sink(); //set the direction of the stepper motors
}

void Diving::run(StateAutomation* state)
{
    //If we have filled the ballast start resurfacing
    //Probably want to add some type of pressure parameter so the sub goes to a certain depth
    if(buoyancy.sinking && buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Resurfacing::getInstance());
        return;
    }

    buoyancy.update(); //update the stepper motors
    if(buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Resurfacing::getInstance());
    }

    //call the continuous loop functions
    continuousFunctions();

    //Log to SD card
    if(!logger.logData(data))
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    //If we reached the end of the mission, we move to the end
    if(mission_duration.time_remaining_mission(scoped_timer.elapsed()) <= 0)
    {
        state->setState(SD_translate::getInstance());
        return;
    }

    //ADD POWER CHECKING
}

void Diving::exit(StateAutomation* state)
{
    //nothing here
}

void Resurfacing::enter(StateAutomation* state)
{
    currentState = CurrentState::RESURFACING;
    buoyancy.setMaxSpeed(7000);
    buoyancy.setSpeed(7000);
    buoyancy.setAcceleration(6000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.rise();
    buoyancy.setMinPulseWidth(1);
}

void Resurfacing::run(StateAutomation* state)
{
    //If we emptied the ballast, we move to the surface
    if(buoyancy.rising && buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Diving::getInstance());
        return;
    }

    buoyancy.run();//update the stepper motors
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
    //nothing here
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
    logger.closeFile();
}

void SD_translate::run(StateAutomation* state)
{
    while(1)
    {
        //just stop the program for now. add reinitialization state later
        delay(100);
    }
    
}

void SD_translate::exit(StateAutomation* state)
{
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

