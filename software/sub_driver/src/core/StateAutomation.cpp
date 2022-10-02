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

void StateAutomation::initialize()
{
    m_currentState = &Initialization::getInstance();
    m_currentState->enter(this);
}

void StateAutomation::setState(State& newState)
{
    m_currentState->exit(this);
    m_currentState = &newState;
    m_currentState->enter(this);
}

void StateAutomation::run()
{
    m_currentState->run(this);
}
