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
#include "cpu.h"
#include "../Data/TransportManager.h"
#include "../Sensors/Sensors.h"
#include "../Sensors/thermistor.h"
#include "../Sensors/transducer.h"
#include "../Sensors/tds.h"
#include "../Sensors/voltage.h"

#include "../Data/SD/SD.h"
#include "../Data/hitl.h"

#include "pins.h"

#include "indication/OutputFuncs.h"
#include "indication/LED.h"
#include "indication/LEDStrip.h"

#include <Arduino.h>
#include <teensy_clock/teensy_clock.h>
#include <cstdint>
#include <array>

#include "../Navigation/SensorFusion/Fusion.h"
#include "../navigation/Orientation.h"
#include "../Navigation/Postioning.h"
#include "../Navigation/hitl_navigation.h"

#include "debug.h"
#include "Time.h"
#include "core/Timer.h"
#include "module/stepper.h"
#include "module/limit.h"


static Fusion SFori;

static Sensors::Thermistor external_temp(RX_RF, 10000, 4100, 25, 30, HZ_TO_NS(5));
static Sensors::Transducer external_pres(TX_RF, 30, HZ_TO_NS(5));
static Sensors::TotalDissolvedSolids total_dissolved_solids(TDS, 30, HZ_TO_NS(5));

static Sensors::Voltage regulator(v_div, 30, HZ_TO_NS(1), 9.95, 1.992);
static Sensors::Voltage battery(TX_GPS, 30, HZ_TO_NS(1), 9.62, 4.47);

static Orientation ori;
 
static LED signal(SIGNAL);

static teensy_clock::time_point start_time;

static Velocity nav_v;
static Position nav_p;

static LoggedData data;

static SD_Logger logger(MissionDuration::mission_time, HZ_TO_NS(10));
 
static bool warning = false;

static StepperPins pins_b{
    STP_b,
    DIR_b,
    MS1_b,
    MS2_b,
    ERR_b,
    STOP_b
};

static StepperPins pins_p{
    STP_p,
    DIR_p,
    MS1_p,
    MS2_p,
    ERR_p,
    STOP_p
};

constexpr double STEPS_PER_HALF = 224.852;
//end at 27000
constexpr int STEPPER_HALF_STEPS_BUOYANCY = 27000; // constant representing how many half steps the stepper motor takes to move the buoyancy to the bottom
constexpr int STEPPER_HALF_STEPS_PITCH = 10850; // constant representing how many half steps the stepper motor takes to reach the end of the carriage

static Mechanics::StepperProperties buoyancy_properties(STEPPER_HALF_STEPS_BUOYANCY / STEPS_PER_HALF, STEPPER_HALF_STEPS_BUOYANCY);
static Mechanics::StepperProperties pitch_properties(STEPPER_HALF_STEPS_PITCH / STEPS_PER_HALF, STEPPER_HALF_STEPS_PITCH);

static Mechanics::Buoyancy buoyancy(pins_b, Mechanics::Stepper::Resolution::HALF, buoyancy_properties);
static Mechanics::Pitch pitch(pins_p, Mechanics::Stepper::Resolution::HALF, pitch_properties);

static CurrentState currentState;

static StaticJsonDocument<STATIC_JSON_DOC_SIZE> data_json;

#if UI_ON
    static CurrentState callbackState; //State to go back to after going into idle
#endif

#if HITL_ON
    HITL::DataProviderManager data_provider((int64_t)618LL*1000000000LL);

    HITL::Data location;
    HITL::Data depth;
    HITL::Data pressure;
    HITL::Data salinity;
    HITL::Data temperature;

    HITL::HITLNavigation hitl_nav;
    
#endif

/**
 * @brief Functions that run in multiple states looped
 */
void continuousFunctions(StateAutomation *state)
{
    data.time_ns = scoped_timer.elapsed(); // scoped timer is a global object to measure time since program epoch
    data.delta_time = scoped_timer.deltaTime(); // delta time is the time since the last time this function was called

    data.system_state = static_cast<uint8_t>(currentState); //update the current state within the logged data

#if PRINT_STATE
    StateAutomation::printState(Serial, currentState);
#endif

    #if HITL_ON
        data_provider.update(data.time_ns); //update the data provider with the current time
        HITL::logData(data, data_provider, location, depth, pressure, salinity, temperature); //log the HITL data to the logged data struct
        hitl_nav.logData(data); //log the HITL navigation data to the logged data struct
    #endif

    //Logging sensor data
    CPU::log_cpu_info(data); //add cpu info to the logged data

    Sensors::logData(data); //add IMU data to the logged data

    external_temp.logToStruct(data);
    external_pres.logToStruct(data);
    total_dissolved_solids.logToStruct(data);

    //log the voltage and regulator data
    battery.logData(data, data.raw_voltage, data.filt_voltage);
    regulator.logData(data, data.raw_regulator, data.filt_regulator);

    nav_v.updateVelocity(data); //calculate velocity from IMU data
    nav_p.updatePosition(data); //calculate position from IMU data
    SFori.update(data); //update the orientation of the sub from IMU data

    Quaternion relative = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z); //convert the relative orientation to a quaternion
    data.wfacc = ori.convertAccelFrame(relative, data.racc.x, data.racc.y, data.racc.z); //convert the acceleration from the relative frame to the world frame

    data.relative = static_cast<Angles_4D>(relative); //add to logged data

