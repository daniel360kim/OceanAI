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

#include "../core/configuration.h"

namespace HITL
{
    class DataProvider
    {
    public:
        virtual ~DataProvider() = default;
        virtual void update(int64_t timestamp) = 0;
        virtual void reset() = 0;
    };

    class DataProviderManager
    {
    public:
        DataProviderManager() = default;
        DataProviderManager(const DataProviderManager&) = delete;
        DataProviderManager& operator=(const DataProviderManager&) = delete;

        void add_provider(DataProvider* provider) { m_providers.push_back(provider); }
        void update(int64_t timestamp);
        void reset();
    
    private:
        std::vector<DataProvider*> m_providers;
    };
    


}











#endif