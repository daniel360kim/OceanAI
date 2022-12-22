/**
 * @file read_functions.h
 * @author Daniel Kim   
 * @brief API to read sensors
 * @version 0.1
 * @date 2022-11-06
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef read_functions_h
#define read_functions_h

#include "../Data/logged_data.h"

namespace Sensors
{
    /**
     * @brief Abstract class to read sensors
     * 
     */
    class ReadFunctions
    {
    public:
        virtual ~ReadFunctions() {}
        virtual double readRaw() = 0; //reads raw sensor value
        virtual double readFiltered(const double delta_time) = 0; //runs filter through raw sensor value
        virtual void logToStruct(Data &data) = 0; //logs raw and filtered sensor values to our data struct

    protected:
        double raw_reading = 0.0;
        double filtered_reading = 0.0;
    };

} // namespace Sensors


#endif