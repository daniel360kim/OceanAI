/*This software is Copyright (C) 2008 Mike McCauley. Use is subject to license
conditions. The main licensing options available are GPL V3 or Commercial:

Open Source Licensing GPL V3

This is the appropriate option if you want to share the source code of your
application with everyone you distribute it to, and you also want to give them
the right to share who uses it. If you wish to use this software under Open
Source Licensing, you must contribute all your source code to the open source
community in accordance with the GPL Version 3 when your application is
distributed. See http://www.gnu.org/copyleft/gpl.html

Commercial Licensing

This is the appropriate option if you are creating proprietary applications
and you are not prepared to distribute and share the source code of your
application. Contact info@open.com.au for details.

*/

#ifndef AccelStepper_h
#define AccelStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#undef round

#if (defined(ARDUINO) && ARDUINO >= 155) || defined(ESP8266)
#define YIELD yield();
#else
#define YIELD
#endif

class AccelStepper
{
public:
    typedef enum
    {
        FUNCTION = 0,
        DRIVER = 1,
        FULL2WIRE = 2,
        FULL3WIRE = 3,
        FULL4WIRE = 4,
        HALF3WIRE = 6,
        HALF4WIRE = 8
    } MotorInterfaceType;

    explicit AccelStepper(uint8_t interface = AccelStepper::FULL4WIRE, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 0, uint8_t pin4 = 0, bool enable = true);
    AccelStepper(void (*forward)(), void (*backward)());

    void moveTo(long absolute);
    void move(long relative);

    bool run();
    bool runSpeed();

    void setMaxSpeed(double speed);
    double maxSpeed();
    void setAcceleration(double acceleration);
    void setSpeed(double speed);
    double speed();

    double distanceToGo();
    double targetPosition();
    double currentPosition();
    double acceleration() const { return _acceleration; }
    double maxSpeed() const { return _maxSpeed; }
    double speed() const { return _speed; }

    void setCurrentPosition(long position);
    void runToPosition();
    bool runSpeedToPosition();
    void runToNewPosition(long position);

    void stop();

    virtual void disableOutputs();
    virtual void enableOutputs();

    void setMinPulseWidth(unsigned int minWidth);
    void setEnablePin(uint8_t enablePin = 0xff);
    void setPinsInverted(bool directionInvert = false, bool stepInvert = false, bool enableInvert = false);
    void setPinsInverted(bool pin1Invert, bool pin2Invert, bool pin3Invert, bool pin4Invert, bool enableInvert);
    bool isRunning();

    uint64_t step_iterations;

    void invertDirection();

protected:

    bool DIRECTION_CCW = 0;
    bool DIRECTION_CW = 1;


    void computeNewSpeed();

    virtual void setOutputPins(uint8_t mask);

    virtual void step(long step);
    virtual void step0(long step);
    virtual void step1(long step);
    virtual void step2(long step);
    virtual void step3(long step);
    virtual void step4(long step);
    virtual void step6(long step);
    virtual void step8(long step);

    bool _direction;
    int dir_multiplier = 1;

    int res_multiplier = 1;
    int pos_multiplier = 1;
    double _maxSpeed;
private:
    uint8_t _interface;
    uint8_t _pin[4];
    uint8_t _pinInverted[4];

    double _currentPos;
    double _targetPos;
    double _speed;


    double _acceleration;
    double _sqrt_twoa;

    uint64_t _stepInterval;
    uint64_t _lastStepTime;

    unsigned int _minPulseWidth;

    bool _enableInverted;
    uint8_t _enablePin;

    void (*_forward)();
    void (*_backward)();

    double _n;
    double _c0;
    double _cn;
    double _cmin;
};

#endif