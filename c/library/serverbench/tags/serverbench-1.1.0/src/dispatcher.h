#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_
#include <sys/types.h>
#include "list.h"

struct bind_config {
	struct list_head list;
	char	*bind_ip;
	u_short bind_port;
	char	type;
	long long timeout;
};

extern int load_bind_file (const char* file_name);
extern void free_bind_file ();
extern void bind_dump (); 

extern void conn_loop ();
#endif
