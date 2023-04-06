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
#include "LEDStrip.h"
#include "OutputFuncs.h"
#include "../core/pins.h"
#include "utility/pitches.h"

#include <Buzzer.h>
#include <Arduino.h>

OutputFuncs output;

RGBLED LEDa(RED_a, GRN_a, BLU_a);
RGBLED LEDb(RED_b, GRN_b, BLU_b);
LEDStrip <RX_GPS, 15> strip;


Buzzer buzzer(BUZZ);

void OutputFuncs::indicateCompleteStartup()
{
    LEDa.setColor(0, 255, 0);
    LEDb.setColor(0, 255, 0);
    strip.setColor(0, 255, 0);

    buzzer.sound(NOTE_D5, 150);

    LEDa.setColor(0, 0, 255);
    LEDb.setColor(0, 0, 255);
    strip.setColor(0, 0, 255);

    buzzer.sound(NOTE_F5, 250);

    LEDa.LEDoff();
    LEDb.LEDoff();
    strip.setColor(0, 0, 0);
}

void OutputFuncs::startupSequence()
{
    LEDa.setColor(255, 0, 0);
    LEDb.setColor(255, 0, 0);
    strip.setColor(255, 0, 0);

    buzzer.sound(NOTE_C5, 350);

    LEDa.setColor(0, 255, 0);
    LEDb.setColor(0, 255, 0);
    strip.setColor(0, 255, 0);

    buzzer.sound(NOTE_G4, 300);

    LEDa.setColor(0, 0, 255);
    LEDb.setColor(0, 0, 255);
    strip.setColor(0, 0, 255);

    buzzer.sound(NOTE_G5, 500);

    LEDa.setColor(255, 255, 255);
    LEDb.setColor(255, 255, 255);
    strip.setColor(255, 255, 255);
}

void OutputFuncs::loopIndication()
{
    LEDa.displaySpectrum();
}

void OutputFuncs::indicateError()
{

    for (int i = 0; i < 3; i++)
    {
        buzzer.sound(NOTE_E6, 10);
        LEDa.setColor(255, 0, 0);
        LEDb.setColor(255, 0, 0);
        strip.setColor(255, 0, 0);
        delay(100);
        LEDa.LEDoff();
        LEDb.LEDoff();
        strip.setColor(0, 0, 0);
    }
    delay(300);
}

void OutputFuncs::indicateWarning()
{
    LEDb.blink(255, 0, 0, 100);
}