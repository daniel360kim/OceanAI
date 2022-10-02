#include "States.h"
#include "StateAutomation.h"

#include "timed_function.h"

StateAutomation submarine;


void setup() 
{
    submarine.initialize();
}

void loop()
{
    submarine.run();
}