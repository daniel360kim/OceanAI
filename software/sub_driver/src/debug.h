#ifndef debug_h
#define debug_h

#include <vector>
#include <Arduino.h>

namespace Debug
{
    //The severity of the message
    enum Severity
    {
        Success, //for success messages
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
        Message(uint64_t timestamp, Severity severity, char* message) : timestamp(timestamp), severity(severity), message(message) {}
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
        void addToBuffer(uint64_t timestamp, Severity severity, char* message);
        void printBuffer_vec();
       
    };
    extern Print error;
    extern Print success;   

};





#endif