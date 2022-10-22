
#ifndef stepper_h
#define stepper_h

#include <Arduino.h>
#include <stdint.h>

#include "AccelStepper.h"
#include "limit.h"
#include "data/data_struct.h"

struct StepperProperties
{
    StepperProperties() {}
    StepperProperties(double carriage_length, long halves_length) : carriage_length(carriage_length), halves_length(halves_length) {}
    double carriage_length; //how long is the carriage in mm
    long halves_length; //how many half steps are in the carriage
};

struct StepperPins
{
    uint8_t STP;
    uint8_t DIR;
    uint8_t MS1;
    uint8_t MS2;
    uint8_t ERR;
    uint8_t limit;
};

class Stepper : public AccelStepper
{
public:
    enum Resolution
    {
        HALF,
        QUARTER,
        EIGHTH,
        SIXTEENTH
    };

    explicit Stepper(StepperPins pins, Resolution resolution, StepperProperties properties);

    void setResolution(Resolution resolution);
    void calibrate();
    void calibrate_noCheck(); //calibrate without checking if the limit switch is pressed a second time

    double currentPosition_mm();
    double targetPosition_mm();

    void goTo(long absolute);
    void move_mm(int mm);

    bool update();

    StepperProperties properties;


protected:
    Limit limit;
    Resolution resolution;
    StepperPins pins;

    double steps_per_mm;
    bool calibrated = true; //change to false when calibration impl.

    void recheckLimit();
};

//Singleton class for buoyancy driver
class Buoyancy : public Stepper
{
public:
    explicit Buoyancy(StepperPins pins, Resolution resolution, StepperProperties properties) : Stepper(pins, resolution, properties) {}
    
    void sink();
    void rise();

    void forward();

    void logToStruct(Data &data);

    bool sinking = false;
    bool rising = false;

    void calibrate();
    void calibrate_noCheck(); //calibrate without checking if the limit switch is pressed a second time


private:
    void recheckLimit();
    
};





#endif