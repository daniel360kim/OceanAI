/**
 * @file debug.h
 * @author Daniel Kim
 * @brief debugging macros
 * @version 0.1
 * @date 2022-12-25 (christmas yay!)
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <cstdint>
#include <string>
#include <vector>

#include "timer.h"
#include "configuration.h"

/**
 * @brief What type of message is it?
 * 
 */
enum class Severity
{
    SUCCESS, //success messages
    INFO, //info messages
    WARNING, //warning messages
    ERROR, //error messages
};

/**
 * @brief Holds information for debugging
 * 
 */
class DebugMessage
{
public:
    DebugMessage() {}
    ~DebugMessage() {}

    void createMessagef(Severity severity, std::string message, ...);
    void createMessage(Severity severity, std::string message);
    void SetTimestamp(int64_t timestamp);
    void SetSeverity(Severity severity);
    void SetMessage(std::string message);

    int64_t GetTimestamp() const;
    int64_t GetTimestampMillis() const;
    int32_t GetTimestampSeconds() const;
    Severity GetSeverity() const;
    std::string GetMessage() const;

    void appendMessage(std::string message);

private:
    int64_t m_Timestamp;
    Severity m_Severity;
    std::string m_Message;
};


class Debug
{
public:
    Debug() {}
    Debug(int maxMessages = 100) : m_maxMessages(maxMessages) {}
    ~Debug() {}

    void addMessage(DebugMessage message);
    void printMessages(bool deleteMessages = false);
    void printMessages(int numMessages, bool deleteMessages = false);

    void clearMessages();

private:
    std::vector<DebugMessage> m_Messages;
    unsigned int m_maxMessages;
};

extern Debug error; // error messages
extern Debug info; // info messages
extern Debug success; // success messages

#if DEBUG_ON
    extern DebugMessage debugMessage;
    #define ERROR_LOGf(severity, message, ...) debugMessage.createMessagef(severity, message, __VA_ARGS__); error.addMessage(debugMessage);
    #define INFO_LOGf(message, ...) debugMessage.createMessagef(Severity::INFO, message, __VA_ARGS__); info.addMessage(debugMessage);
    #define SUCCESS_LOGf(message, ...) debugMessage.createMessagef(Severity::SUCCESS, message, __VA_ARGS__); success.addMessage(debugMessage);

    #define ERROR_LOG(severity, message) debugMessage.createMessage(severity, message); error.addMessage(debugMessage);
    #define INFO_LOG(message) debugMessage.createMessage(Severity::INFO, message); info.addMessage(debugMessage);
    #define SUCCESS_LOG(message) debugMessage.createMessage(Severity::SUCCESS, message); success.addMessage(debugMessage);
#else
    #define ERROR_LOG(severity, message, ...)
    #define INFO_LOG(message, ...)
    #define SUCCESS_LOG(message, ...)
#endif


#endif