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

void Positioning::Integrate(double parameter, double dt)
{
    product = product + (parameter * dt);
}

double Positioning::returnProduct()
{
    return product;
}

/**
 * @brief Integrates acceleration to get vel.
 * 
 * @param data data struct holding acc. and to be updated with vel
 */
void Velocity::updateVelocity(Data &data)
{
    x.Integrate(data.wfacc.x, data.dt);
    y.Integrate(data.wfacc.y, data.dt);
    z.Integrate(data.wfacc.z, data.dt);

    data.vel.x = x.returnProduct();
    data.vel.y = y.returnProduct();
    data.vel.z = z.returnProduct();
}

/**
 * @brief Integrates velocity to get position
 * 
 * @param data data struct holding velocty and to be updated with position
 */
void Position::updatePosition(Data &data)
{
    x.Integrate(data.vel.x, data.dt);
    y.Integrate(data.vel.y, data.dt);
    z.Integrate(data.vel.z, data.dt);

    data.pos.x = x.returnProduct();
    data.pos.y = y.returnProduct();
    data.pos.z = z.returnProduct();
}