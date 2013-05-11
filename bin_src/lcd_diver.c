
// implementation file for lcd driver

#include <avrx-io.h>
#include <avrx-signal.h>
#include <string.h>
#include <avr/pgmspace.h>

#include  "avrx.h"

#include "hardware.h"
#include "gtypes.h"
#include "lcd_driver.h"
#include "font_8x8.h"
#include "arrays.h"

//*****************************************************************************
// exported data
//*****************************************************************************
U8 LCDFrameBuffer[LCD_BUFFER_SIZE];


//*****************************************************************************
// local data
//*****************************************************************************
AVRX_TIMER(LCDInitTimer);
static U8  LCDInitialized 	= 0;
static U16 RowCounter 		= 0;


//*****************************************************************************
//
// Function : LCD Timer tick; called each time the OS tick timer is called
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void LCDTick (void)
{
	
	U8  j;
	U8  b;

	if (LCDInitialized != 0)
	{

		for(j=0; j<LCD_WIDTH_BYTES; j++) 
		{
			// get current byte
			b = LCDFrameBuffer[RowCounter+j];

			// set CP strobe to high
			LCD_CP_HIGH();

			// send high-order nibble HHHHxxxx on the data bus
	    	mCat2(PORT,LCD_PORT) = (mCat2(PORT,LCD_PORT) & 0xF0) | (b >> 4);

			// set CP strobe to low (latch data into LCD row buffer)
		   	LCD_CP_LOW();

			// set CP strobe to high
		  	LCD_CP_HIGH();

			// send low-order nibble xxxxLLLL on the data bus
		  	mCat2(PORT,LCD_PORT) = (mCat2(PORT,LCD_PORT) & 0xF0) | (b & 0x0F);

			// set CP strobe to low (latch data into LCD row buffer)
		  	LCD_CP_LOW();
		 }

         // Latch the sent row into LCD
	     LCD_LP_HIGH();

		 // when running on 16 MHz, pulse is too short without some delay
		 asm volatile ("nop\n");

	     LCD_LP_LOW();

		 // remove the new frame signal if it was set
	     LCD_FLM_LOW();


		 RowCounter += LCD_WIDTH_BYTES;
		 if (RowCounter >= LCD_BUFFER_SIZE)
		 {
		 	RowCounter = 0;
			// set FLM (new frame) signal to active
	       	LCD_FLM_HIGH();
		 }
	 }
}


//*****************************************************************************
//
// Function : LCD Initialization function
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void LCDInitialize (void)
{
	// say LCD is not initialized during initialization sequence
	LCDInitialized = 0;

	// Initialize the peripherals:
	// CP signal inactive
	LCD_CP_HIGH();
	// CP pin: make it output -- sbi (DDRB,PORTB7); or DDRB = DDRB | 1<<DDB7;
	sbi (mCat2(DDR,LCD_PORT_CP),mCat3(PORT,LCD_PORT_CP,LCD_PIN_CP));

	// Data port: make all 8 pins outputs
	mCat2(DDR,LCD_PORT) = 0xFF;
	// Data port + some control signals (macros not used for single instruction)
	mCat2(PORT,LCD_PORT) = 0;

	// EN signal
	LCD_EN_LOW();
	// EN pin: make it output
	sbi (mCat2(DDR,LCD_PORT_EN),mCat3(PORT,LCD_PORT_EN,LCD_PIN_EN));
}


//*****************************************************************************
//
// Function : LCD Start
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void LCDStart (void)
{
	// Clear the frame buffer
	memset (LCDFrameBuffer, 0, sizeof(LCDFrameBuffer));

	// wait the delay: Olimex source code
	AvrXDelay(&LCDInitTimer, mConvertMsToTicks(LCD_INIT_DELAY));

	// Set VHH pin to high
	LCD_VHH_HIGH();

	// wait the delay: Olimex source code
	AvrXDelay(&LCDInitTimer, mConvertMsToTicks(LCD_INIT_DELAY));

	// Set DISP pin to high
	LCD_DISP_HIGH();

	// wait the delay: Olimex source code
	AvrXDelay(&LCDInitTimer, mConvertMsToTicks(LCD_INIT_DELAY));

	// Initialize refresh function
	RowCounter = 0;

	// enable the refresh function: LCD is now usable
	LCDInitialized = 1;
}


