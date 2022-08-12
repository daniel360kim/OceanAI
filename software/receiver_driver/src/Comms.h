#ifndef Comms_h
#define Comms_h

#include <Arduino.h>
#include <Wire.h>

class Comms
{
public:
    enum Subaddress
    {
        ODR_PARAMS = 0x00,

        CMD = 0x10
    };

    enum Def
    {
        I2C_ADDR = 0x1B,
        RESET_CODE = 0xFF
    };

    struct Params
    {
        int therm_odr;
        int lora_odr;
        int pres_odr;
        int pgood_odr;
        int chg_odr;
        int gps_odr;
        int volt_odr;
        int temp_odr;
    } ODR;

    Comms(uint8_t address);
    bool begin();

    

    void setI2CAddress(uint8_t address);


private:
    uint8_t address = Def::I2C_ADDR;

    void initialize_wire();
    void writeByte(uint8_t subaddress, uint8_t data);
    void readBytes(uint8_t subaddress, uint8_t *data, uint16_t length);
    uint8_t readByte(uint8_t subaddress);

    bool checkReset();

};






#endif