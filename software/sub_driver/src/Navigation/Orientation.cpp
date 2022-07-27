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
#include <math.h>
#include <stdlib.h>

#include "Orientation.h"
#include "Quaternion.h"


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
    B.toAxis(gx, gy, gz, dt, Quaternion::findNorm);
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
    *X = (atan2(sinr_cosp, cosr_cosp)) * RAD_TO_DEG;

    
    double sinp = 2 * (orientation.w * orientation.y - orientation.z * orientation.x);
    if (abs(sinp) >= 1)
    {
        *Y = (copysign(HALF_PI, sinp)) * RAD_TO_DEG;
    }
    else
    {
        *Y = (asin(sinp)) * RAD_TO_DEG;
    }

    double siny_cosp = 2 * (orientation.w * orientation.z + orientation.x * orientation.y);
    double cosy_cosp = 1 - 2 * (orientation.y * orientation.y + orientation.z * orientation.z);
    *Z = (atan2(siny_cosp, cosy_cosp)) * RAD_TO_DEG;
}

void Orientation::toEuler(double w, double x, double y, double z, double *X, double *Y, double *Z)
{
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    *X = (atan2(sinr_cosp, cosr_cosp)) * RAD_TO_DEG;

    
    double sinp = 2 * (w * y - z * x);
    if (abs(sinp) >= 1)
    {
        *Y = (copysign(HALF_PI, sinp)) * RAD_TO_DEG;
    }
    else
    {
        *Y = (asin(sinp)) * RAD_TO_DEG;
    }

    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    *Z = (atan2(siny_cosp, cosy_cosp)) * RAD_TO_DEG;
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
void Orientation::convertAccelFrame(Quaternion orientation, double ax, double ay, double az, double *wfax, double *wfay, double *wfaz)
{
    Quaternion acc = { 0, ax, ay, az };
    Quaternion wF_acc = Quaternion::hamiltonProduct(orientation, acc);

    //A simplified hamilton product multiplied by -1
    *wfax = (wF_acc.w * orientation.x * -1 + wF_acc.x * orientation.w + wF_acc.y * orientation.z * -1 - wF_acc.z * orientation.y * -1);
    *wfay = wF_acc.w * orientation.y * -1 - wF_acc.x * orientation.z * -1 + wF_acc.y * orientation.w + wF_acc.z * orientation.x * -1;
    *wfaz = wF_acc.w * orientation.z * -1 + wF_acc.x * orientation.y * -1 - wF_acc.y * orientation.x * -1 + wF_acc.z * orientation.w + 9.8065;
}

double Orientation::constrainAngle_whole(double x) //to 0,360
{
    x = fmod(x, 360);
    if(x < 0)
    {
        x += 360;
    }
    return x;
}

double Orientation::constrainAngle_half(double x)
{
    x = fmod(x + 180, 360);
    if(x < 0)
    {
        x += 360;
    }

    return x - 180;

}

Quaternion Orientation::toQuaternion(double x, double y, double z)
{
    // Abbreviations for the various angular functions
    double cy = cos(z * 0.5);
    double sy = sin(z * 0.5);
    double cp = cos(y * 0.5);
    double sp = sin(y * 0.5);
    double cr = cos(x * 0.5);
    double sr = sin(x* 0.5);

    //Serial.println(z);
    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;

}