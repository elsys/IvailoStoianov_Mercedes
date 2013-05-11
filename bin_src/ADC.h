
//Header file for ADC measurement

#ifndef __ADC_H__
#define __ADC_H__


//*****************************************************************************
// Definitions for ADC measurement
//*****************************************************************************
#define ADC_PRECISION 2
#define LITER_MODE	  0
#define PERC_MODE	  1
#define TANK_CAP	  1050
#define FUEL_PULSES	  5

//*****************************************************************************
// Public variables
//*****************************************************************************
extern U16 FuelLevel;
extern U8  FuelMode;
extern U16  Consumption;
extern U8  Consumption_Dec;
extern U16 Range;

//*****************************************************************************
// Functions for working with ADC for Fuel measurement
//
// Complete descriptions are in ADC.c
//*****************************************************************************
void InitADC();
void MeasureADC();
void DetermineConsumption(U16 tick_count, U8 dist);

#endif