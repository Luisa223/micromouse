/* 
 * File:   motorController.h
 * Author: Luisa
 *
 * Created on 10. August 2021, 13:08
 */

#ifndef MOTORCONTROLLER_H
#define	MOTORCONTROLLER_H

// Control functions for mapping mode
void mapGoStraight(int numberOfSquares);
void getCurrentSpeed();
void calculateMotorPWM();
void updateSpeed();
void getEncoderStatus();
float countsToVelocityMMS(int countsDelta);
float velocityToCounts(float velocity);
#endif	/* MOTORCONTROLLER_H */

