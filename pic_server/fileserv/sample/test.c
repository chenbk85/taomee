#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <iter_serv/net_if.h>

typedef struct TmrTest {
	list_head_t timer_list;
} tmr_test_t;

tmr_test_t tmr;

int test(void* owner, void* data)
{
	net_send((int)data, "dsafdf", 7);
	return 0;
}

int init_service()
{
	DEBUG_LOG("INIT...");

	setup_timer();
	INIT_LIST_HEAD(&tmr.timer_list);

	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");
	return 0;
}

void proc_events()
{
	handle_timer();
}

int	get_pkg_len(int fd, const void* pkg, int pkglen)
{
	int i;

	const char* str = pkg;
	for (i = 0; (i != pkglen) && (str[i] != '\0'); ++i) ;

	if (i != pkglen) {
		net_send(fd, pkg, i + 1);
		return i + 1;
	}

	return 0;
}

int on_pkg_received(int sockfd, void* pkg, int pkglen)
{
	DEBUG_LOG("%s", pkg);
	net_send(sockfd, pkg, pkglen);
	ADD_TIMER_EVENT(&tmr, test, (void*)sockfd, get_now_tv()->tv_sec + 5);
	return 0;
}

