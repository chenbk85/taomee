#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "logproto.h"
#include "benchapi.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <time.h>

int 
main ( int argc, char *argv[] )
{
	TDAY_COUNT d;
	if (argc!=4){
		return -1;
	}
	d.type=atoi ( argv[1]   );					
	d.date=atoi (argv[2]);
	d.count=atoi (argv[3] );
	SAVE_PROTOLOG_DAY_COUNT(d);
	return 0;
}
