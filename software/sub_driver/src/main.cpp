#include <Arduino.h>
#include "pins.h"


void cs_interrupt()
{
    Serial.println("Interrupt");
}

void setup()
{
    pinMode(CS, INPUT);
    attachInterrupt(digitalPinToInterrupt(CS), cs_interrupt, RISING);
}

void loop()
{
    Serial.println(digitalRead(CS));
    delay(1);
}