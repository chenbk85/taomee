/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file test_server.cpp
 * @author richard <richard@taomee.com>
 * @date 2011-07-11
 */

#include <string.h>

#include "log.h"
#include "newbench_so.h"
#include "setproctitle.h"

char send_buff[8172] = {0};

int handle_init(int argc, char **argv, int proc_type)
{
    if (proc_type == PROC_MAIN) {
        DEBUG_LOG("handle_init: PROC_MAIN");
        setproctitle("test_server: MAIN");
        return 0;
    } else if (proc_type == PROC_CONN) {
        DEBUG_LOG("handle_init: PROC_CONN");
        setproctitle("test_server: CONN");
        return 0;
    } else if (proc_type == PROC_WORK) {
        DEBUG_LOG("handle_init: PROC_WORK");
        setproctitle("test_server: WORK");
        return 0;
    } else {
        ERROR_LOG("handle_init: unknown");
        return -1;
    }
}

int handle_dispatch(const char* p_buf, int buf_len, int proc_num, int* p_key)
{
    DEBUG_LOG("handle_dispatch");

    return 0;
}

int handle_input(const char *p_recv, int recv_len, char **pp_send, int *p_send_len, skinfo_t *p_skinfo)
{
    DEBUG_LOG("handle_input: %d", recv_len);
    
    if (recv_len >= (int)sizeof(uint32_t)) {
        int msg_len = (int)*(uint32_t *)p_recv;
        DEBUG_LOG("handle_input: recv_len: %d msg_len: %u", recv_len, msg_len);
        if (recv_len >= msg_len) {
            DEBUG_LOG("handle_input: recv a whole %dB msg", msg_len);
            return msg_len;
        }
    }
   
    DEBUG_LOG("handle_input: recv_len: %d: continue to recv", recv_len);

    return 0;
}

int handle_open(char **, int *, skinfo_t *p_skinfo)
{
    DEBUG_LOG("handle_open");
    
    return 0;
}

int handle_close(const skinfo_t *p_skinfo)
{
    DEBUG_LOG("handle_close");
    
    return 0;
}

void handle_fini(int proc_type)
{
    DEBUG_LOG("handle_fini");
}

int handle_timer(int *p_sec)
{
//    DEBUG_LOG("handle_timer: %d", *p_sec);
//    *p_sec = 1;
    
    return 0;
}

int handle_process(char *p_recv, int recv_len, char **pp_send, int *p_send_len, skinfo_t *p_skinfo)
{
    DEBUG_LOG("handle_process: %d:%s", recv_len, p_recv + sizeof(uint32_t));

    memcpy(send_buff, p_recv, recv_len);
    *pp_send = p_recv;
    *p_send_len = recv_len;
    
    return 0;
}

int handle_schedule()
{
//    DEBUG_LOG("handle_schedule");
    
    return 0;
}
