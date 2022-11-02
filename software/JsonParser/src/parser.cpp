#include "parser.h"

JsonParser::JsonParser()
{
}

JsonParser::~JsonParser()
{
}

void JsonParser::openJson(const std::string& path, nlohmann::json& json)
{
    std::ifstream file(path);
    if (file.is_open())
    {
        file >> json;
    }
    else
    {
        std::cerr << "Error: Could not open file " << path << std::endl;
    }
}

bool JsonParser::getKeys(const nlohmann::json& json, std::vector<std::string>& keys)
{
    for (auto& element : json.items())
    {
        keys.push_back(element.key());
    }
    return true;
}

bool JsonParser::json_to_csv(const nlohmann::json& json, const std::string& path)
{
    std::ofstream file(path);
    if (file.is_open())
    {
        for (auto& element : json.items())
        {
            file << element.key() << "," << element.value() << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return false;
    }
    return true;
}

void JsonParser::removeJsonBlock(const std::string &input_path, const std::string &output_path)
{
    std::ifstream input_file(input_path);
    std::ofstream output_file(output_path);

    if (input_file.is_open() && output_file.is_open())
    {
        std::string line;
        while (std::getline(input_file, line))
        {
            if (line.find("}") != std::string::npos)
            {
                line.erase(line.find("}"));
            }
            output_file << line << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: Could not open file " << input_path << std::endl;
    }
   
}