#ifndef _EPOLL_H_20081120
#define _EPOLL_H_20081120

#include <map>
#include <vector>
#include <ctime>
#include <sys/epoll.h>
#include <stdint.h>
#include "log.h"
#include "CTcpServ.h"


#define MAX_RECV_SIZE	1024
#define MAX_EVENT_SIZE	100
#define MAX_IP_SIZE		1000000
#define TIME_OUT		1

class C_Epoll : public C_TcpServ 
{
private:
    enum E_FdType
    {
        UNUSED_TYPE_FD = 0
        , LISTEN_TYPE_FD = 1
        , REMOTE_TYPE_FD = 2
    };

    struct T_FdInfo
    {
        E_FdType type;
        int		 sockfd;
        T_FdInfo () 
        { 
            type = UNUSED_TYPE_FD; 
        }
    };

public:
    C_Epoll (std::string, uint16_t);
    virtual ~C_Epoll ();

    int epoll_init ();
    int epoll_proc ();
    int proc_recv (int);
    
private:

    int	epfd;
    int	maxfd;
    uint32_t fdcnt;
    struct epoll_event *evs;
    vector<T_FdInfo> fd_vec;
    pair<uint8_t*, int> data;
    map<uint32_t, uint32_t> ip_map;
    map<uint32_t, uint32_t> ip_table;
private:
    bool is_time_reach () const;
    int do_open_conn (int);
    int do_add_conn (int, E_FdType);
    void do_del_conn (int);
    int net_recv (int);
    int add_events (int fd, uint32_t flag);
};

#endif