//*****************************************************************************
//
// Function : LCD Stop
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void LCDStop (void)
{
	// say LCD is not initialized (stops the refresh)
	LCDInitialized = 0;

	// wait the delay: Olimex source code
	AvrXDelay(&LCDInitTimer, mConvertMsToTicks(LCD_INIT_DELAY));

	// Set DISP pin to low
	LCD_DISP_LOW();

	// wait the delay: Olimex source code
	AvrXDelay(&LCDInitTimer, mConvertMsToTicks(LCD_INIT_DELAY));

	// Set VHH pin to low
	LCD_VHH_LOW();

	// wait the delay: Olimex source code
	AvrXDelay(&LCDInitTimer, mConvertMsToTicks(LCD_INIT_DELAY));
}

//*****************************************************************************
//
// Function : LCDWriteChar - write 8x8 font character on the specified place
//
// Arguments: x - horizontal position, y - vertical position, ch - ascii code
//			  inv = 0 -> normal display; inv != 0 -> inverted display
//
// Result   : none
//
//*****************************************************************************
void LCDWriteChar (U8 x, U8 y, U8 ch, U8 inv)
{
	U8 	c;
	U8  ch_line;
	U16 fb_index;

	// validity check of incoming data
	if (
		(x <= LINE_NUM_CHARS)	&&
		(y <= NUM_ROWS) 		&&
		(ch >= FONT_START_CHAR)	&&
		(ch < (FONT_NUM_CHARS + FONT_START_CHAR))
	   )
	{
		// adjust vertical position from char lines to pixels
		y = y * FONT_CHAR_HEIGHT;

		// adjust character for displaying
		ch = ch - FONT_START_CHAR;

		// display the selected character
		for (c = 0; c < FONT_CHAR_HEIGHT; c++)
		{
			// get next line from the character image
			ch_line  = pgm_read_byte(&Font8x8[ch][c]);

			// calculate the position in frame buffer
			fb_index = ((y+c)*LCD_WIDTH_BYTES) + x;

			// display it 		
			if (inv == DISP_INVERTED)
			{
				// inverted
				LCDFrameBuffer[fb_index] = ~ch_line;
			}
			else
			{
				// normal
				LCDFrameBuffer[fb_index] = ch_line;
			}
		}
	}
}

//*****************************************************************************
//
// Function : LCDWriteStr - string stored in RAM on the specified screen place
//
// Arguments: x - horizontal position, y - vertical position, str - asciiz str
//			  inv = 0 -> normal display; inv != 0 -> inverted display
//
// Result   : none
//
// Action   : the function does not check for parameters correctness;
//			  nothing is displayed, if coordinates are out of range
// 			  the functions translates control codes:
//			  C_CR:  carriage return: move to the start of the current line
//			  C_LF:  line feed:       move to the next line
//			  C_CLS: clear screen:	  clear whole screen contents
//
//*****************************************************************************
void LCDWriteStr (U8 x, U8 y, U8 * str, U8 inv)
{
	while (*str)
	{
		// process control codes
		if (*str < FONT_START_CHAR)
		{
			switch(*str)
			{
				case C_CR:
						// carriage return
						x = 0;
					break;
				case C_LF:
						// line feed
						y++;
					break;
				case C_CLS:
						// clear screen
						memset (LCDFrameBuffer, 0, sizeof(LCDFrameBuffer));
					break;
				default:
					break;
			}
			// next character: the current control is processed
			str++;
		}
		else
		{
			// and adjust horizontal position and next character
			// display the current character
			LCDWriteChar(x++, y, *str++, inv);
		}
	}
}

//*****************************************************************************
//
// Function : LCDWriteCharP - write 8x8 font character on the specified place
//							 (character is saved in FLAS)
//
// Arguments: x - horizontal position, y - vertical position, ch - ascii code
//			  inv = 0 -> normal display; inv != 0 -> inverted display
//
// Result   : none
//
//*****************************************************************************
void LCDWriteCharP (U8 x, U8 y,  FLASH U8 * ch_ptrP, U8 inv)
{
	LCDWriteChar(x,y,pgm_read_byte(ch_ptrP),inv);
}

