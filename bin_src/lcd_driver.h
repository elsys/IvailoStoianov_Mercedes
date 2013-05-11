
// header file for lcd driver

#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__

//*****************************************************************************
// LCD dimensions
//*****************************************************************************
#define LCD_HEIGHT		160
#define LCD_WIDTH		160

#define LCD_WIDTH_BYTES 20 		// = LCD_WIDTH / 8
#define LCD_BUFFER_SIZE 3200	// = LCD_WIDTH_BYTES*LCD_HEIGHT


//*****************************************************************************
// LCD attachment to microcontroller
//*****************************************************************************

// LCD control: High voltage booster
#define LCD_PORT_EN		C
#define	LCD_PIN_EN		6

// LCD control: DISP pin
#define LCD_PORT_DISP	A
#define	LCD_PIN_DISP	6	

// LCD control: VHH pin
#define LCD_PORT_VHH	A
#define LCD_PIN_VHH		7

// LCD port nibble
#define LCD_PORT		A


// LCD control: CP pin
#define LCD_PORT_CP		B
#define LCD_PIN_CP		7

// LCD control: LP pin
#define LCD_PORT_LP		A
#define LCD_PIN_LP		5

// LCD control: FLM pin
#define LCD_PORT_FLM	A
#define LCD_PIN_FLM		4

//*****************************************************************************
// initialization delay (ms)
//***************************** ************************************************
#define LCD_INIT_DELAY	1



//*****************************************************************************
// Macros for LCD control
//*****************************************************************************


//*****************************************************************************
//
// Macro    : LCD_EN_HIGH(); Drives the EN signal into logic 1
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_EN_HIGH()		\
	sbi(mCat2(PORT,LCD_PORT_EN),mCat3(PORT,LCD_PORT_EN,LCD_PIN_EN))


//*****************************************************************************
//
// Macro    : LCD_EN_LOW(); Drives the EN signal into logic 0
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_EN_LOW()		\
	cbi(mCat2(PORT,LCD_PORT_EN),mCat3(PORT,LCD_PORT_EN,LCD_PIN_EN))


//*****************************************************************************
//
// Macro    : LCD_DISP_HIGH(); Drives the DISP signal into logic 1
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_DISP_HIGH()		\
	sbi(mCat2(PORT,LCD_PORT_DISP),mCat3(PORT,LCD_PORT_DISP,LCD_PIN_DISP))


//*****************************************************************************
//
// Macro    : LCD_DISP_LOW(); Drives the DISP signal into logic 0
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_DISP_LOW()		\
	cbi(mCat2(PORT,LCD_PORT_DISP),mCat3(PORT,LCD_PORT_DISP,LCD_PIN_DISP))


//*****************************************************************************
//
// Macro    : LCD_VHH_HIGH(); Drives the VHH signal into logic 1
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_VHH_HIGH()		\
	sbi(mCat2(PORT,LCD_PORT_VHH),mCat3(PORT,LCD_PORT_VHH,LCD_PIN_VHH))


//*****************************************************************************
//
// Macro    : LCD_VHH_LOW(); Drives the VHH signal into logic 0
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_VHH_LOW()		\
	cbi(mCat2(PORT,LCD_PORT_VHH),mCat3(PORT,LCD_PORT_VHH,LCD_PIN_VHH))


//*****************************************************************************
//
// Macro    : LCD_CP_HIGH(); Drives the CP signal into logic 1
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_CP_HIGH()		\
	sbi(mCat2(PORT,LCD_PORT_CP),mCat3(PORT,LCD_PORT_CP,LCD_PIN_CP))


//*****************************************************************************
//
// Macro    : LCD_CP_LOW(); Drives the CP signal into logic 0
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_CP_LOW()		\
	cbi(mCat2(PORT,LCD_PORT_CP),mCat3(PORT,LCD_PORT_CP,LCD_PIN_CP))


//*****************************************************************************
//
// Macro    : LCD_LP_HIGH(); Drives the LP signal into logic 1
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_LP_HIGH()		\
	sbi(mCat2(PORT,LCD_PORT_LP),mCat3(PORT,LCD_PORT_LP,LCD_PIN_LP))


//*****************************************************************************
//
// Macro    : LCD_LP_LOW(); Drives the LP signal into logic 0
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_LP_LOW()		\
	cbi(mCat2(PORT,LCD_PORT_LP),mCat3(PORT,LCD_PORT_LP,LCD_PIN_LP))


//*****************************************************************************
//
// Macro    : LCD_FLM_HIGH(); Drives the FLM signal into logic 1
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_FLM_HIGH()		\
	sbi(mCat2(PORT,LCD_PORT_FLM),mCat3(PORT,LCD_PORT_FLM,LCD_PIN_FLM))


//*****************************************************************************
//
// Macro    : LCD_FLM_LOW(); Drives the FLM signal into logic 0
//
// Arguments: none
//
// Result   : none
//
//*****************************************************************************
#define LCD_FLM_LOW()		\
	cbi(mCat2(PORT,LCD_PORT_FLM),mCat3(PORT,LCD_PORT_FLM,LCD_PIN_FLM))


//*****************************************************************************
// Function prototypes
// for description of action and parameters see corresponding function in
// lcd_driver.c
//*****************************************************************************
void LCDTick 		(void);
void LCDInitialize 	(void);
void LCDStart 		(void);
void LCDStop 		(void);
void LCDWriteChar 	(U8 x, U8 y, U8 ch,        U8 inv);
void LCDWriteStr    (U8 x, U8 y, U8 * str,     U8 inv);
void LCDWriteCharP  (U8 x, U8 y, U8 * ch_ptrP, U8 inv);
void LCDWriteStrP   (U8 x, U8 y, U8 * strP   , U8 inv);
void LCDClearScreen	(void);
void LCDCopyScreen	(void);
void LCDWriteDigit_	(U8 x, U8 y, U8 digit_type, U8 digit);
void LCDWriteDigit	(U8 x, U8 y, U8 digit_type, U16 digit, U8 pos_count);
void LCDWriteIcon	(U8 x, U8 y, U8 icon);


//*****************************************************************************
// public data
//*****************************************************************************
extern U8 LCDFrameBuffer[LCD_BUFFER_SIZE];

//*****************************************************************************
// definitions of the control characters used (in LCDWriteStr())
// and display attribute
//*****************************************************************************
// control characters
#define C_LF		0x0A
#define C_CR		0x0D
#define C_CLS		0x1B

// display attribute
#define DISP_NORMAL		0
#define DISP_INVERTED	1


//digit attribute
#define BIG_DIGIT		0
#define SMALL_DIGIT		1

#define CLR_BIG_DIGIT	0
#define CLR_SMALL_DIGIT	1


//gear attribute
#define ECONOMIC_MODE	2
#define SPORT_MODE		3

#define FAST_GEAR		4
#define SLOW_GEAR		5

#define GEAR_UP			6
#define GEAR_DOWN		7

#define N_GEAR			8
#define R_GEAR			9

#define CLR_GEAR_ADV	18
#define CLR_HALF_GEAR	19


//other attribute
#define FUEL_TANK		10
#define LINE			11

//buttons attribute
#define BUTTON_OK		12
#define ARROWS_PAD		13

//temperature attribute
#define MINUS			14
#define CELSIUS			15
#define CLR_TEMP		16
#define CLR_MINUS		17


#endif
