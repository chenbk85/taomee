#include  <stdint.h> 
#include  <time.h> 
#include  <stdio.h> 
#include "common.h"
#define ACTIVE_KEY "T#S213l0" 

#define DEF_VALUE 	0xF03C3F22 

void active_encode(uint32_t userid , uint32_t * code  )
{
	time_t t=time(NULL); 		
	uint32_t code_real[4];	
	code_real[0]=t;
	code_real[1]=userid;
	code_real[2]=DEF_VALUE;
	code_real[3]=t+random();
	DES_n( ACTIVE_KEY,(char*)code_real ,(char*)code ,2);
}

int active_decode( uint32_t * code, uint32_t  *p_userid  )
{
	uint32_t code_real[4];	
	_DES_n( ACTIVE_KEY,(char*)code, (char*)code_real ,2);
	if (code_real[0]<=code_real[3] && code_real[2]==DEF_VALUE ){
		*p_userid=code_real[1];		
		return 0;
	}else return 1;
}

int
main ( int argc, char *argv[] )
{
	uint32_t userid=300111;
	uint32_t de_userid;
	uint32_t code[4];
	char 	codestr[40];
	int i;
	srandom(time(NULL) );

	for (i=1;i<=500;i++){
		userid=20000+i;
		active_encode(userid, code);
 		sprintf (codestr,"%08X%08X%08X%08X", code[0],code[1],code[2],code[3]  );
		if (active_decode(code, &de_userid)==0){
			printf("%u \t%s \thttp://www.taomee.com/active.php?code=%s\n", 
				userid, codestr, codestr );
		}else{
			printf("decode errr");
			return 1;	
		}
	}
	
	/*
	char buf[33];
	uint8_t sign[16];
	MD5(str,strlen(str),sign);
	ASC2HEX_2(buf,sign, 16 );
	printf("%s",buf);
	*/
	return 0;
}	/* ----------  end of function main  ---------- */

