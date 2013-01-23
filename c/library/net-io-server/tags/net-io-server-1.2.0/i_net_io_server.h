/**
 * =====================================================================================
 *       @file  i_net_io_server.h
 *      @brief  TCP网络服务端的简单封装接口
 *
 *
 *   @internal
 *     Created  06/13/2010 01:02:39 PM 
 *    Revision  3.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  henry (韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_I_NET_IO_SERVER_H_2010_06_13
#define H_I_NET_IO_SERVER_H_2010_06_13

#define NET_IO_SERVER_OP_ACCEPT 1 
#define NET_IO_SERVER_OP_READ   2
#define NET_IO_SERVER_OP_WRITE  4

/** 
 *@struct i_net_io_server_events
 *@brief 用户需要继承的  
 */
struct i_net_io_server_events
{
public:
    /** 
     * @brief  底层监听到有新的连接到来时通知用户
     * @param   p_net_io_server 
     * @param   connection_id  连接ID
     * @param   connection_fd
     * @param   peer_ip        对端IP
     * @param   peer_port     对端端口号
     * @return  0success -1failed
     */
    virtual int on_new_connection(void *p_net_io_server, const int connection_id, const int connection_fd, const char *peer_ip, const int peer_port) = 0;

    /** 
     * @brief 底层接收到数据时通知用户  
     * @param   p_net_io_server
     * @param   connection_id 指示哪个连接ID上接收到数据
     * @param   connection_fd
     * @param   p_data   指向接收到的数据
     * @param   data_len 接收到的数据长度
     * @return  0success -1failed
     */
    virtual int on_recv_data(void *p_net_io_server, const int connection_id, const int connection_fd, char *p_data, int data_len) = 0;

    /** 
     * @brief 底层关闭连接时通知用户(连接关闭之前) 
     * @param   p_net_io_server
     * @param   connection_id 指示关闭的是哪个连接
     * @param   connection_fd
     * @return  0success -1failed
     */
    virtual int on_connection_closed(void *p_net_io_server, const int connection_id, const int connection_fd) = 0;
};

/** 
 *@struct i_net_io_server
 *@brief 提供网络服务的实例  
 */
struct i_net_io_server
{
    /** 
     * @brief  初始化函数
     * @param   listen_ip  服务端IP地址
     * @param  listen_port 服务端端口号
     * @param  p_events  
     * @param  id_base 连接ID基值
     * @param  id_step 连接ID增长步长
     * @return  0success -1 failed
     */
    virtual int init(const char *listen_ip, const int listen_port, const i_net_io_server_events *p_events, const int id_base,const int id_step) = 0;

    /** 
     * @brief 发送数据，将数据拷贝到相应连接的缓冲区,并没有实际发送。拷贝数据的长度为min(data_len,缓冲区空闲长度)
     * @param   connection_id 代表连接的ID
     * @param  p_data 要发送的数据
     * @param data_len 要发送的数据的长度
     * @return >=0 实际发送的字节数 -1failed 
     */
    virtual int send_data(const int connection_id, char *p_data, const int data_len) = 0;

    /** 
     * @brief  发送数据，将数据拷贝到发送缓冲区，如果缓冲去无足够空间，就不拷贝
     * @param  参数含义同上个函数 
     * @return >=0 实际发送的字节数 -1failed 
     */
    virtual int send_data_atomic(const int connection_id, char *p_data, const int data_len) = 0;

    /** 
     * @brief  获得连接信息
     * @param   connection_id 连接ID
     * @param  server_ip 存放对端的IP
     * @param  ip_buffer_len 对端IP地址长度
     * @param  p_port 对端端口
     * @param  p_connected_time 此连接的时间
     * @param  p_connection_fd
     * @return  0success -1failed
     */
    virtual int get_connection_info(const int connection_id, char *server_ip, int ip_buffer_len, int *p_port, int *p_connected_time, int* p_connection_fd) = 0;
    
    /** 
     * @brief  枚举所有的连接ID
     * @param  p_recv_buffer 存放所有的连接信息id
     * @param  buffer_len 存放连接缓冲区的长度
     * @param p_data_len 
     * @return  0success -1failed
     */
    virtual int enum_connections(int *p_recv_buffer, int buffer_len, int *p_data_len) = 0;
    
    /** 
     * @brief  关闭连接
     * @param   connection_id 要关闭的连接ID
     * @param   b_notify 关闭连接后是否通知上层
     * @return  0success -1failed
     */
    virtual int close_connection(const int connection_id, bool b_notify) = 0;
    
    /** 
     * @brief  进行实际的网络IO操作
     * @param   time_inteval 超时时间,单位毫秒
     * @param   flags 标志位，用于标示需要监听哪些事件
     * @return  0success -1failed
     */
    virtual int do_io(const int time_interval, const int flags) = 0;

    /** 
     * @brief 获得最后一次错误的错误码
     * 
     * @return 错误码
     */
    virtual int get_last_errno() = 0;

    /** 
     * @brief 获得最后一次错误的错误描述
     * 
     * @return 
     */
    virtual const char* get_last_errstr() = 0;

    /** 
     * @brief 反初始化 
     * @param   
     * @return  
     */
    virtual int uninit() = 0;

    /** 
     * @brief  释放实例
     * @param   
     * @return  
     */
    virtual int release() = 0;
};

/**
 * @brief  创建网络服务端对象
 * @param  i_net_io_server** pp_net_io_server 二级指针， 指向创建的对象的内存的地址的指针
 * @return 成功返回0， 错误返回-1 
 */
int net_io_server_create(i_net_io_server** pp_net_io_server);
#endif //H_I_NET_IO_SERVER_H_2010_06_13
