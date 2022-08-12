#include "module/A4988.h"
#include "pins.h"
#include <cmath>
#include <Arduino.h>
A4988 motor(STP_b, DIR_b, MS1_b, MS2_b, MS3_b, SLP_b, RST_b, STOP_b);
void setup()
{
  
  motor.begin();
  while(1)
  {
    if(motor.calibrate())
    {
      break;
    }
  }
 

}

void loop()
{


}