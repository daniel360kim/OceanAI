#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

std::vector<std::vector<std::string>> read_csv(std::string filename)
{
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> data;
    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::string cell;
        std::stringstream lineStream(line);
        while (std::getline(lineStream, cell, ','))
        {
            //Check for NaN. Remove row
            if (cell == "NaN")
            {
            }
            else
            {
                row.push_back(cell);
            }
        }
        data.push_back(row);
    }
    //Remove the first row (header)
    data.erase(data.begin());

    //Remove first column (time)
    for (int i = 0; i < data.size(); i++)
    {
        data[i].erase(data[i].begin());
    }

    return data;
}

std::vector<unsigned long> time_to_elapsed(std::vector<std::vector<std::string>>& data)
{
    /*
    Time in csv is in format "YYYY-MM-DDTHH:MM:SSZ"
    We want to convert this to elapsed time in seconds after the first time stamp
    */
    unsigned long rows = data.size();
    std::vector<unsigned long> elapsed(rows);

    // Get the first time stamp 
    std::string first_time = data[0][0];
    std::cout << first_time << std::endl;
    unsigned long first_year = std::stoul(first_time.substr(0, 4));
    std::cout << first_year << std::endl;
    unsigned long first_month = std::stoul(first_time.substr(5, 2));
    unsigned long first_day = std::stoul(first_time.substr(8, 2));
    unsigned long first_hour = std::stoul(first_time.substr(11, 2));
    unsigned long first_minute = std::stoul(first_time.substr(14, 2));
    unsigned long first_second = std::stoul(first_time.substr(17, 2));


    std::cout << "First time stamp: " << first_year << "-" << first_month << "-" << first_day << " " << first_hour << ":" << first_minute << ":" << first_second << std::endl;
    
    // Convert the first time stamp to seconds
    unsigned long first_seconds = first_second + first_minute * 60 + first_hour * 3600 + first_day * 86400 + first_month * 2592000 + first_year * 31104000;

    // Convert the rest of the time stamps to seconds and subtract the first time stamp
    //If the time stamp is identical to previous, go till the next one is different and divide by identical count and increment the index by that amount
    unsigned long identical_count = 0;
    for (int i = 0; i < rows; i++)
    {
        std::string time = data[i][0];
        unsigned long year = std::stoul(time.substr(0, 4));
        unsigned long month = std::stoul(time.substr(5, 2));
        unsigned long day = std::stoul(time.substr(8, 2));
        unsigned long hour = std::stoul(time.substr(11, 2));
        unsigned long minute = std::stoul(time.substr(14, 2));
        unsigned long second = std::stoul(time.substr(17, 2));

        unsigned long seconds = second + minute * 60 + hour * 3600 + day * 86400 + month * 2592000 + year * 31104000;

        if (seconds == first_seconds)
        {
            identical_count++;
        }
        else
        {
            elapsed[i] = (seconds - first_seconds) / identical_count;
            identical_count = 0;
        }
    }


    return elapsed;
}

void convert_to_header(std::vector<std::vector<std::string>>& data, std::string filename)
{
    std::string header = filename + ".h";
    std::ofstream file(header);

    file << "#ifndef " << filename << "_H" << std::endl;
    file << "#define " << filename << "_H" << std::endl;
    file << std::endl;
    file << "#include <array>" << std::endl;

    file << "EXTMEM const std::array<std::array<double, " << data[0].size() << ">, " << data.size() << "> " << filename << " = {" << std::endl;

    for (int i = 0; i < data.size(); i++)
    {
        file << "    {";
        for (int j = 0; j < data[i].size(); j++)
        {
            file << data[i][j];
            if (j != data[i].size() - 1)
            {
                file << ", ";
            }
        }
        file << "}";
        if (i != data.size() - 1)
        {
            file << ",";
        }
        file << std::endl;
    }

    file << "};" << std::endl;
    file << std::endl;
    file << "#endif" << std::endl;
}

int main(int argc, char const *argv[])
{
    std::vector<std::vector<std::string>> data = read_csv("long.csv");

    std::cout << "Read " << data.size() << " rows" << std::endl;

    convert_to_header(data, "hitl_data");

    std::cout << "Converted to header file" << std::endl;
    
    return 0;
}
