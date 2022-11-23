
#include "StateAutomation.h"
#include <Arduino.h>

void setup() 
{
	StateAutomation submarine;
	while(1)
	{
		submarine.run();
	}
}

void loop() {}

