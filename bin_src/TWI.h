
//Header file for TWI driver

#ifndef __TWI_H__
#define __TWI_H__


//*****************************************************************************
// Definitions for TWI transmiting -> Address + Read/Write Bit
//*****************************************************************************
#define SLAW 0x90
#define SLAR 0x91


//*****************************************************************************
// Public variables
//*****************************************************************************
extern U8 TWIStopped;
extern U8 TWIByte;


//*****************************************************************************
// Functions for working with TouchScreen
//
// Complete descriptions are in TWI.c
//*****************************************************************************
void TWIInit(void);
void TWIStart(void);
void TWIStop(void);
void TWISLA(U8 sla_byte);


#endif