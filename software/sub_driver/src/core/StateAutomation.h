/**
 * @file StateAutomation.h
 * @author Daniel Kim
 * @brief finite state automation driver
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef STATE_AUTOMATION_H
#define STATE_AUTOMATION_H

namespace Fsm
{
    class StateAutomation;

    /**
     * @brief Abstract class that allows for reference to generic state w/o having to specify true type of state
     * 
     */
    class State
    {
    public:
        virtual void enter(StateAutomation* state) = 0;
        virtual void toggle(StateAutomation* state) = 0;
        virtual void exit(StateAutomation* state) = 0;
        virtual ~State() {}
    };

    class StateAutomation
    {
    public:
        StateAutomation();

        inline State* getCurrentState() { return m_currentState; }
        void toggle();
        void setState(State& newState);

    private:
        State* m_currentState;
    };
};

#endif