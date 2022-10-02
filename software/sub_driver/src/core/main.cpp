#include "States.h"
#include "StateAutomation.h"

#include "timed_function.h"

StateAutomation submarine;

int print(void*)
{
    Serial.println("Hello");
    return 5;
}

Time::Async<int, void*> timed_function(1000000000, print);

void setup() 
{
    //submarine.initialize();
}

void loop()
{
    int i = timed_function.tick(nullptr);
}