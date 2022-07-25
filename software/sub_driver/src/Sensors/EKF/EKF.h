/**
 * @file EKF.h
 * @author Daniel Kim   
 * @brief Extended kalman filter class modified from TinyEKF
 * @version 0.1
 * @date 2022-07-23
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */
#ifndef EKF_H
#define EKF_H   

#include "matrix.h"

template<int N, int M>
class EKF
{
public:
    EKF();
    int ekf_step
private:
    struct ekf_t
    {
        double *x; //state vector;
        double *P; //covariance matrix;
        double *Q; //process noise covariance matrix;
        double *R; //measurement noise covariance matrix;

        double *G; //kalman gain

        double *F; //Jacobian of process model;
        double *H; //Jacobian of measurement model;

        double *Ht; //transpose of H;
        double *Ft; //transpose of F;
        double *Pp; //prediction covariance matrix;

        double *fx; //output of f() state-transition func
        double *hx; //output of h() measurement model func

        /* temporary storage */
        double *tmp0;
        double *tmp1;
        double *tmp2;
        double *tmp3;
        double *tmp4;
        double *tmp5;
    };

    static void unpack(void *v, int m, int n);
};

#endif