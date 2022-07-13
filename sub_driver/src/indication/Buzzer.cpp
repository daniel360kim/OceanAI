/**
 * @file Buzzer.cpp
 * @author Daniel Kim
 * @brief Buzzer driver
 * @version 1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <Arduino.h>

#include "Buzzer.h"

void Piezo::startupSounds()
{
	buzzer->sound(NOTE_C5, 600);
	delay(100);
	buzzer->sound(NOTE_G4, 350);
	delay(100);
	buzzer->sound(NOTE_G5, 700);
}

void Piezo::indicateCompeteStartup()
{
	buzzer->sound(NOTE_D5, 150);
	buzzer->sound(NOTE_F5, 250);
}