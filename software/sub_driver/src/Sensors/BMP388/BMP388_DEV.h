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

#ifndef BMP388_DEV_h
#define BMP388_DEV_h

#include "Device.h"

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Definitions
////////////////////////////////////////////////////////////////////////////////

#define BMP388_I2C_ADDR		 		0x77				// The BMP388 I2C address
#define BMP388_I2C_ALT_ADDR 	0x76				// The BMP388 I2C alternate address
#define BMP388_ID 						0x50				// The BMP388 device ID
#define BMP390_ID							0x60				// The BMP390 device ID
#define RESET_CODE						0xB6				// The BMP388 reset code
#define FIFO_FLUSH						0xB0				// The BMP388 flush FIFO code

enum SPIPort { BMP388_SPI0, BMP388_SPI1 };

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Registers
////////////////////////////////////////////////////////////////////////////////

enum {
	BMP388_CHIP_ID				 = 0x00,					// Chip ID register sub-address
	BMP388_ERR_REG				 = 0x02,					// Error register sub-address
	BMP388_STATUS					 = 0x03,					// Status register sub-address
	BMP388_DATA_0					 = 0x04,					// Pressure eXtended Least Significant Byte (XLSB) register sub-address
	BMP388_DATA_1					 = 0x05,					// Pressure Least Significant Byte (LSB) register sub-address
	BMP388_DATA_2					 = 0x06,					// Pressure Most Significant Byte (MSB) register sub-address
	BMP388_DATA_3					 = 0x07,					// Temperature eXtended Least Significant Byte (XLSB) register sub-address
	BMP388_DATA_4					 = 0x08,					// Temperature Least Significant Byte (LSB) register sub-address
	BMP388_DATA_5					 = 0x09,					// Temperature Most Significant Byte (MSB) register sub-address
	BMP388_SENSORTIME_0		 = 0x0C,					// Sensor time register 0 sub-address
	BMP388_SENSORTIME_1		 = 0x0D,					// Sensor time register 1 sub-address
	BMP388_SENSORTIME_2		 = 0x0E,					// Sensor time register 2 sub-address
	BMP388_EVENT					 = 0x10,					// Event register sub-address
	BMP388_INT_STATUS			 = 0x11,					// Interrupt Status register sub-address
	BMP388_FIFO_LENGTH_0	 = 0x12,					// FIFO Length Least Significant Byte (LSB) register sub-address
	BMP388_FIFO_LENGTH_1	 = 0x13,					// FIFO Length Most Significant Byte (MSB) register sub-address
	BMP388_FIFO_DATA			 = 0x14,					// FIFO LoggedData register sub-address
	BMP388_FIFO_WTM_0			 = 0x15,					// FIFO Water Mark Least Significant Byte (LSB) register sub-address
	BMP388_FIFO_WTM_1			 = 0x16,					// FIFO Water Mark Most Significant Byte (MSB) register sub-address
	BMP388_FIFO_CONFIG_1   = 0x17,					// FIFO Configuration 1 register sub-address
	BMP388_FIFO_CONFIG_2   = 0x18,					// FIFO Configuration 2 register sub-address
	BMP388_INT_CTRL				 = 0x19,					// Interrupt Control register sub-address
	BMP388_IF_CONFIG			 = 0x1A,					// Interface Configuration register sub-address
	BMP388_PWR_CTRL				 = 0x1B,					// Power Control register sub-address
	BMP388_OSR						 = 0x1C,					// Oversampling register sub-address
	BMP388_ODR					   = 0x1D,					// Output LoggedData Rate register sub-address
	BMP388_CONFIG					 = 0x1F,					// Configuration register sub-address
	BMP388_TRIM_PARAMS		 = 0x31,          // Trim parameter registers' base sub-address
	BMP388_CMD						 = 0x7E						// Command register sub-address
};          

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Modes
////////////////////////////////////////////////////////////////////////////////

enum Mode {
	SLEEP_MODE          	 = 0x00,          // Device mode bitfield in the control and measurement register 
	FORCED_MODE         	 = 0x01,
	NORMAL_MODE         	 = 0x03
};

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Register bit field Definitions
////////////////////////////////////////////////////////////////////////////////

