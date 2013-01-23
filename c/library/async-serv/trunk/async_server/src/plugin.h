#ifndef ASYNC_SERVER_PLUGIN_H
#define ASYNC_SERVER_PLUGIN_H

#include <stdint.h>

struct plugin_t {
    int (*plugin_init)(int type);
    int (*plugin_fini)(int type);


    /**
     * @brief 处理工作进程中的定时器事件
     *
     * @param
     */
    void (*time_event)();

    int (*get_pkg_len_cli)(const char *buf, uint32_t len);

    /**
     * @brief int 正常情况下返回包头中的包长度 -1错误  0表示继续接收
     *
     * @param
     * @param len
     */
    int (*get_pkg_len_ser)(const char *buf, uint32_t len);



    /**
     * @brief 由网络进程调用  有新的客户端连接同conn进程建立连接时会调用
     *
     * @param ip
     * @param port
     * return: -1表示关闭新建立的连接
    */
    int (*check_open_cli)(uint32_t ip, uint16_t port);
    int (*select_channel)(int fd, const char *buf, uint32_t len, uint32_t ip, uint32_t work_num);//选择将数据包分发到不同的工作进程接收队列的规则
    int (*shmq_pushed)(int fd, const char *buf, uint32_t len, int flag);//某些业务需要在网络进程中直接给客户端回包，可以通过该接口实现（唯一数、唯一值）

    void (*proc_pkg_cli)(int fd, const char *buf, uint32_t len);
    void (*proc_pkg_ser)(int fd, const char *buf, uint32_t len);

    /**
     * @brief 由工作进程调用 有新的客户端连接上来的时候，网络进程会通知所有的工作进程
     *
     * @param
     * @param ip
     */
    void (*link_up_cli)(int fd, uint32_t ip);


    /**
     * @brief 由工作进程调用  有客户端关闭了连接的时候，网络进程会通知所有的工作进程
     *
     * @param
     */
    void (*link_down_cli)(int fd);

    void (*link_down_ser)(int fd);

};

int plugin_load(const char *filename);
void plugin_unload();

extern struct plugin_t g_plugin;

#endif
