//=============================================================================================
// SensorFusion.h
//=============================================================================================
//
// Madgwick's implementation of Mahony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
// 23/11/2017   Aster			Optimised time handling and melted in one library
//
//=============================================================================================
#ifndef SensorFusion_h
#define SensorFusion_h

#include <cmath>

#include "../../core/Timer.h"
#include "Arduino.h"


//--------------------------------------------------------------------------------------------
// Variable declaration

class SF {
//-------------------------------------------------------------------------------------------
// Function declarations

public:

	SF();
	
	void MahonyUpdate(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz, double deltat);
	void MahonyUpdate(double gx, double gy, double gz, double ax, double ay, double az, double deltat);
	
	void MadgwickUpdate(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz, double deltat);
    void MadgwickUpdate(double gx, double gy, double gz, double ax, double ay, double az, double deltat);
	
	// find initial Quaternios
	// it is good practice to provide mean values from multiple measurements
    bool initQuat(double ax, double ay, double az, double mx, double my, double mz);
	//these values are already defined by arduino
	//const double DEG_TO_RAD = 0.0174532925199433f; //PI/180.0f;	
	//const double RAD_TO_DEG = 57.29577951308233f; //180.0f/PI
	
	double deltatUpdate (){
		Now = scoped_timer.elapsed();
		deltat = ((Now - lastUpdate) / 1000000000.0); // set integration time by time elapsed since last filter update
		lastUpdate = Now;
		return deltat;
	}

	double getRoll() {
		if (!anglesComputed) computeAngles();
		return roll * RAD_TO_DEG;
	}
	double getPitch() {
		if (!anglesComputed) computeAngles();
		return pitch * RAD_TO_DEG;
	}
	double getYaw() {
		if (!anglesComputed) computeAngles();
		return yaw * RAD_TO_DEG + 180.0;
	}
	double getRollRadians() {
		if (!anglesComputed) computeAngles();
		return roll;
	}
	double getPitchRadians() {
		if (!anglesComputed) computeAngles();
		return pitch;
	}
	double getYawRadians() {
		if (!anglesComputed) computeAngles();
		return yaw;
	}
	double* getQuat() {
		memcpy(_copyQuat, &q0, sizeof(double)*4);
		return _copyQuat;
	}

private:
	double beta;				//Madgwick: 2 * proportional gain
	double twoKp;			//Mahony: 2 * proportional gain (Kp)
	double twoKi;			//Mahony: 2 * integral gain (Ki)
	double q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
	double integralFBx, integralFBy, integralFBz;  // integral error terms scaled by Ki
	bool anglesComputed;
	static inline double invSqrt(double x) __attribute__((always_inline));
	void vectorCross(double A[3], double B[3], double cross[3]);
	void computeAngles();
	double roll, pitch, yaw;
	double Now,lastUpdate,deltat;
	double _copyQuat[4];	// copy buffer to protect the quaternion values since getters!=setters
	
};

#endif