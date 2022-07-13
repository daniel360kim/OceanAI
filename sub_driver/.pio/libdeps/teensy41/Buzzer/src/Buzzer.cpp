#include "Buzzer.h"

/* ------------------------------ CONSTRUCTOR CODE ------------------------------ */

/* Constructor - It saves the pin number of the Buzzer and sets it in OUTPUT mode */
Buzzer::Buzzer(int pinBuzzer) {
    this->pinBuzzer = pinBuzzer;
    this->pinLED = -1;
    this->pause = 0;

    pinMode(pinBuzzer, OUTPUT);
}

/* Constructor - It saves the pin number of the Buzzer and the LED and set them in OUTPUT mode */
Buzzer::Buzzer(int pinBuzzer, int pinLED) {
    this->pinBuzzer = pinBuzzer;
    this->pinLED = pinLED;
    this->pause = 0;

    pinMode(pinBuzzer, OUTPUT);
    pinMode(pinLED, OUTPUT);
}

/* ------------------------------ SIMPLE APIs ------------------------------ */

/* Method - It sets the pause before the note (in percentage) */
void Buzzer::begin(int pause) {
    this->pause = pause;
}

/* Method - It sets the pause before the end of the melody */
void Buzzer::end(int pause) {
    delay(pause);
}

/* Method - It makes the Buzzer sound */
void Buzzer::sound(int note, int duration) {
    long timeBreak = 1000000 / note / 2;
    long numCycles = note * duration / 1000;

    if (this->pinLED != -1)
        digitalWrite(this->pinLED, HIGH);

    if (note > 0)
        tone(this->pinBuzzer, note);

    delay(duration);

    if (this->pinLED != -1)
        digitalWrite(this->pinLED, LOW);

    noTone(this->pinBuzzer);
    
    delay(((duration * this->pause) / 100));
}

/* Method - It creates a "slow distortion" effect on the Buzzer */
void Buzzer::slowDistortion(int noteFrom, int noteTo) {
    int nCycles = abs(noteFrom - noteTo);

    if (noteFrom > noteTo)
        for (int i = 0; i < nCycles; i++)
            this->sound(noteFrom - i, (10000 * (nCycles - i)) / nCycles);
    else
        for (int i = 0; i < nCycles; i++)
            this->sound(noteFrom + i, (10000 * (nCycles - i)) / nCycles);
}
/* Method - It creates a "normal distortion" effect on the Buzzer */
void Buzzer::distortion(int noteFrom, int noteTo) {
    int nCycles = abs(noteFrom - noteTo);

    if (noteFrom > noteTo)
        for (int i = 0; i < nCycles; i++)
            this->sound(noteFrom - i, 10000 / nCycles);
    else
        for (int i = 0; i < nCycles; i++)
            this->sound(noteFrom + i, 10000 / nCycles);
}

/* Method - It creates a "fast distortion" effect on the Buzzer */
void Buzzer::fastDistortion(int noteFrom, int noteTo) {
    int nCycles = abs(noteFrom - noteTo);

    if (noteFrom > noteTo)
        for (int i = 0; i < nCycles; i++)
            this->sound(noteFrom - i, (10000 * (i + 1)) / nCycles);
    else
        for (int i = 0; i < nCycles; i++)
            this->sound(noteFrom + i, (10000 * (i + 1)) / nCycles);
}

/* ------------------------------ OTHER FUNCTIONS ------------------------------ */

/* Method - It returns the Buzzer informations */
String Buzzer::toString() {
    String ret = String("Buzzer: {");
    ret.concat("pinBuzzer=");
    ret.concat(this->pinBuzzer);
    ret.concat(", pinLED=");
    ret.concat(this->pinLED);
    ret.concat("}");
    
    return ret;
}
