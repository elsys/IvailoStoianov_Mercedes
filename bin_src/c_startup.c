
// entry point of the program, OS startup, main task implementation

#include <avrx-io.h>
#include <avrx-signal.h>
#include <avr/pgmspace.h>
#include  "avrx.h"

#include "hardware.h"
#include "gtypes.h"
#include "lcd_driver.h"
#include "touchscreen_driver.h"
#include "clock.h"
#include "flash_data.h"
#include "ADC.h"
#include "TWI.h"
#include "util/twi.h"
#include "gear.h"
#include "f_meter.h"

#include <string.h>
#include <stdio.h>


//*****************************************************************************
// local data
//*****************************************************************************
static volatile U16 RPM_counter;
static volatile U16 Gear_counter = 0;
static volatile U16 Consumption_c = 0;
static volatile U8	Speed_c = 0;


ISR(INT4_vect){

	FMeterIntTick();

}

AVRX_SIGINT(TWI_vect){

	IntProlog();                // Switch to kernel stack/context		
	
	switch(TW_STATUS){
		case TW_START: 
						TWISLA(SLAR);
						break;
		
		case TW_MR_SLA_ACK:
						cbi(TWCR, TWSTA);
						sbi(TWCR, TWINT);
						break;

		case TW_MR_DATA_NACK:
						TWIByte = TWDR;
						TWIStop();
						break;			
	};
	
    Epilog();                   // Return to tasks



}


//*****************************************************************************
//
// Function : Timer2 (Watch crystal) interrupt
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
AVRX_SIGINT(SIG_OVERFLOW2)
{
    IntProlog();                // Switch to kernel stack/context		

    outp(0, TCNT2);				// reload the timer 
    
    Epilog();                   // Return to tasks
}


//*****************************************************************************
//
// Function : Timer0 (AvrX timebase) interrupt
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
AVRX_SIGINT(SIG_OVERFLOW0)
{
//	cbi (PORTE, PORTE2);

    IntProlog();                // Switch to kernel stack/context		

    outp(TCNT0_INIT, TCNT0);	// reload the timer 

    LCDTick();					// Call LCD refresh function

	FMeterTick();

    AvrXTimerHandler();		    // Call Time queue manager
	
//	sbi (PORTE, PORTE2);

    Epilog();                   // Return to tasks
}



