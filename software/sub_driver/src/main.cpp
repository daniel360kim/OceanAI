#include <Arduino.h>
#include "pins.h"


void setup() {
  // Sets the two pins as Outputs
  pinMode(STP_b,OUTPUT); 
  pinMode(DIR_b,OUTPUT);
}
void loop() {
  digitalWrite(DIR_b,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 200; x++) {
    digitalWrite(STP_b,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(STP_b,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000); // One second delay
  
  digitalWrite(DIR_b,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 400; x++) {
    digitalWrite(STP_b,HIGH);
    delayMicroseconds(500);
    digitalWrite(STP_b,LOW);
    delayMicroseconds(500);
  }
  delay(1000);
}