#if OPTICS_ON == true
    camera.capture();
    //INFO_LOG("Captured image");
    logger.log_image(camera);
#endif

    signal.blink(80); //blink to look cool :)

    if (!warning)
    {
        //Show the spectrum on the LEDs if there is no warning
        LEDa.displaySpectrum();
        LEDb.displaySpectrum();
    }
    else
    {
        LEDb.blink(255, 0, 0, data.sd_log_rate_hz); //blink red if there is a warning
    }

    strip.setColor(255, 255, 255); //turn on LED strip at the bottom of the vehicle

    //Log and update buoyancy and pitch stepper motor data
    buoyancy.logToStruct(data);
    if(!buoyancy.update())
    {
        warning = true;
    }

    pitch.logToStruct(data);
    if(!pitch.update())
    {
        warning = true;
    }

    logger.update_sd_capacity(data);
    data.sd_log_rate_hz = logger.getLoggingIntervalHz();

    #if SD_ON
    if (!logger.logData(data))
    {
        warning = true;
        return;
    }
    #endif

#if UI_ON
    //Send/receive data to/from the UI
    //If GUI wants to change the state, it will be handled here
    if(TransportManager::handleTransport(data))
    {
        callbackState = currentState;
        state->setState(IdleMode::getInstance());
        return;
    }

    //Get commands from the GUI
    TransportManager::Commands commands = TransportManager::getCommands();

    pitch.runPitch(commands, currentState, buoyancy.currentPosition());

    #if HITL_ON
        data_provider.update_frequency_scale(commands.hitl_scale);

        //NS to HZ
        data.hitl_rate = ((double)data_provider.get_frequency() / 1000000.0);
        data.hitl_progress = data_provider.get_progress() * 100.0; //percentage

    #endif

    #if SD_ON
    if(commands.sd_log_enable > 0)
    {
        logger.setLoggingInterval(std::lround((1.0 / commands.sd_log_enable) * 1000000000.0)); //hz to ns
        if(!logger.logData(data))
        {
            warning = true;   
        }
    }
    #endif

#else
    pitch.runPitch(currentState, buoyancy.currentPosition());
#endif

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
void Initialization::enter(StateAutomation *state)
{
    start_time = teensy_clock::now(); // begin the scoped timer
    // Flashy lights!
    output.startupSequence();
    // If there was a crash from the last run, report it to SD
    #if SD_ON
    if (CrashReport)
    {
        if (!logger.log_crash_report())
        {
            warning = true;
        }
    }
    #endif

    currentState = CurrentState::INITIALIZATION;

    #if HITL_ON
        HITL::initializeProviders(data_provider, location, depth, pressure, salinity, temperature);
        hitl_nav.setInitialCoordinate(HITL_DATA_ALPHA[0][0], HITL_DATA_ALPHA[0][1], scoped_timer.elapsed());
    #endif

    if (battery.readRaw() <= 6 && battery.readRaw() >= 5.5)
    {
        ERROR_LOG(Debug::Critical_Error, "Low battery voltage");
        state->setState(ErrorIndication::getInstance());
    }
    
    CPU::init();

#if LIVE_DEBUG == true
    while (!Serial)
        ; // Wait for serial montior to open
#endif
    Serial.begin(2000000);

    #if UI_ON
        TransportManager::init();
    #endif

    LEDa.setColor(255, 0, 255);
    LEDb.setColor(255, 0, 255);

    LEDa.setColor(0, 255, 255);
    LEDb.setColor(0, 255, 255);

    // Initialize the navigation sensors (IMU, Barometer, Magnetometer)
    if (!Sensors::initAll())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    Sensors::setInterrupts();
    SUCCESS_LOG("Nav Sensor Initialization Complete\n");

    #if DEBUG_ON
        Angles_3D<double> bias = Sensors::setGyroBias();
        INFO_LOGf("Gyro bias set to x:%d y:%d z:%d", bias.x, bias.y, bias.z)
    #else
        Sensors::setGyroBias();
    #endif

// Initialize the optical camera
#if OPTICS_ON == true
    camera.saveSettings();
    if (!camera.initialize())
    {
        //ERROR_LOG(Debug::Critical_Error, "Camera Initialization Failed");
        state->setState(ErrorIndication::getInstance());
    }
    else
    {
        SUCCESS_LOG("Camera Initialization Complete");
    }
#endif

    #if SD_ON
    // Initialize the SD card
    if (!logger.init())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    #endif
    SUCCESS_LOG("SD Card Initialization Complete");

    Mechanics::setDefaultSettings(buoyancy, pitch);
    Mechanics::setDefaultSpeeds(buoyancy, pitch);

    while(!Mechanics::calibrateBoth(buoyancy, pitch))
    {
        continuousFunctions(state);
    }

    #if UI_ON

        /**
         * If the UI is on, we need to wait for the UI to send the initialization commands
         */
        TransportManager::Commands commands = TransportManager::getCommands(); //Wait for the UI to send the initialization commands

        //Set the stepper settings from the UI commands
        buoyancy.setSpeeds(commands.buoyancy.speed, commands.buoyancy.acceleration);
        pitch.setSpeeds(commands.pitch.speed, commands.pitch.acceleration);

        #if HITL_ON
            data_provider.update_frequency_scale(commands.hitl_scale);
        #endif

    #else
        Mechanics::setDefaultSpeeds(buoyancy, pitch);
    #endif
}