AVRX_TIMER(ClockTaskTimer);
AVRX_GCC_TASKDEF(ClockTask, 200, 5){

	U8 TimeBuf[10], item=0;
	
	ClockInit();

	while(1){
	
		if(tobeAdjusted == 0){
		
				ClockStart();
				
				AvrXDelay(&ClockTaskTimer,mConvertMsToTicks(962));// All functions in the cycle without AvrXDelay() take 38 ms

				ClockTick();

				LCDWriteStrP(0, 0, days[day-1], DISP_NORMAL);
			
				LCDWriteStrP(4, 0, months[month-1], DISP_NORMAL);

				sprintf((char *)TimeBuf,"%02u %02u %02u:%02u", date, year, hours, minutes);
				LCDWriteStr(8, 0, TimeBuf, DISP_NORMAL);
				
				if((TouchScreenValueX>=190)&&(TouchScreenValueX<=870)&&(TouchScreenValueY>=878)&&(TouchScreenValueY<=905)){

					tobeAdjusted = 1;

				}
		
		}else{
				
				TouchScreenMeasure();

				if(tobeAdjusted == 1){
					tobeAdjusted++;
						
					ClockStop();

					LCDClearScreen();
					
					LCDWriteIcon(2, 80, ARROWS_PAD);
					LCDWriteIcon(6, 120, BUTTON_OK);

					LCDWriteStrP(2, 3, adjust, DISP_NORMAL);
				}
			
				LCDWriteStrP(0, 5, days[day-1], DISP_NORMAL);
				
				LCDWriteStrP(4, 5, months[month-1], DISP_NORMAL);
				

				sprintf((char *)TimeBuf,"%02u %02u %02u:%02u", date, year, hours, minutes);
				LCDWriteStr(8, 5, TimeBuf, DISP_NORMAL);
				
				LCDWriteStrP(0, 6, spaces20 , DISP_NORMAL);

				switch(item){
					case 0:	LCDWriteStrP(0, 6, pointers3, DISP_NORMAL);
							break;
					case 1:	LCDWriteStrP(4, 6, pointers3, DISP_NORMAL);
							break;
					case 2: LCDWriteStrP(8, 6, pointers2, DISP_NORMAL);
							break;
					case 3: LCDWriteStrP(11, 6, pointers2, DISP_NORMAL);
							break;
					case 4: LCDWriteStrP(14, 6, pointers2, DISP_NORMAL);
							break;
					case 5: LCDWriteStrP(17, 6, pointers2, DISP_NORMAL);
							break;
				}


				
				//Up arrow
				if((TouchScreenValueX>=667)&&(TouchScreenValueX<=800)&&(TouchScreenValueY>=540)&&(TouchScreenValueY<=635)){

					switch(item){
						case 0:	if(day<7)		day++;
								break;
						case 1:	if(month<12)	month++;
								break;
						case 2:	if(date<EndOfMonth(month, year))	date++;
								break;
						case 3: year++;
								break;
						case 4: if(hours<23)	hours++;
								break;
						case 5: if(minutes<59)	minutes++;
								break;
					}
					
				}
				//Down arrow
				if((TouchScreenValueX>=535)&&(TouchScreenValueX<=667)&&(TouchScreenValueY>=540)&&(TouchScreenValueY<=635)){

					switch(item){
						case 0:	if(day>1)		day--;
								break;
						case 1:	if(month>1)		month--;
								break;
						case 2:	if(date>1)		date--;
								break;
						case 3: if(year>0)		year--;
								break;
						case 4: if(hours>0)		hours--;
								break;
						case 5: if(minutes>0)	minutes--;
								break;
					}

				}
				//Left arrow
				if((TouchScreenValueX>=403)&&(TouchScreenValueX<=535)&&(TouchScreenValueY>=540)&&(TouchScreenValueY<=635)){
					
					if(item>0)	item--;
				
				}
				//Right arrow
				if((TouchScreenValueX>=270)&&(TouchScreenValueX<=403)&&(TouchScreenValueY>=540)&&(TouchScreenValueY<=635)){
					
					if(item<5)	item++;

				}
				//OK
				if((TouchScreenValueX>=400)&&(TouchScreenValueX<=670)&&(TouchScreenValueY>=400)&&(TouchScreenValueY<=500)){
					
					seconds = 0;

					item = 0;

					ClockStart();

					tobeAdjusted = 0;
					
					LCDClearScreen();
					
				}

				AvrXDelay(&ClockTaskTimer,mConvertMsToTicks(100));

		}
//*/
	}
}


