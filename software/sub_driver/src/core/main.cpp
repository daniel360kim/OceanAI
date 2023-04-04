
#include "StateAutomation.h"
#include <Arduino.h>

void setup() 
{
	StateAutomation submarine; // create a submarine object

	//We ignore the loop() of the Arduino framework to avoid globals
	while(1) 
	{
		submarine.run();
	}
}
 
void loop() {}