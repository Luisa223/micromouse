#include <xc.h>
#include "motorController.h"
#include "motorPWM.h"
#include "motorQEI.h"

#define MAX_SPEED 26666
#define STOP 0
#define ONE_CELL_DIST 180 // distance of one cell is 180 mm
#define MOVE_SPEED 25000
#define TURN_SPEED 25000


// Needed functions
// moveForward
// turnRight/Left
// checkWall(front, right, left)


// Global variables for controller properties
// Speed
float curSpeedTrans = 0;
float targetSpeedTrans = 26666;
float curSpeedRot = 0;
float targetSpeedRot = 0;
// Encoder
int encoderValTrans = 0;
int encoderValRot = 0;
// Controller
float pidTrans = 0;
float pidRot = 0;
float posErrorTrans = 0;
float posErrorRot = 0;
float oldPosErrorTrans = 0;
float oldPosErrorRot = 0;

int pwmValTrans = 0;
int pwmValRot = 0;


float kpX = 2, kdX = 4;
float kpW = 3, kdW = 6;//used in straight
float kpW1 = 1;//used for T1 and T3 in curve turn
float kdW1 = 26;
float kpW2 = 1;//used for T2 in curve turn
float kdW2 = 36;
float accX = 6;//6m/s/s  
float decX = 6; 
float accW = 1; //cm/s^2
float decW = 1;

float currentEncoderLeft = 0;
float currentEncoderRight = 0;

long oldEncoderLeft = 0;
long oldEncoderRight = 0;

long leftEncoderDelta = 0;
long rightEncoderDelta = 0;
long encoderDelta = 0;

long leftEncoderCount = 0;
long rightEncoderCount = 0;
long encoderCount = 0;

long leftBaseSpeed = 0;
long rightBaseSpeed = 0;


// based on 1ms sampling rate!! in mm/s
float countsToVelocityMMS(int countsDelta)
{
    float countsInMM = 0.1; // 1 count = 0.1 mm
    // Wheels actual mouse: 30*2*pi/(16*4*33) = 0.089 mm/count
    return countsDelta * 1000 * countsInMM;
}

// based on 1ms sampling rate!! in mm/s
float velocityToCounts(float velocity)
{
    float mmInCounts = 10; // 1mm = 10 counts
    // Actual mouse = 11.235 counts/mm
    return velocity/1000/mmInCounts;
}

void getEncoderStatus()
{
    // get encoder values --> Timer
    
    currentEncoderLeft = POS2CNT;
    currentEncoderRight = POSCNT;
    char buffer2[64];
    sprintf(buffer2, "Pos 2: %f, %f \n", currentEncoderLeft, currentEncoderRight);
    printf(buffer2);
    
    
    leftEncoderDelta = currentEncoderLeft - oldEncoderLeft;
    rightEncoderDelta = currentEncoderRight - oldEncoderRight;
    encoderDelta = (leftEncoderDelta + rightEncoderDelta)/2;
    
    oldEncoderLeft = currentEncoderLeft;
    oldEncoderRight = currentEncoderRight;
    
    leftEncoderCount += leftEncoderDelta;
    rightEncoderCount += rightEncoderDelta;
    encoderCount = (rightEncoderCount + leftEncoderCount)/2;
    
}





// Check speed in translational and rotational direction
void updateSpeed()
{
    targetSpeedTrans = countsToVelocityMMS(targetSpeedTrans);
    if(curSpeedTrans < targetSpeedTrans)
    {
        // accelerate
        curSpeedTrans += accX; // add acceleration rate
        if(curSpeedTrans > targetSpeedTrans)
        {
            curSpeedTrans = targetSpeedTrans;
        }
    }
    else if(curSpeedTrans > targetSpeedTrans)
    {
        // brake/deccelerate
        curSpeedTrans -= decX; // add acceleration rate
        if(curSpeedTrans < targetSpeedTrans)
        {
            curSpeedTrans = targetSpeedTrans;
        }
        
    }
    if(curSpeedRot < targetSpeedRot)
    {
        //curSpeedTrans += 
        curSpeedRot += accW;
        if(curSpeedRot > targetSpeedRot)
        {
            curSpeedRot = targetSpeedRot;
        }
    }
    else if(curSpeedTrans > targetSpeedTrans)
    {
        curSpeedRot -= decW;
        if(curSpeedRot < targetSpeedRot)
        {
            curSpeedRot = targetSpeedRot;
        }
        
    }
}

