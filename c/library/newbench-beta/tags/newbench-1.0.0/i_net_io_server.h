/**
 * @file i_net_io_server.h
 * @brief 网络IO服务框架接口
 * @author Yuan B.J. the_guy_1987@hotmail.com
 * @version 1.1
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
 * @brief i_net_io_notifier
 */
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

/**
 * @brief 数据存储小块
 */
union net_io_storage {
    uint32_t u32;
    uint64_t u64;
    void *ptr;
};

/**
 * @brief 网络IO事件处理集
 */
struct i_net_io_event_handler
{
   /**
    * @brief 新连接建立事件回调函数
    *
    * @param p_net_io_server  网络IO服务对象指针
    * @param id  连接ID
    * @param fd  连接文件描述符
    * @param ip  连接IP
    * @param port  连接PORT
    *
    * @return -1-关闭这个连接
    */
    virtual int on_new_connection(void *p_net_io_server,
                                  int id,
                                  int fd,
                                  const char *ip,
                                  int port,
                                  union net_io_storage *p_storage) = 0;

    /**
     * @brief 接收数据事件回调函数
     *
     * @param p_net_io_server  网络IO服务对象指针
     * @param id  连接ID
     * @param fd  连接文件描述符
     * @param ip  连接IP
     * @param port  连接PORT
     * @param p_data  当前连接接收的接收数据缓冲区
     * @param data_len  接收到的数据长度
     * @param p_stroage 连接随带的用户数据
     *
     * @return >0-关闭当前连接 <=0-回调处理后剩余的数据长度
     */
    virtual int on_recv_data(void *p_net_io_server,
                             int id,
                             int fd,
                             const char *ip,
                             int port,
                             char *p_data,
                             int data_len,
                             union net_io_storage *p_storage) = 0;

    /**
     * @brief 关闭对端连接事件回调函数
     *
     * @param p_net_io_server  网络IO服务对象指针
     * @param id  连接ID
     * @param fd  连接文件描述符
     *
     * @return
     */
    virtual int on_connection_closed(void *p_net_io_server,
                                     int id,
                                     int fd,
                                     const char *ip,
                                     int port,
                                     union net_io_storage *p_storage) = 0;

    /**
     * @brief 被唤醒事件回调函数
     *
     * @param p_net_io_server  网络IO服务对象指针
     *
     * @return
     */
    virtual int on_wakeup(void *p_net_io_server) = 0;
};

/**
 * @brief 网络IO服务对象接口
 */
struct i_net_io_server
{
    /**
     * @brief 对象初始化
     *
     * @param ip  服务器监听IP地址
     * @param port  服务器监听端口地址
     * @param p_events_handler  服务器事件处理接口方法集合
     * @param p_events_notifier  服务器提醒机制处理对象
     * @param id_base  连接ID自增长基值
     * @param id_step  连接ID增加步长
     *
     * @return 0-成功 -1-失败
     */
    virtual int init(const char *ip,
                     int port,
                     const i_net_io_event_handler *p_event_handler,
                     const i_net_io_notifier *p_net_io_notifier,
                     int id_base,
                     int id_step) = 0;

    /**
     * @brief 发送数据(尽可能多的发送数据或拷贝至发送缓冲区)
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     *
     * @return >=0-处理数据的总长度 -1-失败
     */
    virtual int send_data(int id, const char *p_data, int data_len) = 0;

    /**
     * @brief 同send_data,但若数据大小无法全部拷贝至发送缓冲区,不对数据进行处理
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     *
     * @return data_len-成功 -1-失败
     */
    virtual int send_data_atomic(int id, const char *p_data, int data_len) = 0;

    /**
     * @brief 广播,包括一个连接像全体连接广播,服务端像全体连接广播
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param date_len  发送数据长度
     *
     * @return
     */
    virtual int broadcast(int id,
                          const char *p_data,
                          int date_len,
                          int is_atomic) = 0;

    /**
     * @brief 获取连接信息
     *
     * @param id  连接ID
     * @param ip  连接IP
     * @param ip_buffer_len  存放IP缓冲区的长度
     * @param p_port  指向连接PORT
     * @param p_time  指向连接建立时间
     * @param p_fd  指向连接对应的文件描述符
     * @param pp_storage  指向连接对应的用户私有数据的指针
     *
     * @return 0-成功 -1-失败
     */
    virtual int get_connection_info(
                    int id,
                    char *ip,
                    int ip_buffer_len,
                    int *p_port,
                    int *p_time,
                    int *p_fd,
                    union net_io_storage **pp_storage) = 0;

    /**
     * @brief 枚举所有连接ID
     *
     * @param p_buffer  存放连接ID
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
    virtual int do_io(int timeout, int cmd) = 0;

    /**
     * @brief 关闭连接
     *
     * @param id  连接ID
     * @param need_notify  是否抛出事件
     *
     * @return 0-成功 -1-失败
     */
    virtual int close_connection(int id, bool need_ontify) = 0;

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
