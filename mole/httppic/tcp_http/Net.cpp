#include "Thread_Main.h"
#include "Net.h"
#include "Log.h"
#include "Options.h"
#include "myepoll.h"
#include "Common.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>

pthread_mutex_t clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;

Epoll_handle::Epoll_handle ()
{
	epfd = epoll_create (ini.max_fds);
	ep_events = (epoll_event*) calloc (ini.max_fds, sizeof (epoll_event));	

	if ( epfd < 0 || ep_events == NULL)
	{
		fprintf (stderr, "epoll_create failed\n");
		exit (-1);
	}

	max_fd.counter = 0;
}

Epoll_handle::~Epoll_handle ()
{
	free (ep_events);
	close (epfd);
	close (listen_fd);
}

int Epoll_handle::add_epoll_events (int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl (epfd, EPOLL_CTL_ADD, fd, &ev) == 0)
		atomic_inc (&max_fd);
	else 
	{
		LOG (ERROR, "epoll_ctl add %d error,%s", fd, strerror(errno));
		return -1;
	}

	return 0; 
}

int Epoll_handle::del_epoll_events (int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl (epfd, EPOLL_CTL_DEL, fd, &ev) == 0)
		atomic_dec (&max_fd);
	else
	{
		LOG (ERROR, "epoll_ctl del %d error,%s", fd, strerror(errno));
		return -1;
	}

	return 0;
}

void Epoll_handle::wait_ready ()
{
	int ready_count = epoll_wait (epfd, ep_events, max_fd.counter + 1, -1);	
	if (ready_count < 0)
	{
		LOG (ERROR, "epoll_wait return %d,%s", ready_count, strerror(errno));
		return;
	}
	LOG (TRACE, "epoll_wait ready=%d,max_fd=%d", ready_count, max_fd.counter);
	
	pthread_mutex_lock (&clifd_mutex);
	for (int i = 0; i < ready_count; i++)
	{
		if (ep_events[i].data.fd  == listen_fd)
			accept_cn ();
		else {
			del_epoll_events (ep_events[i].data.fd);
			if (ep_events[i].events & (EPOLLHUP | EPOLLERR))
				close (ep_events[i].data.fd);
			else
				ready_fds.push (ep_events[i].data.fd);
		}
	}
 
	if (!ready_fds.empty ())
		pthread_cond_signal (&clifd_cond);
	pthread_mutex_unlock (&clifd_mutex);
}

int Epoll_handle::child_wait(int *readable_fd)
{
	pthread_mutex_lock (&clifd_mutex);
	while (ready_fds.empty ())
		pthread_cond_wait (&clifd_cond, &clifd_mutex);

	int cur_fd = ready_fds.front ();
	LOG (TRACE, "child_wait ready fd %d", cur_fd);
	ready_fds.pop ();
	pthread_mutex_unlock (&clifd_mutex);

	*readable_fd = 0;
	*readable_fd = cur_fd;

	return 0;
}

int Epoll_handle::open ()
{
	struct sockaddr_in servaddr;
 	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (ini.bind_port);
	if (strcmp (ini.bind_ip, "*.*.*.*") == 0)
		servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	else
		inet_pton(AF_INET, ini.bind_ip, &servaddr.sin_addr);

	if ((listen_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		fprintf (stderr, "socket error:%s\n",strerror(errno)); 
		return -1;
	} 
	int reuse_addr = 1;
	setsockopt (listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);

	if (bind (listen_fd, (struct sockaddr *)(&servaddr), sizeof(struct sockaddr)) == -1) 
	{ 
		fprintf (stderr, "bind %s:%d error:%s\n", ini.bind_ip, ini.bind_port, strerror(errno)); 
		return -1;
	} 
	
	if (listen (listen_fd, ACCEPT_BACKLOG) == -1) 
	{ 
		fprintf (stderr, "listen error:%s\n",strerror(errno)); 
		return -1;
	} 

	add_epoll_events (listen_fd);	
	printf ("open tcp %s:%d\t[ok]\n", ini.bind_ip, ini.bind_port);
	return 0;
}

int Epoll_handle::accept_cn ()
{
	struct sockaddr_in peer;
	socklen_t peer_size = 0;
	int newfd;

	for ( ; ; ) 
	{
		peer_size = sizeof(struct sockaddr_in); 
		if ((newfd = accept(listen_fd, (struct sockaddr *)&peer, &peer_size)) < 0)
		{
			if (errno == EINTR)
				continue;         /* back to for () */

			LOG (ERROR,"accept failed,listen_fd=%d", listen_fd);
			return -1;
		}
		break;
	}

	//set socket buffer
//	int bufsize = 65535;
//	setsockopt (newfd, SOL_SOCKET, SO_RCVBUF, (char *) bufsize, sizeof (int));
//	setsockopt (newfd, SOL_SOCKET, SO_SNDBUF, (char *) bufsize, sizeof (int));

	add_epoll_events (newfd);	
	LOG (TRACE, "accept connection,fd=%d,ip=%s,port=%d", 
		newfd, inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));

	app_accept (newfd);
	return 0;
}

