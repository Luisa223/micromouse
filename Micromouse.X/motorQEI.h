#include <stdbool.h>

#ifndef MOTORQEI_H
#define	MOTORQEI_H

void initQEI1(unsigned int startpos);
void initQEI2(unsigned int startpos);
void initTimer1(unsigned int period);
void enableTimer1(bool start);

int getEncoder1Pos(void);
int getEncoder2Pos(void);

#endif	/* MOTORQEI_H */

