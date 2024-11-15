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
int MacroEntry()
{
	// identify page and load variables --------------------------------------------
	int i;
		
	short pageBits[2]; 			// page flags													 		=> 0-p1, 1-p2, 2-p3, 3-p4, 4-p5, 5-p6, 6-p7, 7-p8, 8-p9, 9-p10
	short P_elements[10]; 	// number elements of in current page  			=> 0-num1, 1-num2, 2-num3, 3-num4, 4-num5, 5-current, 6-num_elements, 7-view, 8-NULL, 9-NULL
	
	int P_TXT_INP; 				// address of text field input in current page
	int P_TXT_D_BASE; 		// base address of text displays on current page
	int P_DATA_D_BASE; 		// base address of data list on current page
	
	ReadLocal("LB", P1_FLAG, 10, (void*)pageBits, 0);

	for(i = 0; i < 10; i++){
		if((pageBits[0] >> i) & (0x0001)){
			// set addresses
			P_TXT_INP				= (i*PG_OFFSET);
			P_TXT_D_BASE 		=	P_TXT_INP + TXT_LEN + 10;
			P_DATA_D_BASE		= P_TXT_D_BASE + (5*TXT_LEN);

			// read number elements
			ReadLocal("RW", (P_TXT_INP + TXT_LEN), 10, (void*)P_elements, 0);
			break;
		}
	}
	
	// refresh list ----------------------------------------------------------------
	short current = P_elements[5];

	if(current < (LIST_LEN - 2)){
		char data[30];		       
		current = current + 1;

		// write number elements
		P_elements[0] = current - 2;
		P_elements[1] = current - 1;
		P_elements[2] = current;
		P_elements[3] = current + 1;
		P_elements[4] = current + 2;
		P_elements[5] = current;
	
		WriteLocal("RW", (P_TXT_INP + TXT_LEN), 10, (void*)P_elements, 0);
		
		// update list view
		ReadLocal("RW", P_DATA_D_BASE + ((P_elements[0] - 1)*TXT_LEN), TRANS_LEN, (void*)data, 0);
		WriteLocal("RW", P_TXT_D_BASE, TRANS_LEN, (void*)data, 0);
	
		ReadLocal("RW", P_DATA_D_BASE + ((P_elements[1] - 1)*TXT_LEN), TRANS_LEN, (void*)data, 0);
		WriteLocal("RW", (P_TXT_D_BASE + TXT_LEN), TRANS_LEN, (void*)data, 0);
	
		ReadLocal("RW", P_DATA_D_BASE + ((P_elements[2] - 1)*TXT_LEN), TRANS_LEN, (void*)data, 0);
		WriteLocal("RW", (P_TXT_D_BASE +  (2*TXT_LEN)), TRANS_LEN, (void*)data, 0);

		ReadLocal("RW", P_DATA_D_BASE + ((P_elements[3] - 1)*TXT_LEN), TRANS_LEN, (void*)data, 0);
		WriteLocal("RW", (P_TXT_D_BASE +  (3*TXT_LEN)), TRANS_LEN, (void*)data, 0);
	
		ReadLocal("RW", P_DATA_D_BASE + ((P_elements[4] - 1)*TXT_LEN), TRANS_LEN, (void*)data, 0);
		WriteLocal("RW", (P_TXT_D_BASE +  (4*TXT_LEN)), TRANS_LEN, (void*)data, 0);
	}

	return 0;
} 