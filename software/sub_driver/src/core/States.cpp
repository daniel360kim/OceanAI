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

#include "pins.h"


#include "indication/OutputFuncs.h"
#include "indication/LED.h"

#include <Arduino.h>
#include <teensy_clock/teensy_clock.h>
#include <cstdint>
#include <chrono>

#include "Navigation/SensorFusion/Fusion.h"
#include "navigation/Orientation.h"
#include "Navigation/Postioning.h"

#include "debug.h"
#include "Time.h"
#include "core/Timer.h"
#include "module/stepper.h"
#include "module/limit.h"


static Fusion SFori;
static Sensors::Thermistor external_temp(RX_RF, 10000, 4100, 25, 30, HZ_TO_NS(50));
static Sensors::Transducer external_pres(TX_RF, 30, 10000000);
static Sensors::TotalDissolvedSolids total_dissolved_solids(TDS, 30, HZ_TO_NS(50));
static Sensors::Voltage voltmeter(v_div, 30, HZ_TO_NS(50));

static Orientation ori;
 
static LED signal(SIGNAL);

static int64_t previous_time;
static teensy_clock::time_point start_time;

static Velocity nav_v;
static Position nav_p;

static LoggedData data;

static SD_Logger logger(MissionDuration::mission_time, HZ_TO_NS(50));
 
static bool warning = false;

static StepperPins pins_b{
    STP_b,
    DIR_b,
    MS1_b,
    MS2_b,
    ERR_b,
    STOP_b};

constexpr double STEPS_PER_HALF = 224.852;
constexpr int STEPPER_HALF_STEPS = 27000;
static Buoyancy buoyancy(pins_b, Stepper::Resolution::HALF, StepperProperties(STEPPER_HALF_STEPS / STEPS_PER_HALF, STEPPER_HALF_STEPS));

static CurrentState currentState;

static StaticJsonDocument<STATIC_JSON_DOC_SIZE> data_json;

#if UI_ON
    static CurrentState callbackState; //State to go back to after going into idle
#endif



/**
 * @brief Functions that run in multiple states
 */
