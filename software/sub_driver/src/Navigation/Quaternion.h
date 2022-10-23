/**
 * @file Quaternion.h
 * @author Daniel Kim
 * @brief 
 * @version 0.1
 * @date 2022-06-14
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#ifndef Quaternion_h
#define Quaternion_h

#include "../data/data_struct.h"

class Quaternion
{
public:
    double w;
    double x;
    double y;
    double z;

    struct Norm
    {
        double norm;
        double theta;
    };
    
    Quaternion() { w = 1, x = 0, y = 0, z = 0; }
    Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}

    void toAxis(double gx, double gy, double gz, double dt);
    static Quaternion hamiltonProduct(Quaternion A, Quaternion B);
    static Norm findNorm(double gx, double gy, double gz, double dt);

    //overloaded typecast operator to convert quaternion to 4D vector
    operator Angles_4D() const
    {
        return {w, x, y, z};
    }

};


#endif