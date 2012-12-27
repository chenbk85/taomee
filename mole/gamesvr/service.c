#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "service.h"
#include "tcp.h"
#include <libtaomee/log.h>
#include "config.h"
#include "daemon.h"
#include "shmq.h"
#include "dbproxy.h"
#include "sprite.h"
#include "dll.h"
#include "proto.h"
#include "timer.h"
#include "net.h"
#include "mcast.h"
#include "game.h"

fd_array_session_t fds;
static uint32_t idle_timeout;

// handle messages from db
static int worker_handle_net (int fd, const void* buf, int len, sprite_t** u);

int handle_init ()
{
	idle_timeout = config_get_intval("idle_timeout", 300);
	memset(&fds, 0, sizeof(fds));

	/*proxy_fd = connect_to_svr( config_get_strval ("dbproxy_ip"),*/
	/*config_get_intval("dbproxy_port", 0), 65535, 1 );*/

	statistic_file = config_get_strval("statistic_logfile");
	if ( load_dlls(config_get_strval("games_conf")) == -1 )
		return -1;

	setup_timer();
	init_sprites();
	if (config_get_strval("addr_mcast_ip")) {
		if (create_addr_mcast_socket() == 0) {
			send_addr_mcast_pkg(addr_mcast_1st_pkg);
			DEBUG_LOG("send mcast");
		} else {
			return -1;
		}
	}
	return 0;
}

int handle_fini ()
{
	int i;

	for (i = 0; i < epi.maxfd; i++) {
		if (epi.fds[i].type == REMOTE_TYPE_FD && epi.fds[i].cb.sendlen > 0)
			return 0;
	}

	destroy_timer ();
	fini_sprites ();
	unload_dlls ();

	return 1;
}

int shm_ctl_block_push (struct shm_queue *q, int fd, int type)
{
	shm_block_t mb;

	mb.id = fds.cn[fd].id;
	mb.length = sizeof(shm_block_t);
	mb.type = type;
	mb.fd = fd;

	if (type == FIN_BLOCK)
		handle_close(fd, 1);

	return shmq_push(q, &mb, NULL);
}

int handle_input(const char* buffer, int rcvlen, int fd, int is_conn)
{
	int len, v;
	const char *policy   = "<policy-file-request/>";
	const char *response = "<?xml version=\"1.0\"?>"
				"<!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\">"
				"<cross-domain-policy>"
				"<site-control permitted-cross-domain-policies=\"all\"/>"
				"<allow-access-from domain=\"*\" to-ports=\"*\" />"
				"</cross-domain-policy>";

	if (rcvlen < 4) return 0;
	v = *(uint32_t *)buffer;
	if (is_conn) {
		if (rcvlen == 23 && !memcmp (buffer, policy, 23)) {
			net_send (fd, (uint8_t*)response, strlen(response) + 1, 1);
			epi.fds[fd].cb.recvlen = 0;

			TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", policy, response);
			return 0;
		}
		len = ntohl (v);
		v = sizeof(protocol_t);
	} else {
		len = v;
		v = sizeof (server_proto_t);
	}

	if ((len > RCVBUFSZ) || (len < v)) {
		return -1;
	}

	return len;
}

int handle_process (const uint8_t *recvbuf, int rcvlen, int fd, int is_conn)
{
	int err = 0;

	if (is_conn) {
		fds.cn[fd].last_tm = now.tv_sec;
		if ((err = parse_protocol (recvbuf, rcvlen, fd)))
			shm_ctl_block_push(&sendq, fd, FIN_BLOCK);
	} else {
		sprite_t *p = 0;
		if ( (err = worker_handle_net(fd, recvbuf, rcvlen, &p)) ) {
			p->waitcmd = 0;
			DEBUG_LOG("HANDLE_INVALID_DATA handle_process, %d", p->id);
			handle_invalid_data(p, GER_invalid_data);
		}
	}

	TRACE_LOG ("handle_process return %d, conn=%d", err, is_conn);
	return 0;
}

void handle_close (int fd, int is_conn)
{
	if (!is_conn) {
		if (fd == proxy_fd) proxy_fd = -1;
	} else {
		if (fd > fds.fdmax || fds.cn[fd].id == 0) {
			ERROR_LOG ("connection %d has already been closed, id=%u", fd, fds.cn[fd].id);
			return;
		}

		assert (fds.count > 0 && fds.fdmax >= fds.count);

		del_sprite_by_fd(fd, 1);
		memset(&fds.cn[fd], 0, sizeof (fdsession_t));
		if (fds.fdmax == fd) {
			int i;
			for (i = fd - 1; i > 0; i--)
				if (fds.cn[i].id != 0)
					break;
			fds.fdmax = i;
		}
		fds.count--;
	}
}

