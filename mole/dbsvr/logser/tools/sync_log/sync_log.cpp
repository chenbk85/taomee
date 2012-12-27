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
#include <time.h>

char logdir[200]="protolog/log";
FILE *logfp=NULL;
int log_hour=-1;
void set_log_file(time_t cur_time )
{
	struct tm tm;
	localtime_r(&cur_time, &tm);
	char file_name[4096];
	if (tm.tm_hour!=log_hour){
		sprintf (file_name, "%s/%s/%04d%02d%02d%02d",getenv("HOME"),logdir,
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,tm.tm_hour);
		if(logfp!=NULL ) fclose(logfp);
		if ((logfp=fopen(file_name, "a+"))==NULL)
			log_hour=tm.tm_hour;
		//保存当前的日志时段 
	}
}
 

void
sig_exit (int signo)
{
	exit (signo);
}
int 
main ( int argc, char *argv[] )
{
	int i;
	char recvbuf[4096];
	int key=12345;
	/*屏蔽信号*/
	for ( i = 0; i < 32; i ++ )
	{
		if(i == SIGALRM || i == SIGUSR1 )
			continue;
		signal( i, SIG_IGN );
	}
	sigset (SIGUSR2, &sig_exit);
	/*生成精灵进程, 使进程与终端脱节*/
	/*与终端脱节后,scanf等函数将无法使用*/
	switch (fork() )
	{
		case -1:
			return(31);
		case 0:
			break;
		default :
			exit(0);
	}

	int recvlen;
	int qid = msgget(key, IPC_CREAT | 0777);
	if (qid==-1){
		printf("get msg queue err %d \n",errno );
	}
	//deal
	while( true ) {
		if((recvlen=msgrcv( qid,recvbuf ,sizeof(recvbuf),0, 0 )) == -1){
			qid = msgget(key,IPC_CREAT | 0777);
			continue;
		}
		set_log_file(time(NULL));
		fwrite( recvbuf+4, recvlen , 1 ,logfp );
	}
}
