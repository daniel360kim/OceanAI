
#ifndef stepper_h
#define stepper_h

#include <Arduino.h>
#include <cstdint>

#include "AccelStepper.h"
#include "limit.h"
#include "data/logged_data.h"
#include "../core/pins.h"

/**
 * @brief Properties of our stepper motor configuration
 * Carriage length describes the range the stepper mover can move an object in mm
 * Halves length is how many half-resolution steps are in a full carriage length
 * Halves length can be measureed with a different test program
 * This struct is passed into the constructor of the stepper class
 */
struct StepperProperties
{
    StepperProperties() {}
    StepperProperties(double carriage_length, long halves_length) : carriage_length(carriage_length), halves_length(halves_length) {}
    double carriage_length; //how long is the carriage in mm
    long halves_length; //how many half steps are in the carriage
};

/**
 * @brief Child class of AccelStepper
 * Includes code to change the resolution of the stepper 
 * and to calibrate the motor w/ limit switches
 */
class Stepper : public AccelStepper
{
public:
    enum class Resolution
    {
        HALF,
        QUARTER,
        EIGHTH,
        SIXTEENTH
    };

    Stepper(StepperPins pins, Resolution resolution, StepperProperties properties);

    void setResolution(Resolution resolution);
    void calibrate();
    void calibrate_noCheck(); //calibrate without checking if the limit switch is pressed a second time

    double currentPosition_mm();
    double targetPosition_mm();

    void goTo(long absolute);
    void move_mm(int mm);

    bool update();

    StepperProperties properties;

    Limit limit;
protected:
    Resolution resolution;
    StepperPins pins;

    double steps_per_mm;
    bool calibrated = true; //change to false when calibration impl.

    void recheckLimit();
};

//Singleton class for buoyancy driver
//TODO: make this less horrible
class Buoyancy : public Stepper
{
public:
    explicit Buoyancy(StepperPins pins, Resolution resolution, StepperProperties properties) : Stepper(pins, resolution, properties) {}
    
    void sink();
    void rise();

    void forward();

    void logToStruct(LoggedData &data);

    bool sinking = false;
    bool rising = false;

    void calibrate();
    void calibrate_noCheck(); //calibrate without checking if the limit switch is pressed a second time


private:
    void recheckLimit();
    
};





#endif