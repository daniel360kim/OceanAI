/*!
 *  @file Adafruit_ADT7410.cpp
 *
 *  @mainpage Adafruit ADT7410 I2C Temp Sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for Microchip's ADT7410 I2C Temp sensor
 *
 * 	This is a library for the Adafruit ADT7410 breakout:
 * 	http://www.adafruit.com/products/1782
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  K.Townsend (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_ADT7410.h"

/*!
 *    @brief  Instantiates a new ADT7410 class
 */
Adafruit_ADT7410::Adafruit_ADT7410(void) {}

/*!
 *    @brief  Setups the HW
 *    @param  addr The I2C address, defaults to 0x48
 *    @param  wire The I2C interface, pointer to a TwoWire, defaults to WIre
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_ADT7410::begin(uint8_t addr, TwoWire *wire) {

  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(addr, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // Check connection
  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, ADT7410_REG__ADT7410_ID, 1);

  // make sure we're talking to the right chip
  if ((chip_id.read() & 0xF8) != 0xC8) {
    // Not detected ... return false
    return false;
  }

  // soft reset
  reset();

  return true;
}

/*!
 *   @brief  Perform a soft reset
 *   @return True on success
 */
bool Adafruit_ADT7410::reset(void) {
  uint8_t cmd = ADT7410_REG__ADT7410_SWRST;

  if (!i2c_dev->write(&cmd, 1)) {
    return false;
  }
  delay(10);
  return true;
}

/*!
 *   @brief  Reads the 16-bit temperature register and returns the Centigrade
 *           temperature as a float.
 *   @return Temperature in Centigrade.
 */
float Adafruit_ADT7410::readTempC() {
  Adafruit_BusIO_Register temp_reg = Adafruit_BusIO_Register(
      i2c_dev, ADT7410_REG__ADT7410_TEMPMSB, 2, MSBFIRST);

  uint16_t t = temp_reg.read();

  float temp = (int16_t)t;
  temp /= 128.0;

  return temp;
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t device data, Adafruit Unified Sensor format
    @param  sensor Pointer to an Adafruit Unified sensor_t object that we'll
   fill in
*/
/**************************************************************************/
void Adafruit_ADT7410::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "ADT7410", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  sensor->min_delay = 0;
  sensor->max_value = 0;
  sensor->min_value = 0;
  sensor->resolution = 0;
}

/**************************************************************************/
/*!
    @brief  Gets the most recent sensor event, Adafruit Unified Sensor format
    @param  event Pointer to an Adafruit Unified sensor_event_t object that
   we'll fill in
    @returns True on successful read
*/
/**************************************************************************/
bool Adafruit_ADT7410::getEvent(sensors_event_t *event) {
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  event->timestamp = 0;

  event->temperature = readTempC();

  return true;
}
