/**
 * @file States.h
 * @author Daniel Kim
 * @brief finite state automation driver - states classes
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "StateAutomation.h"

namespace Fsm
{
    class State1 : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static State1& getInstance()
        {
            static State1 instance;
            return instance;
        }
    private:
        State1() {}
        State1(const State1& other);
        State1& operator=(const State1& other);
    };

    class State2 : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static State2& getInstance()
        {
            static State2 instance;
            return instance;
        }
    private:
        State2() {}
        State2(const State2& other);
        State2& operator=(const State2& other);
    };

    class State3 : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static State3& getInstance()
        {
            static State3 instance;
            return instance;
        }
    private:
        State3() {}
        State3(const State3& other);
        State3& operator=(const State3& other);
    };
    
    class State4 : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static State4& getInstance()
        {
            static State4 instance;
            return instance;
        }
    private:       
        State4() {}
        State4(const State4& other);
        State4& operator=(const State4& other);
    };
};
       
       
