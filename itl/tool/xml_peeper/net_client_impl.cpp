/**
 * =====================================================================================
 *       @file  net_client_impl.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  05/07/2010 03:02:30 PM 
 *    Revision  3.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  henry (韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <new>
extern "C"
{
#include <libtaomee/log.h>
}

#include "net_client_impl.h"

/** 
 * @brief  创建net_client的实例
 * @param   
 * @return  0success -1failed
 */
int create_net_client_instance(c_net_client_impl **pp_instance)
{
    if(NULL == pp_instance)
    {
        return -1;
    }
    
    c_net_client_impl *p_instance = new (std::nothrow)c_net_client_impl();
    if(NULL == p_instance)
    {
        return -1;
    }
    else
    {
        *pp_instance = (p_instance);
        return 0;
    }
}


/** 
 * @brief  构造函数
 * @param   
 * @return  
 */
c_net_client_impl::c_net_client_impl()
{
    m_sock_fd = -1;
    m_server_addr = 0;
    m_server_port = 0;
    m_timeout = 0;
    m_send_buffer_len = 0;
    m_recv_buffer_len = 0;
    m_inited = 0;
}

/** 
 * @brief  析构函数
 * @param   
 * @return  
 */
c_net_client_impl::~c_net_client_impl()
{
    uninit();
}

/** 
 * @brief  初始化函数,建立到server的连接
 * @param   server_addr 服务端的地址
 * @param   server_port 服务端的端口号
 * @return  0success -1failed
 */
int c_net_client_impl::init(int server_addr, int server_port, int timeout)
{
    if(m_inited)
    {
        return -1;
    }
    
    m_server_addr = server_addr;
    m_server_port = server_port;

    m_timeout = timeout;

    if(connect_to_server(timeout) != 0)
    {
        struct in_addr tmp_addr;
        memcpy(&tmp_addr, &server_addr, 4);
        printf("can not connect to server(%s: %d) now.\n", inet_ntoa(tmp_addr), server_port);
        return -1;
    }

    m_inited = 1;

    return 0;
}


/** 
 * @brief  发送数据
 * @param   p_data 待发送的数据
 * @param   data_len 待发送的数据的长度
 * @return  0success -1failed
 */
int c_net_client_impl::send_data(char *p_data, int data_len)
{
    if(!m_inited)
    {
        return -1;
    }

    if(NULL == p_data || data_len < 0)
    {
        return -1;
    }

    if(m_sock_fd < 0)
    {
        if(connect_to_server(m_timeout) < 0)
        {
            printf("can not connect to server now.\n");
            return -1;
        }
    }

    if(m_send_buffer_len > 0)
    {/**< 发送缓冲中尚有数据，需首先将本次数据添加到发送缓冲中，然后再调用发送函数*/
        if(data_len > 0)
        {
            append_data_to_send_buffer(p_data, data_len);
        }
        
        int bytes_sent = send_to_server(m_sock_fd, m_send_buffer, m_send_buffer_len);
        if(bytes_sent > 0)
        {
            remove_data_from_send_buffer(bytes_sent);
        }
        else if(bytes_sent < 0)
        {/**< 连接遇到问题，关闭连接*/
           disconnect_from_server();
           return -1;
        }
        else
        {
            /**< 无法发送数据 nothing to do*/
        }
    }
    else
    {/**< 发送缓冲为空，直接调用发送函数*/
       if(data_len > 0)
       {
            int bytes_sent = send_to_server(m_sock_fd, p_data, data_len);
            if(bytes_sent >= 0)
            {
                if(bytes_sent < data_len)
                {
                    append_data_to_send_buffer(p_data + bytes_sent, data_len -bytes_sent);
                }
            }
            else
            {
                disconnect_from_server();
                return -1;
            }
       }
       else
       {
            //nothing to do
       }
    }

    return 0;
}


