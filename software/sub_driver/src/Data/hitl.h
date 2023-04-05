/**
 * @file hitl.h
 * @author Daniel Kim   
 * @brief Hardware in the loop simulation
 * @version 0.1
 * @date 2023-01-19
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef HITL_H
#define HITL_H


#include <array>
#include <cstdint>
#include <vector>
#include <cmath>

#include "hitl_data.h"
#include "logged_data.h"
#include "../core/configuration.h"
#include "../core/debug.h"

#if HITL_ON

namespace HITL
{
    class DataProvider
    {
    public:
        virtual ~DataProvider() = default;
        virtual void update(int index) = 0;
        virtual void addColumn(int index) = 0;
        virtual void reset() = 0;
    };

    class Data : public DataProvider
    {
    public:
        Data() = default;
        Data(const Data&) = delete;
        Data& operator=(const Data&) = delete;

        double operator[](int index);

        void addTransform(double (*transform)(double)) { m_transform = transform; }

        void update(int index) override;
        void addColumn(int index) override;
        void reset() override;

    private:
        std::vector<double> m_data; //The data that is being used
        std::vector<int> m_columns; //The columns that are being used

        //Data transformation function pointer
        double (*m_transform)(double) = nullptr;
    };


    //This class is responsible for updating all the data providers
    class DataProviderManager
    {
    public:
        DataProviderManager(int64_t time_between_readings_ns);
        DataProviderManager(const DataProviderManager&) = delete;
        DataProviderManager& operator=(const DataProviderManager&) = delete;

        void update_frequency(unsigned long frequency) { m_DataFrequency = frequency; }
        void update_frequency_scale(float scale) { m_DataFrequency = std::lround(scale * SEC_TO_NS(1)); }

        void add_provider(Data* provider) { m_providers.push_back(provider); }
        void update(int64_t timestamp);
        void update(int index);

        int getIndex() { return m_DataIndex; }
        double getTimestamp() { return m_timestamps[m_DataIndex]; }
        unsigned long get_frequency() { return m_DataFrequency; }
        double get_progress() { return (double)m_DataIndex / (double)HITL_DATA_ROWS; }
    
    private:
        std::vector<Data*> m_providers;
        double m_timestamps[HITL_DATA_ROWS];
        unsigned long m_DataFrequency;
        int m_DataIndex = 0;

        int64_t m_last_timestamp = 0;
    };

    void logData(LoggedData &data, DataProviderManager &provider_manager, Data &location, Data &depth, Data &pressure, Data &salinity, Data &temperature);
}

#endif

#endif