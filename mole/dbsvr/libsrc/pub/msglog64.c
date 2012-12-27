#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <strings.h>
#include <malloc.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include "msglog.h"
#include <string.h>


struct message_header {
	unsigned short len;
	unsigned char  hlen;
	unsigned char  flag0;
	uint32_t  			 flag;
	uint32_t  			 saddr;
	uint32_t  			 seqno;
	uint32_t  			 type;
	uint32_t  			 timestamp;
};

typedef struct message_header message_header_t;

/*
 * standard data format
 *     arg1(4byte) arg2(4byte) arg3(4byte) ...
 * log message format    
 *     level(1byte) reserved(1byte) textlen(2byte) text...
 */
/*******************************************************************************
 * return : 0 sucess
            -1 
 ******************************************************************************/
int msglog(const char *logfile, uint32_t type, time_t timestamp, const void *data, int len) 
{
	message_header_t *h;
	int fd, s;

	s = sizeof(message_header_t)+len;
	h = (message_header_t *)malloc(s);
	bzero(h, s);
	h->len = s;
	h->hlen = sizeof(message_header_t);
	h->type = type;
	h->timestamp = timestamp;
	
	if(len>0)memcpy((char *)(h+1), data, len);

	signal(SIGXFSZ, SIG_IGN);
	fd = open(logfile, O_WRONLY|O_APPEND, 0666);
	if(fd<0) 
	{
		fd = open(logfile, O_WRONLY|O_APPEND|O_CREAT, 0666);
		int ret=fchmod(fd,0777);
		if ((ret!=0)||(fd<0))
		{
			return -1;
		}
	}
	
  write(fd, (char *)h, s);
  close(fd);

	signal(SIGXFSZ, SIG_DFL);
	free(h);
	return 0;
}

void set_str(char *ptr, int pos, const char *str, int *lenp) {
	if(str==NULL) {
		*(unsigned short *)(ptr+pos*4+0) = 0;
		*(unsigned short *)(ptr+pos*4+2) = 0;
	} else {
		int l = strlen(str)+1;
		*(unsigned short *)(ptr+pos*4+0) = *lenp;
		*(unsigned short *)(ptr+pos*4+2) = l;
		strcpy(ptr + (*lenp), str);
		*lenp += l;
	}
}
