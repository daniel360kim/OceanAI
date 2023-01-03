/**
 * @file Orientation.cpp
 * @author Daniel Kim
 * @brief Orientation calculations from quaternions
 * @version 0.1
 * @date 2022-06-14
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#include <Arduino.h>
#include <cmath>
#include <stdlib.h>

#include "Orientation.h"
#include "Quaternion.h"
#include "../data/logged_data.h"

/**
 * @brief updates the class with the measurements
 * 
 * @param gx gyro measurement x
 * @param gy gyro measurement y
 * @param gz gyro measurement z
 * @param dt change in time seconds
 * @return Quaternion quaternions=
 */
Quaternion Orientation::update(double gx, double gy, double gz, double dt)
{
    B.toAxis(gx, gy, gz, dt);
    orientation = Quaternion::hamiltonProduct(A, B);

    A.w = orientation.w;
    A.x = orientation.x;
    A.y = orientation.y;
    A.z = orientation.z;

    return orientation;
}

/**
 * @brief converts quaternion to easy Euler angles (degrees)
 * 
 * @param X x orientation
 * @param Y y orientation
 * @param Z z orientation
 */
void Orientation::toEuler(double *X, double *Y, double *Z)
{
    double sinr_cosp = 2 * (orientation.w * orientation.x + orientation.y * orientation.z);
    double cosr_cosp = 1 - 2 * (orientation.x * orientation.x + orientation.y * orientation.y);
    *X = (std::atan2(sinr_cosp, cosr_cosp)) * RAD_TO_DEG;

    
    double sinp = 2 * (orientation.w * orientation.y - orientation.z * orientation.x);
    if (abs(sinp) >= 1)
    {
        *Y = (std::copysign(HALF_PI, sinp)) * RAD_TO_DEG;
    }
    else
    {
        *Y = (std::asin(sinp)) * RAD_TO_DEG;
    }

    double siny_cosp = 2 * (orientation.w * orientation.z + orientation.x * orientation.y);
    double cosy_cosp = 1 - 2 * (orientation.y * orientation.y + orientation.z * orientation.z);
    *Z = (std::atan2(siny_cosp, cosy_cosp)) * RAD_TO_DEG;
}

void Orientation::toEuler(double w, double x, double y, double z, double *X, double *Y, double *Z)
{
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    *X = (std::atan2(sinr_cosp, cosr_cosp)) * RAD_TO_DEG;

    
    double sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1)
    {
        *Y = (std::copysign(HALF_PI, sinp)) * RAD_TO_DEG;
    }
    else
    {
        *Y = (std::asin(sinp)) * RAD_TO_DEG;
    }

    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    *Z = (std::atan2(siny_cosp, cosy_cosp)) * RAD_TO_DEG;
}

/**
 * @brief converts accelerometer readings to the world reference frame. !MUST BE CALLED AFTER CALCULATING ORIENTATION
 * 
 * @param ax raw ax
 * @param ay raw ay
 * @param az raw az
 * @param wfax converted ax
 * @param wfay converted ay
 * @param wfaz converted az
 */
Angles_3D<double> Orientation::convertAccelFrame(Quaternion orientation, double ax, double ay, double az)
{
    Quaternion acc = { 0, ax, ay, az };
    Quaternion wF_acc = Quaternion::hamiltonProduct(orientation, acc);

    Angles_3D<double> world_frame_acc;
    //A simplified hamilton product multiplied by -1
    world_frame_acc.x = (wF_acc.w * orientation.x * -1 + wF_acc.x * orientation.w + wF_acc.y * orientation.z * -1 - wF_acc.z * orientation.y * -1);
    world_frame_acc.y = wF_acc.w * orientation.y * -1 - wF_acc.x * orientation.z * -1 + wF_acc.y * orientation.w + wF_acc.z * orientation.x * -1;
    world_frame_acc.z = wF_acc.w * orientation.z * -1 + wF_acc.x * orientation.y * -1 - wF_acc.y * orientation.x * -1 + wF_acc.z * orientation.w + 9.8065;

    return world_frame_acc;
}

double Orientation::constrainAngle_whole(double x) //to 0,360
{
    x = std::fmod(x, 360);
    if(x < 0)
    {
        x += 360;
    }
    return x;
}

double Orientation::constrainAngle_half(double x)
{
    x = std::fmod(x + 180, 360);
    if(x < 0)
    {
        x += 360;
    }

    return x - 180;

}

Quaternion Orientation::toQuaternion(double x, double y, double z)
{
    // Abbreviations for the various angular functions
    double cy = std::cos(z * 0.5);
    double sy = std::sin(z * 0.5);
    double cp = std::cos(y * 0.5);
    double sp = std::sin(y * 0.5);
    double cr = std::cos(x * 0.5);
    double sr = std::sin(x* 0.5);

    //Serial.println(z);
    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;

}