#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <stdint.h>

#include "data_struct.h"

class Transmission
{
public:
    Transmission();
    union DataBytes
    {
        DataBytes() {}
        DataBytes(Data data) : data(data) {}
        Data data;
        uint8_t bytes[sizeof(Data)];
    };

    


};










#endif