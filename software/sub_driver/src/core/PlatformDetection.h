/**
 * @file PlatformDetection.h
 * @author Daniel Kim
 * @brief platform detection with macros
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifdef __AVR__
    #error "OceanAI requires ARM-based boards"
#elif defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    #error "OceanAI does not support ESP32 or ESP8266"
#endif





