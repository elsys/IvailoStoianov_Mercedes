/*
    hardware.h

    Define hardware dependent stuff here
*/

#define CPUCLK 		16000000L    // CPU xtal
#define TICKRATE 	10000L       // AvrX timer queue Tick rate


// Peripheral initialization
//#define TCNT0_INIT (0xFF-(CPUCLK/TICKRATE/8))
#define TCNT0_INIT 63


// macro for conversion of timer ticks into ms
#define mConvertMsToTicks(x)	((x)*(TICKRATE)/1000L)
