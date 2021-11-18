#include <xc.h>
#include "UART.h"
#include "motorPWM.h"
#include "motorQEI.h"
#include <stdbool.h>


// Setup UART communiction
void uartInit()
{
    AD1PCFGL = 0xFFFF;
    // Pins remappen
    // Assign U1RX to RP8, Pin 17
    RPINR18bits.U1RXR = 8;
    // Assign U1TX to RP9, Pin 18
    RPOR4bits.RP9R = 3;
    TRISBbits.TRISB8 = 1; 
    TRISBbits.TRISB9 = 0;
    
    U1MODEbits.STSEL = 0; // 1 Stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8 data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1MODEbits.BRGH = 1; // High Speed mode
    U1MODEbits.UEN = 0;
    U1STAbits.ADDEN = 0;
    
    // Baud rate HC 05: 9600
    U1BRG = 172;
    
    U1STAbits.UTXISEL0 = 0; // Interrupt after one TX Character is transmitted
    U1STAbits.UTXISEL1 = 0;
    
    U1STAbits.URXISEL = 0;  // Interrupt after one RX Character is received
    
    U1STAbits.OERR    = 0;	        			// Clear the Receive Overflow Flag.
	IFS0bits.U1RXIF   = 0;          			// Clear the UART1 Receive Interrupt flag.
	IEC0bits.U1RXIE   = 1;       
    
    IEC0bits.U1TXIE = 1; // Enable UART TX Interrupt
    IEC0bits.U1RXIE = 1;
    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
    
    
    __delay32(10);
    
}

void setupUART(void)
{
    U1MODEbits.UARTEN = 0;
    U1BRG = 173;
    U1MODEbits.WAKE = 0;
    U1MODEbits.STSEL = 0; // 1 Stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8 data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1MODEbits.BRGH = 0; // High Speed mode
    U1MODEbits.LPBACK = 0;
    
    
    IFS0bits.U1RXIF   = 0;    
    IFS0bits.U1TXIF   = 0;

    IPC2bits.U1RXIP = 3;
    IPC3bits.U1TXIP = 5;
    
    U1STAbits.URXISEL = 0;
    U1STAbits.UTXEN = 1;
    
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1TXIE = 0;
    
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    
    __delay32(10);
    //U1TXREG = 'a';
    
}



void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
   
    unsigned int rxData;
    rxData = U1RXREG;
    while(!U1STAbits.TRMT);
   
    U1TXREG = rxData;
   
    if(rxData == 'l')
    {
        startMotorRight(1);
        startMotorLeft(1);
        changeSpeedMotorRight(21000);
        changeSpeedMotorLeft(21000);
        initTimer1(416);
        enableTimer1(1);
        
        U1TXREG = 'l';
    }
    
    U1STAbits.OERR = 0;
	IFS0bits.U1RXIF = 0; // clear TX interrupt flag
}

void putsUART(char *buffer)
{
    char * temp_ptr = (char *) buffer;
    if(U1MODEbits.PDSEL == 3)
    {
        while(*buffer != '\0')
        {
            while(U1STAbits.UTXBF);
            U1TXREG = *buffer++;
        }
    }
    else
    {
        while(*temp_ptr != '\0')
        {
            while(U1STAbits.UTXBF);
            U1TXREG = *temp_ptr++;
        }
        
        
    }
}


void uartWrite(char send){
    while(U1STAbits.UTXBF== 1);
    U1TXREG = send;
}


char busyUART(void) {
    if (!IFS0bits.U1TXIF) {
        return 1;
    }else {
        IFS0bits.U1TXIF=0;
        return 0;
    }
}

void printStringUART(unsigned char *String) {
    while(*String) {
        WriteCharToUART1(*String++);
    }
}

void writeCharToUART(unsigned char Data) {
    U1TXREG=Data;
    while(busyUART());
}
