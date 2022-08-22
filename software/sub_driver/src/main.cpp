
#include <Arduino.h>

#include "pins.h"
#include "module/stepper.h"

StepperPins pins_p {
	STP_p,
	DIR_p,
	MS1_p,
	MS2_p,
	ERR_p,
	STOP_p
};

StepperPins pins_b {
	STP_b,
	DIR_b,
	MS1_b,
	MS2_b,
	ERR_b,
	STOP_b
};

StepperProperties properties {
	81.0,
	52670
};


unsigned long previousPrintmicros;
Stepper stepper(pins_p, Stepper::Resolution::HALF, properties);

long target = 0;


void setup() {
	Serial.begin(2000000);
	stepper.calibrate();
	
	stepper.setMaxSpeed(6000);
	stepper.setSpeed(6000);
	stepper.setAcceleration(6000);
	stepper.setResolution(Stepper::Resolution::HALF);
	Serial.println(F("Calibrated"));
}

void loop() {
	if(Serial.available() > 0 && (stepper.currentPosition_mm() >= target - 1 && stepper.currentPosition_mm() <= target + 1)) {
		Serial.println("Target reached");
		long move = Serial.parseInt();
		target += move;
		stepper.move_mm(move);
		Serial.print("Current position: "); Serial.println(stepper.currentPosition_mm());
	}
	// Move the motor one step
	if(micros() - previousPrintmicros > 1000000) {
		previousPrintmicros = micros();
		Serial.print("Current position: "); Serial.println(stepper.currentPosition_mm());
	}
	stepper.update();
}