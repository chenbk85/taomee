/**
 * =====================================================================================
 *       @file  i_net_client.h
 *      @brief  简单的tcp网络客户端封装接口
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

#ifndef H_I_NET_CLIENT_H_2010_05_31
#define H_I_NET_CLIENT_H_2010_05_31

#define NET_CLIENT_MAX_SEND_BUFFER_LEN (1024 * 128)
#define NET_CLIENT_MAX_RECV_BUFFER_LEN (1024 * 16)

/**
 *@class i_net_client
 *@brief 本接口提供对网络客户端的封装, 其中do_io是进行的实际的网络接收于发送
*/
struct i_net_client
{
public:
        
    /** 
     * @brief  初始化接口
     * @param   server_addr 要连接的服务端IP地址
     * @param   server_port 要连接的服务端的端口号
     * @param  timeout 连接超时时间，单位毫秒
     * @return 0success -1failed 
     */
    virtual int init(int server_addr, int server_port, int timeout) = 0;

    /** 
     * @brief 发送数据接口，将数据放到底层缓冲，如果缓冲区为空，就直接发送到网路中去  
     * @param   p_data 指向要发送的数据
     * @param  data_len 要发送的数据的长度
     * @return  0success -1failed
     */
    virtual int send_data(char *p_data, int data_len) = 0;

    /** 
     * @brief 接口功能同上，只是该接口是原子操作，上层传下来的数据要么全部发送成功，要么全部失败  
     * @param   参数通商
     * @return  0success -1failed
     */
    virtual int send_data_atomic(char *p_data, int data_len) = 0;

    /** 
     * @brief 接收数据，将底层缓冲中的数据拷贝到用户的缓存空间  
     * @param   p_recv_buffer 用户提供的接收缓存
     * @param   buffer_len 用户提供的接收缓存的长度
     * @return  -1failed >=0 返回的数据长度
     */
    virtual int recv_data(char *p_recv_buffer, int buffer_len) = 0;

    /** 
     * @brief 进行实际的网络发送于接收  
     * @param   
     * @return  0success -1failed
     */
    virtual int do_io() = 0;

    /** 
     * @brief 提供到服务器的重连功能 
     * @param   timeout超时时间，默认值0表示取初始化时提供的超时时间
     * @return  0success -1failed
     */
    virtual int ping(int timeout = 0) = 0;
    
    /** 
     * @brief  反初始化
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

int create_net_client_instance(i_net_client **pp_instance);
#endif //H_I_NET_CLIENT_H_2010_05_31


