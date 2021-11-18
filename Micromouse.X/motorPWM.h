#include <xc.h>
#include <stdbool.h>
#ifndef MOTORPWM_H
#define	MOTORPWM_H

// Setup function for both motors using PWM1L1/H1 and PWM1L2/H2
void setupPWM();

// Set duty cycle for each motor = change speed
// TODO: define format of speed -> directly in PWM values, macros MAX/MIDDLE etc.
// Right motor uses PMW1L/H1
void changeSpeedMotorRight(unsigned int speed);

void startMotorRight(bool yes);
void startMotorLeft(bool yes);

// Left motor uses PMW1L/H2
void changeSpeedMotorLeft(unsigned int speed);
void changeSpeedMotorRight(unsigned int speed);

// Helpderfunction for motorcontroller
int voltToPWM(float volt);
float pwmToVolt(int pwm);


#endif	/* MOTORPWM_H */

