#include <Arduino.h>
#include <Wire.h>

#include "data_struct.h"

CombinedData datacopy;

unsigned long previous_time;

void receiveEvent()
{
    Serial.println("Int!");
}
void setup()
{
    Wire.begin(0x1B);
    pinMode(9, INPUT);
    attachInterrupt(digitalPinToInterrupt(9), receiveEvent, RISING);
    previous_time = micros();
    
}


void loop()
{
    unsigned long current_time = micros();
    double dt = (current_time - previous_time) / 1000000.0;
    previous_time = current_time;

    while(Wire.available())
    {
        Wire.readBytes((uint8_t *)&datacopy, sizeof(datacopy));
         
        
    }

   if(digitalRead(9))
   {
    Serial.println("true");
   }
   else
   {
    Serial.println("false");
   }
    

}