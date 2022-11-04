#include "timer.h"

Time::Timer scoped_timer; //timer that is used to measure the time since the start of the program

Time::TimerManager Time::TimerManager::instance; //a time manager for our named timers