enum Oversampling {
	OVERSAMPLING_SKIP 		 = 0x00,     			// Oversampling bit fields in the control and measurement register
	OVERSAMPLING_X2   		 = 0x01,
	OVERSAMPLING_X4  		   = 0x02,
	OVERSAMPLING_X8    		 = 0x03,
	OVERSAMPLING_X16   	 	 = 0x04,
	OVERSAMPLING_X32   	 	 = 0x05
};

enum IIRFilter {
	IIR_FILTER_OFF  			 = 0x00,     			// Infinite Impulse Response (IIR) filter bit field in the configuration register
	IIR_FILTER_2    			 = 0x01,
	IIR_FILTER_4           = 0x02,
	IIR_FILTER_8           = 0x03,
	IIR_FILTER_16          = 0x04,
	IIR_FILTER_32          = 0x05,
	IIR_FILTER_64          = 0x06,
	IIR_FILTER_128         = 0x07
};

enum TimeStandby {
	TIME_STANDBY_5MS       = 0x00,      		 // Time standby bit field in the Output LoggedData Rate (ODR) register
	TIME_STANDBY_10MS      = 0x01,
	TIME_STANDBY_20MS      = 0x02,
	TIME_STANDBY_40MS      = 0x03,
	TIME_STANDBY_80MS      = 0x04,
	TIME_STANDBY_160MS     = 0x05,
	TIME_STANDBY_320MS     = 0x06,
	TIME_STANDBY_640MS     = 0x07,
	TIME_STANDBY_1280MS    = 0x08,
	TIME_STANDBY_2560MS    = 0x09,
	TIME_STANDBY_5120MS    = 0x0A,
	TIME_STANDBY_10240MS   = 0x0B,
	TIME_STANDBY_20480MS   = 0x0C,
	TIME_STANDBY_40960MS   = 0x0D,
	TIME_STANDBY_81920MS   = 0x0E,
	TIME_STANDBY_163840MS  = 0x0F,
	TIME_STANDBY_327680MS  = 0x10,
	TIME_STANDBY_655360MS  = 0x11
};

enum OutputDrive {												// Interrupt output drive configuration
	PUSH_PULL 						 = 0x00,
	OPEN_COLLECTOR				 = 0x01
};

enum ActiveLevel {												// Interrupt output active level configuration
	ACTIVE_LOW  		 			 = 0x00,
	ACTIVE_HIGH			 			 = 0x01
};

enum LatchConfig {												// Interrupt output latch configuration
	UNLATCHED							 = 0x00,					// UNLATCHED: interrupt automatically clears after 2.5ms
	LATCHED								 = 0x01						// LATCHED	: interrupt requires INT_STATUS register read to clear
};
	
enum PressEnable {												// FIFO pressure enable configuration
	PRESS_DISABLED			 	 = 0x00,
	PRESS_ENABLED			 		 = 0x01
};

enum AltEnable {													// FIFO altitude enable configuration
	ALT_DISABLED					 = 0x00,
	ALT_ENABLED					 	 = 0x01
};
	
enum TimeEnable {													// FIFO time enable configuration
	TIME_DISABLED					 = 0x00,
	TIME_ENABLED					 = 0x01
};	

enum Subsampling {												// FIFO sub-sampling configuration
	SUBSAMPLING_OFF				 = 0x00,
	SUBSAMPLING_DIV2			 = 0x01,
	SUBSAMPLING_DIV4			 = 0x02,
	SUBSAMPLING_DIV8			 = 0x03,
	SUBSAMPLING_DIV16			 = 0x04,
	SUBSAMPLING_DIV32			 = 0x05,
	SUBSAMPLING_DIV64			 = 0x06,
	SUBSAMPLING_DIV128		 = 0x07
};

enum DataSelect {													// FIFO data select configuration
	UNFILTERED						 = 0x00,
	FILTERED							 = 0x01
};

enum StopOnFull {													// FIFO stop on full configuration
	STOP_ON_FULL_DISABLED	 = 0x00,
	STOP_ON_FULL_ENABLED 	 = 0x01
};

enum FIFOStatus {													// FIFO status
	DATA_PENDING			 		 = 0x00,
	DATA_READY				 		 = 0X01,
	CONFIG_ERROR			 		 = 0x02
};

