#ifndef _TCPSERV_H_20081120
#define _TCPSERV_H_20081120

#include <string>
#include <cstring>
#include <cassert>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>

#include "log.h"

using namespace std;

class C_TcpServ 
{
public:
    C_TcpServ (std::string, uint16_t);
    ~C_TcpServ ();

    int socket_listen ();
    int tcp_accept ();
    int tcp_recv (int sockfd, int total, pair<uint8_t*, int> &);
    int tcp_send (int sockfd, const pair<uint8_t*, int> &);
    
    int get_listenfd () const 
    {
        return listenfd;
    }

private:
    int			listenfd;
    string      ip;
    uint16_t	port;
};


#endif
