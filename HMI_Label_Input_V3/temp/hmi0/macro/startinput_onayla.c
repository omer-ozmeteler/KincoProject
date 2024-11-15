#include "macrotypedef.h"
#include "math.h"
#include "string.h"
/*
	Read,Write Local address function:
  	int ReadLocal( const char *type, int addr, int nRegs, void *buf, int flag  );
	int WriteLocal( const char *type, int addr, int nRegs, void *buf , int flag );

	Parameter:     type     is the string of "LW","LB" etc;
								address is the Operation address ;
 								nRegs    is the length of read or write ;
								buf        is the buffer which store the reading or writing data;
 								flag       is 0,then codetype is BIN,is 1  then codetype is BCD;
	return value : 1  ,Operation success
 								0,  Operation fail.

 	eg: read the value of local lw200 and write it to the lw202,with the codetype BIN,
		The code is :

    	short buf[2] = {0};
		ReadLocal("LW", 200, 2, (void*)buf, 0);
		WriteLocal("LW", 202, 2, (void*)buf, 0);
*/

short power(short a, short b);

int MacroEntry()
{

	int i;
	char title[20];
	short startupBits[2];

	// reset text field data for titles and text fields
	short zero[160000] = {0};
	memset(zero, 0x00, sizeof(zero));
	WriteLocal("LW", HM_TXT_BASE, (TXT_LEN*10), (void*)zero, 0);

	// compute display code and write to registers to display the homepage
	short HMDisplay[2];
	HMDisplay[0] = power(2, labelCount - 1);
	HMDisplay[1] = 0x0000;
	WriteLocal("LB", HM1_FLAG, 10, (void*)HMDisplay, 0);

	// reset data memory
	WriteLocal("LB", P1_FLAG, 10, (void*)zero, 0);
	WriteLocal("LB", WARN_FLAG, 3, (void*)zero, 0);
	WriteLocal("LW", HM_TXT_BASE, (TXT_LEN*10), (void*)zero, 0);
	WriteLocal("RW", 0, (PG_OFFSET*10), (void*)zero, 0);

	// turn off start screen displays
	start = 1;
	startInput = 0;
		
	// set startup bits
	startupBits[0] = 0x03FF;
	startupBits[1] = 0;
	WriteLocal("LB", STARTUP1_FLAG, 10, (void*)startupBits, 0);

	return 0;
}

short power(short a, short b){
	short i;
	short result = 1;
	if(b == 0){
		return result;
	}

	for(i = 0; i < b; i++){
		result *= a;
	}
	return result;
}
 