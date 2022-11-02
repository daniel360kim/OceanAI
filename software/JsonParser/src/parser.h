#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "../include/json.hpp"

class JsonParser
{
public:
    JsonParser();
    ~JsonParser();

    void openJson(const std::string& path, nlohmann::json& json);

    bool getKeys(const nlohmann::json& json, std::vector<std::string>& keys);


    void removeJsonBlock(const std::string& input_path, const std::string& output_path);
    bool json_to_csv(const nlohmann::json& json, const std::string& path);

private:
    nlohmann::json m_json;


    

    

};

