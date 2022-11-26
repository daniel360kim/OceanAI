#include <Arduino.h>

#include "Telemetry.h"

Telemetry telemetry;
Data data;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  telemetry.init(2000000);
}

void loop() {
  // put your main code here, to run repeatedly:
  telemetry.receive(data);

}