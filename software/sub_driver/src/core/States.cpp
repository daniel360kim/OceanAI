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

#define UI_ON true

/**
 * @brief Anonymous namespace to avoid name collisions
 *
 */
namespace
{
    Time::Mission mission_duration;

    Fusion SFori;
    Sensors::Thermistor external_temp(RX_RF, 10000, 4100, 25, 30, HZ_TO_NS(50));
    Sensors::Transducer external_pres(TX_RF, 30, 10000000);
    Sensors::TotalDissolvedSolids total_dissolved_solids(TDS, 30, HZ_TO_NS(50));
    Sensors::Voltage voltmeter(v_div, 30, HZ_TO_NS(50));

    Orientation ori;

    LED signal(SIGNAL);

    int64_t previous_time;
    teensy_clock::time_point start_time;

    Velocity nav_v;
    Position nav_p;

    LoggedData data;

    SD_Logger logger(mission_duration.mission_time, HZ_TO_NS(50));

    bool warning = false;

    StepperPins pins_b{
        STP_b,
        DIR_b,
        MS1_b,
        MS2_b,
        ERR_b,
        STOP_b};

    constexpr double STEPS_PER_HALF = 224.852;
    constexpr int STEPPER_HALF_STEPS = 27000;
    Buoyancy buoyancy(pins_b, Stepper::Resolution::HALF, StepperProperties(STEPPER_HALF_STEPS / STEPS_PER_HALF, STEPPER_HALF_STEPS));

    CurrentState currentState;

    Resolution resolution = RESOLUTION_640x480;
    Frame_Number frame_number = ONE_PHOTO;
    OV2640_Mini camera(CS_VD, resolution, frame_number, true);

    StaticJsonDocument<STATIC_JSON_DOC_SIZE> data_json;

#if UI_ON

    CurrentState callbackState;
    SendData telemetry_data;
    eui_message_t tracked_variables[] =
        {
            EUI_UINT16("lt", telemetry_data.loop_time),
            EUI_FLOAT("v", telemetry_data.voltage),
            EUI_UINT8("sst", telemetry_data.system_state),
            EUI_FLOAT("it", telemetry_data.internal_temp),

            EUI_FLOAT("xd", telemetry_data.rel_ori.x),
            EUI_FLOAT("yd", telemetry_data.rel_ori.y),
            EUI_FLOAT("zd", telemetry_data.rel_ori.z),

            EUI_FLOAT_ARRAY_RO("gd", telemetry_data.gyr),
            EUI_FLOAT_ARRAY_RO("ad", telemetry_data.acc),

            EUI_INT16("sp", telemetry_data.stepper_current_position),
            EUI_INT16("st", telemetry_data.stepper_target_position),
            EUI_INT16("ss", telemetry_data.stepper_speed),
            EUI_INT16("sa", telemetry_data.stepper_acceleration),

            EUI_UINT8("ssc", telemetry_data.system_state_command),
            EUI_INT16("sc", telemetry_data.stepper_speed_command),
            EUI_INT16("ac", telemetry_data.stepper_acceleration_command),
    };

    void serial_write(uint8_t *data, uint16_t len);
    eui_interface_t serial_comms = EUI_INTERFACE(&serial_write);

    void serial_rx_handler()
    {
        // While we have data, we will pass those bytes to the ElectricUI parser
        while (Serial.available() > 0)
        {
            eui_parse(Serial.read(), &serial_comms); // Ingest a byte
        }
    }

    void serial_write(uint8_t *data, uint16_t len)
    {
        Serial.write(data, len); // output on the main serial port
    }

    int64_t previous_telem_send_time = 0;

    bool idle_called = false;
#endif



};

/**
 * @brief Functions that run in multiple states
 * FASTRUN is a macro where the function data is copied to ITCM in RAM and runs from there
 */
FASTRUN void continuousFunctions(StateAutomation *state)
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

    UnifiedSensors::getInstance().logIMUToStruct(data);

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
    serial_rx_handler();

    telemetry_data.convert(data);

    int64_t current_time = scoped_timer.elapsed();
    if (current_time - previous_telem_send_time >= 100000000)
    {
        previous_telem_send_time = current_time;
        eui_send_tracked("lt");
        eui_send_tracked("v");
        eui_send_tracked("sst");
        eui_send_tracked("it");

        eui_send_tracked("xd");
        eui_send_tracked("yd");
        eui_send_tracked("zd");

        eui_send_tracked("gd");
        eui_send_tracked("ad");

        eui_send_tracked("sp");
        eui_send_tracked("st");
        eui_send_tracked("ss");
        eui_send_tracked("sa");
    }

    if(telemetry_data.system_state_command != 0 && !idle_called)
    {
        callbackState = currentState;
        state->setState(IdleMode::getInstance());
        idle_called = true;
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

    LEDa.setColor(255, 0, 255);
    LEDb.setColor(255, 0, 255);

    // I2C Scanner
    UnifiedSensors::getInstance().scanAddresses();
    //SUCCESS_LOG("I2C Scanner Complete");

    LEDa.setColor(0, 255, 255);
    LEDb.setColor(0, 255, 255);

    // Initialize the navigation sensors (IMU, Barometer, Magnetometer)
    if (!UnifiedSensors::getInstance().initNavSensors())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }

    UnifiedSensors::getInstance().setInterrupts(BAR_int, ACC_int, GYR_int, MAG_int);
    //SUCCESS_LOG("Nav Sensor Initialization Complete");

    UnifiedSensors::getInstance().setGyroBias(); // Read readings from gyroscopes and set them as bias

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
        //SUCCESS_LOG("Camera Initialization Complete");
    }
#endif

    // Initialize the SD card
    if (!logger.init())
    {
        state->setState(ErrorIndication::getInstance());
        return;
    }
    //SUCCESS_LOG("SD Card Initialization Complete");

    LEDb.blink(255, 0, 0, 1000);
    LEDa.blink(255, 0, 0, 1000);
#if UI_ON
    while (!Serial)
        ; // Wait for serial montior to open
          // Provide the library with the interface we just setup
    eui_setup_interface(&serial_comms);

    // Provide the tracked variables to the library
    EUI_TRACK(tracked_variables);

    // Provide a identifier to make this board easy to find in the UI
    eui_setup_identifier((char *)"OceanAI", 8);

#endif
    
    #if UI_ON
        buoyancy.setSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setMaxSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setAcceleration(telemetry_data.stepper_acceleration_command);
    #else
        buoyancy.setSpeed(2000);
        buoyancy.setMaxSpeed(2000);
        buoyancy.setAcceleration(2000);
    #endif
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
    if(telemetry_data.system_state_command == 0)
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
    idle_called = false;
    #endif
}

void Diving::enter(StateAutomation *state)
{
    // Set current state for reading
    currentState = CurrentState::DIVING_MODE;

    // Initialize stepper settings
    #if UI_ON
        buoyancy.setSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setMaxSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setAcceleration(telemetry_data.stepper_acceleration_command);
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
        buoyancy.setSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setMaxSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setAcceleration(telemetry_data.stepper_acceleration_command);
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
        buoyancy.setSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setMaxSpeed(telemetry_data.stepper_speed_command);
        buoyancy.setAcceleration(telemetry_data.stepper_acceleration_command);
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
