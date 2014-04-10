/*
 * serialServoControl.c
 *
 * Created: 4/10/2014 0:56:49 PM
 *  Author: jonha860
 */ 

#include <avr/io.h>
#include "serialServoControl.h"

#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>

#define coxa 56
#define femur 66
#define tibia 131
#define femurAngleAddition 0.2426
#define tibiaAngleAddition -3.1415/6
#define centerToFrontLegsY 120
#define centerToSideLegs 100
#define centerToFrontLegs 135
#define centerToFrontLegsX 61.85

double sinAngleToFrontLegs = centerToFrontLegsY / centerToFrontLegs;
double cosAngleToFrontLegs = centerToFrontLegsX / centerToFrontLegs;

void LegGoto(double x,double y,double z, int servospeed, int side, int servo1, int servo2, int servo3);
void moveLeg1too(double x, double y, double z, int servospeed);
void moveLeg2too(double x, double y, double z, int servospeed);
void moveLeg3too(double x, double y, double z, int servospeed);
void moveLeg4too(double x, double y, double z, int servospeed);
void moveLeg5too(double x, double y, double z, int servospeed);
void moveLeg6too(double x, double y, double z, int servospeed);

void LegGoto(double x,double y,double z, int servospeed, int side, int servo1, int servo2, int servo3)
{
	double alpha;
	double beta;
	double gamma;
	double d;
	
	gamma = atan(y/x);
	d = sqrt(pow(sqrt(x*x + y*y)-coxa, 2) + z*z);
	beta = 3.1415 - acos((femur*femur+tibia*tibia-d*d)/(2*femur*tibia));
	alpha = acos((femur*femur-tibia*tibia+d*d)/(2*femur*d))-asin(fabs(z)/d);
	
	servoGoto(servo1, gamma, servospeed);
	servoGoto(servo2, side*(alpha + femurAngleAddition),servospeed);
	servoGoto(servo3, side*(-beta + tibiaAngleAddition),servospeed);
}

void moveLeg1too(double x, double y, double z, int servospeed) //Help function to describe position of leg in standard base x,y,z
{
	double a = (x + centerToFrontLegsX)*cos(-3*3.1415/4)-(y-centerToFrontLegsY)*sin(-3*3.1415/4);
	double b = (x + centerToFrontLegsX)*sin(-3*3.1415/4)+(y-centerToFrontLegsY)*cos(-3*3.1415/4);
	LegGoto(a, b, z, servospeed,1,8,10,12);
}

void moveLeg2too(double x, double y, double z, int servospeed) //Help function to describe position of leg in standard base x,y,z
{
	double a = (x + centerToSideLegs)*cos(-3.1415)-(y)*sin(-3.1415);
	double b = (x + centerToSideLegs)*sin(-3.1415)+(y)*cos(-3.1415);
	LegGoto(a, b, z, servospeed,1,14,16,18);
}

void moveLeg3too(double x, double y, double z, int servospeed) //Help function to describe position of leg in standard base x,y,z
{
	double a = (x + centerToFrontLegsX)*cos(-5*3.1415/4)-(y+centerToFrontLegsY)*sin(-5*3.1415/4);
	double b = (x + centerToFrontLegsX)*sin(-5*3.1415/4)+(y+centerToFrontLegsY)*cos(-5*3.1415/4);
	LegGoto(a, b, z, servospeed,1,2,4,6);
}

void moveLeg4too(double x, double y, double z, int servospeed) //Help function to describe position of leg in standard base x,y,z
{
	double a = (x - centerToFrontLegsX)*cos(3.1415/4)-(y+centerToFrontLegsY)*sin(3.1415/4);
	double b = (x - centerToFrontLegsX)*sin(3.1415/4)+(y+centerToFrontLegsY)*cos(3.1415/4);
	LegGoto(a, b, z, servospeed,-1,1,3,5);
}

void moveLeg5too(double x, double y, double z, int servospeed) //Help function to describe position of leg in standard base x,y,z
{
	double a = (x - centerToSideLegs)*cos(0)-(y)*sin(0);
	double b = (x - centerToSideLegs)*sin(0)+(y)*cos(0);
	LegGoto(a, b, z, servospeed,-1,13,15,17);
}

void moveLeg6too(double x, double y, double z, int servospeed) //Help function to describe position of leg in standard base x,y,z
{
	double a = (x - centerToFrontLegsX)*cos(-3.1415/4)-(y-centerToFrontLegsY)*sin(-3.1415/4);
	double b = (x - centerToFrontLegsX)*sin(-3.1415/4)+(y-centerToFrontLegsY)*cos(-3.1415/4);
	LegGoto(a, b, z, servospeed,-1,7,9,11);
}