enum WatchdogTimout {											// I2C watchdog time-out
	WATCHDOG_TIMEOUT_1MS	 = 0x00,
	WATCHDOG_TIMEOUT_40MS	 = 0x01
};
	
////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Class declaration
////////////////////////////////////////////////////////////////////////////////

class BMP388_DEV : public Device {															// Derive the BMP388_DEV class from the Device class
	public:
		BMP388_DEV();																								// BMP388_DEV object for I2C operation
#ifdef ARDUINO_ARCH_ESP8266
		BMP388_DEV(uint8_t sda, uint8_t scl);												// BMP388_DEV object for ESP8266 I2C operation with user-defined pins
#endif
		BMP388_DEV(uint8_t cs);																			// BMP388_DEV object for SPI operation
#ifdef ARDUINO_ARCH_ESP32
		BMP388_DEV(uint8_t sda, uint8_t scl);												// BMP388_DEV object for ESP32 I2C operation with user-defined pins
		BMP388_DEV(uint8_t cs, uint8_t spiPort, SPIClass& spiClass);	// BMP388_DEV object for SPI1 with supplied SPIClass object
#endif
		uint8_t begin(Mode mode = SLEEP_MODE, 												// Initialise the barometer with arguments
									Oversampling presOversampling = OVERSAMPLING_X16, 
									Oversampling tempOversampling = OVERSAMPLING_X2, 
									IIRFilter iirFilter = IIR_FILTER_OFF, 
									TimeStandby timeStandby = TIME_STANDBY_5MS);
		uint8_t begin(Mode mode, uint8_t addr);											// Initialise the barometer specifying start mode and I2C addrss
		uint8_t begin(uint8_t addr);																// Initialise the barometer specifying I2C address with default initialisation
		uint8_t reset();																						// Soft reset the barometer		
		void startNormalConversion();																// Start continuous measurement in NORMAL_MODE
		void startForcedConversion();															  // Start a one shot measurement in FORCED_MODE
		void stopConversion();																			// Stop the conversion and return to SLEEP_MODE
		void setPresOversampling(Oversampling presOversampling);		// Set the pressure oversampling: OFF, X1, X2, X4, X8, X16, X32
		void setTempOversampling(Oversampling tempOversampling);		// Set the temperature oversampling: OFF, X1, X2, X4, X8, X16, X32
		void setIIRFilter(IIRFilter iirFilter);											// Set the IIR filter setting: OFF, 2, 3, 8, 16, 32
		void setTimeStandby(TimeStandby timeStandby);	 							// Set the time standby measurement interval: 5, 62, 125, 250, 500ms, 1s, 2s, 4s
		void setSeaLevelPressure(double pressure = 1013.23f);				// Set the sea level pressure value
		uint8_t getTemperature(volatile double &temperature);				// Get a temperature measurement
		uint8_t getPressure(volatile double &pressure);							// Get a pressure measurement
		uint8_t getTempPres(volatile double &temperature, 						// Get a temperature and pressure measurement
												volatile double &pressure);		
		uint8_t getAltitude(volatile double &altitude);							// Get an altitude measurement
		uint8_t getMeasurements(volatile double &temperature, 				// Get temperature, pressure and altitude measurements
														volatile double &pressure, 
														volatile double &altitude);	
		void enableInterrupt(OutputDrive outputDrive = PUSH_PULL, 	// Enable the BMP388's interrupt pin
												 ActiveLevel activeLevel = ACTIVE_HIGH,
												 LatchConfig latchConfig = UNLATCHED);
		void disableInterrupt();																		// Disable the BMP388's interrupt pin
		void setIntOutputDrive(OutputDrive outputDrive);						// Sets the interrupt pin's output drive, PUSH_PULL OR OPEN_DRAIN, default PUSH_PULL
		void setIntActiveLevel(ActiveLevel activeLevel);						// Set the interrupt active level, ACTIVE_LOW or ACTIVE_HIGH, default ACTIVE_HIGH
		void setIntLatchConfig(LatchConfig latchConfig);						// Set the interrupt latch, UNLATCHED or LATCHED, default UNLATCHED													
		void enableFIFO(PressEnable pressEnable = PRESS_ENABLED, 		// Enable the FIFO 
										AltEnable altEnable = ALT_ENABLED,
										TimeEnable timeEnable = TIME_ENABLED,
										Subsampling subsampling = SUBSAMPLING_OFF,
										DataSelect dataSelect = FILTERED,
										StopOnFull stopOnFull = STOP_ON_FULL_ENABLED);
		void disableFIFO();																					// Disable the FIFO
		uint8_t setFIFONoOfMeasurements(uint16_t noOfMeaurements);  // Calculate the FIFO watermark from the number of measurements
		uint8_t setFIFOWatermark(uint16_t fifoWatermark);						// Set the FIFO watermark directly
		uint16_t getFIFOWatermark();																// Get the FIFO watermark
		void setFIFOPressEnable(PressEnable pressEnable);						// Set the FIFO pressure enable configuration
		void setFIFOTimeEnable(TimeEnable timeEnable);							// Set the FIFO time enable configuration
		void setFIFOSubsampling(Subsampling subsampling);						// Set the FIFO sub-sampling rate
		void setFIFODataSelect(DataSelect dataSelect);							// Set the if the FIFO data is unfiltered or filtered
		void setFIFOStopOnFull(StopOnFull stopOnFull);							// Set the FIFO stop on full configuration
		uint16_t getFIFOLength();																		// Get the FIFO length	
		FIFOStatus getFIFOData(volatile double *temperature, 				// Get FIFO data 
													 volatile double *pressure, 		
													 volatile double *altitude, 
													 volatile uint32_t &sensorTime);	
		void enableFIFOInterrupt(OutputDrive outputDrive = PUSH_PULL, 	// Enable FIFO interrupt
														 ActiveLevel activeLevel = ACTIVE_HIGH,
														 LatchConfig latchConfig = UNLATCHED);												
		void disableFIFOInterrupt();																// Disable FIFO interrupt
		void flushFIFO();																						// Flush the FIFO
		uint32_t getSensorTime();																		// Get the BMP388 sensor time
		void enableI2CWatchdog();																		// Enable the I2C watchdog timer
		void disableI2CWatchdog();																	// Disable the I2C watchdog timer
		void setI2CWatchdogTimout(WatchdogTimout watchdogTimeout);	// Set the watchdog time-out: 1.25ms or 40ms 
		uint8_t getErrorReg();																			// Read the error register
		uint8_t getStatusReg();																			// Read the status register
	protected:
	private:
		void setMode(Mode mode);																		// Set the barometer mode
		void setOversamplingRegister(Oversampling presOversampling, // Set the BMP388 oversampling register
																 Oversampling tempOversamping);		
		uint8_t dataReady();																				// Checks if a measurement is ready
		uint8_t fifoReady();																				// Checks if the data FIFO is ready
		
