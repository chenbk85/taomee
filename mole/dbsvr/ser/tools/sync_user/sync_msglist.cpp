#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "Cclientproto.h"
#include "benchapi.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <Csync_user.h>


int 
main ( int argc, char *argv[] )
{
	char sendbuf[4096];
	Csync_user *sync_user;
    int sendlen=0;
	if (argc!=3){
		printf("need msglistkey  file \n");
		return 1;
	}
	sync_user=new Csync_user(atoi(argv[1]) );
    FILE *fp=fopen(argv[2],"r");
	if (fp==NULL){
		printf("file nofind:%s\n" , argv[2]);
	}
	printf("sendfile:%s\n" , argv[2]);

    sendlen=fread(sendbuf,1 ,1000,fp);
	printf("sendlen:%d:%d\n" ,sendlen,errno);
	printf("sendmsg:%s\n" ,sendbuf);
	while  ( sync_user->send(sendbuf,sendlen)==FAIL  )
	{
		printf ("wait.....\n");
		sleep(1);
	}
	fclose(fp);
}
