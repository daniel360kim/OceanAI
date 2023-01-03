#include "stepper.h"

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
 * Really need to figure out how to make this nonblocking :(
 * 
 */
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

/**
 * @brief Calibrates the stepper motor without checking if the limit switch is pressed a second time
 * 
 */
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

/**
 * @brief After limit switch is pressed once, it goes back and slowly eases back in
 * For further precision
 * 
 */
void Stepper::recheckLimit()
{
    setCurrentPosition(0);
    move(1000);
    while(currentPosition() != 1000)
    {
        run();
    }
    
    setResolution(Resolution::EIGHTH);


    move(-100000000);
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
    if(!calibrated)
    {
        calibrate();
    }

    moveTo(-1 * properties.halves_length); //go to the other side of the carriage

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


void Buoyancy::calibrate()
{
    setMaxSpeed(4000);
    setAcceleration(2000);
    setSpeed(4000);
    setResolution(Resolution::HALF);

    calibrate_noCheck();

    recheckLimit();

    calibrated = true;
}

void Buoyancy::calibrate_noCheck()
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

void Buoyancy::recheckLimit()
{
    setCurrentPosition(0);
    move(500);
    while(currentPosition() != 500)
    {
        run();
    }
    
    setResolution(Resolution::EIGHTH);
    move(-100000000);
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

