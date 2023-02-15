/**
 * @file hitl.cpp
 * @author Daniel Kim   
 * @brief Hardware in the loop simulation
 * @version 0.1
 * @date 2023-01-19
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#include "hitl.h"

#if HITL_ON

namespace HITL
{
    DataProviderManager::DataProviderManager(int64_t time_between_readings_ns)
    {
        //How long does it take to collect one data point?
        //Splitting the data evenly between the mission duration
        #if !HITL_LOOP
        m_DataFrequency = MissionDuration::mission_time / HITL_DATA_ROWS;

        INFO_LOGf("Mission duration: %d ns", MissionDuration::mission_time);
        INFO_LOGf("HITL data rows: %d", HITL_DATA_ROWS);
        INFO_LOGf("HITL Data updates every %d ns", m_DataFrequency);
        #else
        m_DataFrequency = MS_TO_NS(100);

        for(int i = 0; i < HITL_DATA_ROWS; i++)
        {
            m_timestamps[i] = i * (time_between_readings_ns / 1000000000.0);
        }

        #endif

    }

    void DataProviderManager::update(int64_t timestamp)
    {
        if(timestamp - m_last_timestamp >= m_DataFrequency)
        {
            m_last_timestamp = timestamp;
            for(unsigned int i = 0; i < m_providers.size(); i++)
            {
                m_providers[i]->update(m_DataIndex);
            }
            m_DataIndex++;
            #if HITL_LOOP
            //Restart the data index
            if(m_DataIndex >= HITL_DATA_ROWS)
            {
                m_DataIndex = 0;
            }
            #endif
        }
    }

    void DataProviderManager::update(int index)
    {
        for(unsigned int i = 0; i < m_providers.size(); i++)
        {
            m_providers[i]->update(index);
        }
    }
    double Data::operator[](int index)
    {
        if((unsigned int)index > m_columns.size() || index < 0)
        {
            return -1;
        }
        else
        {
            return m_data[index];
        }
    }

    void Data::update(int index)
    {
        if(m_data.size() != m_columns.size())
        {
            m_data.resize(m_columns.size());
        }
        //If there is a transform function, use it
        if(m_transform != nullptr)
        {
            for(unsigned int i = 0; i < m_columns.size(); i++)
            {
                m_data[i] = m_transform(HITL_DATA_ALPHA[index][m_columns[i]]);
            }
        }
        else
        {
            for(unsigned int i = 0; i < m_columns.size(); i++)
            {
                m_data[i] = HITL_DATA_ALPHA[index][m_columns[i]];
            }
        }
    }

    void Data::addColumn(int index)
    {
        m_columns.push_back(index);
    }

    void Data::reset()
    {
        m_columns.clear();
        m_data.clear();
    }

    


}

#endif
