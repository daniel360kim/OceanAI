/**
 * @file LEDStrip.h
 * @author Daniel Kim
 * @brief Driver for the WS2812B LED strip on the rear of the vehicle
 * @version 0.1
 * @date 2023-04-03
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <cstdint>

template <uint8_t pin, uint8_t numLEDs>
class LEDStrip
{
public:
    LEDStrip();
    void setColor(uint8_t red, uint8_t green, uint8_t blue);
    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);
    void ledOff();
    void setBrightness(uint8_t brightness);

private:
    Adafruit_NeoPixel* m_strip;
    uint8_t m_brightness = 255;
};

template <uint8_t pin, uint8_t numLEDs>
LEDStrip<pin, numLEDs>::LEDStrip()
{
    m_strip = new Adafruit_NeoPixel(numLEDs, pin, NEO_GRB + NEO_KHZ800);
    m_strip->begin();
    m_strip->setBrightness(m_brightness);
}

template <uint8_t pin, uint8_t numLEDs>
void LEDStrip<pin, numLEDs>::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
    setColor(red, green, blue, m_brightness);
}

template <uint8_t pin, uint8_t numLEDs>
void LEDStrip<pin, numLEDs>::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{ 
    for (uint8_t i = 0; i < numLEDs; i++)
    {
        m_strip->setPixelColor(i, m_strip->Color(red, green, blue));
    }
    m_strip->show();
}

template <uint8_t pin, uint8_t numLEDs>
void LEDStrip<pin, numLEDs>::ledOff()
{
    setColor(0, 0, 0, 0);
}

template <uint8_t pin, uint8_t numLEDs>
void LEDStrip<pin, numLEDs>::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
}













#endif