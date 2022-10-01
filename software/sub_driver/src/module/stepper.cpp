#include "stepper.h"

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
void Stepper::setResolution(Resolution resolution)
{
    this->resolution = resolution;
    switch (resolution)
    {
    case HALF:
        digitalWrite(pins.MS1, LOW);
        digitalWrite(pins.MS2, HIGH);
        res_multiplier = 2;
        
        setMaxSpeed(maxSpeed() / res_multiplier);
        setSpeed(speed() / res_multiplier);
        
        break;
    case QUARTER:
        digitalWrite(pins.MS1, HIGH);
        digitalWrite(pins.MS2, LOW);
        res_multiplier = 4;

        setMaxSpeed(maxSpeed() / res_multiplier);
        setSpeed(speed() / res_multiplier);

        break;
    case EIGHTH:
        digitalWrite(pins.MS1, LOW);
        digitalWrite(pins.MS2, LOW);
        res_multiplier = 8;

        setMaxSpeed(maxSpeed() / res_multiplier);
        setSpeed(speed() / res_multiplier);

        break;
    case SIXTEENTH:
        digitalWrite(pins.MS1, HIGH);
        digitalWrite(pins.MS2, HIGH);
        res_multiplier = 16;

        setMaxSpeed(maxSpeed() / res_multiplier);
        setSpeed(speed() / res_multiplier);

        break;
    }
}

void Stepper::calibrate()
{
    setMaxSpeed(6000);
    setAcceleration(6000);
    setSpeed(6000);
    setResolution(Resolution::HALF);

    calibrate_noCheck();

    recheckLimit();

    calibrated = true;

}

void Stepper::calibrate_noCheck()
{
    if(limit.state() == true)
    {
        recheckLimit();
        return;
    }

    move(-100000000); //just move a lot lol
    while(limit.state() == false)
    {
        run();
    }
    
    setCurrentPosition(0);
}

double Stepper::currentPosition_mm()
{
    return currentPosition() / steps_per_mm;
}

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
    if(resolution == HALF)
    {
        move(absolute);
        return;
    }
    else if(resolution == QUARTER)
    {
        absolute /= 2;
        pos_multiplier = 2;
    }
    else if(resolution == EIGHTH)
    {
        absolute *= 4;
        pos_multiplier = 0.25;
    }
    else if(resolution == SIXTEENTH)
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

void Stepper::move_mm(int mm)
{
    goTo(mm * steps_per_mm);
}

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

void Stepper::recheckLimit()
{
    setCurrentPosition(0);
    move(500);
    while(currentPosition() != 500)
    {
        run();
    }
    
    setResolution(EIGHTH);


    move(-100000000);
    while(limit.state() == false)
    {
        run();

    }

    setCurrentPosition(0);
}

//END OF STEPPER

//BEGINNING OF BUOYANCY

/**
 * @brief pulls in water to sink. WILL NOT RUN; MUST CALL an updating function
 * 
 */
void Buoyancy::sink()
{
    if(!calibrated)
    {
        calibrate();
    }

    goTo(-1 * currentPosition()); //go to the other side of the carriage

    sinking = true;
    rising = false;

    //this calibrates the stepper so might as well get rid of errors and set postion to 0
    setCurrentPosition(0);
}

/**
 * @brief pushes all water out to float. WILL NOT RUN; MUST CALL an updating function
 * 
 */
void Buoyancy::rise()
{
    goTo(properties.halves_length - currentPosition()); //moves to the very end of the carriage

    sinking = false;
    rising = true;
}

/**
 * @brief moves syringe back and forth to go up and down, thus going forward
 * Must be called in main loop and updated consistently
 * 
 */
void Buoyancy::forward()
{
    if(sinking && currentPosition() == targetPosition())
    {
        rise();
    }
    else if(rising && currentPosition() == targetPosition())
    {
        sink();
    }

    update();
}

void Buoyancy::logToStruct(Data &data)
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