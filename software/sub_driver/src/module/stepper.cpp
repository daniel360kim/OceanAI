/**
 * @file stepper.h
 * @author Daniel Kim       
 * @brief A4988 stepper motor driver 
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022 Daniel Kim https://github.com/daniel360kim/OceanAI
 * 
 */

#include <Arduino.h>
#include <stdint.h>
#include <cmath>
#include <cstdlib>

#include "stepper.h"

Stepper::Stepper(StepperPins pins, StepperProperties properties, Resolution resolution, Direction direction = Clockwise, Mode mode = Linear)
{
    this->pins = pins;
    this->properties = properties;
    setResolution(resolution);

    this->direction = direction;
    this->mode = mode;

    pinMode(this->pins.step, OUTPUT);
    pinMode(this->pins.dir, OUTPUT);
    pinMode(this->pins.ms1, OUTPUT);
    pinMode(this->pins.ms2, OUTPUT);
    pinMode(this->pins.ms3, OUTPUT);
    pinMode(this->pins.slp, OUTPUT);
    pinMode(this->pins.rst, OUTPUT);

    digitalWrite(pins.slp, HIGH); //keep awake

    onFinish = nullptr;   
}

void Stepper::attachLimit(uint8_t limit_pin)
{
    l_switch.begin(limit_pin);
}

void Stepper::rotateSteps(int steps, Direction direction, void(*onFinish)() = nullptr)
{
    this->direction = direction;
    this->onFinish = onFinish;

    continuousMove = false;

    TravelSteps = std::labs(steps);

    initRotation(speed);
}

void Stepper::rotateAngle(double angle, Direction direction, void(*onFinish)() = nullptr)
{
    rotateSteps(angle / 360.0 * MotorSteps, direction, onFinish);
}

void Stepper::rotateToAngle(double angle, Direction direction, void(*onFinish)() = nullptr)
{
    auto currentAngle = getAngle();
    double angleDelta = direction == Clockwise ? currentAngle - angle : angle - currentAngle;
    if (angleDelta < 0) angleDelta = -angleDelta;
    rotateAngle(angleDelta, direction, onFinish);
}

void Stepper::rotateInTime(int steps, double time, Direction direction, void(*onFinish)() = nullptr)
{
    int requiredTime = timeNeeded(steps, this->speed);

    double speed = requiredTime > time ? this->speed : speedNeeded(steps, time);

    this->direction = direction;
    this->onFinish = onFinish;

    continuousMove = false;
    TravelSteps = labs(steps);

    initRotation(speed);
}

void Stepper::rotateAngleInTime(double angle, double time, Direction direction, void(*onFinish)() = nullptr)
{
    rotateInTime(angle / 360.0f * 200, time, direction, onFinish);
}

void Stepper::rotateToAngleInTime(double angle, double time, Direction direction, void(*onFinish)() = nullptr)
{
    auto currentAngle = getAngle();
    double angleDelta = direction == Clockwise ? currentAngle - angle : angle - currentAngle;
    if (angleDelta < 0) angleDelta = -angleDelta;
    rotateAngleInTime(angleDelta, time, direction, onFinish);
}

void Stepper::rotateContinuous(Direction direction)
{
    this->direction = direction;
    onFinish = nullptr;

    continuousMove = true;
    TravelSteps = 0;

    initRotation(speed);
}

void Stepper::brake()
{
    decSteps = getRampSteps(getCurrentSpeed(), deacceleration);
    TravelSteps = decSteps;
    continuousMove = false;
    CurrentTravelStep = 1;
    state = Braking;
}

uint16_t Stepper::update()
{
    if(state == Idle)
        return 0;
    
    uint16_t stepsDone = 0;
    while(static_cast<unsigned long long>(micros() - lastStepTime) >= interval)
    {
        lastStepTime += interval;
        step();
        stepsDone++;
    }
    return stepsDone;
}
void Stepper::setResolution(Resolution resolution)
{
    this->resolution = resolution;
    switch (resolution)
    {
    case Full:
        digitalWrite(pins.ms1, LOW);
        digitalWrite(pins.ms2, LOW);
        digitalWrite(pins.ms3, LOW);
        MotorSteps = 200;
        break;
    case Half:
        digitalWrite(pins.ms1, HIGH);
        digitalWrite(pins.ms2, LOW);
        digitalWrite(pins.ms3, LOW);
        MotorSteps = 400;
        break;
    case Quarter:
        digitalWrite(pins.ms1, LOW);
        digitalWrite(pins.ms2, HIGH);
        digitalWrite(pins.ms3, LOW);
        MotorSteps = 800;
        break;
    case Eighth:
        digitalWrite(pins.ms1, HIGH);
        digitalWrite(pins.ms2, HIGH);
        digitalWrite(pins.ms3, LOW);
        MotorSteps = 1600;
        break;
    case Sixteenth:
        digitalWrite(pins.ms1, HIGH);
        digitalWrite(pins.ms2, HIGH);
        digitalWrite(pins.ms3, HIGH);
        MotorSteps = 3200;
        break;
    default:
        digitalWrite(pins.ms1, LOW);
        digitalWrite(pins.ms2, LOW);
        digitalWrite(pins.ms3, LOW);
        MotorSteps = 200;
        break;
    }
}

