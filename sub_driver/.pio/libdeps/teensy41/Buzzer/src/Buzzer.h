#ifndef __BUZZER_h
#define __BUZZER_h

#include "Arduino.h"

class Buzzer {
    public:     Buzzer(int);
                Buzzer(int, int);
                void begin(int);
                void end(int);
                void sound(int, int);
                void slowDistortion(int, int);
                void distortion(int, int);
                void fastDistortion(int, int);
                String toString();
    private:    int pinBuzzer;
                int pinLED;
                int pause;
};

#endif
