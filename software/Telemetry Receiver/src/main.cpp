// Import the ElectricUI Library
#include <electricui.h>
#include <Arduino.h>

#include <cmath>

#include "data_struct.h"
#include "telemetry.h"
Telemetry::Telemetry telemetry;

Telemetry::FastData fast_data ;
Telemetry::SlowData slow_data;

uint32_t lastSend = 0;

void serial_write(uint8_t *data, uint16_t len);

float ori[3] = {0, 0, 0};
// Instantiate the communication interface's management object
eui_interface_t serial_comms = EUI_INTERFACE(&serial_write);

// Electric UI manages variables referenced in this array
eui_message_t tracked_variables[] =
	{
		
};

void serial_rx_handler()
{
	// While we have data, we will pass those bytes to the ElectricUI parser
	while (Serial.available() > 0)
	{
		eui_parse(Serial.read(), &serial_comms); // Ingest a byte
	}
}

void serial_write(uint8_t *data, uint16_t len)
{
	Serial.write(data, len); // output on the main serial port
}

void setup()
{
	// Setup the serial port and status LED
	Serial.begin(115200);
	telemetry.init(2000000);

	// Provide the library with the interface we just setup
	eui_setup_interface(&serial_comms);

	// Provide the tracked variables to the library
	EUI_TRACK(tracked_variables);

	// Provide a identifier to make this board easy to find in the UI
	eui_setup_identifier("hello", 5);
}

double constrainAngle_half(double x)
{
    x = std::fmod(x + 180, 360);
    if(x < 0)
    {
        x += 360;
    }

    return x - 180;

}


void loop()
{

	serial_rx_handler(); // check for new inbound data

	telemetry.receive(fast_data, slow_data);



	
}
