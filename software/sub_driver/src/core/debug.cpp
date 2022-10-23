#include <Arduino.h>
#include <vector>
#include <memory>

#include "debug.h"
#include "config.h"

#include "Timer.h"

namespace Debug
{

    Print error;
    Print success;
    Print info;

    void Print::addToBuffer(Severity severity, const char* message)
    {
        std::unique_ptr<Message> msg = std::make_unique<Message>(severity, message);

        printBuffer.push_back(*msg);
        #if LIVE_DEBUG
            Serial.println(message);
        #endif
    }

    void Print::printBuffer_vec()
    {
        
        
        for(unsigned int i = 0; i < printBuffer.size(); i++)
        {   
            Serial.print(printBuffer[i].timestamp); Serial.print("\t");
            
            switch(printBuffer[i].severity)
            {
                case Success:
                    Serial.print(F("Success:"));
                    break;

                case Warning:
                    Serial.print(F("Warning:"));
                    break;
                
                case Fatal:
                    Serial.print(F("Fatal:"));
                    break;
                
                case Critical_Error:
                    Serial.print(F("Critical Error:"));
                    break;
                
                default:
                    Serial.print(F("|No Flag|"));
            }

            Serial.print(" ");
            Serial.println(printBuffer[i].message);
        }
        
    }

};