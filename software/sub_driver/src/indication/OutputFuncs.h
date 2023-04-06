/**
 * @file OutputFuncs.h
 * @author Daniel Kim
 * @brief Buzzer and led sequences
 * @version 1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef OutputFuncs_h
#define OutputFuncs_h

#include "LED.h"
#include "LEDStrip.h"
#include "../core/pins.h"

#include <Buzzer.h>

extern RGBLED LEDa;
extern RGBLED LEDb;
extern LEDStrip<RX_GPS, 15> strip;

extern Buzzer buzzer;

class OutputFuncs
{
public:
    OutputFuncs(){}
    void indicateCompleteStartup();
    void startupSequence();
    void indicateError();
    void loopIndication();
    void indicateWarning();
};

extern OutputFuncs output;


#endif