//*****************************************************************************
//
// Function : LCDWriteStrP - string stored in FLASH on the specified screen place
//
// Arguments: x - horizontal position, y - vertical position, strP - pointer
//			  to asciiz str
//			  inv = 0 -> normal display; inv != 0 -> inverted display
//
// Result   : none
//
// Action   : the function does not check for parameters correctness;
//			  nothing is displayed, if coordinates are out of range
// 			  the functions translates control codes:
//			  C_CR:  carriage return: move to the start of the current line
//			  C_LF:  line feed:       move to the next line
//			  C_CLS: clear screen:	  clear whole screen contents
//
//*****************************************************************************
void LCDWriteStrP (U8 x, U8 y, FLASH U8 * strP, U8 inv)
{
	U8 ch = 1;

	

	while (1)
	{
		// read next char
		ch = pgm_read_byte(strP);

		// exit the loop at the end of the string
		if (ch == 0)
		{
			break;
		}

		// process control codes
		if (ch < FONT_START_CHAR)
		{
			switch(ch)
			{
				case C_CR:
						// carriage return
						x = 0;
					break;
				case C_LF:
						// line feed
						y++;
					break;
				case C_CLS:
						// clear screen
						memset (LCDFrameBuffer, 0, sizeof(LCDFrameBuffer));
					break;
				default:
					break;
			}
			// next character: the current control is processed
			strP++;
		}
		else
		{
			// and adjust horizontal position and next character
			// display the current character
			LCDWriteChar(x++, y, ch, inv);

			// adjust the pointer for the next character
			strP++;
		}
	}
}

//*****************************************************************************
//
// Function : Clear whole screen
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void LCDClearScreen	(void){

	// clear screen
	memset (LCDFrameBuffer, 0, sizeof(LCDFrameBuffer));

}

//*****************************************************************************
//
// Function : Copy screen from FLASH memory to LCD
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
void LCDCopyScreen	(void){

	int i;

	for(i=0;i<3200;i++)
		LCDFrameBuffer[i] = pgm_read_byte(&screen[i]);

}


//*****************************************************************************
//
// Function : Write one digit saved in FLASH memory on specified screen place
//
// Arguments: x				-> position by X axis
//			  y				-> position by Y axis
//			  digit_type	-> Big Digit or Small Digit
//			  digit			-> Digit to be printed(0..9)
//
// Result   : none
//
//*****************************************************************************
void LCDWriteDigit_	(U8 x, U8 y, U8 digit_type, U8 digit){

	U16 i, pos;

	//determine start position
	pos = y*LINE_NUM_CHARS + x;

			//loop for all digit bytes
			for(i=0;i<pgm_read_byte(&SizeWidthValues[digit_type][SIZE]);i++){
		
				//print digit
				if(digit_type == BIG_DIGIT)
					LCDFrameBuffer[pos] = pgm_read_byte(&BigDigit[digit][i]);
				else
					LCDFrameBuffer[pos] = pgm_read_byte(&SmallDigit[digit][i]);
				
				//new row
				if(((i+1)%pgm_read_byte(&SizeWidthValues[digit_type][WIDTH]))==0)
					pos=pos+LINE_NUM_CHARS-pgm_read_byte(&SizeWidthValues[digit_type][WIDTH])+1;
				else
					pos++;

			}

}


//*****************************************************************************
//
// Function : Write many digits saved in flash memory on specified screen place
//
// Arguments: x				-> position by X axis
//			  y				-> position by Y axis
//			  digit_type	-> Big Digit or Small Digit
//			  digit			-> Digit to be printed(0..65535)
//			  pos_count		-> Positions count for the digit
//
// Result   : none
//
//*****************************************************************************
void LCDWriteDigit	(U8 x, U8 y, U8 digit_type, U16 digit, U8 pos_count){

	//get init position
	U8 init_pos_count = pos_count;

	do{ 
		if((digit == 0) && (pos_count!=init_pos_count)) //clear unused positions
			LCDWriteIcon(x+(pos_count-1)*pgm_read_byte(&SizeWidthValues[digit_type][WIDTH]), y, digit_type);
		else{	//write the digit
			LCDWriteDigit_(x+(pos_count-1)*pgm_read_byte(&SizeWidthValues[digit_type][WIDTH]), y, digit_type, digit%10);
			digit = digit/10;
		}

		pos_count--;

	}while(pos_count!=0);

}



