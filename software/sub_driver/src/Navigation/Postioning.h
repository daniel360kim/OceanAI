/**
 * @file Postioning.h
 * @author Daniel Kim
 * @brief Simple positioning
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#ifndef Positioning_h
#define Positioning_h

#include "../Data/data_struct.h"


class Positioning
{
public:
    Positioning() {};

    inline void Integrate(double parameter, double dt);

    inline double returnProduct();

protected:
    double product;

};

class Velocity : public Positioning
{
public:
    Velocity() {}
    void updateVelocity(Data &data);

private:
    Positioning x;
    Positioning y;
    Positioning z;
};

class Position : public Positioning
{
public:
    Position() {}
    void updatePosition(Data& data);
private:
    Positioning x;
    Positioning y;
    Positioning z;
};




#endif