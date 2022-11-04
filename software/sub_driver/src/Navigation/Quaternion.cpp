/**
 * @file Quaternion.cpp
 * @author Daniel Kim
 * @brief 
 * @version 0.1
 * @date 2022-06-14
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#include <Arduino.h>
#include <cmath>
#include <algorithm>

#include "Quaternion.h"

/**
 * @brief calculates the norm from angular rates
 * 
 * @param gx x axis gyro measurement
 * @param gy y axis gyro measurement
 * @param gz z axis gyro measurement
 * @param dt delta time in seconds
 * @return Norm norm and angle
 */
Quaternion::Norm Quaternion::findNorm(double gx, double gy, double gz, double dt)
{
    Norm quat;
    quat.norm = std::sqrt((gx * gx) + (gy * gy) + (gz * gz));
    quat.norm = std::max<double>(std::abs(quat.norm), 1e-12); //Cannot divide by zero

    quat.theta = quat.norm * dt;
    return quat;
}

/**
 * @brief converts angular rates to a quaternion
 * 
 * @param gx gyro measurement x
 * @param gy gyro measurement y
 * @param gz gyro measurement z
 * @param dt delta time seconds
 */
void Quaternion::toAxis(double gx, double gy, double gz, double dt)
{
    Norm quat = findNorm(gx, gy, gz, dt);

    w = std::cos(quat.theta / 2);
    x = (gx / quat.norm) * std::sin(quat.theta / 2);
    y = (gy / quat.norm) * std::sin(quat.theta / 2);
    z = (gz / quat.norm) * std::sin(quat.theta / 2);
}

/**
 * @brief function that gets the Hamilton Product of two quats. used for converting ref. frames and calculating orientation quat
 * 
 * @param A factor A
 * @param B factor B
 * @return Quaternion product 
 */
FASTRUN Quaternion Quaternion::hamiltonProduct(Quaternion A, Quaternion B) 
{ 
  Quaternion product;
 
  product.w = (A.w * B.w) - (A.x * B.x) - (A.y * B.y) - (A.z * B.z);
  product.x = (A.w * B.x) + (A.x * B.w) + (A.y * B.z) - (A.z * B.y);
  product.y = (A.w * B.y) - (A.x * B.z) + (A.y * B.w) + (A.z * B.x);
  product.z = (A.w * B.z) + (A.x * B.y) - (A.y * B.x) + (A.z * B.w);
 
  return product;
}

