#include "motorPWM.h"

#define MYPWM_MAX 26666
#define SPEED_LIMIT_MIN 1000
#define SPEED_LIMIT_MAX 25000

void setupPWM()
{
    /* PWM1H1 *, configured to 1kHz, based on fcyc = 26.666 MIPS, Tcycle=37.5nsec/
     * 1ms/37.5nsec = 26666.666 ==> 26666 (fits in 15 bits)
     * of course, we could use a pre-scaler and end up somewhere else
     */
    P1TCONbits.PTEN = 0; // Switch off PWM generator
    P1TCONbits.PTCKPS = 0b00; // Sets prescaler, available are 1(00),4(01),16(10) or 64(11)
    P1TPER = MYPWM_MAX/2; //15 bit register
    PWM1CON1bits.PMOD1 = 0; // set PWM unit 1 to independent mode
    PWM1CON1bits.PMOD2 = 0;     // -> 1 -> both LEDs are parallel
    
    PWM1CON1bits.PEN1H = 0; // enable  PWM driver PWM1H1
    PWM1CON1bits.PEN2H = 0; // enable PWM driver
    PWM1CON1bits.PEN3H = 0; // enable PWM driver
    PWM1CON1bits.PEN1L = 0; // disable PWM driver 
    PWM1CON1bits.PEN2L = 0; // enable PWM driver
    PWM1CON1bits.PEN3L = 0; // disable PWM driver

    P1TCONbits.PTEN = 1; // Switch on PWM generator
    
}

// start and stop the motor by en/disabling the PWM driver
void startMotorRight(bool yes)
{
    if(yes)
    {
        PWM1CON1bits.PEN1H = 1;
        PWM1CON1bits.PEN1L = 1; // disable PWM driver 
    }
    else
    {
        PWM1CON1bits.PEN1H = 0;
        PWM1CON1bits.PEN1L = 0; 
    }
}

void startMotorLeft(bool yes)
{
    if(yes)
    {
        PWM1CON1bits.PEN2H = 1;
        PWM1CON1bits.PEN2L = 1; 
    }
    else
    {
        PWM1CON1bits.PEN2H = 0;
        PWM1CON1bits.PEN2L = 0; 
    }
}
// Right motor uses PMW1L/H1
void changeSpeedMotorRight(unsigned int speed)
{
    // check if value makes senses
    if(speed > SPEED_LIMIT_MIN && speed < SPEED_LIMIT_MAX)
    {
        P1DC1 = speed; 
    }
    
    
}

// Left motor uses PMW1L/H2
void changeSpeedMotorLeft(unsigned int speed)
{
   // check if value makes senses
    if(speed > SPEED_LIMIT_MIN && speed < SPEED_LIMIT_MAX)
    {
        P1DC2 = speed; 
    }
}

