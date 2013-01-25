#ifndef BENCHAPI_H
#define BENCHAPI_H


#include <stdint.h>

#include <sys/cdefs.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>

enum
{
    PROC_MAIN = 0,
    PROC_CONN,
    PROC_WORK,
    PROC_TIME
};

/**
 * skinfo_t - socket infomation
 * @fd - socket description
 * @type - sock type (%SOCK_STREAM, %SOCK_DGRAM, etc)
 * @local - connection local address
 * @remote - connection peer address
 */
typedef struct skinfo_struct {
	int sockfd;
	int type;
	time_t recvtm;
	time_t sendtm;
	u_int    local_ip;
	u_short  local_port;
	u_int    remote_ip;
	u_short  remote_port;
} skinfo_t;

/*
 *  struct app_config - application program interface
 */
typedef struct dll_func_struct {
	void *handle;
	int (*handle_init) (int, char **, int);
	int (*handle_input) (const char*, int, const skinfo_t*);
    int (*handle_filter_key) (const char*, int, uint32_t*);
	int (*handle_process) (char *, int , char **, int *, const skinfo_t*);
	int (*handle_open) (char **, int *, const skinfo_t*);
	int (*handle_close) (const skinfo_t*);
	int (*handle_timer) (int *);
	void (*handle_fini) (int);
} dll_func_t;

extern dll_func_t dll;


#endif
