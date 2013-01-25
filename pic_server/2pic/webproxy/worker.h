#ifndef WORKER_H_
#define WORKER_H_

#include "util.h"

#define URL_LEN	512
#define	CONNECT_WEB_ERROR	9001

typedef struct _recv_fileserv {
	uint32_t	albumid;
	uint32_t	hostid;
	uint32_t	filelen;
	char	lloccode[LLOCCODE_LEN+1];
	char	filename[MAX_NAME_LEN+1];
}__attribute__((packed)) recv_fileserv_t; 


int post_llocc_cmd(uint32_t uid, uint16_t cmd, char *recvbuf, int rcvlen, char **sendbuf, int *sndlen);

#endif
