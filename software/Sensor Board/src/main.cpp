
#include <Arduino.h>
#include <Wire.h>

#include "pins.h"
#include "thermistor.h"
#include "data_struct.h"
#include "transducer.h"

Thermistor therm(THERM, 10000);
Transducer pres(PRES);

double dt;
unsigned long long previous_time;

constexpr uint8_t I2C_addr = 0x1B;

void setup()
{
    Wire.begin();
    previous_time = micros();
}

void loop()
{
    Data data;

    data.time = micros();
    dt = (data.time - previous_time) / 1000000.0;
    previous_time = data.time;

    data.loop_time = dt;

    data.pressure = pres.getPressure();
    data.temperature = therm.getTemperature();

    Wire.beginTransmission(I2C_addr);
    Wire.write((uint8_t *)&data, sizeof(data));
    Wire.endTransmission();
}