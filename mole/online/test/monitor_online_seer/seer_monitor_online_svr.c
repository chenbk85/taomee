#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <arpa/inet.h>

#include <serverbench/benchapi.h>
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/interprocess/filelock.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/crypt/qdes.h>
#include  <statistic_agent/msglog.h>

#define LOG_SUCC_OFFSET 0x05040001
#define LOG_ERR_OFFSET 0x05040002
#define LOG_ERR_REASON_OFFSET 0x05040003

#define NUM_DATA 6
#define NUM_ERR_REASON 2
#define INBOX_PATH "/var/spool/inbox/bin.log"

int main(int argc, char** argv) 
{
	int socketfd;
	 struct sockaddr_in sa;    
	 bzero(&sa, sizeof(sa));    
	 sa.sin_family = AF_INET;    
	 sa.sin_port = htons(7689);    
	 sa.sin_addr.s_addr = htons(INADDR_ANY);   
	 memset(&(sa.sin_zero), 0, 8);   

	 socketfd = socket(AF_INET, SOCK_STREAM, 0);
	 int on = 1;
	 if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) == -1) {
		printf("errno=%d ", errno);  
		exit(1);  
	 }
	 if(bind(socketfd, (struct sockaddr *)&sa, sizeof(sa))!= 0) { 
	 	printf("bind failed ");  
		printf("errno=%d ", errno);  
		exit(1);  
	 } else { 
	 	printf("bind successfully\n"); 
	 }

	 if(listen(socketfd ,5) != 0) {  
	 	printf("listen error "); 
		exit(1);    
	 }  else  {  
	    printf("listen successfully\n");  
	 }

	 
	 int clientfd;	
	 struct sockaddr_in clientAdd;  
	 char buff[256];
	 int err_reason[NUM_ERR_REASON];
	 int msgret;
	 time_t now;
	 socklen_t len = sizeof(clientAdd);	
	 int closing =0; 
	 int i;
	 int *p;
	 while( closing == 0
	 	&& (clientfd = accept(socketfd, (struct sockaddr *)&clientAdd, &len)) >0 ) { 
		int n;  
		while((n = read(clientfd,buff, 256)) > 0) {
			p = (int*)buff;
			for ( i=0; i<NUM_DATA; i++) { //transfer byte order
				p[i] = ntohl(p[i]);
			}
			//log correct data
			time(&now);
			msgret = msglog(INBOX_PATH, LOG_SUCC_OFFSET, now, buff, NUM_DATA*sizeof(int));
			if (msgret != 0) {
				fprintf(stderr, "statistic log error\t[message type=%x return=%d]\n", LOG_SUCC_OFFSET, msgret);
			}
			//log err reason
			if ( p[2] == 0 ) {
				err_reason[0] = 1;
				err_reason[1] = 0;
			} else if ( p[2] == 1 && p[3] == 1 && p[4] == 1 && p[5] == 1 ) {
				err_reason[0] = 0;
				err_reason[1] = 0;
			} else {
				err_reason[0] = 0;
				err_reason[1] = 1;
			}
			msgret = msglog(INBOX_PATH, LOG_ERR_REASON_OFFSET, now, err_reason, sizeof(err_reason));
			if (msgret != 0) {
				fprintf(stderr, "statistic log error\t[message type=%x return=%d]\n", LOG_ERR_REASON_OFFSET, msgret);
			}
			//log err
			p = (int*)buff;
			for ( i=1; i<NUM_DATA; i++) { //begin from 2nd data
				p[i] = (p[i] == 0) ? 1 : 0; // make 0 to 1, 1 to 0
			}
			msgret = msglog(INBOX_PATH, LOG_ERR_OFFSET, now, buff, NUM_DATA*sizeof(int));
			if (msgret != 0) {
				fprintf(stderr, "statistic log error\t[message type=%x return=%d]\n", LOG_ERR_OFFSET, msgret);
			}
			//never use p now
		}
		close(clientfd);
	 }
	 return 0;
}






