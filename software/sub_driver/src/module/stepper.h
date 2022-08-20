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

#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <stdint.h>

#include "limit.h"

struct StepperPins
{
    StepperPins();
    uint8_t step;
    uint8_t dir;
    uint8_t ms1;
    uint8_t ms2;
    uint8_t ms3;
    uint8_t slp;
    uint8_t rst;
};

struct StepperProperties
{
    int THREAD_ROD_LENGTH; //in mm: length of the threaded rod
    int DISPLACEMENT_PER_CIRCLE; //in mm: how much the nut moves every circle of the thread rod
    int END_STOP_LENGTH; //in mm: how far away from the end of the threaded rod the end stop is. 0 means that the nut should go to the very end of the rod
    int STEP_DELAY; //how between steps
};

class Stepper
{
public:
    enum Resolution
    {
        Full = 0,
        Half = 1,
        Quarter = 2,
        Eighth = 3,
        Sixteenth = 4
    };

    enum Direction
    {
        Clockwise,
        CounterClockwise
    };

    enum State
    {
        Asleep,
        Idle,
        Accelerating,
        Running,
        Braking
    };

    enum Mode
    {
        Constant,
        Linear
    };

    Stepper(StepperPins pins, StepperProperties properties, Resolution resolution = Full, Direction direction = Clockwise, Mode mode = Linear);
    void attachLimit(const uint8_t limit_pin);

    void rotateSteps(int steps, Direction direction, void(*onFinish)() = nullptr);
    void rotateAngle(double angle, Direction direction, void(*onFinish)() = nullptr);
    void rotateToAngle(double angle, Direction direction, void(*onFinish)() = nullptr);
    void rotateInTime(int steps, double time, Direction direction, void(*onFinish)() = nullptr);
    void rotateAngleInTime(double angle, double time, Direction direction, void(*onFinish)() = nullptr);
    void rotateToAngleInTime(double angle, double time, Direction direction, void(*onFinish)() = nullptr);
    void rotateContinuous(Direction direction);
    void stop() { state = Idle; }
    void brake();
    
    uint16_t update();

    void setResolution(const Resolution resolution);
    void setSpeed(const int speed);
    void setSpeedRPM(double rpm);
    void setAcceleration(const int acceleration);
    void setAcceleration(const int acceleration, const int deacceration);
    
    int getRemainingSteps() const;
    unsigned long long getUpTime() const;
    double getAngle() const;
    int getCurrentInterval() const { return interval;}
    int getCurrentTravelStep() const { return CurrentTravelStep; }
    int getAbsoluteStep() const { return AbsoluteStep; }
    int getMaxSpeed() const { return speed; }
    int getCurrentSpeed() const;
    int getSpeedRPM() const;

    double speedNeeded(const int steps, double time);
    double timeNeeded(const int steps, int speed);

    State state = State::Idle;
    Direction direction;
    Mode mode;

private:
    StepperPins pins;
    StepperProperties properties;

    Resolution resolution;
    
    Limit l_switch;

    uint8_t limit_pin;

    void (*onFinish)() = nullptr;

    int speed;
    int acceleration;
    int deacceleration;
    bool continuousMove;

    int MotorSteps;
    int TravelSteps;
    int CurrentTravelStep;
    int AbsoluteStep;
    int accSteps;
    int decSteps;

    unsigned long long startTime;
    unsigned long long lastStepTime;
    int interval;
    int runInterval;
    int restInterval;

    const int pulse_off_width = 250;

    void initRotation(int speed);
    static int getRampSteps(int speed, int acceleration);

    void updateState();
    void updateInterval();

    void step();
    void stepClockwise();
    void stepCounterClockwise();
 
};







#endif