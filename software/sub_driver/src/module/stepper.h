
#ifndef stepper_h
#define stepper_h

#include <Arduino.h>
#include <stdint.h>

#include "AccelStepper.h"
#include "limit.h"

struct StepperPins
{
    uint8_t STP;
    uint8_t DIR;
    uint8_t MS1;
    uint8_t MS2;
    uint8_t ERR;
    uint8_t limit;
};

struct StepperProperties
{
    double carriage_length; //how long is the carriage in mm
    long halves_length; //how many half steps are in the carriage
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

    double currentPosition_mm();

    void goTo(long absolute);
    void move_mm(int mm);

    bool update();

    

private:
    Limit limit;
    Resolution resolution;
    StepperProperties properties;
    StepperPins pins;

    double steps_per_mm;

    void recheckLimit();
    
    
};





#endif