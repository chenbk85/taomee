/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file test_client_1.cpp
 * @author richard <richard@taomee.com>
 * @date 2011-07-22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "i_event.h"

typedef struct {
    int fd;
    bool connected;
    char recv_buff[4096];
    uint32_t recv_buff_used_len;
    char send_buff[4096];
    uint32_t send_buff_used_len;
    i_event::event_item_t *p_event_item;
} sock_info_t;

static i_event *g_p_event = NULL;
static sock_info_t g_sock_info = {0};

int recv_data(sock_info_t *p_sock_info)
{
    int rv = read(p_sock_info->fd, p_sock_info->recv_buff + p_sock_info->recv_buff_used_len, 
                sizeof(p_sock_info->recv_buff) - p_sock_info->recv_buff_used_len);

    if (rv < 0) {
        fprintf(stderr, "ERROR: read: %s\n", strerror(errno));
        close(p_sock_info->fd);
        if (p_sock_info->p_event_item != NULL) {
            g_p_event->del(p_sock_info->p_event_item);
            p_sock_info->p_event_item = NULL;
        }
    } else if (rv == 0) {
        fprintf(stdout, "DEBUG: connection closed by peer party\n");
        close(p_sock_info->fd);
        if (p_sock_info->p_event_item != NULL) {
            g_p_event->del(p_sock_info->p_event_item);
        }
    } else {
        p_sock_info->recv_buff_used_len += rv;
        while (p_sock_info->recv_buff_used_len >=4) {
            uint32_t rqst_msg_len = *(uint32_t *)p_sock_info->recv_buff;
            if (rqst_msg_len > p_sock_info->recv_buff_used_len) {
                // 继续等待
                break;
            } else {
                // 完整的消息
                fprintf(stdout, "DEBUG: recv a msg\n");
                p_sock_info->recv_buff_used_len -= rqst_msg_len;
            }
        }
    }
    
    return 0;
}

int send_data(sock_info_t *p_sock_info, const char *p_data, int data_len)
{
    if (p_sock_info->send_buff_used_len == 0) {            // new msg
        *(uint32_t *)(p_sock_info->send_buff) = 40;
        p_sock_info->send_buff_used_len = 40;
        fprintf(stdout, "DEBUG: send a msg\n");
    }

    while (p_sock_info->send_buff_used_len > 0) {
        int rv = write(p_sock_info->fd, p_sock_info->send_buff, p_sock_info->send_buff_used_len);
        if (rv > 0) {
            memmove(p_sock_info->send_buff, p_sock_info->send_buff + rv, 
                        p_sock_info->send_buff_used_len - rv);
            p_sock_info->send_buff_used_len -= rv;
        } else if (rv == -1 && errno == EINTR) {
            continue;
        } else if (rv == -1 && errno == EAGAIN) {
            break;
        } else {
            fprintf(stderr, "ERROR: write: %s\n", strerror(errno));
            close(p_sock_info->fd);
            if (p_sock_info->p_event_item != NULL) {
                g_p_event->del(p_sock_info->p_event_item);
            }
            return -1;
        }
    }
    
    return 0;
}

void do_io(int fd, i_event::event_type_t event_type, void *p_user_data)
{
    fprintf(stdout, "DEBUG: do_io\n");
    sock_info_t *p_sock_info = (sock_info_t *)p_user_data;
    
    if (event_type == i_event::EVENT_READ) {
        recv_data(p_sock_info);
    } else if (event_type == i_event::EVENT_WRITE) {
        if (!p_sock_info->connected) {
            int opt = -1;
            socklen_t opt_len = sizeof(opt);
            if (getsockopt(p_sock_info->fd, SOL_SOCKET, SO_ERROR, &opt, &opt_len) == -1) {
                fprintf(stderr, "ERROR: getsockopt: %s\n", strerror(errno));
            }
            if (opt != 0) {
                fprintf(stderr, "ERROR: connect: %s\n", strerror(opt));
            }

            p_sock_info->connected = true;
            fprintf(stdout, "DEBUG: connect: true\n");
        }

        send_data(p_sock_info, NULL, 0);
    } else {
        fprintf(stderr, "ERROR: do_io: %s\n", strerror(errno));
        close(p_sock_info->fd);
        if (p_sock_info->p_event_item != NULL) {
            g_p_event->del(p_sock_info->p_event_item);
        }
    }
}

int main(int argc, char **argv)
{
    int flags = -1, rv = -1;
    struct sockaddr_in serv_addr;
    
    g_p_event = create_event_instance();
    if (g_p_event == NULL) {
        fprintf(stderr, "ERROR: create_event_instance");
        return -1;
    }

    if (g_p_event->init() != 0) {
        fprintf(stderr, "ERROR: create_event_instance");
        return -1;
    }

    g_sock_info.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_sock_info.fd == -1) {
        fprintf(stderr, "ERROR: socket: %s\n", strerror(errno));
        goto error;
    }

    flags = fcntl(g_sock_info.fd, F_GETFL);
    if (flags == -1) {
        fprintf(stderr, "ERROR: fcntl: %s\n", strerror(errno));
        goto error;
    }
    if (fcntl(g_sock_info.fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        fprintf(stderr, "ERROR: fcntl: %s\n", strerror(errno));
        goto error;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) != 1) {
        fprintf(stderr, "ERROR: inet_pton %s: %s\n", "127.0.0.1", strerror(errno));
        goto error;
    }
    serv_addr.sin_port = htons(4444);

    for (;;) {
        rv = connect(g_sock_info.fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (rv == 0) {
            g_sock_info.connected = true;
            fprintf(stdout, "DEBUG: connect: true\n");
            break;
        } else if (rv == -1 && errno == EAGAIN) {
            continue;
        } else if (rv == -1 && errno == EINPROGRESS) {
            break;
        } else {
            fprintf(stderr, "ERROR: connect: %s\n", strerror(errno));
            goto error;
        }
    }

    g_sock_info.p_event_item = g_p_event->add(g_sock_info.fd, i_event::EVENT_RDWT, do_io, &g_sock_info);
    if (g_sock_info.p_event_item == NULL) {
        fprintf(stderr, "ERROR: g_p_event->add");
        return -1;
    }

    fprintf(stdout, "DEBUG: begin\n");

    for (;;) {
        if (g_p_event->check(1) != 0) {
            fprintf(stderr, "ERROR: g_p_event->check");
            return -1;
        }
    }

    fprintf(stdout, "DEBUG: end\n");

    if (g_p_event->del(g_sock_info.p_event_item) != 0) {
        fprintf(stderr, "ERROR: g_p_event->del");
        return -1;
    }

    if (g_p_event->uninit() != 0) {
        fprintf(stderr, "ERROR: create_event_instance");
        return -1;
    }

    if (g_p_event->release() != 0) {
        fprintf(stderr, "ERROR: g_p_event->release");
        return -1;
    }
    
error: 
    if (g_sock_info.fd != -1) {
        close(g_sock_info.fd);
        g_sock_info.fd = -1;
    }

    return -1;
}
