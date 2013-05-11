
//Header file for Real-Time Clock

#ifndef __CLOCK_H__
#define __CLOCK_H__


//*****************************************************************************
// Time and Calendar variables
//*****************************************************************************
extern U8 hours;
extern U8 minutes;
extern U8 seconds; 
extern U8 date;
extern U8 month;
extern U8 year;
extern U8 day;

//*****************************************************************************
// service variable
//*****************************************************************************
extern U8 tobeAdjusted;


//*****************************************************************************
// Real-Time Clock functions declarations
//*****************************************************************************
void ClockInit		(void);
void ClockStart		(void);
void ClockStop		(void);
void ClockTick		(void);
U8	 EndOfMonth		(U8 monthp, U8 yearp);


#endif