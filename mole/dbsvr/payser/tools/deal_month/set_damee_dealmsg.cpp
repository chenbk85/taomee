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
	uint32_t dealflag;
	unsigned short port =  21001;
	char ipaddr[16] = "localhost";
	FILE *infp;
	int ret;

	Cclientproto cp(ipaddr, port);
	if (argc!=2){
		printf("%s  filename\n", argv[0]);
		return 1;
	}

	if ((infp =fopen(argv[1] ,"r"))==NULL){
		printf("err for open file[%s]\n", argv[1]);
		return 1;
	}

	while (fscanf(infp ,"%u %u %u",&transid,&userid, &dealflag )!=EOF){
		ret=cp.f_PAY_SET_AUTO_MONTH_DEALMSG(transid ,dealflag);
		if(ret!=SUCC){
			printf("err update dealmsg, transid[%u] userid[%u] dealflag[%u]ret[%d]\n",
				 transid,userid,dealflag,ret);
		}
	}
	return 0;
	fclose(infp);
}	/* ----------  end of function main  ---------- */
