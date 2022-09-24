

#include <Wire.h>

#include <Arduino.h>
#include "EasyTransferI2C.h"
#include "data_struct.h"

// create object
EasyTransferI2C ET;

Data cc;
// define slave i2c address
#define I2C_SLAVE_ADDRESS 0x2c

void receive(int numBytes)
{
}

void setup()
{
  Wire.begin(I2C_SLAVE_ADDRESS);
  Serial.begin(9600);
  // start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(cc), &Wire);
  // define handler function on receiving data
  Wire.onReceive(receive);
}

void loop()
{
  // check and see if a data packet has come in.
  if (ET.receiveData())
  {
    // this is how you access the variables. [name of the group].[variable name]
    // since we have data, we will blink it out.

    char *comma = (char *)",";

   
    Serial.println(cc.dt, 15);
   


  }
}
