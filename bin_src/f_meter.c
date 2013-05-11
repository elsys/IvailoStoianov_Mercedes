// implementation file for frequency meter using Timer1

#include <avrx-io.h>
#include <avrx-signal.h>

#include "avrx.h"

#include "hardware.h"
#include "gtypes.h"
#include "f_meter.h"

#include "gear.h"
#include "ADC.h"

// measurement time period in ticks (1 tick = 100us)
#define MEASUREMENT_TICKS	2000

#define TIMER_ARRAY_SIZE	4

//*****************************************************************************
// local data
//*****************************************************************************
static U8  FMeterInitialized = 0;
static U8  FMeterMeasurementRunning = 0;
static U8  FMeterMeasurementComplete = 1;

static U16 FMeterTickCounter;
static U8  SpeedCounter = 0;

static U16 FMeterTimerResult1;
static U16 FMeterTimerResult3;
static U16 FMeterTimerResultS;

static U16 FMeterTimerArray1[TIMER_ARRAY_SIZE]  = {0, 0, 0, 0};
static U16 FMeterTimerArray3[TIMER_ARRAY_SIZE]  = {0, 0, 0, 0};
static U8 FMeterSpeedCounter[TIMER_ARRAY_SIZE] = {0, 0, 0, 0};


//**********************************************************************************
//
// Function : Filter - filters measurement values according to previous measurements
//
// Arguments: ticks - current measurement
//			  timer - which measurement to be filtered - Timer1 or Timer3	
//
// Result   : none
//
//**********************************************************************************
U16 Filter		(U16 ticks, U8 timer){
	
	U16 i, sum = 0;

	if(timer == FREQ1){
		for(i = (TIMER_ARRAY_SIZE-1); i > 0; i--){
			FMeterTimerArray1[i] = FMeterTimerArray1[i-1]; 		
		}

		FMeterTimerArray1[0] = ticks;

		for(i = 0; i< TIMER_ARRAY_SIZE; i++){
			sum += FMeterTimerArray1[i];
		}
	}else if(timer == FREQ3){
		for(i = (TIMER_ARRAY_SIZE-1); i > 0; i--){
			FMeterTimerArray3[i] = FMeterTimerArray3[i-1]; 		
		}

		FMeterTimerArray3[0] = ticks;

		for(i = 0; i< TIMER_ARRAY_SIZE; i++){
			sum += FMeterTimerArray3[i];
		}
	}else{
	
		for(i = (TIMER_ARRAY_SIZE-1); i > 0; i--){
			FMeterSpeedCounter[i] = FMeterSpeedCounter[i-1]; 		
		}

		FMeterSpeedCounter[0] = ticks;

		for(i = 0; i< TIMER_ARRAY_SIZE; i++){
			sum += FMeterSpeedCounter[i];
		}

	}

	return(sum/TIMER_ARRAY_SIZE);
}


//*****************************************************************************
//
// Function : FMeter tick; called each time the OS tick timer is called
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void FMeterTick (void)
{

	if (FMeterInitialized != 0)
	{
		if (FMeterMeasurementRunning == 1)
		{

			if (FMeterTickCounter == 0)
			{
				// Timer1 & Timer3 start counting now
				TCCR1B = 7; // external pin is the source, rising edge
				TCCR3B = 7;
				EIMSK |= (1<<INT4);

			}
			else
			{
				// do nothing
			}
	
			FMeterTickCounter++;

			if (FMeterTickCounter == (MEASUREMENT_TICKS + 1))
			{
				// Timer1 & Timer3 stop counting now
				TCCR1B = 0; // Timer is stopped
				TCCR3B = 0;	
				EIMSK &= ~(1<<INT4);

				FMeterMeasurementRunning = 0;
				FMeterMeasurementComplete = 1;
				

				//Filter measurement values
				
				FMeterTimerResult1 = Filter(TCNT1, FREQ1);
				FMeterTimerResult3 = Filter(TCNT3, FREQ3);
				FMeterTimerResultS = Filter(SpeedCounter, FREQS);

				DetermineSpeed(FMeterTimerResultS);

				FMeterStartMeasurement();
			}
			
		}
	}
}

//*****************************************************************************
//
// Function : FMeterIntTick; called each time the External Interrupt 4 occurs
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void FMeterIntTick(void){

	SpeedCounter++;

}

//*****************************************************************************
//
// Function : FMeterStopMeasurement: stop measurement in progress, return to 
//			  defined initial state (waiting for start of measurement)
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void FMeterStopMeasurement(void)
{

	//Configuring Timer1 for frequnecy measurement
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	//Configuring Timer3 for frequnecy measurement
	TCCR3A = 0;
	TCCR3B = 0;
	TCNT3 = 0;

	FMeterMeasurementRunning = 0;
	FMeterMeasurementComplete = 1;
}


//*****************************************************************************
//
// Function : FMeterInit: Initialize the driver and put it into
//			  defined initial state (waiting for start of measurement)
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void FMeterInit(void)
{
	FMeterStopMeasurement();

	//Congiguring Timer1 & Timer3 & External Interrupt 4 pins as inputs
	cbi(DDRD, DDD6);
	cbi(DDRE, DDE6);
	cbi(DDRE, DDE4);

	//Configuring External Interrupt 4
	EICRB |= (1<<ISC41);
	EICRB |= (1<<ISC40);


	FMeterInitialized = 1;
	FMeterMeasurementComplete = 1;
}


//*****************************************************************************
//
// Function : FMeterStartMeasurement: Run a measurement
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void FMeterStartMeasurement(void)
{
	FMeterMeasurementComplete = 0;
	FMeterMeasurementRunning = 1;
	FMeterTickCounter = 0;
	
	//Timer1 config
	TCCR1A = 0;	// normal mode (counter)
	TCNT1 = 0;	// counter = 0

	//Timer3 config
	TCCR3A = 0;	// normal mode (counter)
	TCNT3 = 0;	// counter = 0

	//External Interrupt 4 config
	SpeedCounter = 0;

}

//*****************************************************************************
//
// Function : FMeterGetMeasurementStatus: check if measurement is complete
//
// Arguments: none
//
// Result   : 1 = complete, 0 = is still running
//
//*****************************************************************************
U8 FMeterGetMeasurementStatus(void)
{
	return (FMeterMeasurementComplete);
}


//*****************************************************************************
//
// Function : FMeterReadLastMeasurement: read the result of the last measurement
//
// Arguments: freq  - which timer result to be returned
//			  FREQ1 - result from Timer1 will be returned
//			  FREQ3 - result from Timer3 will be returned	
//
// Result   : U16: measured frequency in Hz
//
//*****************************************************************************
U16 FMeterReadLastMeasurement(U8 freq)
{

	if(freq == FREQ1)
		return (FMeterTimerResult1*5);
	else if(freq == FREQ3)
		return (FMeterTimerResult3*5);
	else if(freq == FREQS)
		return (FMeterTimerResultS*5);
}



