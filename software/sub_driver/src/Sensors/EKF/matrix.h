/**
 * @file matrix.h
 * @author Daniel Kim
 * @brief Cholesky-decomposition matrix-inversion code, adapated from http://jean-pierre.moreau.pagesperso-orange.fr/Cplus/choles_cpp.txt
 * @version 0.1
 * @date 2022-07-23
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI) 
 * 
 */

#ifndef Matrix_h
#define Matrix_h

#include <math.h>

class Matrix
{
public:
    Matrix() {}

    static int choldc1(double *a, double *p, int n);
    static int choldcsl(double* A, double *a, double *p, int n);
    static int chols1(double* A, double *a, double *p, int n);
    inline static void zeros(double* a, int m, int n);
    static void mulmat(double* a, double* b, double* c, int arows, int acols, int bcols);
    static void mulvec(double *a, double *x, double *y, int m, int n);
    static void transpose(double *a, double *at, int m, int n);
    static void accum(double *a, double *b, int m, int n);
    static void add(double *a, double *b, double *c, int n);
    static void sub(double *a, double *b, double *c, int n);
    static void negate(double *a, int m, int n);
    static void mat_addeye(double *a, int n);   
};


#endif /* Matrix_h */