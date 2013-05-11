
//Implementation file for TouchScreen driver

#include <avrx-io.h>
#include  "avrx.h"
#include "gtypes.h"
#include "touchscreen_driver.h"



//*****************************************************************************
// Public variables
//*****************************************************************************
U16 TouchScreenValueY;
U16 TouchScreenValueX;
static U8 TouchScreenInitialized;


//*****************************************************************************
//
// Function : Initialize ADC for TouchScreen measurement
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TouchScreenInit(void){
  
	//Enabling AREF as reference voltage
	cbi(ADMUX,REFS1);
	cbi(ADMUX,REFS0);

	//Setting Right adjustment for the result of ADC conversion
	cbi(ADMUX,ADLAR);//cleaer left adjust

	//Clearing the channel selection bits
	ADMUX  &= ~(0x1F<<MUX0);
  
	ADCSRA &= ~(0x07<<ADPS0);		 //clear prescaler
 	ADCSRA |= (ADC_PRESC<<ADPS0); //select prescaler

	cbi(ADCSRA,ADATE);

	TouchScreenInitialized=1;
}


//*****************************************************************************
//
// Function : Initialize ADC for TouchScreen measurement of X or Y axis
//
// Arguments: axis - axis to be initialized for measurement
//			  axis = 'X' - ADC will be initialized for measuring TouchScreenValueX;
//			  axis = 'Y' - ADC will be initialized for measuring TouchScreenValueY ;
//
// Result   : none
//
//*****************************************************************************
void Init(U8 axis){

	if(axis=='X'){

		//axis = 'X'

		//Setting PORTF & PORTC5 for measuring the X axis
		mCat2(PORT,TSCR_PORT) &= (0xF8);
		PORTC &= ~(1<<PC5);
		mCat2(PORT,TSCR_PORT) |= (0x05);
		PORTC |= (1<<PC5);

		//Setting DDRF & DDRC5 for measuring the X axis
		mCat2(DDR,TSCR_PORT) &= (0xF8);
		DDRC &= ~(1<<PC5);
		mCat2(DDR,TSCR_PORT) |= (0x03);

		//Clearing the channel selection bits
		ADMUX  &= ~(0x1F<<MUX0);

		//Setting ADC Channel 2
		sbi(ADMUX,MUX1);

		//Enabling the Measure() function
		TouchScreenInitialized=1;

	}else{
	
		//axis = 'Y'
		
		//Setting PORTF & PORTC5 for measuring the Y axis
		mCat2(PORT,TSCR_PORT) &= (0xF8);
		mCat2(PORT,TSCR_PORT) |= (0x07);
		PORTC &= ~(1<<PC5);

		//Setting DDRF & DDRC5 for measuring the Y axis
		mCat2(DDR,TSCR_PORT) &= (0xF8);
		mCat2(DDR,TSCR_PORT) |= (0x04);//0100
		DDRC |= (1<<PC5);

		//Clearing the channel selection bits
		ADMUX  &= ~(0x1F<<MUX0); 

		//Setting ADC Channel 0
		cbi(ADMUX, MUX0);

		//Enabling the Measure() function
		TouchScreenInitialized=1;
	
	
	}
}


//*****************************************************************************
//
// Function : Enable the ADC measurement
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TouchScreenStart(void){

	//enable ADC measurement
	sbi(ADCSRA, ADEN);

}


//*****************************************************************************
//
// Function : Disable the ADC measurement
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TouchScreenStop(void){
	
	//Disabling the Measure() function
	TouchScreenInitialized=0;
	cbi(ADCSRA, ADEN);

}


//*****************************************************************************
//
// Function : Update ADC values -> TouchScreenValueX and TouchScreenValueY
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TouchScreenMeasure(void){

	if(TouchScreenInitialized){//Ready for work
	
		TouchScreenStop();
		Init('X');
		TouchScreenStart();
		Measure('X');

		TouchScreenStop();
		Init('Y');
		TouchScreenStart();
		Measure('Y');
	
	}
}


//*****************************************************************************
//
// Function : Measure ADC values - > TouchScreenValueX or TouchScreenValueY
//
// Arguments: axis - axis to be measured;
//			  axis = 'X' - TouchScreenValueX will be measured;
//			  axis = 'Y' - TouchScreenValueY will be measured;
//
// Result   : none
//
//*****************************************************************************
void Measure(U8 axis){

	U8 i;
	U16 sum=0;

	sum=0;

	//Real measurement
	for(i=0;i<PRECISION;i++){
		sbi(ADCSRA, ADSC);
		cbi(ADCSRA, ADATE);
		
		while(!(ADCSRA & (1<<ADIF)));
		sbi(ADCSRA, ADIF);
		sum+=ADC;
	}


	if(axis=='X'){//measuring TouchScreenValueX
	
		TouchScreenValueX=sum/PRECISION;

	}else{		  //measuring TouchScreenValueY
	
		TouchScreenValueY=sum/PRECISION;
	
	}


}
