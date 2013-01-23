#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <string.h>
#include <assert.h>
#include "benchapi.h"
#include "dispatcher.h"
#include "config.h"
#include "util.h"
#include "notifier.h"
#include "net.h"
#include "dll.h"
#include "tsc.h"
#include "daemon.h"

int rcvbufsz;
int sndbufsz;

static struct list_head bind_list;
struct fdinfo *fdinfo;

void free_bind_file ()
{
	struct bind_config *bc;
	list_for_each_entry (bc, &bind_list, list) {
		if (bc->bind_ip) {
			free (bc->bind_ip);
			bc->bind_ip = NULL;
		}
		free (bc);
	}
}

static int check_bind_config ()
{
	struct bind_config *bc;
	int cnt = 0;
	
	list_for_each_entry (bc, &bind_list, list) {
		if (INADDR_NONE == inet_addr (bc->bind_ip)) 
			ERROR_RETURN (("invalid ip address:%s", bc->bind_ip), -1);
		if (0 == bc->bind_port) 
			ERROR_RETURN (("invalid bind port:%u", bc->bind_port), -1);
		cnt++;
	}

	if (cnt == 0)
		ERROR_RETURN (("%s", "bind port not found"), -1);

	return 0;
}

int load_bind_file (const char* file_name)
{
	int len, ret_code = -1;
	char *buf;
	struct bind_config *bc = NULL;

	INIT_LIST_HEAD (&bind_list);
	if ((len = mmap_config_file (file_name, &buf)) > 0) {
		char *field[4];
		char* start = buf;

		len = strlen (buf);
		while (buf + len > start) {
			char* end = strchr (start, '\n');
			if (*end) *end = '\0';
			if (*start != '#' && str_explode (NULL, start, field, 4) == 4) {
				bc = (struct bind_config *) malloc (sizeof (struct bind_config));

				bc->bind_ip = strdup (field[0]);
				bc->bind_port = atoi (field[1]);
				if (!strcasecmp (field[2], "tcp"))
					bc->type = SOCK_STREAM;
				else if(!strcasecmp (field[2], "udp"))
					bc->type = SOCK_DGRAM;
				else {
					ERROR_LOG ("invalid socket type:%s", field[2]);
					free (bc->bind_ip);
					free (bc);
					break;
				}
				bc->timeout = tscsec * atoi (field[3]);
				
				list_add_tail (&bc->list, &bind_list);
			}
			start = end + 1;
		}

		munmap (buf, len);
		ret_code = check_bind_config ();
	}

	if (ret_code != 0)
		free_bind_file ();

//	bind_dump ();
	BOOT_LOG (ret_code, "load bind file:%s", file_name);
}

void bind_dump ()
{
#ifdef DEBUG	
	struct bind_config *bc;

	TRACE_LOG ("bind file dump:");
	list_for_each_entry (bc, &bind_list, list) {
		TRACE_LOG ("\t%16s\t%5d\t%d\t%d", bc->bind_ip, bc->bind_port, bc->type, bc->timeout / tscsec);
	}
#endif	
}

void conn_loop ()
{
	struct bind_config *bc;
	struct shm_block *mb;

	daemon_set_title ("%s:[CONN]", prog_name);
#ifdef EPOLL_MODE
	if ((epfd = epoll_create (MAXFDS)) < 0) {
		ERROR_LOG ("epoll_create failed, %s", strerror (errno));
		return ;
	}
	ep_events = (struct epoll_event*) calloc (MAXFDS, sizeof (struct epoll_event));
#else
	pollfd = (struct pollfd*) calloc (sizeof (struct pollfd), MAXFDS);
	if (pollfd == NULL) {
		ERROR_LOG ("malloc failed, %s", strerror (errno));
		return ;
	}
#endif

	rcvbufsz = config_get_intval ("recv_buffer", pagesize);
	sndbufsz = config_get_intval ("send_buffer", pagesize);
	fdinfo = (struct fdinfo*) calloc (sizeof (struct fdinfo), MAXFDS);
	if (fdinfo == NULL) {
		ERROR_LOG ("malloc failed, %s", strerror (errno));
		return;
	}

	list_for_each_entry (bc, &bind_list, list) {
		if (net_start (bc->bind_ip, bc->bind_port, bc->type, bc->timeout))
			return;
	}
	do_add_conn (pipe_rd_fd (), CN_PIPE_MASK, NULL, NULL, 0);

	while (!stop) {
		int nr = net_wait (1000);
		while (shmq_pop (&sendq, &mb, 0) == 0)  {
			schedule_output (mb);
			free (mb);
		}
		
		net_loop (nr);
	}
	net_stop ();

	if (dll.handle_fini)
		dll.handle_fini (PROC_CONN);
}
