#include <Arduino.h>

void setup() {
  Serial1.begin(2000000);
}

void loop() {
  Serial.println(Serial1.read());
}