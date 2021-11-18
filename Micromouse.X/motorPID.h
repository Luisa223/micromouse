#ifndef MOTORPID_H
#define	MOTORPID_H
#include <stdbool.h>

enum mode                           // set mode for controller functions
{
    stop,
    forward,
    turnLeft,
    turnRight
};

enum wallDirection                  // direction to check for wall
{
    ahead,
    right,
    left
};

// API functions
void moveForward(int numberOfCells);
void leftTurn();
void rightTurn();


// Helper functions
float encoderToDistance(long encoderCount);
long distanceToEncoder(float distance);

float encoderToDeg(long encoderCount);
long degToEncoderCount(float degree);
float encoderDeltaToSpeed(float encoderDelta);
void decideForSpeedChanges();

// Controller functions
void readCurrentEncoderPos();
void comparePosToTarget();
void updateMotorPWM();
void resetSpeedParameters();
void forwardController();
void turnController(bool right);
bool checkWall(int direction);
void checkExitCondition();

#endif	/* MOTORPID_H */

