#include "I2CWrapper.h"
#include "xc.h"


void I2CInit(void)
{
    // configure as inputs
    TRISBbits.TRISB8 = 1; 
    TRISBbits.TRISB9 = 1;
    
   // set to OpenDrain mode
    ODCBbits.ODCB5 = 1;     
    ODCBbits.ODCB6 = 1;
    
    
    I2C1CONbits.I2CEN = 0;	
	I2C1CONbits.I2CSIDL 	= 0;
	I2C1CONbits.IPMIEN 	= 0;
	I2C1CONbits.A10M		= 0;
	I2C1CONbits.DISSLW 	= 0;
	I2C1CONbits.SMEN 	= 0;
    
    // for baud rate calculation see:      http://ww1.microchip.com/downloads/en/devicedoc/70000195f.pdf    
    I2C1BRG = 0x01A;           
   
    I2CIdle();
    I2C1CONbits.I2CEN 	= 1;
    I2CIdle();
    
    // Set start condition
    I2C1CONbits.SEN = 1;        
}
 
int setBaudRate(){
    return ((1/I2C_BAUDRATE - 1/10000000) - FCY/2) - 2;
}

void I2CAck(void)
{
    // Send ACK
    I2C1CONbits.ACKDT = 0;
    // Initiate Acknowledge and transmit ACKDT
    I2C1CONbits.ACKEN = 1;      
    while(I2CCONbits.ACKEN);
}


void I2CNack(void)
{
    // Send NACK
    I2C1CONbits.ACKDT = 1;      
    // Initiate Acknowledge and transmit ACKDT
    I2C1CONbits.ACKEN = 1;      
    while(I2C1CONbits.ACKEN);  
}
 
void I2CStop(void)
{
    I2C1CONbits.RCEN = 0;       // receive mode not in progress
    I2C1CONbits.PEN = 1;        // Stop condition
    while(I2C1CONbits.PEN);
}

// For setting start condition again after failure/reboot
void I2CStart(void)
{
    // Reset any ACK
    I2C1CONbits.ACKDT = 0;      
    I2CIdle();
    // Start
    I2C1CONbits.SEN = 1;        
    while(I2C1CONbits.SEN);
    
}

void I2CRestart(void)
{
    // Repeated Start Condition
    I2C1CONbits.RSEN = 1;       
    while(I2C1CONbits.RSEN);
    I2C1CONbits.ACKDT = 0;      
    I2C1STATbits.TBF = 0;       // I2C1TRN must be empty!
}
 
void I2CWaitACK(void)
{
    while(I2C1STATbits.ACKSTAT);
}

// Check all the bits that must be cleared before idle
void I2CIdle(void)
{
    while(I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT || I2C1STATbits.ACKSTAT);
}
 
void I2CWrite(unsigned char c)
{
    while(I2C1STATbits.TBF);
    I2C1TRN = c;
    while(I2C1STATbits.TBF);
}


// 
char I2CRead(void)
{
    
    char temp;
    I2C1CONbits.RCEN = 1;
    Nop();              // Works more reliable with nop instruction!
    
    while(!I2C1STATbits.RBF);
    I2CIdle();
    I2CNack();
    I2CIdle();
    I2CStop();
    temp = I2C1RCV;
    return temp;
}

// Entire write process
void I2CWriteReg(char addr, char byteHigh, char byteLow, char value)
{
    
    I2CStart();
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    I2CWrite(byteHigh);
    I2CIdle();
    I2CWrite(byteLow);
    I2CIdle();
    I2CWrite(value);
    I2CIdle();
    I2CStop();
}
 
// Entire read process 
char I2CReadReg(char addr, char byteHigh, char byteLow)
{
    char temp;
    // Start Condition
    I2CStart();
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    I2CWrite(byteHigh);
    I2CIdle();
    I2CWrite(byteLow);
    I2CIdle();
    I2CRestart();
    I2CWrite((addr<<1)|0x01);
    I2CIdle();  
    I2CRead();
    I2CNack();
    I2CStop();
    temp = I2C1RCV;
    return temp;
}