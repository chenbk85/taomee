/**
 * @file server.cpp
 * @brief demo
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2011-04-25
 */
#include "newbench.h"
#include "proto.h"
#include "cmd_handler.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

static char gp_outbuf[PROTO_MAX_LENGTH];

extern "C" int handle_dispatch(const char *p_recv,
                               int recv_len,
                               int proc,
                               int *p_key)
{
    *p_key = reinterpret_cast<const proto_header_t *>(p_recv)->user_id;
    return 1;
}

extern "C" int handle_open(char **pp_send, int *p_send_len, skinfo_t *p_skinfo)
{
    struct in_addr addr;
    addr.s_addr = static_cast<in_addr_t>(p_skinfo->remote_ip);
    DEBUG_LOG("connect by client: %s:%d",
                inet_ntoa(addr), p_skinfo->remote_port);

    p_skinfo->storage.u64 = p_skinfo->sockfd;
    return 0;
}

extern "C" int handle_close(skinfo_t *p_skinfo)
{
    struct in_addr addr;
    addr.s_addr = static_cast<in_addr_t>(p_skinfo->remote_ip);
    DEBUG_LOG("close: %s:%d",
              inet_ntoa(addr),
              p_skinfo->remote_port);
    return 0;
}

extern "C" int handle_init(int argc, char **argv, int pid)
{
    if (PROC_MAIN == pid)
    {
        DEBUG_LOG("handle_init for process main[%d]", getpid());
    }
    else if (PROC_WORK == pid)
    {
        DEBUG_LOG("handle_init for process work[%d]", getpid());
    }
    else if (PROC_CONN == pid)
    {
        DEBUG_LOG("handle_init for process work[%d]", getpid());
    }
    else
    {
        DEBUG_LOG("who[%d] am i?", getpid());
    }

    return 0;
}

extern "C" int handle_input(const char *p_recv,
                            int recv_len,
                            char **pp_send,
                            int *p_send_len,
                            skinfo_t *p_skinfo)
{
    //*pp_send = gp_outbuf;
    //strcpy(gp_outbuf, "welcome here!");
    //*p_send_len = strlen(gp_outbuf);
    //return 0;

    if (static_cast<size_t>(recv_len) < sizeof(proto_header_t))
    {
        return 0;
    }

    const proto_header_t *p_proto_header
        = reinterpret_cast<const proto_header_t *>(p_recv);

    if (recv_len < p_proto_header->length)
    {
        DEBUG_LOG("package respect %d bytes, not enough & go on",
                  p_proto_header->length);
        return 0;
    }

    if (p_proto_header->length > PROTO_MAX_LENGTH)
    {
        ERROR_LOG("super big packet length");
        return -1;
    }

    return p_proto_header->length;
}

extern "C" int handle_process(const char *p_recv,
                              int recv_len,
                              char **pp_send,
                              int *p_send_len,
                              skinfo_t * /* p_skinfo */)
{
    *pp_send = gp_outbuf;
    int cmd = reinterpret_cast<const proto_header_t *>(p_recv)->cmd;
    int user_id = reinterpret_cast<const proto_header_t *>(p_recv)->user_id;
    int body_len = recv_len - sizeof(proto_header_t);
    int send_len = sizeof(gp_outbuf) - sizeof(proto_header_t);

    DEBUG_LOG("--------------------------[pid:%d][key:%d]", getpid(), user_id);

    int ret = dispatch(cmd,
                       user_id,
                       p_recv + sizeof(proto_header_t),
                       body_len,
                       gp_outbuf + sizeof(proto_header_t),
                       &send_len);

    proto_header_t *p_ack_header
        = reinterpret_cast<proto_header_t *>(gp_outbuf);
    p_ack_header->status = ret;

    if (0 == ret && send_len > 0)
    {
        *p_send_len = p_ack_header->length = sizeof(proto_header_t) + send_len;
    }
    else
    {
        *p_send_len = p_ack_header->length = sizeof(proto_header_t);
    }

    DEBUG_LOG("finish handle_process and return *p_send_len=%d", *p_send_len);

    return 0;
}

extern "C" int handle_schedule()
{
    return 0;
}

extern "C" int handle_fini(int pid)
{
    if (PROC_MAIN == pid)
    {
        DEBUG_LOG("handle_finit for process main[%d]", getpid());
    }
    else if (PROC_WORK == pid)
    {
        DEBUG_LOG("handle_finit for process work[%d]", getpid());
    }
    else if (PROC_CONN == pid)
    {
        DEBUG_LOG("handle_finit for process work[%d]", getpid());
    }
    else
    {
        DEBUG_LOG("who[%d] am i?", getpid());
    }

    return 0;
}