// Calculation of motor PWM based on a PD controller
void calculateMotorPWM(void)
{
    encoderValTrans = rightEncoderDelta + leftEncoderDelta;
    encoderValRot = rightEncoderDelta - leftEncoderDelta;
    
    posErrorTrans += velocityToCounts(curSpeedTrans) - encoderValTrans;
    posErrorRot += velocityToCounts(curSpeedRot) - encoderValTrans;
    
    pwmValTrans = kpX * posErrorTrans + kdX * (posErrorTrans - oldPosErrorTrans);
    pwmValRot = kpW * posErrorRot + kdW * (posErrorRot - oldPosErrorRot);
    
    oldPosErrorTrans = posErrorTrans;
    oldPosErrorRot = posErrorRot;
    
    rightBaseSpeed = pwmValTrans + pwmValRot;
    leftBaseSpeed = pwmValTrans - pwmValRot;
            
    changeSpeedMotorRight(rightBaseSpeed);
    changeSpeedMotorLeft(leftBaseSpeed);
}


void getCurrentSpeed()
{
    static int pos1A = 0;
    static int pos1B = 0;
    static int pos2A = 0;
    static int pos2B = 0;
    
    int posDelta1 = 0;
    int posDelta2 = 0;
    float posDeltaBoth = 0;
    float velocity = 0;
    int count = 0;
    switch(count)
    {
        case 0:
            pos1A = POSCNT;
            pos2A = POS2CNT;
            count = 1;
            break;
        case 1:
            pos1B = POSCNT;
            pos2B = POS2CNT;
            count = 0;
            break;
    }
    
    posDelta1 = pos1B - pos1A;
    posDelta2 = pos2B - pos2A;
    posDeltaBoth = (posDelta1 + posDelta2)/2;
    curSpeedTrans = countsToVelocityMMS(posDeltaBoth);
}

/*
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
   getCurrentSpeed(); // TODO: make one function of the first two
   getEncoderStatus();
   updateSpeed();
   calculateMotorPWM();
   
    
   
   
   static int count=0;
   static int pos1 = 0;
   static int pos2 = 0;
   
   static int pos1B = 0;
   static int pos2B = 0;
   
   float velocity = 0;
   static int uartCounter = 0;
   //static float encoderStartCount = getPositionInCounts_1();
   static float posDelta = 0;
   int helper = 0;
   
   switch(count)
   {
       
       case 1:
        pos2 = POSCNT;
        pos2B = POS2CNT;
        velocity = 0.001 * (pos2 - pos1);
        
        posDelta += abs(pos2 - pos1);
        
        
        char buffer2[64];
        sprintf(buffer2, "Pos 2: %i, %i \n", pos2, pos2B);
        printf(buffer2);
        
        count = 0;
        helper++;
        break;
   
       case 0:
        pos1  = POSCNT;
        pos1B = POS2CNT;
        velocity = 0.001 * (pos2 - pos1);
        
        char buffer1[64];
        sprintf(buffer1, "Pos 1: R: %i, L: %i \n", pos1, pos1B);
        printf(buffer1);
        
        count = 1;
        helper++;
        break;
        
       default:
           count = 0;
           break;
   }
    
   if(posDelta >= 1800)
   {
        PWM1CON1bits.PEN1H = 0;
        PWM1CON1bits.PEN1L = 0; 
        PWM1CON1bits.PEN2H = 0;
        PWM1CON1bits.PEN2L = 0; 
   }
    
   
   
       //sprintf(buffer, "%f", velocity);
   uartCounter++;
    
   IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
  
}

*/

// Control functions for mapping mode
void mapGoStraight(int numberOfSquares)
{
    float distanceToGo = numberOfSquares * ONE_CELL_DIST;
    //int encoderCountsNeeded = distanceMMToEncoderCounts(distanceToGo);
    //do
    //{
        
    //}while(encoderCount < encoderCountsNeeded);
}