		enum HeaderCode {																						// FIFO header codes
			FIFO_EMPTY					   = 0x80,
			FIFO_CONFIG_ERROR			 = 0x44,
			FIFO_CONFIG_CHANGE		 = 0x48,
			FIFO_SENSOR_TIME			 = 0xA0,
			FIFO_SENSOR_TEMP			 = 0x90,
			FIFO_SENSOR_PRESS			 = 0x94
		};
		
		struct {																										// The BMP388 compensation trim parameters (coefficients)
			uint16_t param_T1;
			uint16_t param_T2;
			int8_t   param_T3;
			int16_t  param_P1;
			int16_t  param_P2;
			int8_t   param_P3;
			int8_t   param_P4;
			uint16_t param_P5;
			uint16_t param_P6;
			int8_t   param_P7;
			int8_t   param_P8;
			int16_t  param_P9;
			int8_t 	 param_P10;
			int8_t 	 param_P11;
		} __attribute__ ((packed)) params;
		
		struct doubleParams {																				// The BMP388 double point compensation trim parameters
			double param_T1;
			double param_T2;
			double param_T3;
			double param_P1;
			double param_P2;
			double param_P3;
			double param_P4;
			double param_P5;
			double param_P6;
			double param_P7;
			double param_P8;
			double param_P9;
			double param_P10;
			double param_P11;
		} doubleParams;
		
