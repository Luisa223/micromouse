#include <xc.h>
#include "motorPID.h"
#include <math.h>
#include "motorPWM.h"
#include "motorQEI.h"
#include <stdbool.h>

#define SAMPLING_RATE   1000        // how many times is timer called per second
#define WHEEL_RADIUS    35          // radius of wheel  --> for actual mouse: 30
#define ENCODER_MODE    4           // defines the pulses per wheel rotation
#define GEAR_RATIO      1           // ratio of gearbox --> for actual mouse: 33
#define PULSES_PER_ROT  16          // Pulses per motor rotation of encoder
#define M_PI            3.141592654 // Pi constant
#define ONE_CELL        180         // one cell distance in mm
#define TARGET_SPEED    50          // in mm/s
#define ACC_DEC_RATE    200         // Acceleration/Decelration rate
#define BRAKE_POS       30          // accelearate + decelerate for the first & last 30 cm to reach a trapezoidal speed profil    
#define WHEEL_DISTANCE  80          // distance between the two wheels -> defines, how far both wheels have to turn for direction change
#define MOTOR_VOLTAGE   6           // Max voltage for our motors is 6 volt



int mode = 0;
bool moving = 0; // variable which gets set when an action/command is executed


// Controller values
const float KP_FWD = 0;
const float KD_FWD = 0;

const float KP_ROT = 0;
const float KD_ROT = 0;

float encoderValTrans = 0;
float encoderValRot = 0;

// Pos in mm
float targetPosTrans = 180;         // one cell distance
float targetPosRot = 0;
float currentPosition = 0;
float currentPosErrorTrans = 0;
float currentPosErrorRot = 0;
float oldPosError = 0;
float correctionValueTrans = 0;
float correctionValueRot = 0;

// Encoder helper variables
long currentEncoderVal1 = 0;
long currentEncoderVal2 = 0;

long oldEncoderVal1 = 0;
long oldEncoderVal2 = 0;

int encoderDelta1 = 0;
int encoderDelta2 = 0;

long encoderCount1 = 0;
long encoderCount2 = 0;

float currentVelocity;

bool checkWall(int direction)
{
 return 1;   
}


void moveForward(int numberOfCells)
{
    moving = 1;
    resetSpeedParameters();
    targetPosTrans = numberOfCells * ONE_CELL;
    mode = forward;
    enableTimer1(1);
    while(moving);
}

void leftTurn()
{
    moving = 1;
    resetSpeedParameters();
    mode = turnLeft;
    enableTimer1(1);
    while(moving);
}

void rightTurn()
{
    moving = 1;
    resetSpeedParameters();
    mode = turnRight;
    enableTimer1(1);
    while(moving);
}

// Helper functions - distance in mm
float encoderToDistance(long encoderCount)
{
    // Wheel perimeter through encoder value per wheel rotation = mm/count -> times count
    return (WHEEL_RADIUS*M_PI*2/(ENCODER_MODE*PULSES_PER_ROT*GEAR_RATIO))*encoderCount;
}

long distanceToEncoder(float distance)
{
    return (distance*ENCODER_MODE*PULSES_PER_ROT*GEAR_RATIO)/(WHEEL_RADIUS*M_PI*2);   
}

float encoderToDeg(long encoderCount)
{
    return (WHEEL_RADIUS*M_PI*2/WHEEL_DISTANCE*M_PI)*360/(ENCODER_MODE*PULSES_PER_ROT*GEAR_RATIO)*encoderCount;
}

long degToEncoderCount(float degree)
{
    return (((degree/360)*WHEEL_DISTANCE*M_PI)/WHEEL_RADIUS*M_PI*2)*ENCODER_MODE*PULSES_PER_ROT*GEAR_RATIO;
}

float encoderDeltaToSpeed(float encoderDelta)
{
    return encoderToDistance(encoderDelta)/SAMPLING_RATE;   // velocity in mm/s
}
// Controller functions
void readCurrentEncoderPos()
{
    currentEncoderVal1 = POSCNT;
    currentEncoderVal2 = POS2CNT;
    
    encoderDelta1 = currentEncoderVal1 - oldEncoderVal1;
    encoderDelta2 = currentEncoderVal2 - oldEncoderVal2;
    currentPosition = encoderToDistance((encoderDelta1 + encoderDelta2)/2);
    
    oldEncoderVal1 = currentEncoderVal1;
    oldEncoderVal2 = currentEncoderVal2;
    
    encoderCount1 += encoderDelta1;
    encoderCount2 += encoderDelta2;
    
    
}

void comparePosToTarget()
{
    encoderValTrans = 0.5 * (encoderCount1 + encoderCount2);
    encoderValRot = encoderDelta1 - encoderDelta2;      // -> if both have same value = same speed = 0
    
    currentPosErrorTrans = targetPosTrans - encoderToDistance(encoderValTrans);
    currentPosErrorRot = targetPosRot - encoderToDeg(encoderValRot);      // Todo: encoder difference to angle
    correctionValueTrans = KP_FWD*currentPosErrorTrans + KD_FWD*(currentPosErrorTrans - oldPosError);
    correctionValueRot = KP_ROT*currentPosErrorRot + KD_ROT*(currentPosErrorRot - oldPosError);
    
    oldPosError = currentPosErrorTrans;
}

void decideForSpeedChanges()
{
    if(currentPosition < (targetPosTrans - BRAKE_POS))       // Accelerate
    {
        if(currentVelocity < TARGET_SPEED)
        {
            currentVelocity += ACC_DEC_RATE * (1/SAMPLING_RATE);
        }
    }
    else if(currentPosition > (targetPosTrans - BRAKE_POS))   // Brake
    {
        currentVelocity -= ACC_DEC_RATE * (1/SAMPLING_RATE);
    }
}

void updateMotorPWM()
{
    changeSpeedMotorRight(correctionValueTrans + correctionValueRot);
    changeSpeedMotorLeft(correctionValueTrans - correctionValueRot);
}

void resetSpeedParameters()
{
    encoderDelta1 = 0;
    encoderDelta2 = 0;
    targetPosTrans = 0;
    currentPosition = 0;
    moving = 0;
    mode = 0;
}



void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    switch(mode)
    {
        case stop:
            
            break;
        case forward:
            forwardController();
            break;
        case turnLeft:
            break;
        case turnRight:
            break;
        default:
            break;
    }
            
   IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
  
}

void forwardController()
{
    readCurrentEncoderPos();
    comparePosToTarget();
    decideForSpeedChanges();
    updateMotorPWM();   
}

void turnController(bool right)
{
    float distanceRobotCircle = M_PI * WHEEL_DISTANCE;  // One fourth has to be travelled 
    float distanceTurn = distanceRobotCircle / 4;
    if(right)
    {
        rightTurn();
        
    }
    else if (!right) // Turn left
    {
        leftTurn();
    }
}

