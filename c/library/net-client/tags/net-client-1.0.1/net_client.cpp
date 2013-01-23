/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file net_client.cpp
 * @author richard <richard@taomee.com>
 * @date 2011-07-14
 */

#include <stdio.h>
#include <string.h>
#include <new>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "net_client.h"

std::shared_ptr<i_net_client> create_net_client_instance()
{
    return std::shared_ptr<i_net_client>(new (std::nothrow)net_client());
}

net_client::net_client() : m_inited(false)
{
}

net_client::~net_client()
{
}

i_net_client::~i_net_client()
{
}

int net_client::init(const char *server_ip, int server_port)
{
    if (m_inited) {
        snprintf(m_error_str, sizeof(m_error_str), "alreay inited");
        return -1; 
    }   

    if (server_ip == NULL || server_port <0 || server_port > 65535) {
        snprintf(m_error_str, sizeof(m_error_str), "parameter error");
        return -1; 
    }   

    memset(m_server_ip, 0, sizeof(m_server_ip));
    strncpy(m_server_ip, server_ip, sizeof(m_server_ip) - 1);
    m_server_port = server_port;
    m_sockfd = -1;

    m_inited = true;

    return 0;
}

int net_client::open_conn(struct timeval *timeout)
{
    if (timeout == NULL) {
        snprintf(m_error_str, sizeof(m_error_str), "parameter error");
        return -1;
    }

    int flags = -1, rv = -1;
    struct sockaddr_in serv_addr = {0};
    memset(&serv_addr, 0, sizeof(serv_addr));
    fd_set write_set; 
    memset(&write_set, 0, sizeof(write_set));
    fd_set error_set; 
    memset(&error_set, 0, sizeof(error_set));

    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) {
        snprintf(m_error_str, sizeof(m_error_str), "socket: %s", strerror(errno));
        goto error;
    }

    flags = fcntl(m_sockfd, F_GETFL);
    if (flags == -1) {
        snprintf(m_error_str, sizeof(m_error_str), "fcntl: %s", strerror(errno));
        goto error;
    }
    if (fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        snprintf(m_error_str, sizeof(m_error_str), "fcntl: %s", strerror(errno));
        goto error;
    }

    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, m_server_ip, &serv_addr.sin_addr) != 1) {
        snprintf(m_error_str, sizeof(m_error_str), "inet_pton %s: %s", 
                    m_server_ip, strerror(errno));
        goto error;
    }
    serv_addr.sin_port = htons(m_server_port);

    rv = connect(m_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (rv == 0) {
        return 0;
    } else if (rv == -1 && errno != EINPROGRESS) {
        snprintf(m_error_str, sizeof(m_error_str), "connect: %s", strerror(errno));
        goto error;
    }


    FD_SET(m_sockfd, &write_set);
    FD_SET(m_sockfd, &error_set);

    rv = select(m_sockfd + 1, NULL, &write_set, &error_set, timeout);
    if (rv < 0) {
        snprintf(m_error_str, sizeof(m_error_str), "select: %s", strerror(errno));
        goto error;
    } else if (rv == 0) {
        snprintf(m_error_str, sizeof(m_error_str), "select timeout");
        goto error;
    }

    // rv > 0
    if (FD_ISSET(m_sockfd, &error_set)) {
        snprintf(m_error_str, sizeof(m_error_str), "select error_set: %s", strerror(errno));
        goto error;
    }
    if (FD_ISSET(m_sockfd, &write_set)) {
        int opt = -1;
        socklen_t opt_len = sizeof(opt);
        if (getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &opt, &opt_len) == -1) {
            snprintf(m_error_str, sizeof(m_error_str), "getsockopt: %s", strerror(errno));
            goto error;
        }
        if (opt != 0) {
            snprintf(m_error_str, sizeof(m_error_str), "connect: %s", strerror(opt));
            goto error;
        }
    }

    return 0;

error: 
    if (m_sockfd != -1) {
        close(m_sockfd);
        m_sockfd = -1;
    }

    return -1;
}

int net_client::send_rqst(const char *rqst_msg, char *resp_buf, int buf_len, 
                          struct timeval *timeout)
{
    int rv = -1;
    int rqst_msg_rmng_len = *(uint32_t *)rqst_msg;
    int resp_buf_used_len = 0;

    for (;;) {
        fd_set read_set = {{0}}; 
        fd_set write_set = {{0}}; 
        fd_set error_set = {{0}}; 

        if (resp_buf_used_len >= buf_len) {
            snprintf(m_error_str, sizeof(m_error_str), "resp_buf_used_len: %d >= buf_len: %d", resp_buf_used_len, buf_len);
            return-1;
        }
        FD_SET(m_sockfd, &read_set);
        if (rqst_msg_rmng_len > 0) {
            FD_SET(m_sockfd, &write_set);
        }
        FD_SET(m_sockfd, &error_set);

        rv = select(m_sockfd + 1, &read_set, &write_set, &error_set, timeout);
        if (rv < 0) {
            snprintf(m_error_str, sizeof(m_error_str), "select: %s", strerror(errno));
            return -1;
        } else if (rv == 0) {
            snprintf(m_error_str, sizeof(m_error_str), "select timeout");
            return -1;
        }

        // rv > 0
        if (FD_ISSET(m_sockfd, &write_set)) {
            if (rqst_msg_rmng_len > 0) {
                rv = write(m_sockfd, rqst_msg, rqst_msg_rmng_len);
                if (rv == -1) {
                    snprintf(m_error_str, sizeof(m_error_str), "write: %s", strerror(errno));
                    return -1;
                }

                rqst_msg += rv;
                rqst_msg_rmng_len -= rv;
            }
        }
        if (FD_ISSET(m_sockfd, &read_set)) {
            rv = read(m_sockfd, resp_buf + resp_buf_used_len, buf_len - resp_buf_used_len);
            if (rv == -1) {
                snprintf(m_error_str, sizeof(m_error_str), "read: %s", strerror(errno));
                return -1;
            } else if (rv == 0) {
                snprintf(m_error_str, sizeof(m_error_str), "connection closed by other party");
                return -1;
            } else {
                resp_buf_used_len += rv;
                if (resp_buf_used_len >= (int)sizeof(uint32_t) && 
                            resp_buf_used_len >= (int)*(uint32_t *)resp_buf) {
                    break;                   // 收到完整的回复消息，退出for循环 
                }
            }
        }
        if (FD_ISSET(m_sockfd, &error_set)) {
            snprintf(m_error_str, sizeof(m_error_str), "select error_set: %s", strerror(errno));
            return -1;
        }
    }

    return 0;
}

int net_client::close_conn()
{
    if (m_sockfd != -1) {
        close(m_sockfd);
        m_sockfd = -1;
    }

    return 0;
}

const char * net_client::get_last_error()
{
    return m_error_str;
}

int net_client::uninit()
{
    if (!m_inited) {
        return -1;
    }

    return 0;
}
