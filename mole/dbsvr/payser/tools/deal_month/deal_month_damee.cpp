/*
 * =====================================================================================
 *
 *       Filename:  deal_month_damee.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年02月04日 09时51分57秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */
#include "stdio.h"
#include "string.h"
#include "Cclientproto.h"


int
main ( int argc, char *argv[] )
{
	uint32_t transid;
	uint32_t userid;
	uint32_t damee;
	unsigned short port =  21001;
	char ipaddr[16] = "localhost";
	char infile[100],outfile[100];
	FILE *infp, *outfp;
	int ret, month_err;

	Cclientproto cp(ipaddr, port);
	if (argc!=2){
		printf("%s  filename", argv[0]);
		return 1;
	}

	strcpy (infile, argv[1]);
	sprintf(outfile,"%s_result",infile);
	if ((infp =fopen( infile,"r"))==NULL){
		return 1;
	}
	if ((outfp=fopen(outfile,"w"))==NULL){
		return 1;
	}
	while (fscanf(infp ,"%u %u %u",&transid,&userid, &damee  )!=EOF){
		ret=cp.pay_change_damee( userid , -damee);
		switch ( ret ){
			case SUCC: 
				month_err=  PAY_SUCC ; break;
			case USER_ID_NOFIND_ERR:  
				month_err= PAY_ERR_USERID_NOFIND;   	break;
			case  VALUE_OUT_OF_RANGE_ERR:  
				month_err= PAY_ERR_DAMEE_NO_ENOUGH;    break;
			default : 
				month_err= PAY_ERR_NO_DEFINE ; 		break;
		}
		fprintf(outfp,"%u\t%u\t%u\n", transid, userid, month_err );
	}
	fclose(infp);
	fclose(outfp);
}	/* ----------  end of function main  ---------- */


