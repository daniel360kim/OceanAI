#include "stepper.h"
#include "../core/configuration.h"

#include <cmath>

namespace Mechanics
{
    /**
     * @brief Construct a new Stepper:: Stepper object
     * Also intializes GPIO
     * @param pins pins of the stepper driver
     * @param resolution initial resolution
     * @param properties properties of the stepper structure
     */
    Stepper::Stepper(StepperPins pins, Resolution resolution, StepperProperties properties) : AccelStepper(1, pins.STP, pins.DIR)
    {
        this->pins = pins;
        this->properties = properties;

        pinMode(pins.MS1, OUTPUT);
        pinMode(pins.MS2, OUTPUT);
        pinMode(pins.ERR, INPUT);

        limit.begin(pins.limit);

        setResolution(resolution);

        steps_per_mm = properties.halves_length / properties.carriage_length;
    }


    /**
     * @brief Configures GPIO to control resolution of TMC2208 stepper driver
     * 
     * @param resolution resolution to set
     */
    void Stepper::setResolution(Resolution resolution)
    {
        this->resolution = resolution;
        switch (resolution)
        {
        case Resolution::HALF:
            digitalWrite(pins.MS1, LOW);
            digitalWrite(pins.MS2, HIGH);
            res_multiplier = 2;
        
            break;
        case Resolution::QUARTER:
            digitalWrite(pins.MS1, HIGH);
            digitalWrite(pins.MS2, LOW);
            res_multiplier = 4;

            setMaxSpeed(maxSpeed() / res_multiplier);
            setSpeed(speed() / res_multiplier);

            break;
        case Resolution::EIGHTH:
            digitalWrite(pins.MS1, LOW);
            digitalWrite(pins.MS2, LOW);
            res_multiplier = 8;

            setMaxSpeed(maxSpeed() / res_multiplier);
            setSpeed(speed() / res_multiplier);

            break;
        case Resolution::SIXTEENTH:
            digitalWrite(pins.MS1, HIGH);
            digitalWrite(pins.MS2, HIGH);
            res_multiplier = 16;

            setMaxSpeed(maxSpeed() / res_multiplier);
            setSpeed(speed() / res_multiplier);

            break;
        }
    }

