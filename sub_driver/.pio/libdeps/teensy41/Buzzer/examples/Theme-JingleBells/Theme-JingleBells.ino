#include <Buzzer.h>

Buzzer buzzer(11, 13);

void setup() {}

void loop() {
  int time = 500;

  buzzer.begin(10);

  buzzer.sound(NOTE_G3, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_C4, time / 2);
  buzzer.sound(NOTE_G3, time * 2);
  
  buzzer.sound(NOTE_G3, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_C4, time / 2);
  buzzer.sound(NOTE_A4, time * 2);

  buzzer.sound(NOTE_A4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_G4, time);
  buzzer.sound(NOTE_G4, time);
  
  buzzer.sound(NOTE_A5, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_E4, time * 2);
  
  buzzer.sound(NOTE_G3, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_C4, time / 2);
  buzzer.sound(NOTE_G3, time * 2);
  
  buzzer.sound(NOTE_G3, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_C4, time / 2);
  buzzer.sound(NOTE_A4, time * 2);
  
  buzzer.sound(NOTE_A4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_G4, (time * 3) / 4);
  buzzer.sound(NOTE_G4, time / 4);
  
  buzzer.sound(NOTE_A5, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_C4, time * 2);

  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time);
  
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_C4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_E4, time * 2);
  
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 4);
  buzzer.sound(NOTE_E4, time / 4);

  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_D4, time);
  buzzer.sound(NOTE_G4, time);

  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time);
  
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_C4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_E4, time * 2);
  
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 2);
  buzzer.sound(NOTE_E4, time / 4);
  buzzer.sound(NOTE_E4, time / 4);
  
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_G4, time / 2);
  buzzer.sound(NOTE_F4, time / 2);
  buzzer.sound(NOTE_D4, time / 2);
  buzzer.sound(NOTE_C4, time * 2);

  buzzer.end(2000);
}
