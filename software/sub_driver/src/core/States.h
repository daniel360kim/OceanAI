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

/*
Refer to Resources/project_images/StatesDiagram/StatesDiagram.png for diagram
No remote control functionality yet
*/
#ifndef STATES_H
#define STATES_H

#include "StateAutomation.h"

/*
Each state of our submarine is its own class and has 3 functions:
    enter: runs when the state is entered
    run: runs every loop
    exit: runs when the state is exited

Each state is a singleton and is accessed through the static function getInstance()

To add a new state, just create a new class with the three functions and the singleton functions

All states must inherit the State abstract class to access the enter, run, and exit functions

*/

/**
 * @brief Initialization state
 * Runs all the initializations when the submarine starts up
 * 
 */
class Initialization : public State
{
public:
    void enter(StateAutomation *state);
    void run(StateAutomation *state);
    void exit(StateAutomation *state);
    static Initialization &getInstance();

private:
    Initialization() {}
    Initialization(const Initialization &);
    Initialization &operator=(const Initialization &);

    static Initialization instance;
};

/**
 * @brief What to do when there is a critical error 
 * 
 */
class ErrorIndication : public State
{
public:
    void enter(StateAutomation *state);
    void run(StateAutomation *state);
    void exit(StateAutomation *state);
    static ErrorIndication &getInstance();

private:
    ErrorIndication() {}
    ErrorIndication(const ErrorIndication &);
    ErrorIndication &operator=(const ErrorIndication &);

    static ErrorIndication instance;
};

/**
 * @brief Idle state. Used when under remote control and waiting for commands
 * 
 */
class IdleMode : public State
{
public:
    void enter(StateAutomation *state); 
    void run(StateAutomation *state); 
    void exit(StateAutomation *state);
    static IdleMode &getInstance();

private:
    IdleMode() {}
    IdleMode(const IdleMode &);
    IdleMode &operator=(const IdleMode &);

    static IdleMode instance;
};

/**
 * @brief Resurfacing state. Used when the submarine is resurfacing
 * 
 */
class Resurfacing : public State
{
public:
    void enter(StateAutomation *state);
    void run(StateAutomation *state);
    void exit(StateAutomation *state);
    static Resurfacing &getInstance();

private:
    Resurfacing() {}
    Resurfacing(const Resurfacing &);
    Resurfacing &operator=(const Resurfacing &);

    static Resurfacing instance;

};

class Diving : public State
{
public:
    void enter(StateAutomation *state);
    void run(StateAutomation *state);
    void exit(StateAutomation *state);
    static Diving &getInstance();
private:
    Diving() {}
    Diving(const Diving &);
    Diving &operator=(const Diving &);

    static Diving instance;
};


#endif
