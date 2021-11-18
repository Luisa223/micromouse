/* 
 * File:   UART.h
 * Author: Luisa
 *
 * Created on 11. Juli 2021, 22:12
 */

// UART communication with bluetooth module

#ifndef UART_H
#define	UART_H

#include <xc.h>  

void uartInit();

void printStringUART(unsigned char *String);
void writeCharToUART(unsigned char Data);
char busyUART(void);
void uartWrite(char send);

void setupUART(void);
void putsUART(char *buffer);












#endif	/* UART_H */

