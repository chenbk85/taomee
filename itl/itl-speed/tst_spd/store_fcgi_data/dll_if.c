
#include <stdlib.h>

#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include <pthread.h>
#include "net_if.h"

#include "initiate.h"
#include "recv_data.h"
#include "write_file.h"
#include "proto.h"

int init_service()
{
	DEBUG_LOG("INIT...");

	setup_timer();
	INIT_LIST_HEAD(&g_timer.timer_list);

	if (initiate() == -1)
		ERROR_RETURN(("Initiate failed"), -1);

	pthread_t thread;
	if (pthread_create(&thread, NULL, (void*)write_file, NULL) != 0)
		ERROR_RETURN(("can not create thread:write_file\t"), -1);

	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");

	free(g_buffer);

	return 0;
}

void proc_events()
{
	handle_timer();
}

int get_pkg_len(int fd, const void* pkg, int pkglen)
{
	if (pkglen < 4)
		return 0;
	const protocol_t* pp = (const protocol_t*)pkg;
	int len = pp->len;
	if ((len > proto_max_len) || (len < (int)sizeof(protocol_t))) {
		ERROR_LOG("[c] invalid len=%d from fd=%d", len, fd);
		return -1;
	}
	return len;
}

int on_pkg_received(int sockfd, void* pkg, int pkglen)
{
	static uint64_t total_packet = 0;
	static uint64_t bad_packet = 0;
	protocol_t* pp = (protocol_t*)pkg;
	uint16_t cmd = pp->cmd;


	total_packet ++;
	if (pp->len != (uint32_t)pkglen) {
		bad_packet ++;
		ERROR_RETURN(("error pkg len\t[%u %u]", pp->len, pkglen), -1);
	}

	char* buf = (char*)pkg + (sizeof(protocol_t));
	DEBUG_LOG("recv packet_ len:%u, cmd:%x, sock:%d, id:%u, seg:%u\n",
			pp->len, pp->cmd, sockfd, pp->id, pp->seq);

	switch (cmd) {
	case proto_fcgi_cdn_report:
		return recv_data(buf, proto_fcgi_cdn_report, pp->seq, pp->len);
	case proto_fcgi_url_report:
		return recv_data(buf, proto_fcgi_url_report, pp->seq, pp->len);
	default:
		ERROR_LOG("invalid cmd\t[%u]", cmd);
		break;
	}
	return 0;
}

void on_conn_closed(int sockfd)
{
	if (sockfd == forward_fd)
		forward_fd = -1;
	return;
}