//*****************************************************************************
//
// Function : Write icon saved in flash memory on specified screen place
//
// Arguments: x				-> position by X axis
//			  y				-> position by Y axis
//			  icon			-> icon to be printed
//							   (Definitions available in lcd_driver.h)
//
// Result   : none
//
//*****************************************************************************
void LCDWriteIcon	(U8 x, U8 y, U8 icon){

	U16 i, pos;

	//calculating the initial position
	pos = y*LINE_NUM_CHARS + x;
		
		//Loop for each byte in the icon
		for(i=0;i<pgm_read_word(&SizeWidthValues[icon][SIZE]);i++){
	
			//Determining which array in FLASH to be used;
			//Coping each byte into appropriate place in FrameBuffer array
			switch(icon){
				case CLR_BIG_DIGIT:	LCDFrameBuffer[pos] = pgm_read_byte(&array14[i]);
									break;
				case CLR_SMALL_DIGIT:LCDFrameBuffer[pos] = pgm_read_byte(&array15[i]);
									break;
				case ECONOMIC_MODE: LCDFrameBuffer[pos] = pgm_read_byte(&array2[i]);
									break;
				case SPORT_MODE:	LCDFrameBuffer[pos] = pgm_read_byte(&array3[i]);
									break;
				case FAST_GEAR:		LCDFrameBuffer[pos] = pgm_read_byte(&array4[i]);
									break;
				case SLOW_GEAR:		LCDFrameBuffer[pos] = pgm_read_byte(&array5[i]);
									break;
				case GEAR_UP:		LCDFrameBuffer[pos] = pgm_read_byte(&array6[i]);
									break;
				case GEAR_DOWN:		LCDFrameBuffer[pos] = pgm_read_byte(&array7[i]);
									break;
				case N_GEAR:		LCDFrameBuffer[pos] = pgm_read_byte(&array8[i]);
									break;
				case R_GEAR:		LCDFrameBuffer[pos] = pgm_read_byte(&array9[i]);
									break;
				case FUEL_TANK:		LCDFrameBuffer[pos] = pgm_read_byte(&array10[i]);
									break;
				case LINE:			LCDFrameBuffer[pos] = pgm_read_byte(&array11[i]);
									break;
				case BUTTON_OK:		LCDFrameBuffer[pos] = pgm_read_byte(&array12[i]);
									break;
				case ARROWS_PAD:	LCDFrameBuffer[pos] = pgm_read_byte(&array13[i]);
									break;
				case MINUS:			LCDFrameBuffer[pos] = pgm_read_byte(&array16[i]);
									break;
				case CELSIUS:		LCDFrameBuffer[pos] = pgm_read_byte(&array17[i]);
									break;
				case CLR_TEMP:		LCDFrameBuffer[pos] = pgm_read_byte(&array18[i]);
									break;
				case CLR_MINUS:		LCDFrameBuffer[pos] = pgm_read_byte(&array19[i]);
									break;
				case CLR_GEAR_ADV:	LCDFrameBuffer[pos] = pgm_read_byte(&array20[i]);
									break;
				case CLR_HALF_GEAR:	LCDFrameBuffer[pos] = pgm_read_byte(&array21[i]);
									break;
			}

			//if WIDTH of the icon is reached -> go to next line in FrameBuffer array
			if(((i+1)%pgm_read_byte(&SizeWidthValues[icon][WIDTH]))==0)
				
				//new line
				pos=pos+LINE_NUM_CHARS-pgm_read_byte(&SizeWidthValues[icon][WIDTH])+1;
			else
				
				//continue on the same row - WIDTH now reached
				pos++;
		}
}

