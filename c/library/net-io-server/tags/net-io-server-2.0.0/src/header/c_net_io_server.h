/**
 * @file c_net_io_server.h
 * @brief 网络IO服务框架接口实现
 * @author baron baron@taomee.com
 * @version 2.0
 * @date 2010-12-07
 */

#ifndef __C_NET_IO_SERVER_H__
#define __C_NET_IO_SERVER_H__

#include "i_net_io_server.h"

#define NET_IO_SERVER_LISTEN_BACKLOG  80
#define MAX_CLIENT_NUM  (20 * 1024)
#define MAX_ACTIVE_CLIENT_NUM   4096

#define MAX_BUFFER_RECV_LEN (4 * 1024)
#define MAX_BUFFER_SEND_LEN (4 * 1024)
#define MAX_ERRSTR_LEN      1024

#define NET_IO_SERVER_FLAG_RUNOUT  0x0001

#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <sys/epoll.h>

class c_net_io_server: public i_net_io_server
{
public:

    /**
     * @brief 连接信息结构体
     */
    typedef struct
    {
        int id;
        int fd;
        char ip[16];
        int port;
        int connect_time;
        union net_io_storage storage;
        int buffer_recv_len;
        char buffer_recv[MAX_BUFFER_RECV_LEN];
        int buffer_send_len;
        char buffer_send[MAX_BUFFER_SEND_LEN];
    } connection_info_t;

    /**
     * @brief 连接信息结构体MAP
     */
    typedef std::map<int, connection_info_t> id_connection_map_t;

    /**
     * @brief id_connection_map_t类型的迭代器
     */
    typedef id_connection_map_t::iterator id_conn_map_iter_t;

    /**
     * @brief 构造函数
     */
    c_net_io_server();

    /**
     * @brief 析构函数
     */
    ~c_net_io_server();

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
    int init(const char *ip,
             const int port,
             const i_net_io_event_handler *p_events_handler,
             const i_net_io_notifier *p_net_io_notifier,
             const int id_base = 1,
             const int id_step = 1);

    /**
     * @brief 发送数据(尽可能多的发送数据或拷贝至发送缓冲区)
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     *
     * @return >=0-处理数据的总长度 -1-失败
     */
    int send_data(const int id, char *p_data, const int data_len);

    /**
     * @brief 同send_data,但若数据大小无法全部拷贝至发送缓冲区,不对数据进行处理
     *
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     *
     * @return data_len-成功 -1-失败
     */
    int send_data_atomic(const int id, char *p_data, const int data_len);

    /**
     * @brief 获取连接信息
     *
     * @param id  连接ID
     * @param ip  连接IP
     * @param ip_buffer_len  存放IP缓冲区的长度
     * @param p_port  连接PORT
     * @param p_time  连接建立时间
     * @param p_fd  连接对应的文件描述符
     * @param p_storage  连接对应的用户私有数据
     *
     * @return 0-成功 -1-失败
     */
    int get_connection_info(const int id,
                            char *ip,
                            int ip_buffer_len,
                            int *p_port,
                            int *p_time,
                            int *p_fd,
                            union net_io_storage **pp_storage);

    /**
     * @brief 枚举所有连接ID
     *
     * @param p_buffer 存放连接ID
     * @param length  存放连接ID的缓冲区长度
     * @param p_buffer_len  返回的连接ID个数
     *
     * @return 0-成功 -1-失败
     */
    int enum_connections(int *p_buffer, int length, int *p_buffer_len);

    /**
     * @brief 发送接收网络数据
     *
     * @param timeout  超时限制
     * @param cmd  发送接收命令标志,包括:
     *             NET_IO_SERVER_CMD_ACCEPT,NET_IO_SERVER_CMD_READ,NET_IO_SERVER_CMD_WRITE
     *
     * @return 0-成功 -1-失败
     */
    int do_io(const int timeout, const int cmd);

    /**
     * @brief 关闭连接
     *
     * @param id  连接ID
     * @param need_notify  是否抛出事件
     *
     * @return 0-成功 -1-失败
     */
    int close_connection(const int id, bool need_notify);

    /**
     * @brief 获取上次出错的错误码
     *
     * @return 错误码
     */
    int get_last_errno();

    /**
     * @brief 获取上次出错的错误信息
     *
     * @return 错误信息
     */
    const char *get_last_errstr();

    /**
     * @brief 反初始化
     *
     * @return 0-成功 -1-失败
     */
    int uninit();

    /**
     * @brief 释放本对象资源
     */
    int release();

private:
    /**
     * @brief 是否初始化信息
     */
    int m_init_;

    /**
     * @brief 下一次连接可分配的ID
     */
    int m_id_;

    /**
     * @brief 连接ID生成基质
     */
    int m_id_base_;

    /**
     * @brief 连接ID增长步长
     */
    int m_id_step_;

    /**
     * @brief 生成新的连接ID
     *
     * @return  新连接ID
     */
    int genereate_id_();

    /**
     * @brief 上一次出错的错误码
     */
    int m_errno_;

    /**
     * @brief 上一次出错的错误信息
     */
    char m_errstr_[MAX_ERRSTR_LEN];

    /**
     * @brief 设置错误信息
     *
     * @param error_no  错误码
     * @param msg  错误信息格式字符串
     */
    void set_error_info_(int error_no, const char *msg, ...);

    /**
     * @brief 监听地址IP
     */
    char m_ip_listen_[16];

    /**
     * @brief 监听地址PORT
     */
    int m_port_listen_;

    /**
     * @brief 监听地址对应的文件描述符
     */
    int m_socket_listen_;

    /**
     * @brief EPOLL对应的文件描述符
     */
    int m_epoll_fd_;

    /**
     * @brief 连接ID和连接信息的MAP
     */
    id_connection_map_t m_id_conn_map_;

    /**
     * @brief 网络IO事件处理对象
     */
    i_net_io_event_handler* m_p_net_io_event_handler_;

    /**
     * @brief 通知机制对象
     */
    i_net_io_notifier *m_p_net_io_notifier_;

    /**
     * @brief 全局状态标志(现在包括文件描述符使用完的状态标志)
     */
    int m_flag_;

    /**
     * @brief 事件集合
     */
    epoll_event m_events_[MAX_ACTIVE_CLIENT_NUM + 2];

    /**
     * @brief 连接信息结构体
     */
    connection_info_t m_connection_info_;
};

inline int c_net_io_server::genereate_id_()
{
    int id = m_id_;

    if ((m_id_ += m_id_step_) < 0)
    {
        m_id_ = m_id_base_;
    }

    return id;
}

inline void c_net_io_server::set_error_info_(int error_no, const char *msg, ...)
{
    m_errno_ = error_no;
    va_list args;
    va_start(args, msg);
    vsprintf(m_errstr_, msg, args);
    va_end(args);
}

inline int c_net_io_server::get_last_errno()
{
    return m_errno_;
}

inline const char *c_net_io_server::get_last_errstr()
{
    return m_errstr_;
}

#endif  //!__C_NET_IO_SERVER_H__
