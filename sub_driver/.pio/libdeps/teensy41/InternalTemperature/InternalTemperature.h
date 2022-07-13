/* InternalTemperature - read internal temperature of ARM processor
 * Copyright (C) 2020 LAtimes2
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* Typical usage:
 *   #include "InternalTemperature.h"
 *   
 *   Serial.println(InternalTemperature.readTemperatureC());
 */

#ifndef InternalTemperature_h_
#define InternalTemperature_h_

#define TEMPERATURE_MAX_ACCURACY 0
#define TEMPERATURE_NO_ADC_SETTING_CHANGES 1

#include <Arduino.h>
class InternalTemperatureClass
{
public:
  InternalTemperatureClass();

  //
  // Main functions
  //

  // Note: If settings_type is TEMPERATURE_MAX_ACCURACY, it will change
  //       the ADC settings to be optimal for reading temperature.
  //       If settings_type is TEMPERATURE_NO_ADC_SETTING_CHANGES, it will
  //       keep the default ADC settings or any other settings changes.
  //       readTemperature will detect the current settings and use them.
  static bool begin (int temperature_settings_type = TEMPERATURE_MAX_ACCURACY);

  static double readTemperatureC (void);
  static double readTemperatureF (void);

  //
  // functions to handle going above a high temperature
  //
  static int attachHighTempInterruptCelsius (double triggerTemperature, void (*function)(void));
  static int attachHighTempInterruptFahrenheit (double triggerTemperature, void (*function)(void));
  static int detachHighTempInterrupt (void);

  //
  // functions to handle going below a low temperature
  //
  static int attachLowTempInterruptCelsius (double triggerTemperature, void (*function)(void));
  static int attachLowTempInterruptFahrenheit (double triggerTemperature, void (*function)(void));
  static int detachLowTempInterrupt (void);

  //
  //  Calibration functions
  //
  bool singlePointCalibrationC (double actualTemperatureC, double measuredTemperatureC, bool fromDefault = false);
  bool singlePointCalibrationF (double actualTemperatureF, double measuredTemperatureF, bool fromDefault = false);

  bool dualPointCalibrationC (double actualTemperature1C, double measuredTemperature1C,
                              double actualTemperature2C, double measuredTemperature2C, bool fromDefault = false);
  bool dualPointCalibrationF (double actualTemperature1F, double measuredTemperature1F,
                              double actualTemperature2F, double measuredTemperature2F, bool fromDefault = false);

  bool setVTemp25 (double volts);
  bool setSlope (double voltsPerDegreeC);
  double getVTemp25 (void);
  double getSlope (void);
  static int getUniqueID (void);

  //
  // low level utilities
  //
  static double convertTemperatureC (double volts);
  static double convertUncalibratedTemperatureC (double volts);
  static double readRawTemperatureVoltage (void);
  static double readRawVoltage (int signalNumber);
  static double readUncalibratedTemperatureC (void);
  static double readUncalibratedTemperatureF (void);
  static double toCelsius (double temperatureFahrenheit);
  static double toFahrenheit (double temperatureCelsius);

private:
  enum temperatureAlarmType {
    LowTemperature,
    HighTemperature
  };

  enum alarmType {
    NoAlarm,
    LowTempAlarm,
    HighTempAlarm,
    BothAlarms
  };

private:
  static double convertTemperatureC (double volts, double vTemp25, double slope);
  static void enableBandgap (void);
  static void getRegisterCounts (int *currentCount, int *highTempCount, int *lowTempCount);

  static void alarmISR ();
  static uint32_t celsiusToCount (double temperatureCelsius);

  // Teensy 3
  static double computeVoltsPerBit ();
  static int attachInterruptCelsius (double triggerTemperature, temperatureAlarmType whichTemperature);

private:
  static bool initialized;
  static int temperatureSettingsType;
  static double slope;
  static double vTemp25;
  static double voltsPerBit;

  typedef void (*voidFuncPtr)(void);

  static alarmType alarm;
  static voidFuncPtr highTempISR;
  static voidFuncPtr lowTempISR;
};

// create instance of the class
extern InternalTemperatureClass InternalTemperature;

#endif
