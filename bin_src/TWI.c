
//Implementation file for TWI driver

#include <avrx-io.h>
#include  "avrx.h"
#include "gtypes.h"
#include <util/twi.h>
#include "TWI.h"

//*****************************************************************************
// Public variables
//*****************************************************************************
U8 TWIStopped;
U8 TWIByte;


//*****************************************************************************
//
// Function : Initialize TWI for transmitting
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TWIInit(void){

	//Prescaler is 1 - by default

	//Setting Bit Rate - 18
	TWBR = 18;

	//Enabling TWI
	sbi(TWCR, TWEN);
	
	//Enbaling TWI interrupt
	sbi(TWCR, TWIE);
	
}

//*****************************************************************************
//
// Function : Starts TWI for transmitting
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TWIStart(void){

	TWIStopped = 0;

	//Send START condition
	sbi(TWCR, TWSTA);
	sbi(TWCR, TWINT);
}

//*****************************************************************************
//
// Function : Stops TWI for transmitting
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void TWIStop(void){
	
	TWIStopped = 1;
	
	//Send STOP condition
	sbi(TWCR, TWSTO);
	cbi(TWCR, TWSTA);
	sbi(TWCR, TWINT);
}

//*****************************************************************************
//
// Function : Sends SLA + R/W to a slave
//
// Arguments: sla_byte -> address of slave + R/W
//
// Result   : none
//
//*****************************************************************************
void TWISLA(U8 sla_byte){

	//Write Address + R/W bit into TWI DAta Register
	TWDR = sla_byte;

	//Send the byte through TWI bus
	sbi(TWCR, TWINT);
}

