/**
 * @file OutputFuncs.cpp
 * @author Daniel Kim
 * @brief Buzzer and led sequences
 * @version 1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#include "LED.h"
#include "OutputFuncs.h"
#include "../pins.h"
#include "utility/pitches.h"

#include <Buzzer.h>
#include <Arduino.h>

OutputFuncs output;

RGBLED LEDa(RED_a, GRN_a, BLU_a);
RGBLED LEDb(RED_b, GRN_b, BLU_b);

Buzzer buzzer(BUZZ);

void OutputFuncs::indicateCompleteStartup()
{
    LEDa.setColor(0, 255, 0);
    LEDb.setColor(0, 255, 0);

    buzzer.sound(NOTE_D5, 150);

    LEDa.setColor(0, 0, 255);
    LEDb.setColor(0, 0, 255);

    buzzer.sound(NOTE_F5, 250);

    LEDa.LEDoff();
    LEDb.LEDoff();
}

void OutputFuncs::startupSequence()
{
    LEDa.setColor(255, 0, 0);
    LEDb.setColor(255, 0, 0);

    buzzer.sound(NOTE_C5, 600);

    LEDa.LEDoff();
    LEDb.LEDoff();
    delay(100);

    LEDa.setColor(255, 255, 0);
    LEDb.setColor(255, 255, 0);

    buzzer.sound(NOTE_G4, 350);
    delay(100);

    LEDa.setColor(0, 255, 255);
    LEDb.setColor(0, 255, 255);

    buzzer.sound(NOTE_G5, 700);

    LEDa.setColor(255, 255, 255);
    LEDb.setColor(255, 255, 255);
}

void OutputFuncs::loopIndication()
{
    LEDa.displaySpectrum();
}

void OutputFuncs::indicateError()
{
    while(1)
    {
        LEDa.blink(0, 0, 255, 100);
        LEDb.blink(0, 0, 255, 100);
        buzzer.sound (NOTE_C7, 100);

    }
}

void OutputFuncs::indicateWarning()
{
    LEDb.blink(255, 0, 0, 100);
}