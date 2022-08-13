
#include <Arduino.h>
#include <Wire.h>
#include <vector>

#include "pins.h"
#include "thermistor.h"
#include "data_struct.h"
#include "transducer.h"

Thermistor therm(THERM, 10000);
Transducer pres(PRES);

double dt;
unsigned long long previous_time;
unsigned long long previous_write;

constexpr uint8_t I2C_addr = 0x1B;
constexpr int write_speed = 500; //500 Hz

double pressure_sum = 0;
double temperature_sum = 0;

long iterations = 0;

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

    data.loop_time = 1.0 / dt;

    if(micros() - previous_write > write_speed)
    {
        data.pressure = pressure_sum / iterations;
        data.temperature = temperature_sum / iterations;

        pressure_sum = 0.0;
        temperature_sum = 0.0;
        iterations = 0;

        digitalWrite(INT, HIGH);
        digitalWrite(INT, LOW);
        Wire.beginTransmission(I2C_addr);
        Wire.write((uint8_t*)&data, sizeof(Data));
        Wire.endTransmission();
        previous_write = micros();
    }
    else
    {
        pressure_sum += pres.getPressure();
        temperature_sum += therm.getTemperature();

        iterations++;
    }
    
}