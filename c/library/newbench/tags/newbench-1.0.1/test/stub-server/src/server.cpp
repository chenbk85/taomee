#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <string>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "newbench.h"
#include "do_handle.h"

/// echo 接口
#define ECHO_CMD_ID        0x1001

typedef struct {
    char value[256];
}__attribute__((packed)) mb_echo_req_t;

typedef struct {
    char value[256];
}__attribute__((packed)) mb_echo_ack_t;

/// 命令处理函数
uint32_t do_echo(uint32_t user_id, const char *recv_buf, int recv_len,
        char *ack_buf, int *ack_len, skinfo_t *sk)
{
    if ((u_int)recv_len < sizeof(mb_echo_req_t)) {
        ERROR_LOG("Invalid param len(%d), expect %d", recv_len,
                sizeof(mb_echo_req_t));
        return 101;
    }

    mb_echo_req_t* req = (mb_echo_req_t*)recv_buf;
    mb_echo_ack_t* ack = (mb_echo_ack_t*)ack_buf;
    memcpy(ack->value, req->value, sizeof(req->value));
    *ack_len = sizeof(mb_echo_ack_t);
    return 0;
}

int do_broadcast()
{
    const char *welcome = "欢迎你的到来";
    send_data(-1, welcome, strlen(welcome), 1, 1);
    return 0;
}


/// filter 测试接口
#define TEST_FILTER_CMD_ID        0x1002

typedef struct {
    int need_filter;
    int sleep_sec;
    char val[32];
}__attribute__((packed)) mb_test_filter_req_t;

typedef struct {
    char val[32];
}__attribute__((packed)) mb_test_filter_ack_t;

extern "C" int handle_dispatch(const char *recv_buf, int recv_len, int proc_num, int* mimi_id)
{
    DEBUG_LOG("handle dispatch called, proc num %d", proc_num);
    if ((u_int)recv_len < sizeof(common_protocol_header_t)) {
        ERROR_LOG("recv len %u too small, expect %u", recv_len, sizeof(common_protocol_header_t));
        return -1;
    }

    common_protocol_header_t* ph = (common_protocol_header_t*)recv_buf;
    DEBUG_LOG("Handle dispatch called, proc_num %d, user_id %u", proc_num, ph->user_id);
    if (ph->cmd_id == TEST_FILTER_CMD_ID) {
        mb_test_filter_req_t* pb = (mb_test_filter_req_t*)(recv_buf + sizeof(common_protocol_header_t));
        DEBUG_LOG("need_filter: %d", pb->need_filter);
        if (pb->need_filter == 0) {
            DEBUG_LOG("do not filter");
            return 0;
        }

        DEBUG_LOG("do filter");
        *mimi_id = ph->user_id;
        return 1;
    }

    return 0;
}

/// 命令处理函数
uint32_t do_test_filter(uint32_t user_id, const char *recv_buf, int recv_len, char *ack_buf, int *ack_len, skinfo_t *sk)
{
    if ((u_int)recv_len < sizeof(mb_test_filter_req_t)) {
        ERROR_LOG("Invalid param len(%d), expect %d", recv_len,
                sizeof(mb_test_filter_req_t));
        return 101;
    }

    pid_t pid = getpid();
    mb_test_filter_req_t* pb = (mb_test_filter_req_t*)recv_buf;

    if (pb->sleep_sec != 0) {
        DEBUG_LOG("Proc %u begin to say.", pid);
        for (int i = 0; i < pb->sleep_sec; i++) {
            DEBUG_LOG("Proc %u say: %s", pid, pb->val);
            sleep(1);
        }
        DEBUG_LOG("Proc %u complete.", pid);
    }
    else {
        DEBUG_LOG("Proc %u say: %s", pid, pb->val);
    }

    mb_test_filter_ack_t* pa = (mb_test_filter_ack_t*)ack_buf;
    memcpy(pa->val, pb->val, sizeof(pa->val));
    *ack_len = sizeof(pa->val);
    return 0;
}

void dump_argv(int argc, char **argv)
{
    DEBUG_LOG("argc: %d", argc);
    for (int i = 0; i < argc; i++) {
        DEBUG_LOG("argv[%d]: %s", i, argv[i]);
    }
}

