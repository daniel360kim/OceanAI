/**
 * @file module_settings.h
 * @author Daniel Kim
 * @brief module settings
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022 Daniel Kim https://github.com/daniel360kim/OceanAI
 * 
 */

#ifndef MODULE_SETTINGS_H
#define MODULE_SETTINGS_H

namespace Module
{
    constexpr int THREAD_ROD_LENGTH = 250; //in mm: length of the threaded rod
    constexpr int DISPLACEMENT_PER_CIRCLE = 8; //in mm: how much the nut moves every circle of the thread rod
    constexpr int END_STOP_LENGTH = 0; //in mm: how far away from the end of the thread rod the end stop is. 0 means that the nut should go to the very end of the rod
    constexpr int STEP_DELAY = 1; //in us: how long the motor should wait between steps
};




#endif