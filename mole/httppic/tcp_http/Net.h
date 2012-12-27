#ifndef NET_H
#define NET_H
#include "Atomic.h"
#include <queue>
using namespace std;

class Epoll_handle
{
public:
	Epoll_handle ();
	~Epoll_handle ();

	int open ();
	int accept_cn ();
	int child_wait(int *readable_fd);

	void wait_ready ();
	
	int add_epoll_events (int fd);
	int del_epoll_events (int fd);

	int idle_cn (){ return max_fd.counter; }
	int active_cn () { return ready_fds.size (); }
protected:
	int epfd;
	int listen_fd;
	atomic_t max_fd;

	struct epoll_event *ep_events;
	queue<int> ready_fds; 
};

extern Epoll_handle *g_handle;
#endif