AVRX_TIMER(ADCTaskTimer);
AVRX_GCC_TASKDEF(ADCTask, 50, 4){

		while(1){
			
		
			if(tobeAdjusted == 0){
				
				TouchScreenMeasure();

				//Read touchscreen values for changing fuel mode
				if((TouchScreenValueX>=560)&&(TouchScreenValueX<=896)&&(TouchScreenValueY>=510)&&(TouchScreenValueY<=610)){
					
					FuelMode = !FuelMode;
					
				}		
				
				//Print labels
				LCDWriteStrP(9, 11, cons, DISP_NORMAL);
				LCDWriteStrP(14, 14, dpoint, DISP_NORMAL);
				LCDWriteStrP(18, 13, lph1, DISP_NORMAL);
				LCDWriteStrP(17, 14, lph2, DISP_NORMAL);
				LCDWriteStrP(0, 16, kmleft, DISP_NORMAL);

				//Print data
				LCDWriteStrP(0, 11, fuel, DISP_NORMAL);
				
				Consumption_c = FMeterReadLastMeasurement(FREQ3);
				Speed_c = FMeterReadLastMeasurement(FREQS);
				
				DetermineConsumption(Consumption_c, Speed_c);


				LCDWriteDigit(10, 98, SMALL_DIGIT, Consumption, 2);
				LCDWriteDigit(15, 98, SMALL_DIGIT, Consumption_Dec, 1);

				LCDWriteDigit(1, 138, SMALL_DIGIT, Range, 4);

				//Call functions
				InitADC();
				MeasureADC();

				if(FuelMode == LITER_MODE){
					LCDWriteStrP(8, 14, litre, DISP_NORMAL);
					if(FuelLevel <=147) {
						TWIStopped = 0;
						LCDWriteIcon(0, 16, CLR_TEMP);
						LCDWriteIcon(3, 17, FUEL_TANK);
					}else TWIStopped = 1;
				}else{
					LCDWriteStrP(8, 14, perc, DISP_NORMAL);
					if(FuelLevel <=14) {
						TWIStopped = 0;
						LCDWriteIcon(0, 16, CLR_TEMP);
						LCDWriteIcon(3, 17, FUEL_TANK);	
					}else TWIStopped = 1;
				}
				LCDWriteDigit(0, 98, SMALL_DIGIT, FuelLevel, 4);
			}

				
			AvrXDelay(&ADCTaskTimer, mConvertMsToTicks(100));
			
		}

}



