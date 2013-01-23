/**
 * @file i_net_io_server.h
 * @brief 网络IO服务框架接口
 * @author baron baron@taomee.com
 * @version 2.0
 * @date 2010-12-06
 */

#ifndef __I_NET_IO_SERVER_H__
#define __I_NET_IO_SERVER_H__

// do_io命令一览
#define NET_IO_SERVER_CMD_ACCEPT   0x0001
#define NET_IO_SERVER_CMD_READ     0x0010
#define NET_IO_SERVER_CMD_WRITE    0x0100

// 错误码一览
#define NET_IO_SERVER_OK       0
#define NET_IO_SERVER_EINIT    1
#define NET_IO_SERVER_EIO      2
#define NET_IO_SERVER_ESEND    3
#define NET_IO_SERVER_ERECV    4
#define NET_IO_SERVER_EQUERY   5
#define NET_IO_SERVER_ECLOSE   6

#include <stdint.h>

/**
 * @brief 数据存储小块
 */
union net_io_storage {
    uint32_t u32;
    uint64_t u64;
    void *ptr;
};

struct i_net_io_event_handler
{
   /**
    * @brief 新连接建立回调函数
    *
    * @param p_net_io_server  网络IO服务对象指针
    * @param id  连接ID
    * @param fd  连接文件描述符
    * @param ip  连接IP
    * @param port  连接PORT
    *
    * @return
    */
    virtual int on_new_connection(void *p_net_io_server,
                                  const int id,
                                  const int fd,
                                  const char *ip,
                                  const int port,
                                  union net_io_storage *p_storage) = 0;

    /**
     * @brief 接收数据回调函数
     *
     * @param p_net_io_server  网络IO服务对象指针
     * @param id  连接ID
     * @param fd  连接文件描述符
     * @param ip  连接IP
     * @param port  连接PORT
     * @param p_data  接收的数据缓存
     * @param data_len  接收到的数据长度
     *
     * @return 回调处理后剩余的数据长度
     */
    virtual int on_recv_data(void *p_net_io_server,
                             const int id,
                             const int fd,
                             const char *ip,
                             const int port,
                             char *p_data,
                             int data_len,
                             union net_io_storage *p_storage) = 0;

    /**
     * @brief 关闭对端连接回调函数
     *
     * @param p_net_io_server  网络IO服务对象指针
     * @param id  连接ID
     * @param fd  连接文件描述符
     *
     * @return
     */
    virtual int on_connection_closed(void *p_net_io_server,
                                     const int id,
                                     const int fd,
                                     const char *ip,
                                     const int port,
                                     union net_io_storage *p_storage) = 0;

    /**
     * @brief 被唤醒时的回调函数
     *
     * @param p_net_io_server  网络IO服务对象指针
     *
     * @return
     */
    virtual int on_wakeup(void *p_net_io_server) = 0;
};

struct i_net_io_notifier
{
    /**
     * @brief 初始化
     *
     * @return
     */
    virtual int init() = 0;

    /**
     * @brief 反初始化
     *
     * @return
     */
    virtual int uninit() = 0;

    /**
     * @brief 抛出事件
     *
     * @return
     */
    virtual int popup() = 0;

    /**
     * @brief 确定接收事件
     *
     * @return
     */
    virtual int got() = 0;

    /**
     * @brief 释放本对象资源
     */
    virtual int release() = 0;

    /**
     * @brief get_eventfd
     *
     * @return 返回私有文件描述符-成功 -1-失败
     */
    virtual int get_eventfd() const = 0;
};

struct i_net_io_server
{
    /**
     * @brief 对象初始化
     *
     * @param ip  服务器监听IP地址
     * @param port  服务器监听端口地址
     * @param p_events_handler  服务器事件处理接口方法集合
     * @param p_events_notifier 服务器提醒机制处理对象
     * @param id_base  连接ID自增长基值
     * @param id_step  连接ID增加步长
     *
     * @return 0-成功 -1-失败
     */
    virtual int init(const char *ip,
                     const int port,
                     const i_net_io_event_handler *p_event_handler,
                     const i_net_io_notifier *p_net_io_notifier,
                     const int id_base,
                     const int id_step) = 0;

    /**
     * @brief 发送数据(尽可能多的发送数据或拷贝至发送缓冲区)
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     *
     * @return >=0-处理数据的总长度 -1-失败
     */
    virtual int send_data(const int id, char *p_data, const int data_len) = 0;

    /**
     * @brief 同send_data,但若数据大小无法全部拷贝至发送缓冲区,不对数据进行处理
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     *
     * @return data_len-成功 -1-失败
     */
    virtual int send_data_atomic(const int id,
                                 char *p_data,
                                 const int data_len) = 0;
    /**
     * @brief 获取连接信息
     *
     * @param id  连接ID
     * @param ip  连接IP
     * @param ip_buffer_len  存放IP缓冲区的长度
     * @param p_port  连接PORT
     * @param p_time  连接建立时间
     * @param p_fd  连接对应的文件描述符
     * @param pp_storage  连接对应的用户私有数据
     *
     * @return 0-成功 -1-失败
     */
    virtual int get_connection_info(
                    const int id,
                    char *ip,
                    int ip_buffer_len,
                    int *p_port,
                    int *p_time,
                    int *p_fd,
                    union net_io_storage **pp_storage) = 0;

    /**
     * @brief 枚举所有连接ID
     *
     * @param p_buffer 存放连接ID
     * @param length  存放连接ID的缓冲区长度
     * @param p_buffer_len  返回的连接ID个数
     *
     * @return 0-成功 -1-失败
     */
    virtual int enum_connections(int *p_buffer,
                                 int length,
                                 int *p_buffer_len) = 0;

    /**
     * @brief 发送接收网络数据
     *
     * @param timeout  超时限制
     * @param cmd  发送接收命令标志,包括:
     *             NET_IO_CMD_ACCEPT,NET_IO_CMD_READ,NET_IO_CMD_WRITE
     *
     * @return 0-成功 -1-失败
     */
    virtual int do_io(const int timeout, const int cmd) = 0;

    /**
     * @brief 关闭连接
     *
     * @param id  连接ID
     * @param need_notify  是否抛出事件
     *
     * @return 0-成功 -1-失败
     */
    virtual int close_connection(const int id, bool need_ontify) = 0;

    /**
     * @brief 获取上次出错的错误码
     *
     * @return 错误码
     */
    virtual int get_last_errno() = 0;

    /**
     * @brief 获取上次出错的错误信息
     *
     * @return 错误信息
     */
    virtual const char *get_last_errstr() = 0;

    /**
     * @brief 反初始化
     *
     * @return 0-成功 -1-失败
     */
    virtual int uninit() = 0;

    /**
     * @brief 释放本对象资源
     */
    virtual int release() = 0;
};

/**
 * @brief 生成net_io_server实例
 *
 * @param pp_net_io_server 指向指向i_net_io_server类型的指针的指针
 *
 * @return 0-成功 -1-失败
 */
int net_io_server_create(i_net_io_server **pp_net_io_server);

#endif //!__I_NET_IO_SERVER_H__
