
#include "StateAutomation.h"

#include <Arduino.h>
StateAutomation submarine;

void setup()
{
  submarine.initialize();
}

void loop()
{
  submarine.run();
}