		union {																											// Copy of the BMP388's chip id register
			struct {
				uint8_t chip_id_nvm		: 4;
				uint8_t chip_id_fixed : 4;
			} bit;
			uint8_t reg;
		} chip_id = { .reg = 0 };
				
		/*union {																											// Copy of the BMP388's error register
			struct {
				uint8_t fatal_err : 1;
				uint8_t cmd_err		: 1;
				uint8_t ctrl_err	: 1;
			} bit;
			uint8_t reg;
		} err_reg = { .reg = 0 };
			
		union {																											// Copy of the BMP388's status register
			struct {
				uint8_t 					: 4;
				uint8_t cmd_rdy 	: 1;
				uint8_t press_rdy	: 1;
				uint8_t temp_rdy 	: 1;
			} bit;
			uint8_t reg;
		} status = { .reg = 0 };*/
			
		union {																											// Copy of the BMP388's event register
			struct {
				uint8_t por_detected : 1;
			} bit;
			uint8_t reg;
		} event = { .reg = 0 };
		
		volatile union {																						// Copy of the BMP388's interrupt status register
			struct {
				uint8_t fwm_int		: 1;
				uint8_t ffull_int : 1;
				uint8_t 					: 1;
				uint8_t drdy			: 1;
			} bit;
			uint8_t reg;
		} int_status = { .reg = 0 };
		
		union {																											// Copy of the BMP388's FIFO configuration register 1
			struct {
				uint8_t fifo_mode					: 1;
				uint8_t fifo_stop_on_full : 1;
				uint8_t fifo_time_en			: 1;
				uint8_t fifo_press_en			: 1;
				uint8_t fifo_temp_en			: 1;
			} bit;
			uint8_t reg;
		} fifo_config_1 = { .reg = 0 };
		
		union {																											// Copy of the BMP388's FIFO configuration register 2
			struct {
				uint8_t fifo_subsampling : 3;
				uint8_t data_select			 : 3;
			} bit;
			uint8_t reg;
		} fifo_config_2 = { .reg = 0 };
		
		union {																											// Copy of the BMP388's interrupt control register
			struct {
				uint8_t int_od		: 1;
				uint8_t int_level : 1;
				uint8_t int_latch : 1;
				uint8_t fwtm_en		: 1;
				uint8_t ffull_en	: 1;
				uint8_t 					: 1;
				uint8_t drdy_en		: 1;
			} bit;
			uint8_t reg;
		} int_ctrl = { .reg = 0 };
		
		union {																											// Copy of the BMP388's interface configuration register
			struct {
				uint8_t spi3				: 1;
				uint8_t i2c_wdt_en	: 1;
				uint8_t i2c_wdt_sel : 1;
			} bit;
			uint8_t reg;
		} if_conf = { .reg = 0 };
		
		volatile union {																						// Copy of the BMP388's power control register
			struct {
				uint8_t press_en : 1;
				uint8_t temp_en	 : 1;
				uint8_t 				 : 2;
				uint8_t mode		 : 2;
			} bit;
			uint8_t reg;
		} pwr_ctrl = { .reg = 0 };
		
		union {																											// Copy of the BMP388's oversampling register
			struct {
				uint8_t osr_p : 3;
				uint8_t osr_t : 3;
			} bit;
			uint8_t reg;
		} osr = { .reg = 0 };
		
		union {																											// Copy of the BMP388's output data rate register
			struct {
				uint8_t odr_sel : 5;
			} bit;
			uint8_t reg;
		} odr = { .reg = 0 };
		
		union {																											// Copy of the BMP388's configuration register
			struct {
				uint8_t 					 : 1;
				uint8_t iir_filter : 3;
			} bit;
			uint8_t reg;
		} config = { .reg = 0 };
			
		double bmp388_compensate_temp(double uncomp_temp); 						// Bosch temperature compensation function																			
		double bmp388_compensate_press(double uncomp_press, double t_lin); 		// Bosch pressure compensation function		
		volatile bool alt_enable;																		// Altitude enable flag
		const uint16_t FIFO_SIZE = 0x01FF;													// The BMP388 FIFO size 512 bytes
		const uint8_t MAX_PACKET_SIZE = 7;													// The BMP388 maximum FIFO packet size in bytes
		double sea_level_pressure = 1013.23f;												// Pressure at sea level
};
#endif