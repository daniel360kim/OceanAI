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

SD_Logger logger(mission_duration, 100000);

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

Buoyancy buoyancy(pins_b, Stepper::Resolution::HALF, StepperProperties(169.0, 76000));

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
    
    #if PRINT_STATE
        StateAutomation::printState(Serial, currentState);
    #endif

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

    #if DEBUG == true
        while(!Serial); //Wait for serial montior to open
    #endif
    Serial.begin(2000000);

    LEDa.setColor(255, 0, 255);
    LEDb.setColor(255, 0, 255);

    UnifiedSensors::getInstance().scanAddresses();
    Serial.println("Addresses scanned");

    LEDa.setColor(0, 255, 255);
    LEDb.setColor(0, 255, 255);
    if(!UnifiedSensors::getInstance().initNavSensors())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    Serial.println("Nav sensors initialized");

    UnifiedSensors::getInstance().initVoltmeter(v_div, (uint32_t)10000000, 10.0);
    UnifiedSensors::getInstance().initTDS(TDS, (uint32_t)10000000, 10.0);
    UnifiedSensors::getInstance().initPressureSensor(TX_GPS, (uint32_t)10000000, 20.0);

    UnifiedSensors::getInstance().setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);

    UnifiedSensors::getInstance().setGyroBias();
    Serial.println("Other sensors initialized");

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
    Serial.println("Logger initialized");

    LEDb.blink(255, 0, 0, 1000);
    LEDa.blink(255, 0, 0, 1000);

    currentState = CurrentState::INITIALIZATION;

    //buoyancy.calibrate();
}

void Initialization::run(StateAutomation* state)
{
    state->setState(Resurfacing::getInstance());
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
    buoyancy.setMaxSpeed(10000);
    buoyancy.setSpeed(10000);
    buoyancy.setAcceleration(10000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.setMinPulseWidth(1);
    buoyancy.sink();
}

void Diving::run(StateAutomation* state)
{
    if(buoyancy.sinking && buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Resurfacing::getInstance());
        return;
    }
    Serial.println("Diving");
    buoyancy.update();
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
    buoyancy.setMaxSpeed(10000);
    buoyancy.setSpeed(10000);
    buoyancy.setAcceleration(10000);
    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.rise();
    buoyancy.setMinPulseWidth(1);
}

void Resurfacing::run(StateAutomation* state)
{
    if(buoyancy.rising && buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Diving::getInstance());
        return;
    }

    buoyancy.run();
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
    Time::NamedTimer sd_timer("SD Translation");
    if(!logger.rewindPrint())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    Time::TimerManager::getInstance().addTimer(sd_timer.getTimeInfo());
    LEDa.setColor(0, 255, 0);
    Time::TimerManager::getInstance().printTimers();
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

