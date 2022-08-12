/**
 * @file transmit.h
 * @author Daniel Kim
 * @brief transmissions between the board and transmitter board
 * @version 0.1
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include <Arduino.h>
#include <Wire.h>

#include "transmit.h"

Transmit::Transmit(const uint8_t address, unsigned long interval_micros, uint8_t int_tx, uint8_t int_rx)
{
    this->interval_micros = interval_micros;
    this->int_tx = int_tx;
    this->int_rx = int_rx;
    this->address = address;

    pinMode(int_tx, OUTPUT);
    pinMode(int_rx, INPUT);

    //attachInterrupt(int_rx, Transmit::interrupt_rx, RISING);

    Wire.begin();
}

void Transmit::transmit(const Data data)
{
    if(micros() - previous_micros >= interval_micros)
    {
        digitalWrite(int_tx, HIGH);
        delay(1);
        digitalWrite(int_tx, LOW);
        
        previous_micros = micros();
    }
}