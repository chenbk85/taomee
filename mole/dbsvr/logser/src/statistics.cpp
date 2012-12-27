#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<time.h>
#include<utime.h>
#include "mysql_iface.h"
#include "Cdbdeal.h"

int
main ( int argc, char *argv[] )
{
	mysql_interface *db ;
	Cdbdeal *dbdeal ;
	char date [9];
	memset(date,0,sizeof(date));
	db=new mysql_interface(argv[1],argv[2],argv[3]);
	//db=new mysql_interface("localhost","root","ta0mee");
	//db=new mysql_interface("localhost","jim","jim123");
	dbdeal=new Cdbdeal(db);	
	if (argc!=6){
		return 1;
	}
	dbdeal->daydeal_userinfo(argv[4],argv[5],0);
	//dbdeal->daydeal_userinfo(argv[1],argv[2],1);
	strncpy(date,argv[4],8 );
//	dbdeal->online_usercount_all(date);	
//	dbdeal->daydeal_petinfo(argv[1],argv[2]);
	delete db; 	
	delete dbdeal; 	
	return 0;
}				/* ----------  end of function main  ---------- */
