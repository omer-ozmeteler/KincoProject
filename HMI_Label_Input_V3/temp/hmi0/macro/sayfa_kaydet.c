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

// Read and Write functions address sizes are related to register domains.
/*
short pageBits[2];

pageBits[0] = 0x03FF;    -> turns on all 10 lamps
pageBits[1] = 0x0000;    -> do not care but necessary since WriteLocal works with minimum of 32 bit write size

WriteLocal("LB", 110, 10, (void*)pageBits, 0); -> writes the first 10 bits from LB 110 to LB120
*/


int MacroEntry()
{

	// identify page and load variables ------------------------------------------------------------------
	int i;
	int j;
		
	char data[30]; 					// data field
	short pageBits[2]; 			// page flags													 		=> 0-p1, 1-p2, 2-p3, 3-p4, 4-p5, 5-p6, 6-p7, 7-p8, 8-p9, 9-p10
	short P_elements[10]; 	// number elements of in current page  	=> 0-num1, 1-num2, 2-num3, 3-num4, 4-num5, 5-current, 6-num_elements, 7-view, 8-NULL, 9-NULL
	
	int P_TXT_INP; 					// address of text field input in current page
	int HM_TXT_D; 					// address of text field display in home page for the current page
	int P_TXT_D_BASE; 			// base address of text displays on current page
	int P_DATA_D_BASE; 		// base address of data list on current page
	
	ReadLocal("LB", P1_FLAG, 10, (void*)pageBits, 0);

	for(i = 0; i < 10; i++){
		if((pageBits[0] >> i) & (0x0001)){
			// set addresses
			P_TXT_INP					= (i*PG_OFFSET);
			HM_TXT_D					= HM_TXT_BASE + (i*TXT_LEN);
			P_TXT_D_BASE 		=	P_TXT_INP + TXT_LEN + 10;
			P_DATA_D_BASE		= P_TXT_D_BASE + (5*TXT_LEN);

			// read number elements
			ReadLocal("RW", (P_TXT_INP + TXT_LEN), 10, (void*)P_elements, 0);
			break;
		}
	}
	
	// process text input ---------------------------------------------------------------------------------

	// clear homepage before writing 
	char zero[30] = {0};
	memset(zero, 0x00, sizeof(zero));
	WriteLocal("LW", HM_TXT_D, TRANS_LEN, (void*)zero, 0);

	// read text input
	ReadLocal("RW", P_TXT_INP, TRANS_LEN, (void*)data, 0);

	// count whitespaces from beginning
	i = 0;
	while(data[i] == 32){
		i++; 
	}

	// count white spaces from end
	j = TXT_LEN-1;
	while(data[j] == 32){
		j--; 
	}
	
	short final = (j - i) + 1;
	if(final%2 != 0){
		final++;
	}

	// edit list  --------------------------------------------------------------------------------------------
	short current 					= P_elements[5];
	short num_elements 	= P_elements[6];
	short view 						= P_elements[7];

	// check if view index is in list boundaries
	if((view > 0) && (view < num_elements)){

		// edit data list
		j = P_DATA_D_BASE  + ((view - 1)*TXT_LEN);
		WriteLocal("RW", j, TRANS_LEN, (void*)zero, 0);
		WriteLocal("RW", j,  final/2,  (void*)data + i, 0);

		// check if entry is in current window and refresh table row
		if((view >= current - 2) && (view <= current + 2)){
			
			// edit list view
			j = P_TXT_D_BASE + ((view - current)*TXT_LEN);
			WriteLocal("RW", j, TRANS_LEN, (void*)zero, 0);
			WriteLocal("RW", j,  final/2,  (void*)data + i, 0);

		}
	}
	
	// write to homepage
   WriteLocal("LW",  HM_TXT_D,  final/2,  (void*)data + i, 0);

	// return to homepage and turn off all page flags
	short del[2] = {0};
	memset(del, 0x00, sizeof(del));
	WriteLocal("LB", P1_FLAG, 10, (void*)del, 0);
	
	return 0;
} 