void Stepper::setSpeed(const int speed)
{
    this->speed = speed;
}

void Stepper::setSpeedRPM(double rpm)
{
    setSpeed(rpm / 60.0 * MotorSteps);
}

void Stepper::setAcceleration(const int acceleration)
{
    this->acceleration = acceleration;
    deacceleration = acceleration;
}

void Stepper::setAcceleration(const int acceleration, const int deacceration)
{
    this->acceleration = acceleration;
    this->deacceleration = deacceleration;
}

int Stepper::getRemainingSteps() const
{
    return TravelSteps - CurrentTravelStep;
}

unsigned long long Stepper::getUpTime() const
{
    return micros() - startTime;
}

double Stepper::getAngle() const
{
    return 360 * (AbsoluteStep % MotorSteps) / MotorSteps;
}

int Stepper::getCurrentSpeed() const
{
    if(state == State::Idle)
        return 0;
    else
        return 1e+6 / interval;
}

int Stepper::getSpeedRPM() const
{
    return getCurrentSpeed() * 60.0 / MotorSteps;
}


double Stepper::speedNeeded(const int steps, double time)
{
    double speed = 0.0;

    if(mode == Linear)
    {
        double a_2  = 1.0 / acceleration + 1.0 / deacceleration;
        double discriminant = time * time - 2.0 * a_2 * steps;
        if(discriminant >= 0)
        {
            speed = (time - std::sqrt(discriminant)) / a_2;
        }
    }
    else
    {
        speed = steps / time;
    }

    return speed;
}

double Stepper::timeNeeded(const int steps, int speed)
{
    double time = 0.0;

    if(steps == 0)
        return 0.0;
    else
    {
        if(mode == Linear)
        {
            double accSteps = (double)getRampSteps(this->speed, acceleration);
            double decSteps = (double)getRampSteps(this->speed, deacceleration);

            double ramps = accSteps + decSteps;
            double runSteps = 0.0;

            if(steps > ramps)
            {
                accSteps = steps * deacceleration / (acceleration + deacceleration);
                decSteps = steps - accSteps;
            }
            else
            {
                runSteps = steps - ramps;
            }
            time = (runSteps / speed) + std::sqrt(2.0 * accSteps / acceleration) + std::sqrt(2.0 * decSteps / deacceleration);
        }
        else
        {
            time = steps / speed;
        }
    }
    return time;
}

void Stepper::initRotation(int speed)
{
    state = mode == Mode::Constant ? State::Running : State::Accelerating;
    
    accSteps = getRampSteps(speed, acceleration);
    interval = (double)1e+6 * 0.956 * std::sqrt(1.0 / acceleration);
    runInterval = (double)1e+6 / speed;
    restInterval = 0;

    CurrentTravelStep = 0;
    startTime = micros();
    lastStepTime = startTime;
}

int Stepper::getRampSteps(int speed, int acceleration)
{
    return (speed * speed) / (2.0 * acceleration);
}

void Stepper::updateState()
{
    if (state == Idle) return;

    const auto remaining = getRemainingSteps();
    if (continuousMove == false && remaining <= 0)
    {
        state = Idle;
        if (onFinish != nullptr) onFinish();
    }
    else
    {
        if (mode == Linear)
        {
            if (state == Braking) return;

            decSteps = getRampSteps(getCurrentSpeed(), deacceleration);
            if (continuousMove == false && remaining <= decSteps)
            {
                state = Braking;
            }
            else if (CurrentTravelStep <= accSteps)
            {
                state = Accelerating;
            }
            else
            {
                state = Running;
            }
        }
    }
}

void Stepper::updateInterval()
{
    if (state == Accelerating && CurrentTravelStep > 0)
    {
        interval = interval - (2 * interval + restInterval) / (4 * CurrentTravelStep + 1);
        restInterval = (CurrentTravelStep < accSteps) ? (2 * interval + restInterval) % (4 * CurrentTravelStep + 1) : 0;
    }

    else if (state == Braking && CurrentTravelStep > 0)
    {
        const long remain = getRemainingSteps();
        interval = interval - (2 * interval + restInterval) / (-4 * remain + 1);
        restInterval = (2 * interval + restInterval) % (-4 * remain + 1);
    }

    else if (state == Running)
    {
        interval = runInterval;
    }

    if (interval < runInterval)
    {
        interval = runInterval;
    }
}

void Stepper::step()
{
    if(direction == Clockwise)
        stepClockwise();
    else
        stepCounterClockwise();
    
    CurrentTravelStep++;

    
}

void Stepper::stepClockwise()
{
    digitalWrite(pins.dir, HIGH);
    digitalWrite(pins.step, HIGH);
    digitalWrite(pins.step, LOW);

    AbsoluteStep++;
}

void Stepper::stepCounterClockwise()
{
    digitalWrite(pins.dir, LOW);
    digitalWrite(pins.step, HIGH);
    digitalWrite(pins.step, LOW);

    AbsoluteStep--;
}