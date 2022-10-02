#include <Arduino.h>
#include <vector>

#include "debug.h"

#include "Timer.h"
#define SUCCESS_LOG(message) Debug::success.addToBuffer(scoped_timed.elapsed(), Debug::Success, message)
#define ERROR_LOG(...) Debug::error.addToBuffer(scoped_timer.elapsed(), __VA_ARGS__)

namespace Debug
{

Print error;
Print success;

void Print::addToBuffer(Severity severity, char* message)
{
    Message *metadata = new Message(severity, message);

    printBuffer.push_back(*metadata);

    delete metadata;

    
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