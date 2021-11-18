#include <xc.h>
#include "motorQEI.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

long rotationCount1;
long rotationCount2;

int getEncoder1Pos()
{
    return rotationCount1 * MAXCNT + POSCNT;
}

int getEncoder2Pos()
{
    return rotationCount2 * MAX2CNT + POS2CNT;
}

// Two Encoder Modules for one motor each
void initQEI1(unsigned int startpos)
{
    
    QEI1CONbits.QEISIDL = 1; // discontinue in idle mode
    QEI1CONbits.QEIM = 0b111; // Quadrature Interface endcoder enabled (x4mode) with positon counter reset by match (MAX1CNT)
    QEI1CONbits.SWPAB = 1; // Swap Phase A & B
    QEI1CONbits.PCDOUT = 0; // Disable position counter direction pin
    QEI1CONbits.TQGATE = 0; // Timer gated time acc disabled
    QEI1CONbits.POSRES = 0; // index does not reset position counter
    QEI1CONbits.TQCS = 0; // internal clock slource Tcy
    QEI1CONbits.UPDN_SRC = 0; // direction of position counter determined using internal logic
    
    MAXCNT = 0xFFFF;
    POSCNT = 0x7FFF;
    rotationCount1 = 0;
    
    IFS3bits.QEI1IF = 0; // clear interrupt flag
    IEC3bits.QEI1IE = 1; // enable interrupt
    IPC14bits.QEI1IP = 5; // interrupt priority
    
    
}
void initQEI2(unsigned int startpos)
{
    QEI2CONbits.QEISIDL = 1; // discontinue in idle mode
    QEI2CONbits.QEIM = 0b111; // Quadrature Interface endcoder enabled (x4mode) with positon counter reset by match (MAX1CNT)
    QEI2CONbits.SWPAB = 0; // Swap Phase A & B
    QEI2CONbits.PCDOUT = 0; // Disable position counter direction pin
    QEI2CONbits.TQGATE = 0; // Timer gated time acc disabled
    QEI2CONbits.POSRES = 0; // index does not reset position counter
    QEI2CONbits.TQCS = 0; // internal clock slource Tcy
    QEI2CONbits.UPDN_SRC = 0; // direction of position counter determined using internal logic
    
    MAX2CNT = 0xFFFF;
    POS2CNT = 0x7fff;        //startpos;
    rotationCount2 = 0;
    
    IFS4bits.QEI2IF = 0; // clear interrupt flag
    IEC4bits.QEI2IE = 1; // enable interrupt
    IPC18bits.QEI2IP = 5; // interrupt priority
}

void __attribute__((__interrupt__,auto_psv)) _QEI1Interrupt(void)
{
    IFS3bits.QEI1IF = 0;
    
    if(POSCNT < 32768)
    {
        rotationCount1 = rotationCount1+ (long) 0x10000; // positive roll-over
    }
    else
    {
        rotationCount1 = rotationCount1- (long) 0x10000; // negative roll-over
    }
}

void __attribute__((__interrupt__,auto_psv)) _QEI2Interrupt(void)
{
    IFS4bits.QEI2IF = 0;
    
    if(POS2CNT < 32768)
    {
        rotationCount2 = rotationCount2+ (long) 0x10000; // positive roll-over
    }
    else
    {
        rotationCount2 = rotationCount2- (long) 0x10000; // negative roll-over
    }
}

float getPositionInRad()
{
    long currentEncoderPosition;
    // disable interrupts to have consitent data
    _NSTDIS = 1;
    //GET_ENCODER_1 (currentEncoderPosition);
    currentEncoderPosition = getEncoder1Pos();
    _NSTDIS = 0; // enable interrupts again
    return 3.141592*2*currentEncoderPosition/(16*4*33); // 16 pulses per rotation times 4 mode times 33 motor rotations = 1 wheel rotation
}

long getPositionInCounts_1()
{
    long currentEncoderPosition;
    _NSTDIS = 1;
    currentEncoderPosition = getEncoder1Pos();
    _NSTDIS = 0;
    return currentEncoderPosition;
}

void initTimer1(unsigned int period) 
{
    T1CON = 0;              // ensure Timer 1 is in reset state
    T1CONbits.TGATE = 0;    // gated time accumulation disabled
    T1CONbits.TCKPS = 0b10; // FCY divide by 64: tick = 2.4us
    T1CONbits.TCS = 0;      // select internal FCY clock source
    TMR1 = 0;
    PR1 = period;           // set Timer 1 period register ()
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4;      // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1;      // enable Timer 1 interrupt
    T1CONbits.TON = 0;      // leave timer disabled initially
}

void enableTimer1(bool start) 
{
    T1CONbits.TON = start; //
}



