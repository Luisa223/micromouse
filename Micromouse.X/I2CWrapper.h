/* 
 * File:   I2CWrapper.h
 * Author: Luisa
 *
 * Created on 7. June 2021, 16:29
 */

#ifndef I2C_H
#define	I2C_H

// default pull ups are too low for 400kHz!!
#define FOSC            (unsigned long) 53300000
#define FCY             (unsigned long) FOSC/2 //default FCY (MHZ --> 1/2* FOSC)
#define I2C_BAUDRATE    (unsigned long) 400000 //default baud rate for 100kHz


// Helper functions for I2C communication with range sensors
extern void I2CInit(void);
extern void I2CStart(void);
extern void I2CRestart(void);
extern void I2CStop(void);
extern void I2CWaitACK(void);
extern void I2CIdle(void);
extern void I2CAck(void);
extern void I2CNack(void);
extern void I2CWrite(unsigned char c);
extern char I2CRead(void);


// functions to be called for I2C communication
extern void I2CWriteReg(char addr, char byteHigh, char byteLow, char value);
extern char I2CReadReg(char addr, char byteHigh, char byteLow);
extern int setBaudRate(void);

#endif	/* I2C_H */

