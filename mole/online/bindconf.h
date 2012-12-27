#ifndef ONLINE_BIND_CONF_H_
#define ONLINE_BIND_CONF_H_

// POSIX
#include <netinet/in.h>
// Self-define
#include "shmq.h"

#define MAX_LISTEN_FDS	 60

typedef struct bind_config_elem {
	int			online_id;
	char		online_name[16];
	char		bind_ip[16];
	in_port_t	bind_port;
	char		gameserv_ip[16];
	in_port_t	gameserv_port;
	char		gameserv_test_ip[16];
	in_port_t	gameserv_test_port;
	struct shm_queue sendq;
	struct shm_queue recvq;
} bind_config_elem_t;

typedef struct bind_config {
	int online_start_id;
	int	bind_num;
	bind_config_elem_t configs[MAX_LISTEN_FDS];
} bind_config_t;

extern bind_config_t bindconf;

int load_bind_file();

static inline bind_config_t*
get_bind_conf()
{
	return &bindconf;
}

static inline int
get_bind_conf_idx(const bind_config_elem_t* bc_elem)
{
	return (bc_elem - &(bindconf.configs[0]));
}

#endif // ONLINE_BIND_CONF_H_
