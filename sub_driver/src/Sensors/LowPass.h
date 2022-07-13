/**
 * @file LowPass.h
 * @author Daniel Kim
 * @brief Low Pass filter class
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#ifndef LowPass_h
#define LowPass_h

#include <Arduino.h>

/**
 * @brief Low pass filter class
 * 
 * @tparam order The order of the low pass filter
 */
template <int order> // order is 1 or 2
class LowPass
{
public: 

    LowPass(double f0, double fs, bool adaptive);
    void setCoef();

    inline double filt(double xn);
    
private:
    double a[order];
    double b[order+1];
    double omega0;
    double dt;
    bool adapt;
    double tn1 = 0;
    double x[order+1]; // Raw values
    double y[order+1]; // Filtered values
};



/**
 * @brief Construct a new Low Pass object
 * 
 * @param f0 cutoff frequency
 * @param fs sample frequency
 * @param adaptive flag - true will automatically set fs (the sample frequency)
 */
template<int order>
LowPass<order>::LowPass(double f0, double fs, bool adaptive)
{
    omega0 = 6.28318530718 * f0;
    dt = 1.0 / fs;
    adapt = adaptive;
    tn1 = -dt;
    for(int k = 0; k < order+1; k++)
    {
    x[k] = 0;
    y[k] = 0;        
    }
    setCoef();
}

    
/**
 * @brief Set the Coef object
 * 
 */
template<int order>
void LowPass<order>::setCoef()
{
    if(adapt)
    {
    double t = micros()/1.0e6;
    dt = t - tn1;
    tn1 = t;
    }
    
    double alpha = omega0*dt;
    if(order == 1){
    a[0] = -(alpha - 2.0) / (alpha+2.0);
    b[0] = alpha/(alpha+2.0);
    b[1] = alpha/(alpha+2.0);        
    }
    if(order==2){
    double c1 = 2*sqrt(2)/alpha;
    double c2 = 4/(alpha*alpha);
    double denom = 1.0+c1+c2;
    b[0] = 1.0/denom;
    b[1] = 2.0/denom;
    b[2] = b[0];
    a[0] = -(2.0-2.0*c2)/denom;
    a[1] = -(1.0-c1+c2)/(1.0+c1+c2);      
    }
}


/**
 * @brief return the fitered value
 * 
 * @param xn the value to be filtered
 * @return double the filtered value
 */
template<int order>
inline double LowPass<order>::filt(double xn)
{
    // Provide me with the current raw value: x
    // I will give you the current filtered value: y
    if(adapt){
    setCoef(); // Update coefficients if necessary      
    }
    y[0] = 0;
    x[0] = xn;
    // Compute the filtered values
    for(int k = 0; k < order; k++){
    y[0] += a[k]*y[k+1] + b[k]*x[k];
    }
    y[0] += b[order]*x[order];

    // Save the historical values
    for(int k = order; k > 0; k--){
    y[k] = y[k-1];
    x[k] = x[k-1];
    }

    // Return the filtered value    
    return y[0];
}
#endif