AVRX_TIMER(GearTaskTimer);
AVRX_GCC_TASKDEF(GearTask, 50, 3){


	//Init Gear Modul
	InitGear();

	FMeterStartMeasurement();

	while(1){

	if(tobeAdjusted == 0){	// If Clock modul is not in adjustment mode

		//Print Labels
		LCDWriteStrP(17, 8, km, DISP_NORMAL);
		LCDWriteStrP(17, 9, kmh, DISP_NORMAL);
		LCDWriteStrP(0, 6, rpm, DISP_NORMAL);
		LCDWriteStrP(18, 19, km, DISP_NORMAL);

		
		RPM_counter = FMeterReadLastMeasurement(FREQ1);
		

		//Print Speed & RPM
			
			LCDWriteDigit(11, 46, BIG_DIGIT, (Speed/5)*5, 2);	

			DetermineRPM(RPM_counter);

			LCDWriteDigit(1, 58, SMALL_DIGIT, (RPM/50)*50, 4);	
		
		//Print Distance/Trip 
		if(DistanceTripMode == TRIP){

				LCDWriteStrP(9, 16, clr_distance, DISP_NORMAL);	
				LCDWriteStrP(9, 16, trip, DISP_NORMAL);
				LCDWriteDigit(10, 138, SMALL_DIGIT, (Trip/1000), 4);
			
		}else{

				LCDWriteStrP(9, 16, distance, DISP_NORMAL);
				LCDWriteDigit(10, 138, SMALL_DIGIT, (Distance/1000), 4);

		}
		

		
		//Indications for Gear Advisor Mode
		if(GearAdvisorMode == ECONOMIC_MODE){
				LCDWriteIcon(18, 12, ECONOMIC_MODE);
		}else{
				LCDWriteIcon(18, 29, SPORT_MODE);
			
		}
		
		//Indication for Reverse Gear
		if(GearType == REVERSE_GEAR){
				LCDWriteIcon(11, 12, R_GEAR);	
				LCDWriteIcon(13, 17, CLR_GEAR_ADV);
				LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
		}else{
				LCDWriteIcon(11, 12, CLR_SMALL_DIGIT);
		}

		//The Function determines which is the current gear
		DetermineGear();


		//Print Current Gear
		if(Gear == 0){
			LCDWriteIcon(8, 12, N_GEAR);
			LCDWriteIcon(11, 37, CLR_HALF_GEAR);
			LCDWriteIcon(11, 31, CLR_HALF_GEAR);
			//Clear
			LCDWriteIcon(13, 17, CLR_GEAR_ADV);
			//Clear
			LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
		}else//Gears are 16, but they are printed in mode: "1..8+-"
			if((Gear)%2 == 0){	//Fast Gear
				LCDWriteDigit(8, 12, BIG_DIGIT, Gear/2, 1);
				LCDWriteIcon(11, 31, FAST_GEAR);
				LCDWriteIcon(11, 37, CLR_HALF_GEAR);
			}else{				//Slow Gear
				LCDWriteDigit(8, 12, BIG_DIGIT, (Gear/2)+1, 1);
				LCDWriteIcon(11, 31, CLR_HALF_GEAR);
				LCDWriteIcon(11, 37, SLOW_GEAR);
			}

		//Reading button status for Forward and Reverse Gears
		if ((PINC & (1<<PC0)) == 0)	   GearType = FORWARD_GEAR;
		else if((PINC & (1<<PC1)) == 0) GearType = REVERSE_GEAR;
		else GearType = NEUTRAL_GEAR;
		
		if ((PINC & (1<<PC2)) == 0)	{
			
			ClearTrip();
		
			SaveTripDist();
		}else{
		
			//nothing
		}

		if ((PINC & (1<<PC3)) == 0)	   LCD_EN_HIGH();
		else LCD_EN_LOW();


		if(Gear_counter == 1200){ // 2 mins elapsed - write EEPROM
			SaveTripDist();
		
			Gear_counter = 0;
		}



		//Functionality of Gear Advisor
		if((Gear != 0) && (GearType == FORWARD_GEAR))
		if(GearAdvisorMode == ECONOMIC_MODE){
					
			if(RPM<1000){
				if(Gear > 1){
						LCDWriteIcon(13, 30, GEAR_DOWN);
						//Clear
						LCDWriteIcon(13, 17, CLR_GEAR_ADV);
				}else{
				
					//Clear
					LCDWriteIcon(13, 17, CLR_GEAR_ADV);
					//Clear
					LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
				
				}
			}else if(RPM >1500){
				if(Gear < 16){
						LCDWriteIcon(13, 17, GEAR_UP);
						//Clear
						LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
			
				}else{

					//Clear
					LCDWriteIcon(13, 17, CLR_GEAR_ADV);
					//Clear
					LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
									
				}	
			}else{
				
				//Clear
				LCDWriteIcon(13, 17, CLR_GEAR_ADV);
				//Clear
				LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
			
			}
		}else{
			if(RPM<1500){
				if(Gear > 1){
						LCDWriteIcon(13, 30, GEAR_DOWN);
						//Clear
						LCDWriteIcon(13, 17, CLR_GEAR_ADV);
				}else{

					//Clear
					LCDWriteIcon(13, 17, CLR_GEAR_ADV);
					//Clear
					LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
			
				}	
			}else if(RPM >2000){
				if(Gear < 16){
					LCDWriteIcon(13, 17, GEAR_UP);
					//Clear
					LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
				}else{
						
					//Clear
					LCDWriteIcon(13, 17, CLR_GEAR_ADV);
					//Clear
					LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
				
				}
			}else{
				
				//Clear
				LCDWriteIcon(13, 17, CLR_GEAR_ADV);
				//Clear
				LCDWriteIcon(13, 30,  CLR_GEAR_ADV);
			
			}
		}

		//Reading touchscreen values for changing Gear Advisor Mode
		if((TouchScreenValueX>=173)&&(TouchScreenValueX<=605)&&(TouchScreenValueY>=755)&&(TouchScreenValueY<=860)){
			
			if(GearAdvisorMode == ECONOMIC_MODE){
					
				GearAdvisorMode = SPORT_MODE;
				LCDWriteIcon(18, 12, CLR_SMALL_DIGIT);
				LCDWriteIcon(18, 29, SPORT_MODE);
			}else{
			
				GearAdvisorMode = ECONOMIC_MODE;
				LCDWriteIcon(18, 29, CLR_SMALL_DIGIT);
				LCDWriteIcon(18, 12, ECONOMIC_MODE);
			}

		}

		//Reading touchscreen values for changing Distance/Trip
		if((TouchScreenValueX>=190)&&(TouchScreenValueX<=550)&&(TouchScreenValueY>=370)&&(TouchScreenValueY<=470)){
			
			if(DistanceTripMode == DISTANCE){
			
				DistanceTripMode = TRIP;
			
			}else{
				
				DistanceTripMode = DISTANCE;

			}

		}		


	}
	
		Gear_counter++;
		AvrXDelay(&ADCTaskTimer, mConvertMsToTicks(100));
	
	}



}

