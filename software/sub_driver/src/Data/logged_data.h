/**
 * @file logged_data.h
 * @author Daniel Kim
 * @brief the data struct where data to be logged is saved
 * @version 0.1
 * @date 2022-07-01
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */
#ifndef Data_struct_h
#define Data_struct_h

#include <Arduino.h>
#include <stdint.h>
#include <ArduinoJson.h>
#include <electricui.h>
#include <cmath>

#define ARDUINO_JSON_USE_DOUBLE 0
#define ARDUINO_JSON_USE_LONG_LONG 0

static constexpr int STATIC_JSON_DOC_SIZE = 1536;
static constexpr int TELEM_STATIC_JSON_DOC_SIZE = 512;

struct StepperData
{
    bool limit_state;
    bool homed;
    double current_position;
    double current_position_mm;

    double target_position;
    double target_position_mm;

    double speed;
    double acceleration;
    double max_speed;
};

struct OpticalData
{
    uint32_t capture_time;
    uint32_t save_time;
    uint32_t FIFO_length;
};

// Sometimes we use single precision for smaller packets in telemetry so we template this struct
template <typename T>
struct Angles_3D
{
    T x, y, z;
};

struct Angles_4D
{
    double w, x, y, z;
};

struct BMP388Data
{
    double pressure;
    double temperature;
};

// To do: organize into different structs for optimization and organization
class LoggedData
{
public:
    int64_t time_ns;
    int loop_time;
    int system_state;
    double delta_time;
    uint32_t sd_capacity;

    double raw_voltage;
    double filt_voltage;
    int clock_speed;
    double internal_temp;

    BMP388Data raw_bmp;

    double bmi_temp;
    Angles_3D<double> racc;
    Angles_3D<double> wfacc;
    Angles_3D<double> vel;
    Angles_3D<double> pos;

    Angles_3D<double> rgyr;
    Angles_3D<double> rel_ori;

    Angles_4D relative;

    Angles_3D<double> rmag;
    Angles_3D<double> fmag;

    double raw_TDS;
    double filt_TDS;

    double raw_ext_pres;
    double filt_ext_pres;

    double raw_ext_temp;
    double filt_ext_temp;

    StepperData dive_stepper;
    StepperData pitch_stepper;

    OpticalData optical_data;

