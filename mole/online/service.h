#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <time.h>

#include <glib/ghash.h>
#include <libtaomee/dirtyword/dirtyword.h>

#include "benchapi.h"
#include "bindconf.h"
#include "daemon.h"

struct shm_queue;
struct shm_block;
struct sprite;

typedef struct fdsession {
	int			fd;
	uint32_t	id;
	uint16_t	remote_port;
	uint32_t	remote_ip;
	time_t		last_tm;
	time_t		last_sent_tm;
} fdsession_t;

typedef struct fd_array_session {
	int count;
	GHashTable* cn;
} fd_array_session_t;

typedef struct {
	int	idle_timeout;
	bind_config_elem_t*	bc_elem;
} config_cache_t;

extern fd_array_session_t fds;
extern config_cache_t config_cache;

extern DirtyWordHandle* dirtyword_hdl;
extern char* statistic_logfile;

extern char* security_key;
extern char* signature_key;

int  handle_input(const char *, int, int, int);
int  handle_process(uint8_t *, int, int, int);

int  handle_init(bind_config_elem_t* bc_elem);
int  handle_close(int, int, int del_node);
int  handle_open(const struct shm_block *mb);
int  handle_fini();

int  shm_ctl_block_push(struct shm_queue *q, int fd, int type, int del_node);

void run_worker_process(bind_config_t* bc, int bc_elem_idx, int n_inited_bc);
void restart_child_process(bind_config_elem_t* bc_elem);

void notify_stamp_svr(uint32_t id);
void send_chatlog(int cmd, const sprite_t* sender, userid_t rcver_id, const void* content, int len);
void send_chat_monitor(int cmd, const sprite_t* sender, userid_t rcver_id, const void* content, int len);

static inline void
add_fdsess(fdsession_t* fdsess)
{
	g_hash_table_insert(fds.cn, &(fdsess->fd), fdsess);
	++(fds.count);
}

static inline fdsession_t*
get_fdsess(int fd)
{
	return (fdsession_t*)g_hash_table_lookup(fds.cn, &fd);
}

static inline uint32_t
get_ip(int fd)
{
	fdsession_t* fdsess = (fdsession_t*)g_hash_table_lookup(fds.cn, &fd);
	return (fdsess ? fdsess->remote_ip : 0);
}

static inline uint32_t
get_ip2(const sprite_t* p)
{
	return p->fdsess->remote_ip;
}

static inline void
remove_fdsess(int fd)
{
	g_hash_table_remove(fds.cn, &fd);
	--(fds.count);
}

#endif
