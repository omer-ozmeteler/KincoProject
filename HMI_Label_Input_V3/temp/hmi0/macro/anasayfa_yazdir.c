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

void strConcat(char*, char*, int*);
int strLen(char* );

int MacroEntry()
{
	// initialize
	int i = 0;
	int j = 0;
	send = 0;
	char data[31];

	// clear write buffer
	char total[310];
	memset(total, 0x00, sizeof(total));
	WriteLocal("LW", 0, ((TRANS_LEN*10)+5), (void*)total, 0);
	
	// concatenate data into a single string
	for(i = 0; i < 10; i++){
		memset(data, 0x00, sizeof(data));
		ReadLocal("LW", (HM_TXT_BASE + (i*TXT_LEN)), TRANS_LEN, (void*)data, 0);
		if(strlen(data) > 0){
			strConcat(total, data, &j);
		}
	}

	// write into buffer and send
	WriteLocal("LW",  0,  j/2,  (void*)total, 0);
	send = 1;

	return 0;
}

void strConcat(char* total, char* data, int* j){
	int i = 0;

	// copy all data into array without whitespaces
	while((data[i] > 32) && (data[i] < 127)){
			total[*j] = data[i];
			i++;
			*j += 1;
	}
	 // add newline after input
	total[*j] = '\n';
	*j += 1;
}

int strLen(char* data){

	int len = 0;
	while((data[len] > 32) && (data[len] < 127)){
		len++;
	}
	return len;
	
}
 