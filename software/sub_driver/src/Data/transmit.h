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
#ifndef transmit_h
#define transmit_h

#include <Arduino.h>
#include <Wire.h>

#include "data_struct.h"

class Transmit
{
public:
    Transmit(const uint8_t address, const unsigned long interval_micros, const uint8_t int_tx, const uint8_t int_rx);

    void transmit(const Data data);

private:
    unsigned long interval_micros;
    uint8_t int_tx;
    uint8_t int_rx;
    uint8_t address;

    bool tx_flag;

    //static bool interrupt_flag;

    //static void interrupt_rx() { interrupt_flag = true; }

    unsigned long long previous_micros;

};






#endif // transmit_h