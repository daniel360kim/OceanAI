/**
 * @file States.cpp
 * @author Daniel Kim
 * @brief finite state automation driver - states classes
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "States.h"

void Fsm::State1::toggle(StateAutomation* state)
{
    state->setState(State2::getInstance());
}

void Fsm::State2::toggle(StateAutomation* state)
{
    state->setState(State3::getInstance());
}

void Fsm::State3::toggle(StateAutomation* state)
{
    state->setState(State4::getInstance());
}

void Fsm::State4::toggle(StateAutomation* state)
{
    state->setState(State1::getInstance());
}