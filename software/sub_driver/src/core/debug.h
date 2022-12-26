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

#if DEBUG_ON
    #define ERROR_LOG(severity, message, ...) error.addMessage(DebugMessage(severity, message, ##__VA_ARGS__))
    #define INFO_LOG(message, ...) info.addMessage(DebugMessage(Severity::INFO, message, ##__VA_ARGS__))
    #define SUCCESS_LOG(message, ...) success.addMessage(DebugMessage(Severity::SUCCESS, message, ##__VA_ARGS__))
#else
    #define ERROR_LOG(severity, message, ...)
    #define INFO_LOG(message, ...)
    #define SUCCESS_LOG(message, ...)
#endif


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
    DebugMessage(Severity severity, std::string message) : m_Timestamp(scoped_timer.elapsed()), m_Severity(severity), m_Message(message) {}
    DebugMessage(Severity severity, std::string message, ...);
    ~DebugMessage() {}

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
    int m_maxMessages;
};

extern Debug error; // error messages
extern Debug info; // info messages
extern Debug success; // success messages


#endif