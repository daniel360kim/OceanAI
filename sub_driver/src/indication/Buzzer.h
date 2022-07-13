/**
 * @file Buzzer.h
 * @author Daniel Kim
 * @brief Buzzer driver
 * @version 1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef BUZZER_h
#define BUZZER_h

#include <Arduino.h>
#include <Buzzer.h>

#include "utility/pitches.h"

class Piezo
{
public:
	Piezo(const uint8_t pin) { buzzer = new Buzzer(pin); }
	void startupSounds();
	void indicateCompeteStartup();
	
private:
	Buzzer *buzzer;
};

#endif