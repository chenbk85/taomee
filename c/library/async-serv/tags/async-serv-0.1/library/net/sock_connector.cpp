/**
 * @file sock_connector.cpp
 * @brief socket连接封装类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-08
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "sock_connector.h"
#include "sock.h"
#include "sock_stream.h"

int sock_connector::open_connecting()
{
#ifdef _IPV6
    if (get_fd() < 0 && open(AF_INET6, SOCK_STREAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return CONN_ERR;
    }
#else
    if (get_fd() < 0 && open(AF_INET, SOCK_STREAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return CONN_ERR;
    }
#endif

    int fd = -1;
    if ((fd = get_fd()) < 0) {
        ERROR_LOG("sock::get_fd failed");
        return CONN_ERR;
    }

    ///设置非阻塞模式
    set_nonblocking();

    int rt = ::connect(fd,
                       (struct sockaddr*)(m_serv_addr_.get_sockaddr()),
                       m_serv_addr_.get_sockaddr_len());
    if (rt == 0) {
        return CONN_COMPL;
    } else {
        switch(errno) {
            case 0:
            case EISCONN:
                return CONN_COMPL;
            case EINPROGRESS:
                return IS_CONNECTING;
            case EINTR:
            case EAGAIN:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case ECONNREFUSED:
            case ENETUNREACH:
                DEBUG_LOG("connect temperally unavailable: %s", ::strerror(errno));
                close();
                return CONN_TEMP_UNAVAIL;
            default:
                ERROR_LOG("connect failed, err: %s", ::strerror(errno));
                close();
                return CONN_ERR;
        }
    }
}