/** 
 * @brief  发送数据（原子操作）
 * @param   p_data 待发送的数据
 * @param   data_len 待发送的数据长度
 * @return  0success -1failed
 */
int c_net_client_impl::send_data_atomic(char *p_data, int data_len)
{
    if(!m_inited)
    {
        return -1;
    }

    if(NULL == p_data || data_len < 0)
    {
        return -1;
    }

    if(m_sock_fd < 0)
    {
        if(connect_to_server(m_timeout) < 0)
        {
            printf("can not connect to server now.\n");
            return -1;
        }
    }

    int empty_data_len = sizeof(m_send_buffer) - m_send_buffer_len;
    if(empty_data_len < data_len)
    {
        printf("It doesn't have enough space\n");
        return -1;
    }

    int bytes_sent = 0;
    int remain_data_len = 0;
    if(m_send_buffer_len == 0) /**< 发送缓冲为空*/
    {
        bytes_sent = send_to_server(m_sock_fd, p_data, data_len);
        if(bytes_sent >= 0)
        {
            remain_data_len = data_len - bytes_sent;
        }
    }

    if(remain_data_len == 0)
    {
        return 0;
    }

    append_data_to_send_buffer(p_data + bytes_sent, remain_data_len);

    return 0;
}

/** 
 * @brief  接收数据
 * @param   p_recv_buffer 接收缓存
 * @param   buffer_len 接收缓存的长度
 * @param   min_buffer_len 返回消息的长度
 * @return  -1无法接收数据  >=0 返回数据的长度
 */
int c_net_client_impl::recv_data(char *p_recv_buffer, int buffer_len)
{
    if(!m_inited)
    {
        return -1;
    }

    if(NULL == p_recv_buffer || buffer_len < 0)
    {
        return -1;
    }

    int copy_len = 0;
    if(buffer_len < m_recv_buffer_len)
    {
        copy_len = buffer_len;
    }
    else
    {
        copy_len = m_recv_buffer_len;
    }

    memcpy(p_recv_buffer, m_recv_buffer, copy_len);
    remove_data_from_recv_buffer(copy_len);
    return copy_len;
}

/** 
 * @brief  将发送缓冲中的数据发送出去,同时接收一次网络中的数据
 * @param   
 * @return  0success -1failed
 */
int c_net_client_impl::do_io()
{
    if(!m_inited)
    {
        return -1;
    }

    if(m_sock_fd < 0)
    {
        if(connect_to_server(m_timeout) != 0)
        {
            printf("can not connect now\n");
            return -1;
        }    
    }
	int recv = recv_data_from_server();
    if( recv == -1)
    {
        return -1;
    }
	else if(recv == -2)
	{
		return -2;
	}

    if(m_send_buffer_len > 0)
    {
        int bytes_sent = send_to_server(m_sock_fd, m_send_buffer, m_send_buffer_len);
        if(bytes_sent > 0)
        {
            remove_data_from_send_buffer(bytes_sent);
        }
        else if(bytes_sent < 0)
        {/**< 连接遇到问题，关闭连接*/
           disconnect_from_server();
           return -1;
        }
        else
        {
            //nothing to do
        }
    }
    return 0;
}


/** 
 * @brief 尝试重新同服务端建立连接 
 * @param   
 * @return  0success -1failed
 */
int c_net_client_impl::ping(int timeout)
{
    if(m_sock_fd < 0)
    {
        if(connect_to_server(timeout) != 0)
        {
            printf("cat not connect to server\n");
            return -1;
        }
    }

    return 0;
}

/** 
 * @brief  反初始化 断开同服务器的连接
 * @param   
 * @return  0success -1failed
 */
int c_net_client_impl::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    if(m_sock_fd >= 0)
    {
        close(m_sock_fd);
        m_sock_fd = -1;
    }
    
    
    m_send_buffer_len = 0;
    m_recv_buffer_len = 0;
    m_inited = 0;

    return 0;
}