void Initialization::run(StateAutomation *state)
{
    // initialization happens once and we move on...
    state->setState(Diving::getInstance());
}

void Initialization::exit(StateAutomation *state)
{
    output.indicateCompleteStartup();
    // set previous time before main loop
    scoped_timer.setPreviousTime(scoped_timer.elapsed());
}

void ErrorIndication::enter(StateAutomation *state)
{
    currentState = CurrentState::ERROR_INDICATION;
}

void ErrorIndication::run(StateAutomation *state)
{
    output.indicateError();
    continuousFunctions(state);
}

void ErrorIndication::exit(StateAutomation *state)
{

}

void IdleMode::enter(StateAutomation *state)
{
    currentState = CurrentState::IDLE_MODE;
    
    buoyancy.setSpeeds(0, 0);
    pitch.setSpeeds(0, 0);
}

void IdleMode::run(StateAutomation *state)
{
    continuousFunctions(state);

    buoyancy.update();
    pitch.update();

    #if UI_ON
    if(TransportManager::getCommands().system_state == 0)
    {
        switch(callbackState)
        {
            case CurrentState::DIVING_MODE:
                state->setState(Diving::getInstance());
                return;
            case CurrentState::ERROR_INDICATION:
                state->setState(ErrorIndication::getInstance());
                return;
            case CurrentState::INITIALIZATION:
                state->setState(Initialization::getInstance());
                return;
            case CurrentState::RESURFACING:
                state->setState(Resurfacing::getInstance());
                return;
            default:
                state->setState(Diving::getInstance());
                return;
        }
    }
    #endif
}


void IdleMode::exit(StateAutomation *state)
{
    #if UI_ON
        TransportManager::setIdle(false);
    #endif
}


void Resurfacing::enter(StateAutomation *state)
{
    currentState = CurrentState::RESURFACING;
    #if UI_ON
        TransportManager::Commands stepper_commands = TransportManager::getCommands();

        buoyancy.setSpeeds(stepper_commands.buoyancy.speed, stepper_commands.buoyancy.acceleration);
        pitch.setSpeeds(stepper_commands.pitch.speed, stepper_commands.pitch.acceleration);
    #else  
        Mechanics::setDefaultSettings(buoyancy, pitch);
    #endif

    buoyancy.rise();
}

void Resurfacing::run(StateAutomation *state)
{
    // If we emptied the ballast, we move to the surface
    if (buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        state->setState(Diving::getInstance());
        return;
    }

    continuousFunctions(state);
}

void Resurfacing::exit(StateAutomation *state)
{
    
}

void Diving::enter(StateAutomation *state)
{
    currentState = CurrentState::DIVING_MODE;
    #if UI_ON
        TransportManager::Commands stepper_commands = TransportManager::getCommands();

        buoyancy.setSpeeds(stepper_commands.buoyancy.speed, stepper_commands.buoyancy.acceleration);
        pitch.setSpeeds(stepper_commands.pitch.speed, stepper_commands.pitch.acceleration);
    #else
        Mechanics::setDefaultSettings(buoyancy, pitch);
    #endif

    buoyancy.move(500000);
}

void Diving::run(StateAutomation *state)
{
    continuousFunctions(state);

    if (buoyancy.limit.state() == true)
    {
        state->setState(Resurfacing::getInstance());
    }
}

void Diving::exit(StateAutomation *state)
{
    buoyancy.setCurrentPosition(0);
}

/**
 * Singleton return instance functions
 */

// Instantiates the singletons
Initialization Initialization::instance;
ErrorIndication ErrorIndication::instance;
IdleMode IdleMode::instance;
Resurfacing Resurfacing::instance;
Diving Diving::instance;

Initialization &Initialization::getInstance()
{
    return instance;
}

ErrorIndication &ErrorIndication::getInstance()
{
    return instance;
}

IdleMode &IdleMode::getInstance()
{
    return instance;
}

Resurfacing &Resurfacing::getInstance()
{
    return instance;
}

Diving &Diving::getInstance()
{
    return instance;
}

