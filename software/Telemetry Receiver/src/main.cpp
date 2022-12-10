// Import the ElectricUI Library
#include <electricui.h>
#include <Arduino.h>

#include <cmath>

#include "data_struct.h"
#include "telemetry.h"
Telemetry::Telemetry telemetry;

Telemetry::TransmissionData send_data;

uint32_t lastSend = 0;

void serial_write(uint8_t *data, uint16_t len);

float ori[3] = {0, 0, 0};
// Instantiate the communication interface's management object
eui_interface_t serial_comms = EUI_INTERFACE(&serial_write);

// Electric UI manages variables referenced in this array
eui_message_t tracked_variables[] =
{
	EUI_UINT16("loop_time", send_data.loop_time),
	EUI_UINT8("system_state", send_data.system_state),
	EUI_FLOAT("delta_time", send_data.delta_time),
	EUI_FLOAT("internal_temp", send_data.internal_temp),

	EUI_FLOAT("wfacc_x", send_data.wfacc.x), 
	EUI_FLOAT("wfacc_y", send_data.wfacc.y),
	EUI_FLOAT("wfacc_z", send_data.wfacc.z),
	EUI_FLOAT("rgyr_x", send_data.rgyr.x),
	EUI_FLOAT("rgyr_y", send_data.rgyr.y),
	EUI_FLOAT("rgyr_z", send_data.rgyr.z),
	EUI_FLOAT("mag_x", send_data.mag.x),
	EUI_FLOAT("mag_y", send_data.mag.y),
	EUI_FLOAT("mag_z", send_data.mag.z),
	EUI_FLOAT("rel_x", send_data.rel_ori.x),
	EUI_FLOAT("rel_y", send_data.rel_ori.y),
	EUI_FLOAT("rel_z", send_data.rel_ori.z),	
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
	while(!Serial);
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

	telemetry.receive(send_data);
	
}
