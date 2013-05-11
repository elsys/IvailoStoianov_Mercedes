
//Header file for Gear driver

#ifndef __GEAR_H__
#define __GEAR_H__


//*****************************************************************************
// Definitions
//*****************************************************************************

//Gear Data attribute
#define SPEED		0
#define GEAR		1
#define LEFTLIMIT	3
#define RIGHTLIMIT	2

//Refresh rate of Gear task
#define GEAR_TASK_REFRESH_RATE	6

//Gead advisor mode attribute
#define ECONOMIC_MODE	2
#define SPORT_MODE		3

//Forward/Reverse gear attribute
#define FORWARD_GEAR	0
#define REVERSE_GEAR	1
#define NEUTRAL_GEAR	2

//Distance/Trip attribute
#define DISTANCE		0
#define TRIP			1
#define DIST_ADDRESS	10
#define TRIP_ADDRESS	100

//Car info
#define KM_PULSES		4414
#define RPM_PULSES		120


//*****************************************************************************
// Gear, Speed & RPM variables
//*****************************************************************************
extern U8 Speed;
extern U16 RPM;
extern U8 Gear;
extern U8 GearAdvisorMode;
extern U8 GearType;
extern U16 Distance;
extern U16 Trip;
extern U8 DistanceTripMode;

//*****************************************************************************
// Gear functions declarations
//*****************************************************************************
void InitGear		(void);
void DetermineGear	(void);
void DetermineSpeed	(U16 tick_count);
void DetermineRPM	(U16 tick_count);
void SaveTripDist	(void);
void ReadTripDist	(void);
void ClearTrip		(void);


#endif