    /**
     * @brief Prints out all the data to a printing object
     *
     * @param p printer
     * @param delim how the data should be delimited
     * @param data data class to be printed
     */
    static void printData(Print &p, const char *delim, const Data &data)
    {
        p.print(data.time_ns);
        p.print(delim);
        p.print(data.loop_time);
        p.print(delim);
        p.print(data.system_state);
        p.print(delim);
        p.print(data.delta_time);
        p.print(delim);
        p.print(data.sd_capacity);
        p.print(delim);
        p.print(data.raw_voltage);
        p.print(delim);
        p.print(data.filt_voltage);
        p.print(delim);
        p.print(data.clock_speed);
        p.print(delim);
        p.print(data.internal_temp);
        p.print(delim);
        p.print(data.raw_bmp.pressure);
        p.print(delim);
        p.print(data.raw_bmp.temperature);
        p.print(delim);
        p.print(data.bmi_temp);
        p.print(delim);
        p.print(data.racc.x);
        p.print(delim);
        p.print(data.racc.y);
        p.print(delim);
        p.print(data.racc.z);
        p.print(delim);
        p.print(data.wfacc.x);
        p.print(delim);
        p.print(data.wfacc.y);
        p.print(delim);
        p.print(data.wfacc.z);
        p.print(delim);
        p.print(data.vel.x);
        p.print(delim);
        p.print(data.vel.y);
        p.print(delim);
        p.print(data.vel.z);
        p.print(delim);
        p.print(data.pos.x);
        p.print(delim);
        p.print(data.pos.y);
        p.print(delim);
        p.print(data.pos.z);
        p.print(delim);
        p.print(data.rgyr.x);
        p.print(delim);
        p.print(data.rgyr.y);
        p.print(delim);
        p.print(data.rgyr.z);
        p.print(delim);
        p.print(data.rel_ori.x);
        p.print(delim);
        p.print(data.rel_ori.y);
        p.print(delim);
        p.print(data.rel_ori.z);
        p.print(delim);
        p.print(data.relative.w);
        p.print(delim);
        p.print(data.relative.x);
        p.print(delim);
        p.print(data.relative.y);
        p.print(delim);
        p.print(data.relative.z);
        p.print(delim);
        p.print(data.rmag.x);
        p.print(delim);
        p.print(data.rmag.y);
        p.print(delim);
        p.print(data.rmag.z);
        p.print(delim);
        p.print(data.fmag.x);
        p.print(delim);
        p.print(data.fmag.y);
        p.print(delim);
        p.print(data.fmag.z);
        p.print(delim);
        p.print(data.raw_TDS);
        p.print(delim);
        p.print(data.filt_TDS);
        p.print(delim);
        p.print(data.raw_ext_pres);
        p.print(delim);
        p.print(data.filt_ext_pres);
        p.print(delim);
        p.print(data.dive_stepper.limit_state);
        p.print(delim);
        p.print(data.dive_stepper.homed);
        p.print(delim);
        p.print(data.dive_stepper.current_position);
        p.print(delim);
        p.print(data.dive_stepper.current_position_mm);
        p.print(delim);
        p.print(data.dive_stepper.target_position);
        p.print(delim);
        p.print(data.dive_stepper.target_position_mm);
        p.print(delim);
        p.print(data.dive_stepper.speed);
        p.print(delim);
        p.print(data.dive_stepper.acceleration);
        p.print(delim);
        p.print(data.dive_stepper.max_speed);
        p.print(delim);
        p.print(data.pitch_stepper.limit_state);
        p.print(delim);
        p.print(data.pitch_stepper.homed);
        p.print(delim);
        p.print(data.pitch_stepper.current_position);
        p.print(delim);
        p.print(data.pitch_stepper.current_position_mm);
        p.print(delim);
        p.print(data.pitch_stepper.target_position);
        p.print(delim);
        p.print(data.pitch_stepper.target_position_mm);
        p.print(delim);
        p.print(data.pitch_stepper.speed);
        p.print(delim);
        p.print(data.pitch_stepper.acceleration);
        p.print(delim);
        p.print(data.pitch_stepper.max_speed);
        p.print(delim);
        p.print(data.optical_data.capture_time);
        p.print(delim);
        p.print(data.optical_data.save_time);
        p.print(delim);
        p.print(data.optical_data.FIFO_length);
        p.print("\n");
    }

    // 65 total elements

