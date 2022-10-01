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

#include "../core/pins.h"
#include "../module/stepper.h"


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

class IdleMode : public State
{
public:
    void enter(StateAutomation *state) {}
    void run(StateAutomation *state) {}
    void exit(StateAutomation *state) {}
    static IdleMode &getInstance();

private:
    IdleMode() {}
    IdleMode(const IdleMode &);
    IdleMode &operator=(const IdleMode &);

    static IdleMode instance;
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

class Surfaced : public State
{
public:
    void enter(StateAutomation *state);
    void run(StateAutomation *state);
    void exit(StateAutomation *state);
    static Surfaced &getInstance();

private:
    Surfaced() {}
    Surfaced(const Surfaced &);
    Surfaced &operator=(const Surfaced &);

    static Surfaced instance;
};

class SD_translate : public State
{
public:
    void enter(StateAutomation *state);
    void run(StateAutomation *state);
    void exit(StateAutomation *state);
    static SD_translate &getInstance();

private:
    SD_translate() {}
    SD_translate(const SD_translate &);
    SD_translate &operator=(const SD_translate &);

    static SD_translate instance;
};

class SD_reinitialize : public State
{
public:
    void enter(StateAutomation *state) {}
    void run(StateAutomation *state) {}
    void exit(StateAutomation *state) {}
    static SD_reinitialize &getInstance();

private:
    SD_reinitialize() {}
    SD_reinitialize(const SD_reinitialize &);
    SD_reinitialize &operator=(const SD_reinitialize &);

    static SD_reinitialize instance;
};



#endif
