
//Implemention file for ADC measuring

#include <avrx-io.h>
#include  "avrx.h"
#include "gtypes.h"
#include"ADC.h"


//*****************************************************************************
// Public variables
//*****************************************************************************
U16 FuelLevel;
U8  FuelMode;
U16  Consumption;
U8  Consumption_Dec;
U16 Range;


//*****************************************************************************
//
// Function : Initialize ADC for Fuel measurement
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void InitADC(){

		//Enabling AREF as reference voltage
		cbi(ADMUX,REFS1);
		cbi(ADMUX,REFS0);

		//Clearing the channel selection bits
		ADMUX  &= ~(0x1F<<MUX0);
		
		//Selecting Channel
		ADMUX |= 0x03;


		//Clear Prescaler
		ADCSRA &= ~(0x07);
		
		//Select prescaler
		ADCSRA |= 0x07;

		//Init public variables
		FuelLevel = 0;
		//Consumption = 0;
		//Consumption_Dec = 0;
		Range = 0;


}

//*****************************************************************************
//
// Function : Update ADC values -> FuelLevel
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void MeasureADC(){

	U8 i;
	U16 sum=0;

	//starting
	sbi(ADCSRA, ADEN);
	sbi(ADCSRA, ADSC);

	//Real measurement - ADC_PRECISION times
	for(i=0;i<ADC_PRECISION;i++){
		sbi(ADCSRA, ADSC);
		cbi(ADCSRA, ADATE);
	
		while(!(ADCSRA & (1<<ADIF)));
		sbi(ADCSRA, ADIF);
		sum+=ADC;
		}

	//Finding average value of the measurement anad assign it to FuelLevel
	if((((sum/ADC_PRECISION/10)*135)/10) >1050)
		FuelLevel = 1050;
	else
		FuelLevel = ((sum/ADC_PRECISION/10)*135)/10;

	//convert FuelLevel to percent mode
	if(FuelMode == PERC_MODE)
		FuelLevel = (FuelLevel*10/(TANK_CAP/10));

}


//*****************************************************************************
//
// Function : Update ADC values -> Consumption, Consumption_Dec, Range
//
// Arguments: tick_count - Consumption value
//			  dist       - Distance value
//
// Result   : none
//
//*****************************************************************************
void DetermineConsumption(U16 tick_count, U8 dist){

	//Define some special cases
	if(dist > 110) dist = 110;

	if(tick_count<60) tick_count = 0;

	if(dist<7) dist = 0;

	if(dist == 0)
		Consumption = 0;
	else 
		Consumption = ((tick_count*10)/dist);

	if(Consumption > 990)
		Consumption = 999;

	if(Consumption == 0)
		Range = 0;
	else
		Range = ((FuelLevel*10)/Consumption)*100;

	if(Range > 9999)
		Range = 9999;

	//XX.
	Consumption = Consumption/10;

	//.X
	Consumption_Dec = Consumption%10;
}