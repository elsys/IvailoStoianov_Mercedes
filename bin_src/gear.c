
//Implementation file for Gear driver

#include <avrx-io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include  "avrx.h"
#include "gtypes.h"
#include "gear.h"
#include "gear_data.h"


//*****************************************************************************
// externed variables from gear.h
//*****************************************************************************
U8 Speed;
U16 RPM;
U8 Gear;
U8 GearType;
U8 GearAdvisorMode;
U16 Distance;
U16 Trip;
U8 DistanceTripMode;


//*****************************************************************************
//
// Function : Gear, Speed & RPM initialization function
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void InitGear	(void){

	//init variables
	Speed = 0;
	RPM = 1500;
	Gear = 0;
	GearAdvisorMode = ECONOMIC_MODE;
	GearType = FORWARD_GEAR;

	ReadTripDist();

	DistanceTripMode = DISTANCE;
}


//*****************************************************************************
//
// Function : Determines which is the current gear
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void DetermineGear(void){
	
	U8 i = 0;

	if((Speed >= 2) && (GearType != NEUTRAL_GEAR)){		//Speed less than 2 km/h is impossible

		if(GearType == FORWARD_GEAR)	//Forward Gear (these tables to be used)

			for(i=0; i<(pgm_read_word(&GearIndex[Speed-1]) - pgm_read_word(&GearIndex[Speed-2])); i++){
			
				if((RPM >= pgm_read_word(&GearLimits[pgm_read_word(&GearIndex[Speed-2])+i][LEFTLIMIT])) && (RPM <=pgm_read_word(&GearLimits[pgm_read_word(&GearIndex[Speed-2])+i][RIGHTLIMIT]))){
					//Return determined gear
					Gear = (U8)(pgm_read_word(&GearLimits[pgm_read_word(&GearIndex[Speed-2])+i][GEAR]));
					break;
				}else Gear = 0;	//If no gear was found - return neutral gear 
			}
		else if(Speed <= 10){							//Reverse Gear (these tables to be used)
				for(i=0; i<(pgm_read_word(&GearIndexR[Speed-1]) - pgm_read_word(&GearIndexR[Speed-2])); i++){
				
					if((RPM >= pgm_read_word(&GearLimitsR[pgm_read_word(&GearIndexR[Speed-2])+i][LEFTLIMIT])) && (RPM <=pgm_read_word(&GearLimitsR[pgm_read_word(&GearIndexR[Speed-2])+i][RIGHTLIMIT]))){
						//Return determined gear
						Gear = (U8)(pgm_read_word(&GearLimitsR[pgm_read_word(&GearIndexR[Speed-2])+i][GEAR]));
						break;
					}else Gear = 0; //If no gear was found - return neutral gear 
				}
			}else	Gear = 0;

	}else 
		Gear = 0;	//If Speed is less than 2 km/h - neutral gear
}

//*****************************************************************************
//
// Function : Determines the current speed
//
// Arguments: Speed Sensor value
//
// Result   : none
//
//*****************************************************************************
void DetermineSpeed(U16 tick_count){

	if(tick_count > 22)
		tick_count = 22;
	
	Speed = (tick_count*5*100)/122;

	Distance += (tick_count*1000/4414);
	Trip += (tick_count*1000/4414);

}


//*****************************************************************************
//
// Function : Determines the current RPM
//
// Arguments: tick_count = RPM Sensor value
//
// Result   : none
//
//*****************************************************************************
void DetermineRPM(U16 tick_count){

	RPM = tick_count/2;

	if(RPM<500)
		RPM = 0;
	else if(RPM > 2500)
		RPM = 2500;

}


//*****************************************************************************
//
// Function : Writes Distance & Trip values to EEPROM
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void SaveTripDist	(void){

	eeprom_write_word((U16*) DIST_ADDRESS, Distance);

	eeprom_write_word((U16*) TRIP_ADDRESS, Trip);

}


//*****************************************************************************
//
// Function : Reads Distance & Trip values from EEPROM
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void ReadTripDist	(void){

	Distance = eeprom_read_word((U16*) DIST_ADDRESS);

	Trip = eeprom_read_word((U16*) TRIP_ADDRESS);

}


//*****************************************************************************
//
// Function : Clears Trip value
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void ClearTrip		(void){

	Trip = 0;

}
