/**
 * @file LED.h
 * @author Daniel Kim
 * @brief LED drivers header
 * @version 1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef LED_H
#define LED_H

#include <Arduino.h>

/**
 * @brief class for RGB LED
 * 
 */
class RGBLED
{
public:
    RGBLED(uint8_t red, uint8_t green, uint8_t blue); //pinouts
    ~RGBLED() {}

    void setColor(uint8_t red_value, uint8_t green_value, uint8_t blue_value);
    void blink(uint8_t red_value, uint8_t green_value, uint8_t blue_value, unsigned long interval); //blinks the LED at a certain interval
    void LEDoff(); //turns the LED off
    void setBrightness(uint8_t brightness);
    void randomColors();
    void displaySpectrum(); //lovely little colorwheel
    bool returnState();

private:
    uint8_t red, green, blue; //pins
    bool LED_state; //true = LED is on. false = LED is off
    uint8_t brightness = 100; //brightness of LED 0-100
    
    //settings for the colorwheel
    int counter = 0;
    int numColors = 255;
    const unsigned long animationDelay = 5;
    unsigned long previousAnimation = 0;

    //last time the LED blinked for blink()
    unsigned long previous_millis = 0;

    //converts HSB to RGB values
    long HSBtoRGB(double _hue, double _sat, double _brightness);
    
};

/**
 * @brief Simple LED class for single LED connected to GPIO (no pwm)
 * 
 */
class LED
{
public:
    LED(uint8_t pin);

    //Simple APIs
    void on();
    void off();
    void blink(unsigned long interval);

    void show(bool state);

    bool state() const { return LED_state; }
private:
    uint8_t pin = 0;
    bool LED_state = false;
    unsigned long previous_millis = 0;
};

#endif