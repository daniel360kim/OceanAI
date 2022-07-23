/*
  BMP388_DEV is an I2C/SPI compatible library for the Bosch BMP388 barometer.
	
	Copyright (C) Martin Lindupp 2020
	
	V1.0.0 -- Initial release 	
	V1.0.1 -- Fix uninitialised structures, thanks to David Jade for investigating and flagging up this issue
	V1.0.2 -- Modification to allow user-defined pins for I2C operation on the ESP32
	V1.0.3 -- Initialise "device" constructor member variables in the same order they are declared
	V1.0.4 -- Fix incorrect oversampling definition for x1, thanks to myval for raising the issue
	V1.0.5 -- Modification to allow ESP8266 SPI operation, thanks to Adam9850 for the generating the pull request
	V1.0.6 -- Include getErrorReg() and getStatusReg() functions
	V1.0.7 -- Fix compilation issue with Arduino Due
	
	The MIT License (MIT)
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#include "BMP388_DEV.h"

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Class Constructors
////////////////////////////////////////////////////////////////////////////////

BMP388_DEV::BMP388_DEV() { setI2CAddress(BMP388_I2C_ADDR); }		// Constructor for I2C communications	
#ifdef ARDUINO_ARCH_ESP8266
BMP388_DEV::BMP388_DEV(uint8_t sda, uint8_t scl) : Device(sda, scl) { setI2CAddress(BMP388_I2C_ADDR); } 	// Constructor for I2C comms on ESP8266
#endif
BMP388_DEV::BMP388_DEV(uint8_t cs) : Device(cs) {}			   			// Constructor for SPI communications
#ifdef ARDUINO_ARCH_ESP32 																			
BMP388_DEV::BMP388_DEV(uint8_t sda, uint8_t scl) : Device(sda, scl) { setI2CAddress(BMP388_I2C_ADDR); } 	// Constructor for I2C comms on ESP32
BMP388_DEV::BMP388_DEV(uint8_t cs, uint8_t spiPort, SPIClass& spiClass) : Device(cs, spiPort, spiClass) {} // Constructor for SPI communications on the ESP32
#endif
////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Public Member Functions
////////////////////////////////////////////////////////////////////////////////

uint8_t BMP388_DEV::begin(Mode mode, 																// Initialise BMP388 device settings
												  Oversampling presOversampling, 
													Oversampling tempOversampling,
													IIRFilter iirFilter,
													TimeStandby timeStandby)
{
	initialise();																											// Call the Device base class "initialise" function
	if (!reset())                                                     // Reset the BMP388 barometer
	{		
		return 0;																												// If unable to reset return 0
	}
	
	byte chipId = readByte(BMP388_CHIP_ID);														// Read the device ID	
	if (chipId != BMP388_ID && chipId != BMP390_ID)             			// Check the device ID
  {  
		return 0;                                                     	// If the ID is incorrect return 0
  }	 
	
  readBytes(BMP388_TRIM_PARAMS, (uint8_t*)&params, sizeof(params)); // Read the trim parameters into the params structure
	doubleParams.param_T1 = (double)params.param_T1 / pow(2.0, -8.0); // Calculate the doubleing point trim parameters
	doubleParams.param_T2 = (double)params.param_T2 / pow(2.0, 30.0);
	doubleParams.param_T3 = (double)params.param_T3 / pow(2.0, 48.0);
	doubleParams.param_P1 = ((double)params.param_P1 - pow(2.0, 14.0)) / pow(2.0, 20.0);
	doubleParams.param_P2 = ((double)params.param_P2 - pow(2.0, 14.0)) / pow(2.0, 29.0);
	doubleParams.param_P3 = (double)params.param_P3 / pow(2.0, 32.0);
	doubleParams.param_P4 = (double)params.param_P4 / pow(2.0, 37.0);
	doubleParams.param_P5 = (double)params.param_P5 / pow(2.0, -3.0);
	doubleParams.param_P6 = (double)params.param_P6 / pow(2.0, 6.0);
	doubleParams.param_P7 = (double)params.param_P7 / pow(2.0, 8.0);
	doubleParams.param_P8 = (double)params.param_P8 / pow(2.0, 15.0);
	doubleParams.param_P9 = (double)params.param_P9 / pow(2.0, 48.0);
	doubleParams.param_P10 = (double)params.param_P10 / pow(2.0, 48.0);
	doubleParams.param_P11 = (double)params.param_P11 / pow(2.0, 65.0);
	setIIRFilter(iirFilter);																					// Initialise the BMP388 IIR filter register
	setTimeStandby(timeStandby); 																			// Initialise the BMP388 standby time register
	setOversamplingRegister(presOversampling, tempOversampling);			// Initialise the BMP388 oversampling register	
	pwr_ctrl.bit.press_en = 1;																				// Set power control register to enable pressure sensor
	pwr_ctrl.bit.temp_en = 1;																					// Set power control register to enable temperature sensor
	setMode(mode);																										// Set the BMP388 mode
	return 1;																													// Report successful initialisation
}

uint8_t BMP388_DEV::begin(Mode mode, uint8_t addr)									// Initialise BMP388 with default settings, but selected mode and
{																																		// I2C address
	setI2CAddress(addr);
	return begin(mode);
}

uint8_t BMP388_DEV::begin(uint8_t addr)															// Initialise BMP388 with default settings and selected I2C address
{
	setI2CAddress(addr);
	return begin();
}

uint8_t BMP388_DEV::reset()																					// Reset the BMP388 barometer
{
	writeByte(BMP388_CMD, RESET_CODE);                    						// Write the reset code to the command register 									
    delay(10);																												// Wait for 10ms
	event.reg = readByte(BMP388_EVENT);																// Read the BMP388's event register
	return event.bit.por_detected; 																		// Return if device reset is complete																					
}

void BMP388_DEV::startNormalConversion() { setMode(NORMAL_MODE); }	// Start continuous measurement in NORMAL_MODE

void BMP388_DEV::startForcedConversion() 														// Start a one shot measurement in FORCED_MODE
{ 
	if (pwr_ctrl.bit.mode == SLEEP_MODE)															// Only set FORCED_MODE if we're already in SLEEP_MODE
	{
		setMode(FORCED_MODE);
	}	
}			

void BMP388_DEV::stopConversion() { setMode(SLEEP_MODE); }					// Stop the conversion and return to SLEEP_MODE

void BMP388_DEV::setPresOversampling(Oversampling presOversampling)	// Set the pressure oversampling rate
{
	osr.bit.osr_p = presOversampling;
	writeByte(BMP388_OSR, osr.reg);
}

void BMP388_DEV::setTempOversampling(Oversampling tempOversampling)	// Set the temperature oversampling rate
{
	osr.bit.osr_t = tempOversampling;
	writeByte(BMP388_OSR, osr.reg);
}

void BMP388_DEV::setIIRFilter(IIRFilter iirFilter)									// Set the IIR filter setting
{
	config.bit.iir_filter = iirFilter;
	writeByte(BMP388_CONFIG, config.reg);
}

void BMP388_DEV::setTimeStandby(TimeStandby timeStandby)						// Set the time standby measurement interval
{
	odr.bit.odr_sel = timeStandby;
	writeByte(BMP388_ODR, odr.reg);
}

void BMP388_DEV::setSeaLevelPressure(double pressure)								// Set the sea level pressure value
{
	sea_level_pressure = pressure;
}

uint8_t BMP388_DEV::getTemperature(volatile double &temperature)			// Get the temperature
{
	if (!dataReady())																									// Check if a measurement is ready
	{
		return 0;
	}
	uint8_t data[3];                                                  // Create a data buffer
	readBytes(BMP388_DATA_3, &data[0], 3);       						      		// Read the temperature 
	int32_t adcTemp = (int32_t)data[2] << 16 | (int32_t)data[1] << 8 | (int32_t)data[0];  // Copy the temperature data into the adc variables
	temperature = bmp388_compensate_temp((double)adcTemp);       			// Temperature compensation (function from BMP388 datasheet)
	return 1;
}

uint8_t BMP388_DEV::getPressure(volatile double &pressure)						// Get the pressure
{
	double temperature;
	return getTempPres(temperature, pressure);
}

uint8_t BMP388_DEV::getTempPres(volatile double &temperature, 				// Get the temperature and pressure
																volatile double &pressure)	
{
	if (!dataReady())																									// Check if a measurement is ready
	{	
		return 0;
	}
	uint8_t data[6];                                                  // Create a data buffer
	readBytes(BMP388_DATA_0, &data[0], 6);             	  						// Read the temperature and pressure data
	int32_t adcTemp = (int32_t)data[5] << 16 | (int32_t)data[4] << 8 | (int32_t)data[3];  // Copy the temperature and pressure data into the adc variables
	int32_t adcPres = (int32_t)data[2] << 16 | (int32_t)data[1] << 8 | (int32_t)data[0];
	temperature = bmp388_compensate_temp((double)adcTemp);   					// Temperature compensation (function from BMP388 datasheet)
	pressure = bmp388_compensate_press((double)adcPres, temperature); 	// Pressure compensation (function from BMP388 datasheet)
	pressure /= 100.0f;                         											// Calculate the pressure in millibar/hPa
	return 1;
}

uint8_t BMP388_DEV::getAltitude(volatile double &altitude)						// Get the altitude
{
	double temperature, pressure;
	return getMeasurements(temperature, pressure, altitude);
}

uint8_t BMP388_DEV::getMeasurements(volatile double &temperature, 		// Get all measurements temperature, pressure and altitude
																		volatile double &pressure, 
																		volatile double &altitude)		
{  
	if (getTempPres(temperature, pressure))
	{
		altitude = ((double)pow(sea_level_pressure / pressure, 0.190223f) - 1.0f) * (temperature + 273.15f) / 0.0065f; // Calculate the altitude in metres 
		return 1;
	}
	return 0;
}

void BMP388_DEV::enableInterrupt(OutputDrive outputDrive, 					// Enable the BMP388's data ready interrupt on the INT pin
																 ActiveLevel activeLevel,
																 LatchConfig latchConfig)
{
	int_ctrl.reg = latchConfig << 2 | activeLevel << 1 | outputDrive;
	int_ctrl.bit.drdy_en = 1;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);
}

void BMP388_DEV::disableInterrupt()																	// Disable the data ready interrupt on the INT pin
{
	int_ctrl.bit.drdy_en = 0;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);
}

void BMP388_DEV::setIntOutputDrive(OutputDrive outputDrive)					// Set the interrupt output drive: PUSH_PULL or OPEN_COLLECTOR
{
	int_ctrl.bit.int_od = outputDrive;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);
}

void BMP388_DEV::setIntActiveLevel(ActiveLevel activeLevel)					// Set the interrupt active level: LOW or HIGH
{
	int_ctrl.bit.int_level = activeLevel;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);
}

void BMP388_DEV::setIntLatchConfig(LatchConfig latchConfig)					// Set the interrupt latch configuration
{
	int_ctrl.bit.int_latch = latchConfig;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);
}

void BMP388_DEV::enableFIFO(PressEnable pressEnable,								// Enable the FIFO
														AltEnable altEnable,
														TimeEnable timeEnable,
														Subsampling subsampling,
														DataSelect dataSelect,
														StopOnFull stopOnFull)
{
	alt_enable = altEnable;
	fifo_config_1.reg = 1 << 4 | pressEnable << 3 | timeEnable << 2 | stopOnFull << 1;	
	fifo_config_1.bit.fifo_mode = 1;	
	writeByte(BMP388_FIFO_CONFIG_1, fifo_config_1.reg);
	fifo_config_2.reg = dataSelect << 3 | subsampling;
	writeByte(BMP388_FIFO_CONFIG_2, fifo_config_2.reg);
}

void BMP388_DEV::disableFIFO()																			// Disable the FIFO
{
	fifo_config_1.bit.fifo_mode = 0;
	writeByte(BMP388_FIFO_CONFIG_1, fifo_config_1.reg);
}

uint8_t BMP388_DEV::setFIFONoOfMeasurements(uint16_t noOfMeasurements) 		// Set the FIFO watermark from the number of measurements								
{																		
	// Calculate the FIFO watermark from the number of measurements
	uint16_t fifoWatermark = noOfMeasurements * ((fifo_config_1.bit.fifo_press_en | fifo_config_1.bit.fifo_temp_en) + 		
													 3 * fifo_config_1.bit.fifo_press_en + 3 * fifo_config_1.bit.fifo_temp_en);
	return setFIFOWatermark(fifoWatermark);
}

uint8_t BMP388_DEV::setFIFOWatermark(uint16_t fifoWatermark)				// Set the FIFO watermark
{
	if (fifoWatermark + fifo_config_1.bit.fifo_time_en +  						// Check if the FIFO watermark + appended sensor time (4 bytes) 
			3 * fifo_config_1.bit.fifo_time_en + 2 > FIFO_SIZE)						// if enabled + 2 byte overhead is larger than the FIFO size of 512 bytes
	{
		return 0;																												// FIFO watermark larger than FIFO, return error
	}
	uint8_t fifoWatermarkLSB = (uint8_t)(fifoWatermark & 0xFF);
	uint8_t fifoWatermarkMSB = (uint8_t)(fifoWatermark >> 8 & 0x01);
	writeByte(BMP388_FIFO_WTM_0, fifoWatermarkLSB);
	writeByte(BMP388_FIFO_WTM_1, fifoWatermarkMSB);
	return 1;
}

uint16_t BMP388_DEV::getFIFOWatermark()															// Retrieve the FIFO watermark
{
	uint16_t fifoWatermark;
	readBytes(BMP388_FIFO_WTM_0, (uint8_t*)&fifoWatermark, sizeof(fifoWatermark));
	return fifoWatermark;
}

void BMP388_DEV::setFIFOPressEnable(PressEnable pressEnable)				// Enable pressure measurements
{
	fifo_config_1.bit.fifo_press_en = pressEnable;
	writeByte(BMP388_FIFO_CONFIG_1, fifo_config_1.reg);
}

void BMP388_DEV::setFIFOTimeEnable(TimeEnable timeEnable)						// Enable sensor time
{
	fifo_config_1.bit.fifo_time_en = timeEnable;
	writeByte(BMP388_FIFO_CONFIG_1, fifo_config_1.reg);
}

void BMP388_DEV::setFIFOSubsampling(Subsampling subsampling)				// Set the FIFO sub-sampling rate
{
	fifo_config_2.bit.fifo_subsampling = subsampling;
	writeByte(BMP388_FIFO_CONFIG_2, fifo_config_2.reg);
}

void BMP388_DEV::setFIFODataSelect(DataSelect dataSelect)						// Set if the FIFO data is unfiltered or filtered
{
	fifo_config_2.bit.data_select = dataSelect;
	writeByte(BMP388_FIFO_CONFIG_2, fifo_config_2.reg);
}

void BMP388_DEV::setFIFOStopOnFull(StopOnFull stopOnFull)						// Set if to stop reading the FIFO when full or overwrite data
{
	fifo_config_1.bit.fifo_stop_on_full = stopOnFull;
	writeByte(BMP388_FIFO_CONFIG_1, fifo_config_1.reg);
}

uint16_t BMP388_DEV::getFIFOLength()																// Get the FIFO length
{
	uint16_t fifoLength; 
	readBytes(BMP388_FIFO_LENGTH_0, (uint8_t*)&fifoLength, sizeof(fifoLength));
	return fifoLength;
}

FIFOStatus BMP388_DEV::getFIFOData(volatile double *temperature, volatile double *pressure, 		// Get FIFO data
																	 volatile double *altitude, volatile uint32_t &sensorTime)	
{
	if (!fifoReady())																									// Check if the FIFO data is ready
	{
		return DATA_PENDING;																						// Return if the measurements are pending
	}
	bool configError = false;																					// Set the configuration error flag
	uint16_t fifoLength = getFIFOLength() + fifo_config_1.bit.fifo_time_en + 		// Get the FIFO length plus sensor time bits if required
												3 * fifo_config_1.bit.fifo_time_en;
	uint16_t count = 0;																								// Initialise the byte count
	uint16_t measCount = 0;																						// Initialise the measurement count
	uint8_t packetSize = (fifo_config_1.bit.fifo_press_en | fifo_config_1.bit.fifo_temp_en) + 		// Determine the size of the data packets		
												3 * fifo_config_1.bit.fifo_press_en + 3 * fifo_config_1.bit.fifo_temp_en;
	uint8_t data[MAX_PACKET_SIZE];																		// Declare data packet memory
	while (count < fifoLength)																				// Parse the data until the FIFO is empty
	{
		readBytes(BMP388_FIFO_DATA, &data[0], packetSize);							// Acquire the next data packet
		uint8_t header = data[0];																				// Acquire the data header
		int32_t adcTemp, adcPres;																				// Declare the raw temperature and pressure variables
		switch(header)
		{
			case FIFO_SENSOR_PRESS:																				// Header indicates that temperature and pressure data follows
				adcTemp = (int32_t)data[3] << 16 | (int32_t)data[2] << 8 | (int32_t)data[1];  // Copy the temperature and pressure data into the adc variables
				temperature[measCount] = bmp388_compensate_temp((double)adcTemp);   					// Temperature compensation (function from BMP388 datasheet)
				adcPres = (int32_t)data[6] << 16 | (int32_t)data[5] << 8 | (int32_t)data[4];
				pressure[measCount] = bmp388_compensate_press((double)adcPres, temperature[measCount]); 	// Pressure compensation (function from BMP388 datasheet)
				pressure[measCount] /= 100.0f;
				if (alt_enable)																							// Check if altitude measurements have been enabled
				{
					altitude[measCount] = ((double)pow(sea_level_pressure / pressure[measCount], 0.190223f) - 1.0f) * 	// Calculate the altitude in metres 
						(temperature[measCount]  + 273.15f) / 0.0065f; 
				}
				count += 6;																									// Increment the byte count by the size of the data payload
				measCount++;																								// Increment the measurement count
				break;
			case FIFO_SENSOR_TEMP:																				// Header indicates that temperature data follows
				adcTemp = (int32_t)data[3] << 16 | (int32_t)data[2] << 8 | (int32_t)data[1];  // Copy the temperature and pressure data into the adc variables
				temperature[measCount] = bmp388_compensate_temp((double)adcTemp);   					// Temperature compensation (function from BMP388 datasheet)
				count += 3;																									// Increment the byte count by the size of the data payload
				measCount++;																								// Increment the measurement count
				break;
			case FIFO_SENSOR_TIME:																				// Header indicates that sensor time follows
				// Sensor time isn't actually stored in the FIFO, but is appended once the FIFO is read
				sensorTime = (uint32_t)data[3] << 16 | (uint32_t)data[1] << 8 | (uint32_t)data[1];	// Read the sensor time
				count += 3;																									// Increment the byte count by the size of the data payload
				break;
			case FIFO_CONFIG_CHANGE:																			// Header indicates that configuration change or FIFO empty data follows
			case FIFO_EMPTY:
				count++;																										// Increment the byte count
				break;
			case FIFO_CONFIG_ERROR:
				configError = true;																					// Set the configuration error flag
				count++;																										// Increment the byte count
				break;
			default:
				break;
		}
	}
	return configError ? CONFIG_ERROR : DATA_READY;										// Indicate that the measurements are ready, (or if a config error occured)
}

void BMP388_DEV::enableFIFOInterrupt(OutputDrive outputDrive, 			// Enable the BMP388's FIFO interrupts on the INT pin
																		 ActiveLevel activeLevel,
																		 LatchConfig latchConfig)
{
	int_ctrl.reg = latchConfig << 2 | activeLevel << 1 | outputDrive;
	int_ctrl.bit.fwtm_en = 1;
	int_ctrl.bit.ffull_en = 1;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);																 
}

void BMP388_DEV::disableFIFOInterrupt()															// Disable the FIFO interrupts on the INT pin
{
	int_ctrl.bit.fwtm_en = 0;
	int_ctrl.bit.ffull_en = 0;
	writeByte(BMP388_INT_CTRL, int_ctrl.reg);
}

void BMP388_DEV::flushFIFO()																				// Flush the FIFO
{
	writeByte(BMP388_CMD, FIFO_FLUSH);
}

uint32_t BMP388_DEV::getSensorTime()																// Get the sensor time
{
	uint32_t sensorTime;
	readBytes(BMP388_SENSORTIME_0, (uint8_t*)&sensorTime, sizeof(sensorTime - 1));
	return sensorTime & 0x00FFFFFF;
}

void BMP388_DEV::enableI2CWatchdog()																// Enable the I2C watchdog
{
	if_conf.bit.i2c_wdt_en = 1;
	writeByte(BMP388_IF_CONFIG, if_conf.reg);
}

void BMP388_DEV::disableI2CWatchdog()																// Disable the I2C watchdog
{
	if_conf.bit.i2c_wdt_en = 0;
	writeByte(BMP388_IF_CONFIG, if_conf.reg);
}

void BMP388_DEV::setI2CWatchdogTimout(WatchdogTimout watchdogTimeout)		// Set the I2C watchdog time-out: 1.25ms or 40ms
{
	if_conf.bit.i2c_wdt_sel = watchdogTimeout;
	writeByte(BMP388_IF_CONFIG, if_conf.reg);
}

uint8_t BMP388_DEV::getErrorReg()																		// Read the error register
{
	return readByte(BMP388_ERR_REG);
}

uint8_t BMP388_DEV::getStatusReg()																	// Read the status register
{
	return readByte(BMP388_STATUS);
}

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Private Member Functions
////////////////////////////////////////////////////////////////////////////////

void BMP388_DEV::setMode(Mode mode)																	// Set the BMP388's mode in the power control register
{
	pwr_ctrl.bit.mode = mode;
	writeByte(BMP388_PWR_CTRL, pwr_ctrl.reg);
}

void BMP388_DEV::setOversamplingRegister(Oversampling presOversampling,  // Set the BMP388 oversampling register 
																				 Oversampling tempOversampling)
{
	osr.reg = tempOversampling << 3 | presOversampling;
	writeByte(BMP388_OSR, osr.reg);                              
}

uint8_t BMP388_DEV::dataReady()																			// Check if measurement data is ready
{		
	if (pwr_ctrl.bit.mode == SLEEP_MODE)															// If we're in SLEEP_MODE return immediately
	{
		return 0;
	}
	int_status.reg = readByte(BMP388_INT_STATUS);											// Read the interrupt status register
	if (int_status.bit.drdy)																					// Check if the data ready flag has been cleared
	{
		if (pwr_ctrl.bit.mode == FORCED_MODE)					 									// If we're in FORCED_MODE switch back to SLEEP_MODE
		{		
			pwr_ctrl.bit.mode = SLEEP_MODE;	
		}
		return 1;																												// The measurement is ready
	}
	return 0;																													// The measurement is still pending
}

uint8_t BMP388_DEV::fifoReady()																			// Check if the FIFO data is ready
{		
	return dataReady() && int_status.bit.fwm_int ? 1 : 0;							// Return 1 if a FIFO measurement is ready, otherwise 0
}

////////////////////////////////////////////////////////////////////////////////
// Bosch BMP388_DEV (Private) Member Functions
////////////////////////////////////////////////////////////////////////////////

double BMP388_DEV::bmp388_compensate_temp(double uncomp_temp)
{
	double partial_data1 = uncomp_temp - doubleParams.param_T1;
	double partial_data2 = partial_data1 * doubleParams.param_T2;
	return partial_data2 + partial_data1 * partial_data1 * doubleParams.param_T3;	
}

double BMP388_DEV::bmp388_compensate_press(double uncomp_press, double t_lin)
{
	double partial_data1 = doubleParams.param_P6 * t_lin;
	double partial_data2 = doubleParams.param_P7 * t_lin * t_lin;
	double partial_data3 = doubleParams.param_P8 * t_lin * t_lin * t_lin;
	double partial_out1 = doubleParams.param_P5 + partial_data1 + partial_data2 + partial_data3;
	partial_data1 = doubleParams.param_P2 * t_lin;
	partial_data2 = doubleParams.param_P3 * t_lin * t_lin;
	partial_data3 = doubleParams.param_P4 * t_lin * t_lin * t_lin;
	double partial_out2 = uncomp_press * (doubleParams.param_P1 +
		partial_data1 + partial_data2 + partial_data3);
	partial_data1 = uncomp_press * uncomp_press;
	partial_data2 = doubleParams.param_P9 + doubleParams.param_P10 * t_lin;
	partial_data3 = partial_data1 * partial_data2;
	double partial_data4 = partial_data3 + uncomp_press * uncomp_press * uncomp_press * doubleParams.param_P11;
	return partial_out1 + partial_out2 + partial_data4;
}