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
    class Idle : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static Idle& getInstance()
        {
            static Idle instance;
            return instance;
        }
    private:
        Idle() {}
        Idle(const Idle& other);
        Idle& operator=(const State1& other);
    };

    class Diving : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static Diving& getInstance()
        {
            static Diving instance;
            return instance;
        }
    private:
        Diving() {}
        Diving(const Diving& other);
        Diving& operator=(const Diving& other);
    };

    class Resurfacing : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static Resurfacing& getInstance()
        {
            static Resurfacing instance;
            return instance;
        }
    private:
        Resurfacing() {}
        Resurfacing(const Resurfacing& other);
        Resurfacing& operator=(const Resurfacing& other);
    };
    
    class TranslatingData : public State
    {
    public:
        void enter(StateAutomation* state) {}
        void toggle(StateAutomation* state);
        void exit(StateAutomation* state) {}
        static TranslatingData& getInstance()
        {
            static TranslatingData instance;
            return instance;
        }
    private:       
        TranslatingData() {}
        TranslatingData(const TranslatingData& other);
        TranslatingData& operator=(const TranslatingData& other);
    };
};
       
       
