

//Implementation file for Real-Time Clock

#include "gtypes.h"
#include "clock.h"

//*****************************************************************************
// local variable
//*****************************************************************************
static U8 ClockStarted = 0;

//*****************************************************************************
// externed variables from clock.h
//*****************************************************************************
U8 hours;
U8 minutes;
U8 seconds;
U8 date;
U8 month;
U8 year;
U8 day;

U8 tobeAdjusted;


//*****************************************************************************
//
// Function : Real-Time Clock initialization function
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void ClockInit		(void){


	//init clock and calendar values
	hours = 0;
	minutes = 0;
	seconds = 0;

	date = 1;
	month = 1;
	year = 0;

	day = 1;

	//init service variables
	ClockStarted = 0;

	tobeAdjusted = 0;

}


//*****************************************************************************
//
// Function : Real-Time Clock start function
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void ClockStart		(void){
	
	//Start Clock
	ClockStarted = 1;

}


//*****************************************************************************
//
// Function : Real-Time Clock stop function
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void ClockStop		(void){

	//Stop Clock
	ClockStarted = 0;

}


//*****************************************************************************
//
// Function : Actual functionality of the Real-Time Clock
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void ClockTick		(void){
	
	if(ClockStarted != 0){
		 if(seconds==59){
			
			 seconds = 0;
			
			 if(minutes==59){
			
				 minutes = 0;

				 if(hours == 23){

					 hours=0;
					
					if(date == EndOfMonth(month, year)){
						
						date = 1;
						
						if(month == 12){

							month = 1;

							year++;

						}else

							month++;

					}else
						
						date++;
					
					if(day == 7)
						
						day = 1;
				
					else

						day++;

				}else
				
					hours++;

			 }else{

				 minutes++;

			 }

		 }else{

			 seconds++; 
		}
	}
}


//*****************************************************************************
//
// Function : Returns days count in different months
//
// Arguments: monthp -> days count to be returned
//			  yearp  -> necessary for determining days count of Month 2
//
// Result   : End of the month
//
//*****************************************************************************
U8 EndOfMonth		(U8 monthp, U8 yearp){

	switch(monthp){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12: return 31;//months with 31 days
				break;
		case 4:
		case 6:
		case 9:
		case 11: return 30;//months with 30 days
				break;
		case 2: if ((yearp & 0x03) == 0) //check for leap years
					return 29;//Leap year
				else
					return 28;//Not leap year
				break;
	}


}