    /**
     * @brief Calibrates the stepper motor
     * Must set stepper speeds and accelerations before calling this
     * 
     */
    bool Stepper::calibrate()
    {
        moveTo(100000000); //move to end of rail

        if(limit.state() == true)
        {
            moveTo(0);
            setCurrentPosition(0);
            update();
            calibrated = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    void Stepper::setSpeeds(double speed, double acceleration)
    {
        setSpeed(speed);
        setMaxSpeed(speed);
        setAcceleration(acceleration);
    }


    /**
     * @brief After limit switch is pressed once, it goes back and slowly eases back in
     * For further precision
     * 
     */
    void Stepper::recheckLimit()
    {
        setCurrentPosition(0);
        move(-1000);
        while(currentPosition() != 1000)
        {
            run();
        }
        
        setResolution(Resolution::EIGHTH);

        move(100000000);
        uint64_t start = millis();
        while(limit.state() == false)
        {
            if(millis() - start > 10000)
            {
                return;
            }
            run();
        }

        setCurrentPosition(0);
    }

    /**
     * @brief Overhead to account for resolution changes
     * 
     * @return double 
     */
    double Stepper::currentPosition_mm()
    {
        return currentPosition() / steps_per_mm;
    }

    /**
     * @brief Move in mm. Due to floating point precision, small errors will integrate and cause drit
     * Use move_steps() instead
     * 
     * @return double 
     */
    double Stepper::targetPosition_mm()
    {
        return targetPosition() / steps_per_mm;
    }

    /**
     * @brief accounts for resolution changes
     * 
     * @param absolute 
     */
    void Stepper::goTo(long absolute)
    {
        if(resolution == Resolution::HALF)
        {
            move(absolute);
            return;
        }
        else if(resolution == Resolution::QUARTER)
        {
            absolute /= 2;
            pos_multiplier = 2;
        }
        else if(resolution == Resolution::EIGHTH)
        {
            absolute *= 4;
            pos_multiplier = 0.25;
        }
        else if(resolution == Resolution::SIXTEENTH)
        {
            absolute *= 8;
            pos_multiplier = 0.125;
        }
        else
        {
            return;
        }
        move(absolute);
    }

    /**
     * @brief Moves in mm. Use move() instead when you can
     * Floating point inprecision adds up and causes drift
     * 
     * @param mm 
     */
    void Stepper::move_mm(int mm)
    {
        goTo(mm * steps_per_mm);
    }

    /**
     * @brief Must call in loop to update
     * 
     * @return true all is good!
     * @return false all is not good
     */
    bool Stepper::update()
    {
        if(digitalRead(pins.ERR) == HIGH) //TMC2208 is reporting an error :(
        {
            return false; 
        }
        else
        {
            run();
            return true;
        }
    }

    //END OF STEPPER

    //BEGINNING OF BUOYANCY

    /**
     * @brief pulls in water to sink. WILL NOT RUN; MUST CALL an updating function
     * 
     */
    void Buoyancy::sink()
    {
        moveTo(properties.halves_length); //go to the other side of the carriage

        sinking = true;
        rising = false;
    }

    /**
     * @brief pushes all water out to float. WILL NOT RUN; MUST CALL an updating function
     * 
     */
    void Buoyancy::rise()
    {
        moveTo(-1 * properties.halves_length); //moves to the very end of the carriage

        sinking = false;
        rising = true;
    }

    void Buoyancy::rise(long half_steps)
    {
        move(-1 * half_steps);

        sinking = false;
        rising = true;
    }


    void Buoyancy::logToStruct(LoggedData &data)
    {
        data.dive_stepper.current_position = currentPosition();
        data.dive_stepper.target_position = targetPosition();

        data.dive_stepper.current_position_mm = currentPosition_mm();
        data.dive_stepper.target_position_mm = targetPosition_mm();

        data.dive_stepper.limit_state = limit.state();

        data.dive_stepper.speed = speed();
        data.dive_stepper.max_speed = maxSpeed();
        data.dive_stepper.acceleration = acceleration();
    }

    void Pitch::runPitch(TransportManager::Commands &commands, CurrentState state, double buoyancy_position)
    {
        if(commands.auto_pitch != 0)
        {
            manualMode(commands);
        }
        else
        {
            autoMode(state, buoyancy_position);
        }
    }

    void Pitch::runPitch(CurrentState state, double buoyancy_position)
    {
        autoMode(state, buoyancy_position);
    }

    void Pitch::manualMode(TransportManager::Commands &commands)
    {
        if(!isCalibrated())
        {
            setSpeeds(1000, 500);
            calibrate();
        }
        else
        {
            if(commands.pitch.direction == 0)
            {
                commands.pitch.speed *= -1;
            }

            if(currentPosition() * -1 == properties.halves_length && commands.pitch.direction == 0)
            {
                commands.pitch.speed = 0;
            }
            
            if(currentPosition() == 0 && commands.pitch.direction == 1)
            {
                commands.pitch.speed = 0;
            }

            setSpeeds(commands.pitch.speed, commands.pitch.acceleration);
        }

        if(commands.recalibrate_pitch != 0)
        {
            setCalibrated(false);
        }
    }

    void Pitch::autoMode(CurrentState state, double buoyancy_position)
    {
        setSpeeds(PITCH_DEFAULT_STEPPER_SPEED, PITCH_DEFAULT_STEPPER_ACCELERATION);

        //convert to long for comparisons
        //GUI position readings are absolute, so we need to convert to absolute values
        long new_buoyancy_position = static_cast<long>(buoyancy_position);
        new_buoyancy_position = std::abs<long>(new_buoyancy_position);

        long new_pitch_position = static_cast<long>(currentPosition());
        new_pitch_position = std::abs<long>(new_pitch_position);

        if(state == CurrentState::RESURFACING)
        {
            if(new_buoyancy_position < 11000)
            {
                setSpeed(0);
            }
            else if(new_pitch_position == properties.halves_length)
            {
                setSpeed(0);
            }
            else
            {
                moveTo(-1 * properties.halves_length);
                setSpeed(-1 * PITCH_DEFAULT_STEPPER_SPEED);
            }
        }
        else if(state == CurrentState::DIVING_MODE)
        {
            if(new_pitch_position == 0)
            {
                setSpeed(0);
            }
            else if(new_buoyancy_position < 9000)
            {
                moveTo(properties.halves_length);
                setSpeed(PITCH_DEFAULT_STEPPER_SPEED);
            }
            else
            {
                setSpeed(0);
            }
        }
        else
        {
            setSpeed(0);
        }
    }


    void Pitch::logToStruct(LoggedData &data)
    {
        data.pitch_stepper.current_position = currentPosition();
        data.pitch_stepper.target_position = targetPosition();

        data.pitch_stepper.current_position_mm = currentPosition_mm();
        data.pitch_stepper.target_position_mm = targetPosition_mm();

        data.pitch_stepper.limit_state = limit.state();

        data.pitch_stepper.speed = speed();
        data.pitch_stepper.max_speed = maxSpeed();
        data.pitch_stepper.acceleration = acceleration();
    }

    /**
     * @brief Sets the buoyancy and pitch steppers to their default settings set in configuration.h
     * 
     * @param buoyancy buoyancy object
     * @param pitch   pitch object
     */
    void setDefaultSettings(Buoyancy &buoyancy, Pitch &pitch)
    {
        setDefaultSpeeds(buoyancy, pitch);

        buoyancy.setMinPulseWidth(MIN_PULSE_WIDTH);
        buoyancy.setResolution(Stepper::Resolution::HALF);

        pitch.setMinPulseWidth(MIN_PULSE_WIDTH);
        pitch.setResolution(Stepper::Resolution::HALF);
    }

    /**
     * @brief Sets the buoyancy and pitch steppers to their default speeds set in configuration.h
     * 
     * @param buoyancy buoyancy object
     * @param pitch   pitch object
     */
    void setDefaultSpeeds(Buoyancy &buoyancy, Pitch &pitch)
    {
        buoyancy.setSpeeds(BUOYANCY_DEFAULT_STEPPER_SPEED, BUOYANCY_DEFAULT_STEPPER_ACCELERATION);
        pitch.setSpeeds(PITCH_DEFAULT_STEPPER_SPEED, PITCH_DEFAULT_STEPPER_ACCELERATION);
    }

    bool calibrateBoth(Buoyancy &buoyancy, Pitch &pitch)
    {
        bool buoyancy_calibrated = buoyancy.calibrate();
        bool pitch_calibrated = pitch.calibrate();

        return buoyancy_calibrated && pitch_calibrated;
    }

}


