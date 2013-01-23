/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file newbench_so.h
 * @author richard <richard@taomee.com>
 * @date 2011-07-11
 */

#ifndef NEWBENCH_2011_07_11
#define NEWBENCH_2011_07_11

#include <stdint.h>

enum {
    PROC_MAIN = 0,
    PROC_CONN,
    PROC_WORK,
};

/**
 * @brief 存储小块
 */
union bench_storage {
    uint32_t u32;
    uint64_t u64;
    void *ptr;
};

/**
 * skinfo_t - socket infomation
 */
typedef struct skinfo_struct {
    int sockfd;                                            // socket description
    int type;                                              // sock type SOCK_STREAM, %SOCK_DGRAM, etc
    long long recvtm;
    long long sendtm;

    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;

    union bench_storage storage;
    int connection_id;
    void *ptr_lookout;
} skinfo_t;

/**
 * @brief 初始化(主进程，网络进程，工作进程都会调用)
 * @param argc 参数个数
 * @param argv 指向参数保存的地址
 * @param proc_type 进程类型
 * @return 0-成功 other-失败
 */
extern "C" int handle_init(int argc, char **argv, int proc_type);

/**
 * @brief 用来获取数据包对应KEY
 * @param p_buf 数据包缓冲区
 * @param buf_len 数据包长度
 * @param proc_num 进程数量(暂无意义)
 * @param p_key 指向key地址
 * @return 0-不屏蔽 <0-出错 >0-屏蔽
 */
extern "C" int handle_dispatch(const char* p_buf, int buf_len, int proc_num, int* p_key);

/**
 * @brief 收到数据后进行回调
 * @param p_recv 指向收到的数据缓冲区
 * @param recv_len 收到的数据长度
 * @param pp_send 指向要发送的数据的缓冲区的头指针
 * @param p_send_len 指向要发送数据的缓冲区长度
 * @param p_skinfo 指向连接客户端的信息结构体
 * @return -1-异常，并终止此次会话 0-需要继续接收 >0-数据包应有长度
 */
extern "C" int handle_input(const char *p_recv, int recv_len, char **pp_send, int *p_send_len, 
                            skinfo_t *p_skinfo);

/**
 * @brief 当客户端和服务端建立连接后进行调用
 * @param 暂无意义
 * @param 暂无意义
 * @param p_skinfo 指向连接客户端的信息结构体
 * @return 0-成功，建立连接 -1-失败，断开连接
 */
extern "C" int handle_open(char **, int *, skinfo_t *p_skinfo);

/**
 * @brief 当客户端与服务器断开连接后进行调用
 * @param p_skinfo 指向连接客户端的信息结构体
 * @return 0-成功 -1-失败
 */
extern "C" int handle_close(const skinfo_t *p_skinfo);


/**
 * @brief 进程终止时反初始化调用
 * @param proc_type 进行类型
 */
extern "C" void handle_fini(int proc_type);

extern "C" int handle_timer(int *);

/**
 * @brief 当有完整数据包时给工作进行处理时调用
 * @param p_recv 收到的数据
 * @param recv_len 收到的数据
 * @param pp_send 指向要发送的数据的指针
 * @param p_send_len 指向要发送的数据的长度
 * @param p_skinfo 指向客户端连接信息结构
 * @return 0-成功 other-关闭客户端
 */
extern "C" int handle_process(char *p_recv, int recv_len, char **pp_send, int *p_send_len, 
                              skinfo_t *p_skinfo);

/**
 * @brief 每当平台工作进程醒来调用
 */
extern "C" int handle_schedule();

#endif /* NEWBENCH_2011_07_11 */