int handle_open (const shm_block_t * mb)
{
	assert (mb->fd < MAXFDS);
	if (fds.cn[mb->fd].id != 0
			|| mb->length != sizeof (struct shm_block) + sizeof (struct skinfo))
		ERROR_RETURN (("worker_handle_queue OPEN_BLOCK,fd=%d, length=%d, last=%u",
					mb->fd, mb->length, fds.cn[mb->fd].last_tm), -1);
	else {
		fds.cn[mb->fd].id = mb->id;
		fds.cn[mb->fd].remote_port = *(uint16_t *) mb->data;
		fds.cn[mb->fd].remote_ip = *(uint32_t *) & mb->data[2];
		fds.cn[mb->fd].last_tm = *(uint16_t *) & mb->data[6];

		fds.fdmax = fds.fdmax > mb->fd ? fds.fdmax : mb->fd;
		fds.count++;
		assert (fds.fdmax < MAXFDS && fds.count <= MAXFDS);
		TRACE_LOG ("open connection, fd=%d, id=%u", mb->fd, mb->id);
	}

	return 0;
}

// handle messages from db
static int worker_handle_net (int fd, const void* buf, int len, sprite_t** u)
{
	assert( len >= sizeof(server_proto_t) );

	const server_proto_t* p = buf;
	int connfd  = p->seq >> 16;
	int waitcmd = p->seq & 0xFFFF;
	if(0 ==  p->seq )
	{
		return 0;
	}
//	DEBUG_LOG("WORKER_HANDLE_NET: cmd %x, ret %d", p->cmd, p->ret);
	//to prevent from reclose the connection, it return 0
	// !!problem here!! what if p->waitcmd exceeded 16bits
	if ( !(*u = get_sprite_by_fd(connfd)) || (*u)->waitcmd != waitcmd ) {
		if(*u) {
			ERROR_LOG("WORKER_HANDLE_NET, player %d waitcmd %d, actual %d", (*u)->id, (*u)->waitcmd, waitcmd);
		}
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%x, seq=%u", p->id, p->cmd, p->seq), 0);
	}

	int err = -1;
	switch (p->ret) {
	case 0:
	case 1006:
	case 2547:
		break;
	case 1116:
		DEBUG_LOG("ERROR:%d\t[%d %d %d]", waitcmd, (*u)->id, p->id, p->ret);
		return send_to_self_error(*u, (*u)->waitcmd, -ERR_daily_money_limit_exceeded, 1);
	case 1147:
		//card game experience limited
	case 1114:
	case 2675:
	case 2676:
	case 2679:
	case 1119:
		//已经拿过长枪了
//	case 1325:
		//gold yuanbao limited
//	case 1326:
		//silver yuanbao limited
		break;
	default:
		ERROR_LOG("db error: ret=%d, id=%u, cmd=0x%x", p->ret, p->id, p->cmd);
		goto failed;
	}

#define DO_MESSAGE(n, func) \
	case n: err = func(*u, p->id, p->body, len - sizeof(server_proto_t), p->ret); break

	switch (p->cmd) {
	DO_MESSAGE(db_change_game_attr, upd_game_attr_callback);
	DO_MESSAGE(db_obtain_item, get_item_callback);
	//DO_MESSAGE(db_obtain_items, get_items_callback);
	DO_MESSAGE(db_obtain_pet_item, get_pet_item_callback);
//------------------------------------------------
//	DO_MESSAGE(db_get_yuanbao, add_yuanbao_callback);
//------------------------------------------------

//------------------------------------------------
//	DO_MESSAGE(SVR_PROTO_ADD_MEDAL, add_medal_callback);
//------------------------------------------------
	//DO_MESSAGE(SVR_PROTO_ADD_FIRE_MEDAL, add_fire_medal_callback);

	default:
		err = on_db_return(*u, p->id, p->body, len - sizeof(server_proto_t), p->ret);
//		ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", p->cmd, (*u)->id, (*u)->waitcmd);
		break;
	}
#undef DO_MESSAGE

failed:
	return err;
}

void restart_child_process()
{
	close(pipe_handles[1]);
	do_del_conn(pipe_handles[2], 2);
	shmq_destroy ();

	shmq_create();
	pipe_create ();

	pid_t pid;

	if ((pid = fork ()) < 0)
		ERROR_LOG("fork child process, fail");
	//parent process
	else if (pid > 0) {
		close (pipe_handles[3]);
		close (pipe_handles[0]);
		do_add_conn (pipe_handles[2], PIPE_TYPE_FD, NULL);

	//child process
	} else {
		close (pipe_handles[1]);
		close (pipe_handles[2]);

		net_init (MAXFDS, pipe_handles[0]);

		if (handle_init () != 0)
			goto out;

		while (!stop || !handle_fini ())
			net_loop (1000, RCVBUFSZ, 0);

out:
		shmq_destroy ();
	}
}