    /**
     * @brief Converts data to json that is then logged to SD
     * @details Should only be called when there is a new JSON doc
     * @param data data struct to be used
     * @param doc reference to json document. must match size of specified json doc in parameters
     */
    static void data_to_json(Data &data, StaticJsonDocument<STATIC_JSON_DOC_SIZE> &doc)
    {
        doc.clear();
        doc["time"] = data.time_ns;

        JsonArray sys_data = doc.createNestedArray("sys_data");
        sys_data.add(data.loop_time);
        sys_data.add(data.system_state);
        sys_data.add(data.delta_time);
        sys_data.add(data.sd_capacity);
        sys_data.add(data.raw_voltage);
        sys_data.add(data.filt_voltage);
        sys_data.add(data.clock_speed);
        sys_data.add(data.internal_temp);

        JsonArray baro_data = doc.createNestedArray("baro_data");
        baro_data.add(data.raw_bmp.pressure);
        baro_data.add(data.raw_bmp.temperature);

        JsonArray ori_data = doc.createNestedArray("IMU_data");
        ori_data.add(data.bmi_temp);
        ori_data.add(data.racc.x);
        ori_data.add(data.racc.y);
        ori_data.add(data.racc.z);
        ori_data.add(data.wfacc.x);
        ori_data.add(data.wfacc.y);
        ori_data.add(data.wfacc.z);
        ori_data.add(data.vel.x);
        ori_data.add(data.vel.y);
        ori_data.add(data.vel.z);
        ori_data.add(data.pos.x);
        ori_data.add(data.pos.y);
        ori_data.add(data.pos.z);
        ori_data.add(data.rgyr.x);
        ori_data.add(data.rgyr.y);
        ori_data.add(data.rgyr.z);
        ori_data.add(data.rel_ori.x);
        ori_data.add(data.rel_ori.y);
        ori_data.add(data.rel_ori.z);
        ori_data.add(data.relative.w);
        ori_data.add(data.relative.x);
        ori_data.add(data.relative.y);
        ori_data.add(data.relative.z);
        ori_data.add(data.rmag.x);
        ori_data.add(data.rmag.y);
        ori_data.add(data.rmag.z);
        ori_data.add(data.fmag.x);
        ori_data.add(data.fmag.y);
        ori_data.add(data.fmag.z);

        JsonArray external_data = doc.createNestedArray("external_data");
        external_data.add(data.raw_TDS);
        external_data.add(data.filt_TDS);
        external_data.add(data.raw_ext_pres);
        external_data.add(data.filt_ext_pres);
        external_data.add(data.raw_ext_temp);
        external_data.add(data.filt_ext_temp);

        JsonArray step_data = doc.createNestedArray("step_data");
        step_data.add(data.dive_stepper.limit_state);
        step_data.add(data.dive_stepper.homed);
        step_data.add(data.dive_stepper.current_position);
        step_data.add(data.dive_stepper.current_position_mm);
        step_data.add(data.dive_stepper.target_position);
        step_data.add(data.dive_stepper.target_position_mm);
        step_data.add(data.dive_stepper.speed);
        step_data.add(data.dive_stepper.acceleration);
        step_data.add(data.dive_stepper.max_speed);

        step_data.add(data.pitch_stepper.limit_state);
        step_data.add(data.pitch_stepper.homed);
        step_data.add(data.pitch_stepper.current_position);
        step_data.add(data.pitch_stepper.current_position_mm);
        step_data.add(data.pitch_stepper.target_position);
        step_data.add(data.pitch_stepper.target_position_mm);
        step_data.add(data.pitch_stepper.speed);
        step_data.add(data.pitch_stepper.acceleration);
        step_data.add(data.pitch_stepper.max_speed);

        JsonArray optics_data = doc.createNestedArray("optics");
        optics_data.add(data.optical_data.capture_time);
        optics_data.add(data.optical_data.save_time);
        optics_data.add(data.optical_data.FIFO_length);
    }
};

class SendData
{
public:
    uint16_t loop_time = 0;
    float voltage = 0.f;
    uint8_t system_state = 0;
    float internal_temp = 0.f;

    Angles_3D<float> rel_ori = {0.f};
    float acc[3] = {0.f};
    float gyr[3] = {0.f};

    int16_t stepper_current_position = 0;
    int16_t stepper_target_position = 0;
    int16_t stepper_speed = 0;
    int16_t stepper_acceleration = 0;

    uint8_t system_state_command = 0;
    int16_t stepper_speed_command = 0;
    int16_t stepper_acceleration_command = 0;

    void convert(Data &data)
    {
        loop_time = static_cast<uint16_t>(data.loop_time);
        voltage = static_cast<float>(data.filt_voltage);
        system_state = static_cast<uint8_t>(data.system_state);
        internal_temp = static_cast<float>(data.bmi_temp);

        rel_ori.x = static_cast<float>(data.rel_ori.x);
        rel_ori.y = static_cast<float>(data.rel_ori.y);
        rel_ori.z = static_cast<float>(data.rel_ori.z);

        gyr[0] = static_cast<float>(data.rgyr.x);
        gyr[1] = static_cast<float>(data.rgyr.y);
        gyr[2] = static_cast<float>(data.rgyr.z);

        acc[0] = static_cast<float>(data.racc.x);
        acc[1] = static_cast<float>(data.racc.y);
        acc[2] = static_cast<float>(data.racc.z);

        stepper_acceleration = static_cast<int16_t>(data.dive_stepper.acceleration);
        stepper_current_position = static_cast<int16_t>(data.dive_stepper.current_position);
        stepper_target_position = static_cast<int16_t>(data.dive_stepper.target_position);
        stepper_speed = static_cast<int16_t>(data.dive_stepper.speed);

        stepper_acceleration = std::abs<int16_t>(stepper_acceleration);
        stepper_current_position = std::abs<int16_t>(stepper_current_position);
        stepper_target_position = std::abs<int16_t>(stepper_target_position);
        stepper_speed = std::abs<int16_t>(stepper_speed);
    }
};

#endif