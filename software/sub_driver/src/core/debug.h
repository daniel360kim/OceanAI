#ifndef debug_h
#define debug_h

#include <vector>
#include <Arduino.h>

#include "Timer.h"

#if DEBUG_ON
    #define ERROR_LOG(severity, message) Debug::error.addToBuffer(severity, message)
    #define SUCCESS_LOG(message) Debug::success.addToBuffer(Debug::Success, message)
    #define INFO_LOG(message) Debug::info.addToBuffer(Debug::Info, message)
#else
    #define SUCCESS_LOG(message) 
    #define ERROR_LOG(Severity, message)
    #define INFO_LOG(message)
#endif

namespace Debug
{
    //The severity of the message
    enum Severity
    {
        Success,
        Info,
        Trace,
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
        Message(Severity severity, const char* message) : timestamp(scoped_timer.elapsed()), severity(severity), message(message) {}
        ~Message() {}
        uint64_t timestamp;
        Severity severity;
        const char* message;
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
        void addToBuffer(Severity severity, const char* message);
        void printBuffer_vec();
       
    };
    extern Print error;
    extern Print info;
    extern Print success;   

};





#endif