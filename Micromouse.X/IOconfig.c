#include "xc.h"
#include "IOconfig.h"


void setupIO()
{

   
   //all pins are now digital, by default analogue
    AD1PCFGL=0xFFFF;
    //ADPCFG = 0x0000;
    // set LEDs as output
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;
   
    //PIN MAPPING
    
    // unlock
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)
    
    // PERIPHERAL receives data from which INPUT  
    //RPINR18bits.U1RXR = 9; //mapped to RP9 is U1 RX, CHANGE THIS
    // Assign U1RX to RP8, Pin 17
    RPINR18bits.U1RXR = 8;
    // Assign U1TX to RP9, Pin 18
    RPOR4bits.RP9R = 3;
    
    //PERIPHERAL QEA Encoder 1, receives data from RP10
   RPINR14bits.QEA1R = 10; 
    //PERIPHERAL QEB Encoder 1, receives data from RP11
   RPINR14bits.QEB1R = 11;
   
   RPINR16bits.QEA2R = 5;
   RPINR16bits.QEB2R = 6;
   
    
    // lock again
     __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)
     int i;
    for (i = 0; i < 30000; i++); // short dirty delay for changes to take effect,

    
}
