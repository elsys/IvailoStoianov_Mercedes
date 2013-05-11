
// header file for frequency meter using Timer1

#ifndef __F_METER_TIMER1_H__
#define __F_METER_TIMER1_H__

//*****************************************************************************
// Frequency Meter definitions
//*****************************************************************************
#define FREQ1	1
#define FREQ3	3
#define FREQS	5

//*****************************************************************************
// Frequency Meter functions declarations
//*****************************************************************************
void FMeterTick 				(void);
void FMeterIntTick				(void);
void FMeterInit 				(void);
void FMeterStartMeasurement 	(void);
void FMeterStopMeasurement		(void);
U16  FMeterReadLastMeasurement	(U8 freq);
U8   FMeterGetMeasurementStatus (void);


#endif //__F_METER_TIMER1_H__

