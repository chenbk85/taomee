#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "tcpip.h"
#include "Cclientproto.h"


int main (int argc, char* argv[])
{
	
	Cclientproto *cp;
	int ret; //succ = 0, fail= 0;	
	if (argc<4){
		printf("arg err!! need : ip port file \n");	
		return 1 ;
	}
	cp=new  Cclientproto(argv[1],atoi(argv[2]));
	ret=cp->send_data(argv[3] );
	printf("ret[%d]\n" ,ret);	
	return ret;
}
