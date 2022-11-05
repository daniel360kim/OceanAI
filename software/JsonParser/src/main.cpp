#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "../include/json.hpp"


void read_lines(std::ifstream &file, std::vector<std::string> &json)
{
    int current_line = 0;
    int index = 0;
    std::string output;
    std::string combined;

    while(std::getline(file, output))
    {
        json.push_back(output);
    }
}

int main()
{
    std::ifstream json_input("../data/data19.json");

    if(!json_input.is_open())
    {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    else
    {
        std::vector<std::string> json_vector;
        read_lines(json_input, json_vector);

        std::ofstream csv("../data/data19.csv");
        constexpr char delimiter = ',';
        csv << "time(ns),loop_time(hz),system_state,delta_time(s),sd_capacity(bytes),r_voltage,f_voltage,clockspeed(hz),internal_temp(°C),";
        csv << "bmp_pres(atm),bmp_temp(°C),";
        csv << "bmi_temp(°C),rax(m/s^2),ray(m/s^2),raz(m/s^2),wfax(m/s^2),wfay(m/s^2),wfaz(m/s^2),velx(m/s),vely(m/s),velz(m/s),posx(m),posy(m),posz(m),";
        csv << "rgx(rad/s),rgy(rad/s),rgz(rad/s),ori_x(deg),ori_y(deg),ori_z(deg),q_w,q_x,q_y,q_z,";
        csv << "rmagx(uT),rmagy(uT),rmagz(uT),fmagx(uT),fmagy(uT),fmagz(uT),";
        csv << "rTDS,fTDS,r_ext_pres,f_ext_pres,";
        csv << "dive_limit,dive_homed,dive_current_position,dive_current_position_mm,dive_target_position,dive_target_position_mm,dive_speed,dive_acceleration,dive_maxspeed,";
        csv << "pitch_limit,pitch_homed,pitch_current_position,pitch_current_position_mm,pitch_target_position,pitch_target_position_mm,pitch_speed,pitch_acceleration,pitch_maxspeed,";
        csv << "cap_time(ms),save_time(ms),fifo_length(bytes)\n";

        unsigned long json_elements = json_vector.size() - 1;

        std::cout << "Number of elements: " << json_elements << std::endl;

        for(unsigned long i = 0; i < json_elements; i++)
        {
            nlohmann::json json = nlohmann::json::parse(json_vector[i], nullptr, false, true);
            csv << json["time"] << delimiter;
            for(int i = 0; i < json["sys_data"].size(); i++)
            {
                csv << json["sys_data"][i] << delimiter;
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


        
        
    }
}