
#include <Arduino.h>

#include "pins.h"
#include "thermistor.h"
#include "data_struct.h"

Thermistor therm(THERM, 10000);

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    float temperature = therm.getTemperature();
    Serial.print(F("Temperature: ")); Serial.println(temperature);
}