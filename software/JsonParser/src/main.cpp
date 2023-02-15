#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <cmath>
#include "../include/json.hpp"


void read_lines(std::ifstream &file, std::vector<std::string> &json)
{
    int current_line = 0;
    int index = 0;
    std::string output;
    std::string combined;

    while(std::getline(file, output))
    {
        if(!output.empty())
        {
            json.push_back(output);
        }
    }
}

void to_csv(std::string json_line, std::ofstream &csv, char delimiter)
{
    nlohmann::json json = nlohmann::json::parse(json_line, nullptr, false, true);
    csv << json["time"] << delimiter;
    for(int i = 0; i < json["sys_data"].size(); i++)
    {
        csv << json["sys_data"][i] << delimiter;
    }
    
    for(int i = 0; i < json["location_data"].size(); i++)
    {
        csv << json["location_data"][i] << delimiter;
    }

    for(int i = 0; i < json["baro_data"].size(); i++)
    {
        csv << json["baro_data"][i] << delimiter;
    }

    for(int i = 0; i < json["IMU_data"].size(); i++)
    {
        csv << json["IMU_data"][i] << delimiter;
    }

    for(int i = 0; i < json["external_data"].size(); i++)
    {
        csv << json["external_data"][i] << delimiter;
    }

    for(int i = 0; i < json["step_data"].size(); i++)
    {
        csv << json["step_data"][i] << delimiter;
    }

    for(int i = 0; i < json["optics"].size(); i++)
    {
        csv << json["optics"][i] << delimiter;
    }
    csv << "\n";
}

void thread_convert(std::vector<std::string> &json, std::ofstream &csv, char delimiter, unsigned long start, unsigned long end)
{
    for(unsigned long i = start; i < end; i++)
    {
        to_csv(json[i], csv, delimiter);

        //Print progress
        if(i % 1000 == 0)
        {
            std::cout << "Progress: " << i << std::endl;
        }
    }
}

int main(int argc, char const *argv[])
{
    std::string input_file(argv[1]);

    std::string output_file = input_file.substr(0, input_file.find_last_of(".")) + ".csv";
    std::ifstream json_input(input_file);

    if(!json_input.is_open())
    {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    else
    {
        std::vector<std::string> json_vector;
        read_lines(json_input, json_vector);

        std::ofstream csv(output_file);
        constexpr char delimiter = ',';
        csv << "time(ns),loop_time(hz),system_state,delta_time(s),sd_capacity(bytes),r_voltage,f_voltage,clockspeed(hz),internal_temp(°C),";
        csv << "lat(deg),lon(deg),"
        csv << "bmp_pres(atm),bmp_temp(°C),";
        csv << "bmi_temp(°C),rax(m/s^2),ray(m/s^2),raz(m/s^2),wfax(m/s^2),wfay(m/s^2),wfaz(m/s^2),velx(m/s),vely(m/s),velz(m/s),posx(m),posy(m),posz(m),";
        csv << "rgx(rad/s),rgy(rad/s),rgz(rad/s),ori_x(deg),ori_y(deg),ori_z(deg),q_w,q_x,q_y,q_z,";
        csv << "rmagx(uT),rmagy(uT),rmagz(uT),fmagx(uT),fmagy(uT),fmagz(uT),";
        csv << "rTDS,fTDS,r_ext_pres,f_ext_pres,depth,r_ext_temp,f_ext_temp,";
        csv << "dive_limit,dive_homed,dive_current_position,dive_current_position_mm,dive_target_position,dive_target_position_mm,dive_speed,dive_acceleration,dive_maxspeed,";
        csv << "pitch_limit,pitch_homed,pitch_current_position,pitch_current_position_mm,pitch_target_position,pitch_target_position_mm,pitch_speed,pitch_acceleration,pitch_maxspeed,";
        csv << "cap_time(ms),save_time(ms),fifo_length(bytes)\n";

        unsigned long json_elements = json_vector.size() - 1;

        std::cout << "Number of elements: " << json_elements << std::endl;

        int num_available_threads = std::thread::hardware_concurrency();

        std::cout << "Number of threads: " << num_available_threads << std::endl;

        //Divide the number of elements by the number of threads. Round down
        unsigned long elements_per_thread = std::floor(json_elements / num_available_threads);
        int remainder = json_elements % num_available_threads;

        std::cout << "Elements per thread: " << elements_per_thread << std::endl;

        for(int i = 0; i < num_available_threads; i++)
        {
            unsigned long start = i * elements_per_thread;
            unsigned long end = start + elements_per_thread;
            if(i == num_available_threads - 1)
            {
                end += remainder;
            }
            std::thread t(thread_convert, std::ref(json_vector), std::ref(csv), delimiter, start, end);
            t.join();
        }

        csv.close();
        json_input.close();

        std::cout << "Done" << std::endl;
        
    }
}