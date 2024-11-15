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
short computeDisplayCode(short count);

int MacroEntry()
{

	if(labelCount > 0){
		// reset text field data for titles
		short zero[200] = {0};
		memset(zero, 0x00, sizeof(zero));
		WriteLocal("LW", START_TITLE_BASE, 200, (void*)zero, 0);
	
		// compute display code and write to registers to display text fields
		short textDisplay[2];
		short count = labelCount;
		textDisplay[0] = computeDisplayCode(count);
		textDisplay[1] = 0x0000;
	
		WriteLocal("LB", TITLE1_FLAG, 10, (void*)textDisplay, 0);

		// stop displaying start screens
		start = 0;
		startInput = 1;
	}else{
		
		// continue displaying start screens
		start = 0;
		startInput = 0;
	}
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

short computeDisplayCode(short count){
	short i;
	short displayCode = 1;

	for(i = count; i > 1; i--){
		displayCode += power(2, i - 1);
	}
	return displayCode;
}


 