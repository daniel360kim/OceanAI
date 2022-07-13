#include <Buzzer.h>

Buzzer buzzer(11);

void setup() {}

void loop() {
  buzzer.begin(0);

  buzzer.distortion(NOTE_C3, NOTE_C5);
  buzzer.distortion(NOTE_C5, NOTE_C3);

  buzzer.end(1000);
}
