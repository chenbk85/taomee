#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libtaomee/conf_parser/config.h>

#include "common.h"
#include "service.h"
#include "curl_init.h"

char web_serv_host[HOST_NAME_LEN];
int handle_init (int argc, char **argv, int pid_type)
{
	if (pid_type == PROC_WORK)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		strncpy(web_serv_host,config_get_strval("web_serv_host"),strlen(config_get_strval("web_serv_host")));
		DEBUG_LOG("web server host[%s]",web_serv_host);
	}
	return 0;
}

int handle_input (const char* buffer, int length, const skinfo_t *sk)
{
    if (length >= 4) {
        return *(uint32_t*)buffer;
    } else {
        return 0;
    }
}

int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
	protocol_t* pp = (protocol_t*)recvbuf;
	if (pp->len != rcvlen || rcvlen < sizeof(protocol_t)) {
		ERROR_RETURN(("error pkg len\t[%u %u]", pp->len, rcvlen), -1);
	}
	uint16_t cmd = pp->cmd;
	DEBUG_LOG("RECEIVE DATA\t[%u %u %u %u]", pp->id, pp->len, cmd, rcvlen);
	void* buf = recvbuf + (sizeof(protocol_t));
	int   len = rcvlen - (sizeof(protocol_t));
	switch (cmd) {
	case proto_wp_post_llocc:
		return post_llocc_cmd(pp->id, cmd, pp->body, len, sendbuf, sndlen);
	default:
		ERROR_RETURN(("invalid cmd\t[%u %u]", pp->id, cmd), -1);	
	}
	
	return 0;
}

int handle_close (const skinfo_t* sk)
{
    return 0;
}

void handle_fini (int pid_type)
{
	if (pid_type == PROC_WORK)
	{
		curl_global_cleanup();
	}
}
