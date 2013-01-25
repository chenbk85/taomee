/**
 * =====================================================================================
 *       @file  net_client_impl.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  05/07/2010 02:47:20 PM 
 *    Revision  3.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  henry (韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_NET_CLIENT_IMPL_H_2010_05_07
#define H_NET_CLIENT_IMPL_H_2010_05_07

#define NET_CLIENT_MAX_SEND_BUFFER_LEN (1024 * 16)
#define NET_CLIENT_MAX_RECV_BUFFER_LEN (1024 * 1024)

class c_net_client_impl
{
public:
    c_net_client_impl();
    virtual ~c_net_client_impl();
    virtual int init(int server_addr, int server_port, int timeout);
    virtual int send_data(char *p_data, int data_len);
    virtual int send_data_atomic(char *p_data, int data_len);
    virtual int recv_data(char *p_recv_buffer, int buffer_len);
    virtual int do_io();
    virtual int ping(int timeout = 0);
    virtual int uninit();
    virtual int release();

protected:
    int connect_to_server(int timeout);
    int send_to_server(int fd, char *p_data, int data_len);
    int recv_from_server(int fd, char *p_recv_buffer, int buffer_len);

    int append_data_to_send_buffer(char *p_data, int data_len);
    int remove_data_from_send_buffer(int data_len);

    int remove_data_from_recv_buffer(int data_len);

    int recv_data_from_server();
    int disconnect_from_server();

private:
    int m_inited;      /**< 是否初始化标志*/
    int m_sock_fd;     /**< 连接fd*/
    int m_timeout;   /**< 用于控制select的超时时间(微级)*/
    int m_server_addr;
    int m_server_port;

    char m_send_buffer[NET_CLIENT_MAX_SEND_BUFFER_LEN]; /**< 存放待发送数据*/
    int m_send_buffer_len;                              /**< 发送缓存中的数据长度*/
    char m_recv_buffer[NET_CLIENT_MAX_RECV_BUFFER_LEN]; /**< 存放接收的数据*/
    int m_recv_buffer_len;                              /**< 接收缓存中的数据长度*/
};

int create_net_client_instance(c_net_client_impl **pp_instance);

#endif 


