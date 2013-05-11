
//Definitions of constants saved in  Flash memory

#ifndef __FLASH_DATA_H__
#define __FLASH_DATA_H__

//*****************************************************************************
// Labels
//*****************************************************************************
const FLASH U8 kmh[4]		= " /H";

const FLASH U8 rpm[4]		= "RPM";

const FLASH U8 fuel[5]		= "FUEL";

const FLASH U8 temp[5]		= "TEMP";

const FLASH U8 perc[2]		= "%";

const FLASH U8 litre[2]		= "L";

const FLASH U8 cons[12]		= "CONSUMPTION";

const FLASH U8 dpoint[2]	= ".";

const FLASH U8 lph1[3]		= "l/";
const FLASH U8 lph2[4]		= "100";

const FLASH U8 kmleft[8]	= "KM LEFT";

const FLASH U8 distance[9]	= "DISTANCE";

const FLASH U8 clr_distance[9]	= "        ";

const FLASH U8 trip[5]		= "TRIP";

const FLASH U8 km[3]		= "KM";



//*****************************************************************************
// Clock & Calendar attribute
//*****************************************************************************
const FLASH U8 days[7][4] = {
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

const FLASH U8 months[12][4] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

const FLASH U8 pointers2[]	= "^^";

const FLASH U8 pointers3[]	= "^^^";

const FLASH U8 spaces20[]	= "                    ";

const FLASH U8 adjust[]		= "Adjust the time";



#endif