void continuousFunctions(StateAutomation *state)
{
    data.time_ns = scoped_timer.elapsed(); // scoped timer is a global object to measure time since program epoch
    data.delta_time = (scoped_timer.elapsed() - previous_time) / 1000000000.0;
    previous_time = scoped_timer.elapsed();

    data.system_state = static_cast<uint8_t>(currentState);

#if PRINT_STATE
    StateAutomation::printState(Serial, currentState);
#endif

    data.loop_time = 1.0 / data.delta_time;

    CPU::log_cpu_info(data);

    Sensors::logData(data);

    external_temp.logToStruct(data);
    external_pres.logToStruct(data);
    total_dissolved_solids.logToStruct(data);
    voltmeter.logToStruct(data);

    nav_v.updateVelocity(data);
    nav_p.updatePosition(data);
    SFori.update(data);

    Quaternion relative = Orientation::toQuaternion(data.rel_ori.x, data.rel_ori.y, data.rel_ori.z);
    data.wfacc = ori.convertAccelFrame(relative, data.racc.x, data.racc.y, data.racc.z);

    data.relative = static_cast<Angles_4D>(relative);

#if OPTICS_ON == true
    camera.capture();
    //INFO_LOG("Captured image");
    logger.log_image(camera);
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

    logger.update_sd_capacity(data);

    LoggedData::data_to_json(data, data_json);

    if (!logger.logData(data_json))
    {
        warning = true;
        return;
    }
#if UI_ON
    //Send/receive data to/from the UI
    //If GUI wants to change the state, it will be handled here
    if(TransportManager::handleTransport(data))
    {
        callbackState = currentState;
        state->setState(IdleMode::getInstance());
        return;
    }
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

    // If there was a crash from the last run, report it to SD
    if (CrashReport)
    {
        if (!logger.log_crash_report())
        {
            warning = true;
        }
    }

    // Flashy lights!
    output.startupSequence();

    if (voltmeter.readRaw() <= 11.1 && voltmeter.readRaw() >= 6)
    {
        //ERROR_LOG(Debug::Critical_Error, "Low battery voltage");
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
        TransportManager::Commands stepper_commands = TransportManager::getCommands();
        buoyancy.setSpeed(stepper_commands.stepper_speed);
        buoyancy.setMaxSpeed(stepper_commands.stepper_speed);
        buoyancy.setAcceleration(stepper_commands.stepper_speed);

    #else
        buoyancy.setSpeed(2000);
        buoyancy.setMaxSpeed(2000);
        buoyancy.setAcceleration(2000);
    #endif

    LEDa.setColor(255, 0, 255);
    LEDb.setColor(255, 0, 255);

    // I2C Scanner
    Sensors::scanI2C();
    //SUCCESS_LOG("I2C Scanner Complete");

    LEDa.setColor(0, 255, 255);
    LEDb.setColor(0, 255, 255);

    // Initialize the navigation sensors (IMU, Barometer, Magnetometer)
    if (!Sensors::initAll())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    Sensors::setInterrupts();
    SUCCESS_LOG("Nav Sensor Initialization Complete");

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

    // Initialize the SD card
    if (!logger.init())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    SUCCESS_LOG("SD Card Initialization Complete");

    LEDb.blink(255, 0, 0, 1000);
    LEDa.blink(255, 0, 0, 1000);

    currentState = CurrentState::INITIALIZATION;
}

void Initialization::run(StateAutomation *state)
{
    // initialization happens once and we move on...
    state->setState(Calibrate::getInstance());
}

void Initialization::exit(StateAutomation *state)
{
    output.indicateCompleteStartup();
    // set previous time before main loop
    previous_time = scoped_timer.elapsed();
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
    buoyancy.setSpeed(0);
}

void IdleMode::run(StateAutomation *state)
{
    continuousFunctions(state);
    buoyancy.run();
    #if UI_ON
    if(TransportManager::getCommands().system_state == 0)
    {
        switch(callbackState)
        {
            case CurrentState::CALIBRATE:
                state->setState(Calibrate::getInstance());
                return;
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
                state->setState(Calibrate::getInstance());
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

void Diving::enter(StateAutomation *state)
{
    // Set current state for reading
    currentState = CurrentState::DIVING_MODE;

    // Initialize stepper settings
    #if UI_ON
        TransportManager::Commands stepper_commands = TransportManager::getCommands();
        buoyancy.setSpeed(stepper_commands.stepper_speed);
        buoyancy.setMaxSpeed(stepper_commands.stepper_speed);
        buoyancy.setAcceleration(stepper_commands.stepper_acceleration);
    #else
        buoyancy.setSpeed(2000);
        buoyancy.setMaxSpeed(2000);
        buoyancy.setAcceleration(2000);
    #endif
    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.setMinPulseWidth(1); // how long to wait between high and low pulses
    buoyancy.sink();              // set the direction of the stepper motors
}

void Diving::run(StateAutomation *state)
{
    // If we have filled the ballast start resurfacing
    // Probably want to add some type of pressure parameter so the sub goes to a certain depth
    if (buoyancy.sinking && buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        #if UI_ON
        state->setState(Resurfacing::getInstance());
        return;
        #else
        state->setState(Resurfacing::getInstance());
        return;
        #endif
    }

    buoyancy.update(); // update the stepper motors

    // call the continuous loop functions
    continuousFunctions(state);
}

void Diving::exit(StateAutomation *state)
{
}

void Resurfacing::enter(StateAutomation *state)
{
    currentState = CurrentState::RESURFACING;
    #if UI_ON
        TransportManager::Commands stepper_commands = TransportManager::getCommands();
        buoyancy.setSpeed(stepper_commands.stepper_speed);
        buoyancy.setMaxSpeed(stepper_commands.stepper_speed);
        buoyancy.setAcceleration(stepper_commands.stepper_acceleration);
    #else  
        buoyancy.setSpeed(2000);
        buoyancy.setMaxSpeed(2000);
        buoyancy.setAcceleration(2000);
    #endif
    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.rise();
    buoyancy.setMinPulseWidth(1);
}

void Resurfacing::run(StateAutomation *state)
{
    // If we emptied the ballast, we move to the surface
    if (buoyancy.currentPosition() == buoyancy.targetPosition())
    {
        if (m_iterations >= 3) // every three times we dive and resurface, recalibrate to correct any integrated small errors
        {
            state->setState(Calibrate::getInstance());
            m_iterations = 0;
            return;
        }
        else
        {
            state->setState(Diving::getInstance());
            return;
        }
    }
    
    buoyancy.update(); // update the stepper motors
    // Just filling and refilling syringe at this point. need to check accelerometers and pressure
    // sensors to check minimums and maximums of our dive

    continuousFunctions(state);
}

void Resurfacing::exit(StateAutomation *state)
{
    m_iterations++;
}

void Calibrate::enter(StateAutomation *state)
{
    currentState = CurrentState::CALIBRATE;
    #if UI_ON
        TransportManager::Commands stepper_commands = TransportManager::getCommands();
        buoyancy.setSpeed(stepper_commands.stepper_speed);
        buoyancy.setMaxSpeed(stepper_commands.stepper_speed);
        buoyancy.setAcceleration(stepper_commands.stepper_acceleration);
    #else
        buoyancy.setSpeed(2000);
        buoyancy.setMaxSpeed(2000);
        buoyancy.setAcceleration(2000);
    #endif

    buoyancy.setResolution(Stepper::Resolution::HALF);
    buoyancy.setMinPulseWidth(1);

    buoyancy.move(500000);
}

void Calibrate::run(StateAutomation *state)
{
    continuousFunctions(state);
    buoyancy.update();

    if (buoyancy.limit.state() == true)
    {
        state->setState(Resurfacing::getInstance());
    }
}

void Calibrate::exit(StateAutomation *state)
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
Diving Diving::instance;
Resurfacing Resurfacing::instance;
Calibrate Calibrate::instance;

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

Diving &Diving::getInstance()
{
    return instance;
}

Resurfacing &Resurfacing::getInstance()
{
    return instance;
}

Calibrate &Calibrate::getInstance()
{
    return instance;
}
