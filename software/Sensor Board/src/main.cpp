
#include <Arduino.h>
#include <Wire.h>

#include "pins.h"
#include "thermistor.h"
#include "data_struct.h"
#include "transducer.h"
#include "transmission.h"

ExternalSensor ext_sensor(9);
Thermistor therm(THERM, 10000);
Transducer pres(PRES);

double dt;
unsigned long long previous_time;
unsigned long long previous_send;

constexpr int us_interval = 2000;

float pressure_sum = 0;
float temperature_sum = 0;
float loop_sum = 0;

long iterations = 0;

RawData raw_data;

void setup()
{
	ext_sensor.initialize();
}

void loop()
{
	// Getting sensor data
	Data data;

	data.time = micros();
	dt = (data.time - previous_time) / 1000000.0;
	previous_time = data.time;

	data.loop_time = dt;
	data.loop_time = 1.0 / dt;

	if (micros() - previous_send > us_interval)
	{
		raw_data.data[0] = loop_sum / iterations;
		raw_data.data[1] = pressure_sum / iterations;
		raw_data.data[2] = temperature_sum / iterations;

		loop_sum = 0.0;
		pressure_sum = 0.0;
		temperature_sum = 0.0;
		iterations = 0;

		previous_send = micros();
		digitalWrite(INT, HIGH);
		digitalWrite(INT, LOW);
		ext_sensor.sendData(raw_data);
	}
	else
	{
		loop_sum += data.loop_time;
		pressure_sum += pres.getPressure();
		temperature_sum += therm.getTemperature();
		iterations++;
	}
	// Send drdy signal

	// ext_sensor.writeRegisters(ExternalSensor::SubAddress::DATA, raw_data);
}