/** 
 * @brief  对应create，释放create创建的实例
 * @param   
 * @return  
 */
int c_net_client_impl::release()
{
    delete this;
    return 0;
}

/** 
 * @brief  将数据添加到发送缓冲的末尾
 * @param   p_data 需添加的数据
 * @param   data_len 待添加的数据的长度
 * @return  -1 发送缓冲空间不够 >=0 添加的数据长度
 */
int c_net_client_impl::append_data_to_send_buffer(char *p_data, int data_len)
{
    if(m_send_buffer_len + data_len > (int)sizeof(m_send_buffer))
    {
        return -1;
    }

    memcpy(m_send_buffer + m_send_buffer_len, p_data, data_len);
    m_send_buffer_len +=  data_len;
    return data_len;
}

/** 
 * @brief  发送数据
 * @param   fd sockfd
 * @param   p_data 指向待发送的数据
 * @param   data_len 待发送的数据长度
 * @return  -1failed 0发送被中断 >0发送出去的数据长度
 */
int c_net_client_impl::send_to_server(int fd, char *p_data, int data_len)
{
    if(data_len <= 0)
    {
        return -1;
    }

    int bytes_sent = send(fd, p_data, data_len, 0);
    if(bytes_sent > 0)
    {
        return bytes_sent;
    }
    else if(bytes_sent == 0)
    {
       return -1; 
    }
    else
    {
        if(errno == EAGAIN || errno == EINTR)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
}


/** 
 * @brief  将send_buffer中的数据移走
 * @param   data_len 需要移走的数据的长度
 * @return  -1failed >0 移走的数据长度
 */
int c_net_client_impl::remove_data_from_send_buffer(int data_len)
{
    if(data_len > m_send_buffer_len)
    {
        return -1;
    }
    else if(data_len == m_send_buffer_len)
    {
        m_send_buffer_len = 0;
        return data_len;
    }
    else
    {
        memmove(m_send_buffer, m_send_buffer + data_len, m_send_buffer_len - data_len);
        m_send_buffer_len -= data_len;
        return data_len;
    }
}


/** 
 * @brief  从服务端接收数据
 * @param   
 * @return  -1无法接收数据 >0 接收到的数据长度
 */
int c_net_client_impl::recv_data_from_server()
{
    if(m_recv_buffer_len >= (int)sizeof(m_recv_buffer))
    {/**< 检查是否有足够的空间接收继续数据*/
        return 0;
    }    

    int bytes_recved = recv_from_server(m_sock_fd, m_recv_buffer + m_recv_buffer_len, sizeof(m_recv_buffer) - m_recv_buffer_len);
    if(bytes_recved < 0)
    {
        disconnect_from_server();
		if(bytes_recved == -2)
			return -2;
		else
			return -1;
    }
    else if(bytes_recved == 0)
    {
        //nothing to do
    }
    else
    {
        m_recv_buffer_len += bytes_recved;
    }

    return bytes_recved;
}


/** 
 * @brief  从fd上接收数据，放入缓存p_recv_buffer
 * @param   fd 连接fd
 * @param   p_recv_buffer 接收缓存
 * @param   buffer_len 接收缓存的长度
 * @return  -1无法接收数据 0接收过程被中断 >0接收到数据长度 
 */
int c_net_client_impl::recv_from_server(int fd, char *p_recv_buffer, int buffer_len)
{
    if(buffer_len <= 0)
    {
        return -1;
    }

    fd_set read_fd;
    FD_ZERO(&read_fd);

    FD_SET(fd, &read_fd);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    int result = select(fd + 1, &read_fd, NULL, NULL, &tv);
    if(result < 0)
    {
        printf("errno[%d]:%s\n",errno, strerror(errno));
        return -1; 
    }
    else if(result == 0)
    {//no data on socket
        return 0;
    }
    else
    {
        int bytes_recved = recv(fd, p_recv_buffer, buffer_len, 0);
        if(bytes_recved > 0)
        {
            return bytes_recved;
        }
        else if(bytes_recved == 0)
        {
            return -2;
        }
        else
        {
            if(errno == EAGAIN || errno == EINTR)
            {
                return 0;
            }
            else
            {
                printf("errno[%d]:%s\n",errno, strerror(errno));
                return -1;
            }
        }
    }

}

/** 
 * @brief  从接收缓存中移走数据
 * @param   data_len 需要移走的数据长度
 * @return  
 */
int c_net_client_impl::remove_data_from_recv_buffer(int data_len)
{
    if(data_len > m_recv_buffer_len)
    {
        return -1;
    }
    else if(data_len == m_recv_buffer_len)
    {
        m_recv_buffer_len = 0;
        return data_len;
    }
    else
    {
        memmove(m_recv_buffer, m_recv_buffer + data_len, m_recv_buffer_len - data_len);
        m_recv_buffer_len -= data_len;
        return data_len;
    }
}

/** 
 * @brief  连接到服务端
 * @param   
 * @return 0success -1failed 
 */
int c_net_client_impl::connect_to_server(int timeout)
{
    if(m_sock_fd > 0)
    {
        close(m_sock_fd);
        m_sock_fd = -1;
    }

    m_send_buffer_len = 0;
    m_recv_buffer_len = 0;

    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_sock_fd < 0)
    {
        printf("errno[%d]:%s\n",errno, strerror(errno));
        m_sock_fd = -1;
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = m_server_addr;
    serv_addr.sin_port = htons(m_server_port);

    int flags = fcntl(m_sock_fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(m_sock_fd, F_SETFL, flags);

    int result = connect(m_sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(result < 0)
    {
        if(errno == EINPROGRESS || errno == EISCONN)
        {
            struct timeval tv;
            if(timeout < 1000)
            {
                timeout = 1000;
            }

            tv.tv_sec = timeout / 1000;
            tv.tv_usec = timeout % 1000;

            fd_set write_set, err_set;
            FD_ZERO(&write_set);
            FD_ZERO(&err_set);

            FD_SET(m_sock_fd, &write_set);
            FD_SET(m_sock_fd, &err_set);

            int result = select(m_sock_fd + 1, NULL, &write_set, &err_set, &tv);
            if(result < 0)
            {
                printf("errno[%d]:%s\n",errno, strerror(errno));
                close(m_sock_fd);
                m_sock_fd = -1;
                return -1;
            }
            else if(result == 0)
            {
                printf("timeout in select\n");
                close(m_sock_fd);
                m_sock_fd = -1;
                return -1;
            }
            else
            {
                int opt = -1;
                socklen_t opt_len = sizeof(opt);
                getsockopt(m_sock_fd, SOL_SOCKET, SO_ERROR, &opt, &opt_len);
                if(opt)
                {
                    printf("errno[%d]:%s\n",errno, strerror(errno));
                    close(m_sock_fd);
                    m_sock_fd = -1;
                    return -1;
                }
                else
                {//连接可用
                    return 0;
                }
            }
        }
        else
        {
            printf("errno[%d]:%s\n",errno, strerror(errno));
            close(m_sock_fd);
            m_sock_fd = -1;
            return -1;
        }
    }
    else if(result == 0)
    {
        return 0;
    }
    else
    {
        printf("errno[%d]:%s\n",errno, strerror(errno));
        close(m_sock_fd);
        m_sock_fd = -1;
        return -1;
    }

    return 0;
}

/** 
 * @brief  断开同服务端的连接
 * @param   
 * @return  
 */
int c_net_client_impl::disconnect_from_server()
{
    if(m_sock_fd >= 0)
    {
        close(m_sock_fd);
        m_sock_fd = -1;
    }

    m_send_buffer_len = 0;
    m_recv_buffer_len = 0;

    return 0;
}
