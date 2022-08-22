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

    recheckLimit();

}

double Stepper::currentPosition_mm()
{
    return currentPosition() / steps_per_mm;
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
    Serial.println("GOTO: " + String(absolute));
    move(absolute);
}

void Stepper::move_mm(int mm)
{
    goTo(currentPosition() + (mm * steps_per_mm));
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
    move(1000);
    while(currentPosition() != 1000)
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