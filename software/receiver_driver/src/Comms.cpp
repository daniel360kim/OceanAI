#include <Arduino.h>
#include <Wire.h>

#include "Comms.h"

Comms::Comms(uint8_t address)
{
    this->address = address;
    Wire.begin();
    Wire.setClock(400000);
}

bool Comms::begin()
{
    while(1)
    {
        if(checkReset())
        {
            break;
        }
    }

    uint8_t data = 0x00;
    readBytes(ODR_PARAMS, (uint8_t *)&ODR, sizeof(ODR)); //set the odr from register 0x00
    return true;
}

void Comms::initialize_wire()
{
    Wire.begin();
    Wire.setClock(400000);
}

void Comms::writeByte(uint8_t subaddress, uint8_t data)
{
    Wire.beginTransmission(address);
    Wire.write(subaddress);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t Comms::readByte(uint8_t subaddress)
{
    Wire.beginTransmission(address);
    Wire.write(subaddress);
    Wire.endTransmission(false);
    Wire.requestFrom(address, (uint8_t)1);
    return Wire.read();
}

void Comms::readBytes(uint8_t subaddress, uint8_t *data, uint16_t length)
{
    Wire.beginTransmission(address);
    Wire.write(subaddress);
    Wire.endTransmission();
    uint8_t i = 0;
    Wire.requestFrom(address, length);
    while(Wire.available())
    {
        data[i++] = Wire.read();
    }
}

bool Comms::checkReset()
{
    uint8_t reset_code = readByte(Subaddress::CMD);
    if(reset_code == Def::RESET_CODE)
    {
        return true;
    }
}
