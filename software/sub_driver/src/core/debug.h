#ifndef debug_h
#define debug_h

#include <vector>
#include <Arduino.h>

#include "Timer.h"
#define SUCCESS_LOG(message) Debug::success.addToBuffer(Debug::Success, message); Serial.println(F(message));
#define ERROR_LOG(Severity, message) Debug::error.addToBuffer(Severity, message); Serial.println(F(message));

namespace Debug
{
    //The severity of the message
    enum Severity
    {
        Success, //for success messages
        Info, //for information messages
        Warning, //Something might go wrong
        Fatal, //A non critical component won't work (GPS, RF etc.)
        Critical_Error //A critica component won't work (IMU, Stepper motors etc.)
    };
    
    /**
     * @brief message to be logged
     * 
     */
    struct Message
    {
        Message(Severity severity, char* message) : timestamp(scoped_timer.elapsed()), severity(severity), message(message) {}
        uint64_t timestamp;
        Severity severity;
        char* message;
    };

    /**
     * @brief buffers and prints the debug messages
     * 
     */
    class Print
    {
    public:
        Print() {}
        std::vector<Message> printBuffer;
        void addToBuffer(Severity severity, char* message);
        void printBuffer_vec();
       
    };
    extern Print error;
    extern Print success;   

};





#endif