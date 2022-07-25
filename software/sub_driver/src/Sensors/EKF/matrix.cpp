/**
 * @file matrix.cpp
 * @author Daniel Kim
 * @brief Cholesky-decomposition matrix-inversion code, adapated from http://jean-pierre.moreau.pagesperso-orange.fr/Cplus/choles_cpp.txt
 * @version 0.1
 * @date 2022-07-23
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#include <math.h>
#include "matrix.h"

int choldc1(double *a, double *p, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            sum = a[i * n + j];
            for (int k = i - 1; k >= 0; k--)
            {
                sum -= a[i * n + k] * a[j * n + k];
            }
            if (i == j)
            {
                if (sum <= 0)
                {
                    return -1;
                }
                p[i] = sqrt(sum);
            }
            else
            {
                a[j * n + i] = sum / p[i];
            }
        }
    }

    return 0;
}

int Matrix::choldcsl(double *A, double *a, double *p, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            a[i * n + j] = A[i * n + j];
        }
    }
    if (choldc1(a, p, n))
    {
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        a[i * n + i] = 1 / p[i];
        for (int j = i + 1; j < n; j++)
        {
            sum = 0.0;
            for (int k = i; k < j; k++)
            {
                sum -= a[j * n + k] * a[k * n + i];
            }
            a[j * n + i] = sum / p[j];
        }
    }

    return 0; /* success */
}

int Matrix::chols1(double *A, double *a, double *p, int n)
{
    if (choldcsl(A, a, p, n))
    {
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            a[i * n + j] = 0.0;
        }
    }
    for (int i = 0; i < n; i++)
    {
        a[i * n + i] *= a[i * n + i];
        for (int k = i + 1; k < n; k++)
        {
            a[i * n + i] += a[k * n + i] * a[k * n + i];
        }
        for (int j = i + 1; j < n; j++)
        {
            for (int k = j; k < n; k++)
            {
                a[i * n + j] += a[k * n + i] * a[k * n + j];
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            a[i * n + j] = a[j * n + i];
        }
    }

    return 0; /* success */
}

inline void Matrix::zeros(double *a, int m, int n)
{
    for (int j = 0; j < m * n; j++)
    {
        a[j] = 0.0;
    }
}

void Matrix::mulmat(double *a, double *b, double *c, int arows, int acols, int bcols)
{
    for (int i = 0; i < arows; i++)
        for (int j = 0; j < bcols; j++)
        {
            c[i * bcols + j] = 0;
            for (int l = 0; l < acols; l++)
                c[i * bcols + j] += a[i * acols + l] * b[l * bcols + j];
        }
}

void Matrix::mulvec(double *a, double *x, double *y, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        y[i] = 0;
        for (int j = 0; j < n; j++)
            y[i] += x[j] * a[i * n + j];
    }
}
void Matrix::transpose(double *a, double *at, int m, int n)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            at[j * m + i] = a[i * n + j];
        }
}
void Matrix::accum(double *a, double *b, int m, int n)
{
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; j++)
            a[i * n + j] += b[i * n + j];
}
void Matrix::add(double *a, double *b, double *c, int n)
{
    for (int j = 0; j < n; j++)
    {
        c[j] = a[j] + b[j];
    }
}
void Matrix::sub(double *a, double *b, double *c, int n)
{
    for (int j = 0; j < n; j++)
    {
        c[j] = a[j] - b[j];
    }
}
void Matrix::negate(double *a, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            a[i * n + j] = -a[i * n + j];
        }
    }
}
void Matrix::mat_addeye(double *a, int n)
{
    for (int i = 0; i < n; i++)
    {
        a[i * n + i] += 1;
    }
}
