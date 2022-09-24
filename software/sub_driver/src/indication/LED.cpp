/**
 * @file LED.cpp
 * @author Daniel Kim
 * @brief LED drivers
 * @version 1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <Arduino.h>
#include <Entropy.h>

#include "LED.h"

/******* RGB LED ********/

/**
 * @brief Construct a new RGBLED::RGBLED object
 * 
 * @param red Pin to where the red LED is connected
 * @param green Pin to where the green LED is connected
 * @param blue Pin to where the blue LED is connected
 */
RGBLED::RGBLED(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) 
{
    pinMode(this->red, OUTPUT);
    pinMode(this->green, OUTPUT);
    pinMode(this->blue, OUTPUT);

    Entropy.Initialize();
}

/**
 * @brief Sets the LED to any RGB value
 * 
 * @param red_value red led value (0-255)
 * @param green_value green led value (0-255)
 * @param blue_value blue led value (0-255)
 */
void RGBLED::setColor(uint8_t red_value, uint8_t green_value, uint8_t blue_value)
{
    red_value = (red_value * brightness) / 100;
    green_value = (green_value * brightness) / 100;
    blue_value = (blue_value * brightness) / 100;

    analogWrite(red, red_value);
    analogWrite(green, green_value);
    analogWrite(blue, blue_value);

    LED_state = true;
}

/**
 * @brief Non-blocking LED blinker
 * 
 * @param red_value red LED value (0-255)
 * @param green_value green LED value (0-255)
 * @param blue_value blue LED value (0-255)
 * @param interval interval (millis) that the led should blink
 */
void RGBLED::blink(uint8_t red_value, uint8_t green_value, uint8_t blue_value, unsigned long interval)
{
    unsigned long current_millis = millis();
    
    if(current_millis - previous_millis >= interval)
    {
        previous_millis = current_millis; //saving the last time the LED was blinked

        if(LED_state == false)
        {
            setColor(red_value, green_value, red_value);
        }
        else
        {
            LEDoff();
        }
    }

}

/**
 * @brief turns the LED off
 * 
 */
void RGBLED::LEDoff()
{
    setColor(0, 0, 0);
    LED_state = false;
}

/**
 * @brief sets the brightness of the LED (default max)
 * 
 * @param brightness intensity from 0-100
 */
void RGBLED::setBrightness(uint8_t brightness)
{
    this->brightness = brightness;
}

void RGBLED::randomColors()
{
    setColor(Entropy.random(0, 256), Entropy.random(0, 256), Entropy.random(0, 256));
}

/**
 * @brief displays a spectrum of colors across the LED
 * 
 */
void RGBLED::displaySpectrum()
{

    unsigned long currentAnimation = millis();
    if(currentAnimation - previousAnimation >= animationDelay)
    {
        previousAnimation = currentAnimation;
        double colorNumber = counter > numColors ? counter - numColors: counter;
    
        double saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
        double brightness = 1; // Between 0 and 1 (0 = dark, 1 is full brightness)
        double hue = (colorNumber / double(numColors)) * 360; // Number between 0 and 360
        long color = HSBtoRGB(hue, saturation, brightness); 
    
        // Get the red, blue and green parts from generated color
        int red = color >> 16 & 255;
        int green = color >> 8 & 255;
        int blue = color & 255; 
        setColor(red, green, blue);
    
        /*
        Counter can never be greater then 2 times the number of available colors
        the colorNumber = line above takes care of counting backwards (nicely looping animation)
        when counter is larger then the number of available colors
        */
        counter = (counter + 1) % (numColors * 2);
  
        //uncomment to make animation restart instead of rewind
        //counter = (counter + 1) % (numColors);
    }
}

/**
 * @brief returns LED state
 * 
 * @return true LED is on
 * @return false LED is off
 */
bool RGBLED::returnState()
{
    return LED_state;
}

/**
 * @brief Converts HSB format to RGB. Useful in spectrums and other features
 * 
 * @param _hue hue value
 * @param _sat saturation value
 * @param _brightness brightness value
 * @return generated RGB color: Red shifted left 16 bits, green shifted left 8 bits
 */
long RGBLED::HSBtoRGB(double _hue, double _sat, double _brightness)
{
    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;
    
    if (_sat == 0.0) 
    {
        red = _brightness;
        green = _brightness;
        blue = _brightness;
    } 
    else 
    {
        if (_hue == 360.0) 
        {
            _hue = 0;
        }

        int slice = _hue / 60.0;
        double hue_frac = (_hue / 60.0) - slice;

        double aa = _brightness * (1.0 - _sat);
        double bb = _brightness * (1.0 - _sat * hue_frac);
        double cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
        
        switch(slice) 
        {
            case 0:
                red = _brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = _brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = _brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = _brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = _brightness;
                break;
            case 5:
                red = _brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }

    }

    long ired = red * 255.0;
    long igreen = green * 255.0;
    long iblue = blue * 255.0;
    
    return long((ired << 16) | (igreen << 8) | (iblue));
}

/********* LED ***********/

/**
 * @brief Construct a new LED::LED object
 * 
 * @param pin GPIO the LED is connected to (non-pwm) 
 */
LED::LED(uint8_t pin) : pin(pin)
{
    pinMode(this->pin, OUTPUT);
}

/**
 * @brief turns LED on
 * 
 */
void LED::on()
{
    digitalWrite(pin, HIGH);
    LED_state = true;
}

/**
 * @brief turns LED off
 * 
 */
void LED::off()
{
    digitalWrite(pin, LOW);
    LED_state = false;
}

/**
 * @brief non blocking blink function
 * 
 * @param interval interval at which the LED should blink
 */
void LED::blink(unsigned long interval)
{
    unsigned long current_millis = millis();

    if(current_millis - previous_millis >= interval)
    {
        previous_millis = current_millis;

        if(LED_state == false)
        {
            on();
        }
        else
        {
            off();
        }
    }
}

void LED::show(bool state)
{
    if(state == true)
    {
        on();
    }
    else
    {
        off();
    }
}