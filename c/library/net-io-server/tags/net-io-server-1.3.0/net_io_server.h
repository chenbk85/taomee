/**
 * =====================================================================================
 *       @file  net_io_server.h
 *      @brief  TCP网络服务端的简单封装定义
 *
 *
 *   @internal
 *     Created  03/19/2010 01:15:12 PM 
 *    Revision  3.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  henry (韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_NET_IO_SERVER_H_2010_03_19
#define H_NET_IO_SERVER_H_2010_03_19
#include "./i_net_io_server.h"

#define MAX_CLIENT_NUM (1024*20)
#define MAX_SEND_BUFFER_LEN (1024*4)
#define MAX_RECV_BUFFER_LEN (1024*4)
#define MAX_ERRSTR_LEN 1024

#include <stdarg.h>
#include <map>

#define NETIOSERVER_IFACE_EOK		0
#define NETIOSERVER_IFACE_EINIT		1
#define NETIOSERVER_IFACE_ESEND		2
#define NETIOSERVER_IFACE_ERECV		3
#define NETIOSERVER_IFACE_EQUERY	4
#define NETIOSERVER_IFACE_ECLOSE	5

/**
 * @class c_net_io_server
 * @brief TCP网络服务端的封装实现类
 */
class c_net_io_server: public i_net_io_server
{
public:
    c_net_io_server();
    ~c_net_io_server();
    int init(const char *listen_ip, const int listen_port, const i_net_io_server_events *p_events, const int id_base, const int id_step);
    int send_data(const int connection_id, char *p_data, const int data_len);
    int send_data_atomic(const int connection_id, char *p_data, const int data_len);
    int get_connection_info(const int connection_id, char *server_ip, int ip_buffer_len, int *p_port, int *p_connected_time, int *p_connection_fd);
    int enum_connections(int *p_recv_buffer, int buffer_len, int *p_data_len);
    int close_connection(const int connection_id, bool b_notify);
    int do_io(const int time_interval, const int flags);
    int get_last_errno();
    const char* get_last_errstr();
    int uninit();
    int release();

protected:
    int get_conn_id();
    void set_err(int err_no, const char *msg, ...);

    int m_inited; 			/**< 是否初始化标志*/

    char m_listen_ip[16];		/**< 监听的机器IP*/
    int m_listen_port;			/**< 监听的端口号*/

    int m_listen_socket;		/**< 监听的socket套接口*/
    int m_epoll_fd;			/**< 建立的epoll描述符*/
	
    int m_connect_id;			/**< 用于自增的连接标识*/
    int m_connect_id_base;      /**<连接ID的基值*/
    int m_connect_id_step;      /**<连接ID的增长步长*/

    typedef struct
    {
        int connection_id;		/**< id标识，自增*/
	    int sock_fd;      		/**< 连接fd*/
        char peer_ip[16];		/**< 对端IP*/
        int peer_port; 			/**< 对端端口*/
        int connected_time;		/**< 已经连接的时间*/
        int recv_buffer_data_len;       /**<接收缓存中数据的长度*/
        char recv_buffer[MAX_RECV_BUFFER_LEN];/**接收缓存*/
        int send_buffer_data_len;		/**< 发送缓存中数据的长度*/
        char send_buffer[MAX_SEND_BUFFER_LEN];/**< 发送缓存*/
    }connection_info_t;

    typedef std::map<int, connection_info_t> id_connection_map_t;
    typedef id_connection_map_t::iterator id_conn_map_iter_t;
    typedef id_connection_map_t::const_iterator id_conn_map_const_iter_t;
    typedef id_connection_map_t::value_type id_conn_map_value_t;
    
    id_connection_map_t m_id_conn_map; /**< 代表所有链接的映射,以id为主键*/
    i_net_io_server_events* m_p_events; 

    int m_errno;
    char m_errstr[MAX_ERRSTR_LEN];
    int m_fd_flag;  /**< 用于标识系统的文件描述符是否使用完毕 */
};


#endif //H_net_io_server_H_2010_03_19

