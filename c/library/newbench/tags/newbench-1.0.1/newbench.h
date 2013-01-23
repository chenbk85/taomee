/**
 * =====================================================================================
 *       @file  newbench.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/14/2010 03:10:46 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu (TCL), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef _NEWBENCH_H_
#define _NEWBENCH_H_

#include <stdlib.h>

#include "log.h"

#define MAX_CONF_STR_LEN 512//配置文件中字符串最大值
#define MAX_PACKAGE_LEN (1024 * 1024)

#define NB_BOOT_LOG(OK, fmt, args...) do{\
    boot_log(OK, 0, fmt, ##args);\
}while(0)

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
 * @fd - socket description
 * @type - sock type (%SOCK_STREAM, %SOCK_DGRAM, etc)
 * @local - connection local address
 * @remote - connection peer address
 */
typedef struct skinfo_struct {
    int sockfd;
    int type;
    long long recvtm;
    long long sendtm;

    u_int local_ip;
    u_short local_port;
    u_int remote_ip;
    u_short remote_port;

    union bench_storage storage;
    int connection_id;
    void *ptr_lookout;
}skinfo_t;

/**
 * @brief 平台接口定义
 */
typedef struct  dll_func_struct {
    void *handle;

    /**
     * @brief 初始化(主进程，网络进程，工作进程都会调用)
     *
     * @param argc 参数个数
     * @param argv 指向参数保存的地址
     * @param proc_type 进程类型
     *
     * @return 0-成功 other-失败
     */
    int (* handle_init)(int argc, char **argv, int proc_type);

    /**
     * @brief 用来获取数据包对应KEY
     *
     * @param p_buf 数据包缓冲区
     * @param buf_len 数据包长度
     * @param proc_num 进程数量(暂无意义)
     * @param p_key 指向key地址
     *
     * @return 0-不屏蔽 <0-出错 >0-屏蔽
     */
    int (* handle_dispatch)(const char* p_buf,
                            int buf_len,
                            int proc_num,
                            int* p_key);

    /**
     * @brief 收到数据后进行回调
     *
     * @param p_recv 指向收到的数据缓冲区
     * @param recv_len 收到的数据长度
     * @param pp_send 指向要发送的数据的缓冲区的头指针
     * @param p_send_len 指向要发送数据的缓冲区长度
     * @param p_skinfo 指向连接客户端的信息结构体
     *
     * @return -1-异常，并终止此次会话 0-需要继续接收 >0-数据包应有长度
     */
    int (* handle_input)(const char *p_recv,
                         int recv_len,
                         char **pp_send,
                         int *p_send_len,
                         skinfo_t *p_skinfo);

    /**
     * @brief 收到完整数据，并压入环形队列后进行回调
     *
     * @param p_recv 指向收到的数据缓冲区
     * @param recv_len 收到的数据长度
     * @param pp_send 指向要发送的数据的缓冲区的头指针
     * @param p_send_len 指向要发送数据的缓冲区长度
     * @param p_skinfo 指向连接客户端的信息结构体
     * @param flag 数据是否成功压入了环形队列 1-已压入 0-未压入
     *
     * @return
     */
    int (* handle_input_complete)(const char *p_recv,
                                  int recv_len,
                                  char **pp_send,
                                  int *p_send_len,
                                  skinfo_t *p_skinfo,
                                  int flag);

    /**
     * @brief 当客户端和服务端建立连接后进行调用
     *
     * @param 暂无意义
     * @param 暂无意义
     * @param p_skinfo 指向连接客户端的信息结构体
     *
     * @return 0-成功，建立连接 -1-失败，断开连接
     */
    int (* handle_open)(char **, int *, skinfo_t *p_skinfo);

    /**
     * @brief 当客户端与服务器断开连接后进行调用
     *
     * @param p_skinfo 指向连接客户端的信息结构体
     *
     * @return 0-成功 -1-失败
     */
    int (* handle_close)(const skinfo_t *p_skinfo);


    /**
     * @brief 进程终止时反初始化调用
     *
     * @param proc_type 进行类型
     */
    void (* handle_fini)(int proc_type);

    int (* handle_timer)(int *);

    /**
     * @brief 当有完整数据包时给工作进行处理时调用
     *
     * @param p_recv 收到的数据
     * @param recv_len 收到的数据
     * @param pp_send 指向要发送的数据的指针
     * @param p_send_len 指向要发送的数据的长度
     * @param p_skinfo 指向客户端连接信息结构
     *
     * @return 0-成功 other-关闭客户端
     */
    int (* handle_process)(char *p_recv,
                           int recv_len,
                           char **pp_send,
                           int *p_send_len,
                           skinfo_t *p_skinfo);

    /**
     * @brief 每当平台工作进程醒来调用
     */
    int (* handle_schedule)();

} dll_func_t;

/**
 * @brief 发送数据 - 仅用在work进程
 *
 * @param connection_id 连接id
 * @param p_data        指向数据缓冲区
 * @param data_len      数据缓冲区长度
 * @param is_atomic     是否保持发送的原子性(暂无效,都为原子性发送)
 * @param is_broadcast  是否位广播发送
 *
 * @return 0-成功 -1-失败
 */
extern int send_data(int connection_id, const char *p_data, int data_len, int is_atomic, int is_broadcast);

/**
 * @brief 关闭连接
 *
 * @param connection_id 连接id,负数表示所有连接
 *
 * @return 0-成功 -1-失败
 */
extern int close_connection(int connection_id);

#endif
