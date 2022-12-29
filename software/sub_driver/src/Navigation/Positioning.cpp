/**
 * @file Postioning.cpp
 * @author Daniel Kim
 * @brief Simple positioning
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#include "Postioning.h"

FASTRUN inline void Positioning::Integrate(double parameter, double dt)
{
    product = product + (parameter * dt);
}

FASTRUN inline double Positioning::returnProduct()
{
    return product;
}

/**
 * @brief Integrates acceleration to get vel.
 * 
 * @param data data struct holding acc. and to be updated with vel
 */
FASTRUN void Velocity::updateVelocity(LoggedData &data)
{
    x.Integrate(data.wfacc.x, data.delta_time);
    y.Integrate(data.wfacc.y, data.delta_time);
    z.Integrate(data.wfacc.z, data.delta_time);

    data.vel.x = x.returnProduct();
    data.vel.y = y.returnProduct();
    data.vel.z = z.returnProduct();
}

/**
 * @brief Integrates velocity to get position
 * 
 * @param data data struct holding velocty and to be updated with position
 */
FASTRUN void Position::updatePosition(LoggedData &data)
{
    x.Integrate(data.vel.x, data.delta_time);
    y.Integrate(data.vel.y, data.delta_time);
    z.Integrate(data.vel.z, data.delta_time);

    data.pos.x = x.returnProduct();
    data.pos.y = y.returnProduct();
    data.pos.z = z.returnProduct();
}