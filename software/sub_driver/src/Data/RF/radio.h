/**
 * @file radio.h
 * @author Daniel Kim
 * @brief transmitting data
 * @version 0.1
 * @date 2022-07-02
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef Radio_h
#define Radio_h

#include <RF24.h>
#include <RF24Network.h>

#include <SPI.h>
#include <Arduino.h>

#include "../../pins.h"
#include "../data_struct.h"
#include "../../config.h"
#include "../../debug.h"

RF24 radio(CS, SET);
RF24Network network(radio);

#define nodeID 1

const uint16_t this_node = 01;   // Address of our node in Octal format
const uint16_t other_node = 00;  // Address of the other node in Octal format

class Radio
{
public:
    
    Radio(unsigned long log_freq_micros) : log_freq(log_freq_micros) {}
    bool init()
    {
        if(!radio.begin())
        {
            #if DEBUG_ON == true
                char* message = (char*)"RF24 Network: RF Initialization Failed";

                Debug::error.addToBuffer(micros(), Debug::Fatal, message);
                #if LIVE_DEBUG == true
                    Serial.println(F(message));
                #endif

            #endif

            return false;
        }
        else
        {
            #if DEBUG_ON == true
                char* message = (char*)"RF24 Network: RF Initialization Successful";

                Debug::success.addToBuffer(micros(), Debug::Success, message);
                #if LIVE_DEBUG == true
                    Serial.println(F(message));
                #endif
                
            #endif
        }

        radio.setChannel(90);
        network.begin(this_node);
        radio.setAutoAck(1);
        radio.enableAckPayload();
        radio.setPALevel(RF24_PA_HIGH);

        return true;
    }
    
    /**
     * @brief How often do we send data
     * 
     * @param log_freq_micros frequency in us
     */
    void setLogFreq(unsigned long log_freq_micros)
    {
        log_freq = log_freq_micros;
    }

    /**
     * @brief sends an object through RF
     * 
     * @tparam T data type of object
     * @param data the object to be transmitted
     */
    template <typename T>
    bool writeData(T data)
    {
        network.update();
        
        RF24NetworkHeader header(other_node, 1);

        
        unsigned long long current_micros = micros();
        if(current_micros - previousLog >= log_freq)
        {
            if(!network.write(header, &data, sizeof(data)))
            {
                #if DEBUG_ON == true
                    char* message = (char*)"RF24 Network: Error sending packet";

                    Debug::error.addToBuffer(micros(), Debug::Fatal, message);
                    #if LIVE_DEBUG == true
                        //Serial.println(F(message));
                    #endif
                    
                #endif

                return false;
            }

            else
            {
                #if DEBUG_ON == true
                    char* message = (char*)"RF24 Network: Packet sent successfully";

                    Debug::success.addToBuffer(micros(), Debug::Success, message);
                    #if LIVE_DEBUG == true
                    Serial.println(F(message));
                    #endif
                    
                #endif
            }
       
            previousLog = current_micros;
            
        }
           
        return true;
        
    }


private:
    unsigned long long previousLog;
    unsigned long log_freq = 500000;

    static int count;
    
};


#endif