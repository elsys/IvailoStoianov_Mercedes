
//Header file for TouchScreen driver 

#ifndef __TOUCHSCREEN_DRIVER_H__
#define __TOUCHSCREEN_DRIVER_H__


//*****************************************************************************
// Definitions for ADC measurement
//*****************************************************************************
#define ADC_PRESC 0x06
#define PRECISION 5


#define TSCR_PORT F


//*****************************************************************************
// Public variables
//*****************************************************************************
extern U16 TouchScreenValueY;
extern U16 TouchScreenValueX;


//*****************************************************************************
// Functions for working with TouchScreen
//
// Complete descriptions are in touchscreen_driver.c
//*****************************************************************************
void TouchScreenInit(void);
void Init(U8 axis);
void TouchScreenStart(void);
void TouchScreenStop(void);
void TouchScreenMeasure(void);
void Measure(U8 axis);



#endif