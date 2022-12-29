/**
 * @file pins.h
 * @author Daniel Kim
 * @brief Pin definitions for the main board
 * @version 1
 * @date 2022-06-12
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

/************************************************************************************
 * Teensy 4.1 (IMXRT1062) Pin Assignments on OceanAI main board
 * https://www.pjrc.com/teensy/teensyduino.html
************************************************************************************/
#ifndef pins_h
#define pins_h

#include <Arduino.h>
#include <stdint.h>

///// Stepper Motors /////
//Buoyancy Engine
constexpr uint8_t RST_b = 21;
constexpr uint8_t DIR_b = 24;
constexpr uint8_t STP_b = 25;
constexpr uint8_t SLP_b = 26;

constexpr uint8_t MS1_b = 9;
constexpr uint8_t MS2_b = 7;
constexpr uint8_t ERR_b = 8;

//Pitexprch Manipulation Engine
constexpr uint8_t RST_p = 40;
constexpr uint8_t DIR_p = 38;
constexpr uint8_t STP_p = 39;
constexpr uint8_t SLP_p = 41;

constexpr uint8_t MS1_p = 28;
constexpr uint8_t MS2_p = 29;
constexpr uint8_t ERR_p = 30;

/////expr Limit Switches /////
constexpr uint8_t STOP_b = 32; //buoyancy engine limit switch
constexpr uint8_t STOP_p = 31; //pitch manipulation limit switch

/////expr Sensor (I2C) Interrupts (LoggedData-ready only)
constexpr uint8_t ACC_int = 34; //BMI088 Accelerometer
constexpr uint8_t GYR_int = 33; //BMI088 Gyroscope
constexpr uint8_t BAR_int = 35; //BMP388 Barometer
constexpr uint8_t MAG_int = 36; //LIS3MDLTR Magnetometer 

/////expr Indication /////
//RGBexpr A
constexpr uint8_t RED_a = 1;
constexpr uint8_t GRN_a = 0;
constexpr uint8_t BLU_a = 2;

//RGBexpr B
constexpr uint8_t RED_b = 23;
constexpr uint8_t GRN_b = 22;
constexpr uint8_t BLU_b = 15;

//Sigexprnal
constexpr uint8_t SIGNAL = 5;

//Buzexprzer
constexpr uint8_t BUZZ = 14;

/////expr Voltage Reading /////
constexpr uint8_t v_div = 16;

/////expr Total Dissolved Sensor ADC /////
constexpr uint8_t TDS = 17;

/////expr Comms /////
//RFexpr
constexpr uint8_t RX_RF = 3;
constexpr uint8_t TX_RF = 4;
constexpr uint8_t SET = 6;

//Serial comms
constexpr uint8_t RX_5 = 21;
constexpr uint8_t TX_5 = 20;

/////expr SPI /////
constexpr uint8_t CS_VD = 10;
constexpr uint8_t CS = 37;

/**
 * @brief Pins of the TMC2208 stepper driver
 * 
 */
struct StepperPins
{
    uint8_t STP;
    uint8_t DIR;
    uint8_t MS1;
    uint8_t MS2;
    uint8_t ERR;
    uint8_t limit;
};



#endif