#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include "CEpoll.h"

using namespace std;

#define DV_IP_DATABASES "dat/DVIP.dat"
const char* socket_addr_int2str(uint32_t _ui32Addr, char *_sAddr=NULL);
const char* socket_addr_int2str(uint32_t _ui32Addr, char *_sAddr)
{
    struct in_addr addr;
    const char *sAddr;

    addr.s_addr = htonl(_ui32Addr);
    sAddr =  inet_ntoa(addr);
    if(_sAddr)
        strcpy(_sAddr, sAddr);

    return sAddr;
}

C_Epoll::C_Epoll (string i, uint16_t p) : C_TcpServ (i, p), fd_vec (MAX_EVENT_SIZE)
{
	data.first = new uint8_t[MAX_RECV_SIZE];
	data.second = 0;

	evs = new struct epoll_event[MAX_EVENT_SIZE];
	maxfd = 0;
	fdcnt = 0;
}

C_Epoll::~C_Epoll ()
{
	delete [] data.first;
	delete [] evs;
}

int C_Epoll::epoll_init ()
{
    // ‘ÿ»ÎIPø‚
    fstream db_file;
    db_file.open(DV_IP_DATABASES, ios::in|ios::out);
    if (!db_file.is_open())
    {
        ERROR_LOG("open db file errorr!!!");
        return -1;
    }

    char line[256];
    uint32_t b;
    pair<uint32_t, uint32_t> item;
    while (!db_file.eof())
    {
        db_file.getline(line, sizeof(line));
        sscanf(line, "%u%u%u", &item.first, &b, &item.second);
        ip_table.insert(item);
    }
    
    /*for (map<uint32_t, uint32_t>::iterator it=ip_table.begin(); it!=ip_table.end(); it++)
    {
        DEBUG_LOG("===%u===%u", it->first, it->second);
    }*/

    DEBUG_LOG("===<%u>====", ip_table.size());
	if ((epfd = epoll_create(MAX_EVENT_SIZE)) == -1)
    {
		ERROR_LOG ("epoll_create failed : %s", strerror(errno));
		return -1;
	}

	if (socket_listen () == -1)
        return -1;
	int listenfd = get_listenfd ();

	do_add_conn (listenfd, LISTEN_TYPE_FD);

	return 0;
}

int C_Epoll::epoll_proc ()
{
	int nfds;
    while (1)
    {
		nfds = epoll_wait (epfd, evs, MAX_EVENT_SIZE, TIME_OUT);
		if (nfds < 0 && errno != EINTR)
        {
			ERROR_LOG ("epoll_wait failed : %s", strerror(errno));
			return -1;
		}

        for (int i=0; i<nfds; ++i) 
        {
			int fd = evs[i].data.fd;
			if (fd > maxfd || fd_vec[fd].sockfd != fd 
                || fd_vec[fd].type == UNUSED_TYPE_FD || fdcnt > MAX_EVENT_SIZE ) 
            {
				ERROR_LOG ("delayed epoll events : fd=%d, cache fd=%d, type=%d, maxfd=%d, fdcnt=%d"
                    , fd, fd_vec[fd].sockfd, fd_vec[fd].type, maxfd, fdcnt);
				continue;
			}

			if (evs[i].events & EPOLLIN) 
            {
				if (fd_vec[fd].type == LISTEN_TYPE_FD)
					do_open_conn (fd);
				else 
                {
					if (net_recv (fd) == -1)
						do_del_conn (fd);
					else
						proc_recv (fd);
				}
			}

			if (evs[i].events & EPOLLOUT) 
            {
			}

            if (evs[i].events & EPOLLHUP) 
            {
				do_del_conn (fd);
			}
		}
		
        // DEBUG_LOG("hear begin check ip !!!!");

        if (is_time_reach())
			ip_map.clear ();
	}

	return 0;
}

bool C_Epoll::is_time_reach () const
{
	struct tm *tt;

	time_t t = time(NULL);
	tt = localtime(&t);

	if (((tt->tm_hour==1)&&(tt->tm_min>=55&&tt->tm_min<60)) 
        || ((tt->tm_hour==2)&&(tt->tm_min>=0&&tt->tm_min<=5)))
		return true;

	return false;
}

int C_Epoll::do_open_conn (int fd)
{
	int newfd;

	newfd = tcp_accept ();
	if (newfd > 0) 
		do_add_conn (newfd, REMOTE_TYPE_FD);

	return newfd;
}

int C_Epoll::do_add_conn (int fd, E_FdType type)
{
	uint32_t flag = EPOLLIN | EPOLLET;

	if (add_events (fd, flag) == -1)
		return -1;

	fd_vec[fd].sockfd = fd;
	fd_vec[fd].type = type;

	maxfd = maxfd > fd ? maxfd : fd;
	++fdcnt;

	return 0;
}

int C_Epoll::add_events (int fd, uint32_t flag)
{
	struct epoll_event ev;

	ev.events = flag;
	ev.data.fd = fd;
	
epoll_add_again:
	if (epoll_ctl (epfd, EPOLL_CTL_ADD, fd, &ev) != 0 ) {
		if (errno == EINTR)
			goto epoll_add_again;
		ERROR_LOG ("epoll_ctl add=%d, error : %s", fd, std::strerror(errno));
		return -1;
	}

	return 0;
}


void C_Epoll::do_del_conn (int fd)
{
	// DEBUG_LOG ("Del fd=%d", fd);
	if (fd_vec[fd].type == UNUSED_TYPE_FD)
		return ;

	fd_vec[fd].type = UNUSED_TYPE_FD;

	//must
	close(fd);

	if (maxfd == fd) {
		int i;
		for (i = fd-1; i>=0; --i)
			if (fd_vec[i].type != UNUSED_TYPE_FD)
				break;
		maxfd = i;
	}
	if (fdcnt>0) --fdcnt;
}


int C_Epoll::net_recv (int fd)
{
	return tcp_recv (fd, 4, data);
}



int C_Epoll::proc_recv (int fd)
{
	if (data.second != 4) 
    {
		ERROR_LOG ("Error recv pkg ! recv len=%d", data.second);
		return -1;
	}

    map<uint32_t, uint32_t>::iterator it=ip_table.lower_bound(*(uint32_t*)data.first);
    if (it==ip_table.begin()||it==ip_table.end())
    {
        DEBUG_LOG("<%s><0.0.0.0><0>", socket_addr_int2str(*(uint32_t*)data.first));
        std::pair<uint8_t*, int> snd;
        uint32_t a=0;
        snd.first = (uint8_t*)&a;
        snd.second = 4;
	    return tcp_send (fd, snd);
    }
    it--;
    
    char ip1[16];
    char ip2[16];
    socket_addr_int2str(*(uint32_t*)data.first, ip1);
    socket_addr_int2str(it->first, ip2);
    DEBUG_LOG("<%u><%u>:<%s><%s>:<%u>", *(uint32_t*)data.first, it->first, ip1, ip2, it->second);
	std::pair<uint8_t*, int> snd;
	snd.first = (uint8_t*)&it->second;
	snd.second = 4;
	return tcp_send (fd, snd);
}

