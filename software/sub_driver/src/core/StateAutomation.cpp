/**
 * @file StateAutomation.cpp
 * @author Daniel Kim
 * @brief finite state automation driver
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "StateAutomation.h"
#include "States.h"

StateAutomation::StateAutomation()
{
}

void StateAutomation::setState(State& newState)
{
    m_currentState->exit(this);
    m_currentState = &newState;
    m_currentState->enter(this);
}

void StateAutomation::run()
{
    if(!m_initialized)
    {
        m_currentState = &Initialization::getInstance();
        m_currentState->enter(this);
        m_initialized = true;
    }
    m_currentState->run(this);
}

void StateAutomation::printState(Print &printer, CurrentState &state)
{
    uint8_t current_state = static_cast<uint8_t>(state);
    switch (current_state)
    {
    case 0:
        printer.print("Initialization");
        break;
    case 1:
        printer.print("Error Indication");
        break;
    case 2:
        printer.print("Idle Mode");
        break; 
    case 3:
        printer.print("Diving Mode");
        break;
    case 4:
        printer.print("Resurfacing");
        break;
    case 5:
        printer.print("Surfaced");
        break;
    case 6:
        printer.print("SD Translate");
        break;
    case 7: 
        printer.print("SD Reinitialize");
        break;
    default:
        printer.print("Unknown State");
        break;
    }
}