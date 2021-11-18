#ifndef IOCONFIG_H
#define	IOCONFIG_H

// LEDs for debugging purposes
#define LED4 LATBbits.LATB15
#define LED5 LATBbits.LATB14
#define LED6 LATBbits.LATB13
#define LED7 LATBbits.LATB12

#define SW1 !PORTBbits.RB5 

#define SEN_SHDN1 LATBbits.LATB11
#define SEN_SHDN2 LATBbits.LATB10
#define SEN_SHDN3 LATBbits.LATB9
#define SEN_SHDN4 LATBbits.LATB8
#define SEN_SHDN5 LATBbits.LATB7

#define LEDON 0
#define LEDOFF 1

void setupIO();

#endif	/* IOCONFIG_H */