//*****************************************************************************
//
// Function : MainTask (demo: AVRX_GCC_TASKDEF(TaskName, c_stack, priority);
//
// Arguments: Task name, Stack size needed for the task, relative priority
//
// Result   : none
//
//*****************************************************************************


AVRX_TIMER(MainTaskTimer);
AVRX_GCC_TASKDEF(MainTask, 200, 6)
{
	U8 buf[10];

	// do some initialization here
	LCDInitialize();

	LCDStart();

	TouchScreenInit();
	
	TouchScreenStart();

	TWIInit();

	TWIStart();

	FMeterInit();

    // start another task	
	AvrXRunTask(TCB(ClockTask));
	AvrXRunTask(TCB(ADCTask));
	AvrXRunTask(TCB(GearTask));

	
//*/

	// the endless loop of the task
	while (1)
	{

		LCDWriteIcon(0, 10, LINE);

		// do the action once each 3s
		AvrXDelay (&MainTaskTimer, mConvertMsToTicks(3000));

		if((TWIStopped == 1) && (tobeAdjusted == 0)){
					if(FuelMode == LITER_MODE){
						if(FuelLevel >147) LCDWriteIcon(0, 16, CLR_TEMP);
					}else if(FuelLevel >14) LCDWriteIcon(0, 16, CLR_TEMP);
					LCDWriteIcon(5, 25, CELSIUS);
					LCDWriteStrP(0, 2, temp, DISP_NORMAL);
					if((TWIByte & (1<<7)) != 0)	LCDWriteIcon(0, 25, MINUS);
					else LCDWriteIcon(0, 25, CLR_MINUS);
					LCDWriteDigit(1, 25, SMALL_DIGIT, TWIByte, 2);

		TWIStart();
		}

	}
}



//*****************************************************************************
//
// Function : main: C main function - initialization and startup of the system
//
// Arguments: none
//
// Result   : int, exit code
//
//*****************************************************************************
int main(void)                 
{

	
	
	sbi (DDRE, PORTE3);				// PORTE3: set to output
	sbi (PORTE, PORTE3);			// used to measure by oscilloscope
									// the CPU time spent in Timer0 interrupt
									// where is serviced the AvrX timer handler
									// and LCD refresh.
									// low = consumed time, high = free time
									// task switching time is not included!	
	
	outp((1<<SE), MCUCR); 			// Enable "Sleep" instruction for idle loop
		
    AvrXSetKernelStack(0);			// use default SP for AvrX stack

    outp((1<<AS2), ASSR);		   	// set Timer2 to use watch crystal
    outp(0, TCNT2);
    outp(5, TCCR2A);  				// prescaler for 32768/128 rate = 1 int/sec
	outp((1<<TOIE2), TIMSK2); 		// enable Timer2 interrupt

    outp(TCNT0_INIT, TCNT0);
    outp(2, TCCR0A);    			// Set up Timer0 for CLK/8 rate
    outp((1<<TOIE0), TIMSK0);     	// Enable Timer0 overflow interrupt

	cbi(DDRC, PC0);					//Button config
	cbi(DDRC, PC1);
	cbi(DDRC, PC2);
	cbi(DDRC, PC3);

	sbi(PORTC, PC0);				//...
	sbi(PORTC, PC1);
	sbi(PORTC, PC2);
	sbi(PORTC, PC3);

	EndCritical();					// Global interupts enable

    AvrXRunTask(TCB(MainTask));    	// start the program main task

    Epilog();                   	// Switch from AvrX Stack to first task
									// never returns
    
    return (1);						// added to remove warning
}
