#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "service.h"

static int bufsz = 0;

int handle_init (int argc, char **argv, int pid_type)
{
	int c;

	switch (pid_type) {
		case PROC_MAIN:
			while ((c = getopt (argc, argv, "s:")) != -1) {
				if (c == 's')
					bufsz = atoi (optarg);
			}
			if (bufsz <= 0)
				ERROR_RETURN (("invalid buffer size:%s", optarg), -1);
			return 0;
		case PROC_WORK:
			return 0;
		case PROC_CONN:
			return 0;
		default:
			ERROR_LOG ("invalid pid_type=%d", pid_type);
			return -1;
	}

	return -1;
}


int handle_input (const char* buffer, int length, const skinfo_t *sk)
{
	struct in_addr addr;

	addr.s_addr = sk->remote_ip;	
	DEBUG_LOG ("recv from %s:%u %d bytes, package length=%d",
		inet_ntoa (addr), sk->remote_port, length, length); 
        return length ;
        
}

int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
    const char *policy="<policy-file-request/>";
    const char *response= "<?xml version=\"1.0\"?> \
                           <!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\"> \
                           <cross-domain-policy> \
                           <site-control permitted-cross-domain-policies=\"all\"/> \
                           <allow-access-from domain=\"*\" to-ports=\"*\" /> \
                           </cross-domain-policy>\0";
    if (rcvlen == 23 && !memcmp (recvbuf, policy, 23)) {
        DEBUG_LOG("Valid request,send policy ....") ;
        *sndlen = strlen(response)+1 ;
        *sendbuf = (char*)malloc (*sndlen);
        if (! *sendbuf)
            ERROR_RETURN (("malloc error, %s", strerror (errno)), -1);

        memcpy (*sendbuf, response, *sndlen);
    }
	return 0;
}