void dump_skinfo(const skinfo_t* sk)
{
    DEBUG_LOG("Skinfo, sockfd: %d", sk->sockfd);
    DEBUG_LOG("Skinfo, type: %d", sk->type);
    DEBUG_LOG("Skinfo, recvtm: %lld", sk->recvtm);
    DEBUG_LOG("Skinfo, sendtm: %lld", sk->sendtm);

    in_addr addr;
    addr.s_addr = sk->local_ip;
    DEBUG_LOG("Skinfo, local_ip: %s", inet_ntoa(addr));
    DEBUG_LOG("Skinfo, local_port: %u", sk->local_port);

    addr.s_addr = sk->remote_ip;
    DEBUG_LOG("Skinfo, remote_ip: %s", inet_ntoa(addr));
    DEBUG_LOG("Skinfo, remote_port: %u", sk->remote_port);
}

/// 导出函数
extern "C" int handle_init(int argc, char **argv, int pid)
{
    if (pid == PROC_MAIN) {
        /// 初始化共享内存以保存
        DEBUG_LOG("MAIN %u handle init calledi", getpid());
        dump_argv(argc, argv);
    }
    else if (pid == PROC_WORK) {
        DEBUG_LOG("WORK %u handle init called", getpid());
        dump_argv(argc, argv);
    }
    else if (pid == PROC_CONN) {
        DEBUG_LOG("CONN %u handle init called", getpid());
        dump_argv(argc, argv);
    }
    else {
        ERROR_LOG("Unknown pid: %d", pid);
    }

    command_entry_t entry_arr[10];
    entry_arr[0].cmd_id = ECHO_CMD_ID;
    entry_arr[0].do_cmd = do_echo;
    entry_arr[0].flag = 0;
    entry_arr[1].cmd_id = TEST_FILTER_CMD_ID;
    entry_arr[1].do_cmd = do_test_filter;
    entry_arr[1].flag = 0;
    return do_handle_init(pid, 4096, NULL, 0, entry_arr, 2, NULL);
}

extern "C" int handle_input(const char *recv_buf, int recv_len, char **send_buf, int *send_len, skinfo_t* p_skinfo)
{
    DEBUG_LOG("-----------input connection_id:%d", p_skinfo->connection_id);
    DEBUG_LOG("%u handle input called, len %d", getpid(), recv_len);
    return do_handle_input(recv_buf, recv_len);
}

extern "C" int handle_process(const char *recv_buf, int recv_len, char **send_buf, int *send_len, skinfo_t *sk)
{
    DEBUG_LOG("-----------------close_connection %d", sk->connection_id);
    int ret = do_handle_process(recv_buf, recv_len, send_buf, send_len, sk);
    //if (sk->storage.u32 != (uint32_t)sk->sockfd)
    //{
        //ERROR_LOG("%d != %d", sk->storage.u32, sk->sockfd);
    //}

    //sk->storage.u32 = 7;

    dump_skinfo(sk);
    return ret;
}

extern "C" int handle_schedule()
{
    //do_broadcast();
    //ERROR_LOG("do broadcast");
    return 0;
}

extern "C" int handle_fini(int pid)
{
    if (pid == PROC_MAIN) {
        DEBUG_LOG("MAIN %u handle fini called", getpid());
    }
    else if (pid == PROC_WORK) {
        DEBUG_LOG("WORK %u handle fini called", getpid());
        do_handle_fini();
    }
    else if (pid == PROC_CONN) {
        DEBUG_LOG("CONN %u handle fini called", getpid());
    }
    else {
        ERROR_LOG("Unknown pid_t %d handle_fini called", pid);
    }

    return 0;
}

extern "C" int handle_close(skinfo_t* sk)
{
    DEBUG_LOG("Handle close called");
    dump_skinfo(sk);
    return 0;
}

extern "C" int handle_open(char** sendptr, int* sendlen, skinfo_t* sk)
{
    DEBUG_LOG("Handle open called, sendptr: %p, sendlen: %p", sendptr, sendlen);
    sk->storage.u64 = sk->sockfd;
    dump_skinfo(sk);
    return 0;
}

extern "C" int handle_timer(int* sec)
{
    DEBUG_LOG("Handle timer called, sec: %d", *sec);
    return 0;
}

extern "C" int handle_input_complete(char **pp_send, int *sndlen, skinfo_t *sk)
{
    DEBUG_LOG("-----------------------here");
    return 0;
}
