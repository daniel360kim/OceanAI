/**
 * @file Orientation.h
 * @author Daniel Kim
 * @brief Orientation calculations from quaternions
 * @version 0.1
 * @date 2022-06-14
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */
#ifndef Orientation_h
#define Orientation_h

#include "Quaternion.h"
#include "../data/data_struct.h"

class Orientation 
{
public:
    Orientation() {}
    
    Quaternion update(double gx, double gy, double gz, double dt);
    void toEuler(double *X, double *Y, double *Z);
    static void toEuler(double w, double x, double y, double z, double *X, double *Y, double *Z);

    //static Quaternion toQuaternion(double x, double y, double z);
    Angles_3D<double> convertAccelFrame(Quaternion orientation, double ax, double ay, double az);
    
    static Quaternion toQuaternion(double x, double y, double z);
    static double constrainAngle_whole(double x);
    static double constrainAngle_half(double x);
private:
    Quaternion A;
    Quaternion B;
    Quaternion orientation;
};

#endif