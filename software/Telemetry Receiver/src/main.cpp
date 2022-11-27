// Import the ElectricUI Library
#include <electricui.h>
#include <Arduino.h>

#include <cmath>

#include "data_struct.h"
#include "telemetry.h"

Data data = {0};
Telemetry telemetry;

uint32_t lastSend = 0;

void serial_write(uint8_t *data, uint16_t len);

float ori[3] = {0, 0, 0};
// Instantiate the communication interface's management object
eui_interface_t serial_comms = EUI_INTERFACE(&serial_write);

// Electric UI manages variables referenced in this array
eui_message_t tracked_variables[] =
	{
		EUI_UINT32("time_ms", data.time_ms),
		EUI_UINT16("loop_time", data.loop_time),
		EUI_UINT16("system_state", data.system_state),

		EUI_FLOAT("wfacc_x", data.wfacc.x),
		EUI_FLOAT("wfacc_y", data.wfacc.y),
		EUI_FLOAT("wfacc_z", data.wfacc.z),
		EUI_FLOAT("rgyr_x", data.rgyr.x),
		EUI_FLOAT("rgyr_y", data.rgyr.y),
		EUI_FLOAT("rgyr_z", data.rgyr.z),
		EUI_FLOAT("rel_ori_x", data.rel_ori.x),
		EUI_FLOAT("rel_ori_y", data.rel_ori.y),
		EUI_FLOAT("rel_ori_z", data.rel_ori.z),
		EUI_FLOAT("mag_x", data.fmag.x),
		EUI_FLOAT("mag_y", data.fmag.y),
		EUI_FLOAT("mag_z", data.fmag.z),
		EUI_FLOAT("rel_w", data.relative.w),
		EUI_FLOAT("rel_x", data.relative.x),
		EUI_FLOAT("rel_y", data.relative.y),
		EUI_FLOAT("rel_z", data.relative.z),

		EUI_FLOAT("ext_pres", data.filt_ext_pres),
		EUI_FLOAT("ext_temp", data.filt_ext_temp),

		EUI_FLOAT_ARRAY("ori", ori),
		
/*
		EUI_UINT8("limit_state", data.dive_stepper.limit_state),
		EUI_UINT8("homed", data.dive_stepper.homed),
		EUI_FLOAT("pos", data.dive_stepper.current_position),
		EUI_FLOAT("pos_mm", data.dive_stepper.current_position_mm),
		EUI_FLOAT("target_pos", data.dive_stepper.target_position),
		EUI_FLOAT("target_pos_mm", data.dive_stepper.target_position_mm),
		EUI_FLOAT("step_speed", data.dive_stepper.speed),
		EUI_FLOAT("step_accel", data.dive_stepper.acceleration),
		EUI_FLOAT("step_max_speed", data.dive_stepper.max_speed),
		*/
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

	telemetry.receive(data);

	ori[0] = constrainAngle_half(data.rel_ori.x) * DEG_TO_RAD;
	ori[1] = constrainAngle_half(data.rel_ori.y) * DEG_TO_RAD;
	ori[2] = constrainAngle_half(data.rel_ori.z) * DEG_TO_RAD;

	
}
