/**
 * =====================================================================================
 *       @file  net_io_server.cpp
 *      @brief  TCP网络服务端的简单封装类实现
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/22/2010 07:18:40 PM
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
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "net_io_server.h"
using namespace std;

/**
 * @brief  创建一个net_io_server的实例
 * @param
 * @return  0success -1failed
 */
int net_io_server_create(i_net_io_server** pp_net_io_server)
{
    if(NULL == pp_net_io_server)
    {
        return -1;
    }

    c_net_io_server* p_net_io_server = new (std::nothrow) c_net_io_server();

    if(NULL == p_net_io_server)
    {
        return -1;
    }
    else
    {
        *pp_net_io_server = dynamic_cast<i_net_io_server*>(p_net_io_server);
        return 0;
    }
}

/**
 * @brief  构造函数
 * @param
 * @return
 */
c_net_io_server::c_net_io_server()
{
    memset(m_listen_ip, 0, sizeof(m_listen_ip));
    m_listen_port = 0;

    m_listen_socket = -1;
    m_epoll_fd = -1;

    m_p_events = NULL;
    m_connect_id = 0;
    m_connect_id_base = 1;
    m_connect_id_step = 1;
    m_id_conn_map.clear();

    m_errno = NETIOSERVER_IFACE_EOK;
    memset(&m_errstr, 0, sizeof(m_errstr));

    m_fd_flag = 0;
    m_inited = 0;
}

/**
 * @brief  析构函数
 * @param
 * @return
 */
c_net_io_server::~c_net_io_server()
{
    uninit();
}

/**
 * @brief  初始化函数
 * @param   listen_ip  服务端IP地址
 * @param  listen_port 服务端端口号
 * @param  p_events
 * @param  id_base 连接ID基值
 * @param  id_step 连接ID增长步长
 * @return  0success -1 failed
 */
int c_net_io_server::init(const char *listen_ip, const int listen_port, const i_net_io_server_events *p_events,
                          const int id_base = 1, const int id_step = 1)
{
    if(m_inited)
    {
        return -1;
    }

    if(NULL == listen_ip || listen_port < 0 || listen_port > 65535)
    {
        return -1;
    }

    if(id_base <= 0 || id_step <= 0)
    {
        return -1;
    }

    m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listen_socket < 0)
    {
	    set_err(NETIOSERVER_IFACE_EINIT, "socket failed(%s).", strerror(errno));
        return -1;
    }

    sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port);
    listen_addr.sin_addr.s_addr = inet_addr(listen_ip);

    if(listen_addr.sin_addr.s_addr == INADDR_NONE)
    {
	    set_err(NETIOSERVER_IFACE_EINIT, "listen ip(%s) failed.", listen_ip);
        return -1;
    }

    //设置地址重用
    int reuseaddr_flag = 1;
    if(setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_flag, sizeof(reuseaddr_flag)))
    {
        close(m_listen_socket);
        m_listen_socket = -1;
    	set_err(NETIOSERVER_IFACE_EINIT, "setsockopt failed(%s).",strerror(errno) );
        return -1;
    }

    if(bind(m_listen_socket, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0)
    {
        close(m_listen_socket);
        m_listen_socket = -1;
	    set_err(NETIOSERVER_IFACE_EINIT, "setsockopt failed(%s).",strerror(errno) );
        return -1;
    }

    //设置非阻塞模式
    int flags = fcntl(m_listen_socket, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(m_listen_socket, F_SETFL, flags);

    if(listen(m_listen_socket, 80) < 0)
    {
        close(m_listen_socket);
        m_listen_socket = -1;
	    set_err(NETIOSERVER_IFACE_EINIT, "listen failed(%s).",strerror(errno) );
        return -1;
    }

    m_epoll_fd = epoll_create(MAX_CLIENT_NUM);
    if(m_epoll_fd < 0)
    {
        close(m_listen_socket);
        m_listen_socket = -1;
    	set_err(NETIOSERVER_IFACE_EINIT, "epoll create failed(%s).",strerror(errno) );
        return -1;
    }

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLET | EPOLLIN;
    event.data.ptr = NULL;

    if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_socket, &event) < 0)
    {
        close(m_listen_socket);
        m_listen_socket = -1;

        close(m_epoll_fd);
        m_epoll_fd = -1;
	    set_err(NETIOSERVER_IFACE_EINIT, "epoll ctl failed(%s).",strerror(errno) );

        return -1;
    }

    strcpy(m_listen_ip, listen_ip);
    m_listen_port = listen_port;
    m_p_events = (i_net_io_server_events*)p_events;
    m_id_conn_map.clear();
    m_connect_id = m_connect_id_base = id_base;
    m_connect_id_step = id_step;

    m_fd_flag = 0;
    m_inited = 1;

    return 0;
}

/**
 * @brief 发送数据，将数据拷贝到相应连接的缓冲区,并没有实际发送。拷贝数据的长度为min(data_len,缓冲区空闲长度)
 * @param   connection_id 代表连接的ID
 * @param  p_data 要发送的数据
 * @param data_len 要发送的数据的长度
 * @return >=0 实际发送的字节数 -1failed
 */
int c_net_io_server::send_data(const int connection_id, char *p_data, const int data_len)
{
    if(!m_inited)
    {
        return -1;
    }

    if(NULL == p_data || data_len <= 0)
    {
        return -1;
    }

    id_conn_map_iter_t iter = m_id_conn_map.find(connection_id);
    if(iter == m_id_conn_map.end())
    {
	    set_err(NETIOSERVER_IFACE_ESEND, "not found connection %d", connection_id);
        return -1;
    }

    int remain_data_len = data_len;/**< 剩余的待发送的数据的长度*/
    int bytes_sent = 0;
    if(iter->second.buffer_data_len == 0)
    {
        /** 底层缓冲中没有数据，那么就直接发送数据*/
        bytes_sent = send(iter->second.sock_fd, p_data, remain_data_len, 0);
        if(bytes_sent > 0)
        {
           remain_data_len -= bytes_sent;
        }
        // baron add
        else
        {
            bytes_sent = 0;
        }
    }

    if(remain_data_len == 0)
    {
        return data_len;
    }

    int empty_buffer_len = sizeof(iter->second.send_buffer) - iter->second.buffer_data_len;

    int copy_len = 0;
    if(remain_data_len <= empty_buffer_len)
    {
        copy_len = remain_data_len;
    }
    else
    {
        copy_len = empty_buffer_len;
    }

    memcpy(iter->second.send_buffer + iter->second.buffer_data_len, p_data + bytes_sent, copy_len);
    iter->second.buffer_data_len += copy_len;
    return copy_len + bytes_sent;
}

/**
 * @brief  发送数据，将数据拷贝到发送缓冲区，如果缓冲去无足够空间，就不拷贝
 * @param  参数含义同上个函数
 * @return >=0 实际发送的字节数 -1failed
 */
int c_net_io_server::send_data_atomic(const int connection_id, char *p_data, const int data_len)
{
    if(!m_inited)
    {
        return -1;
    }

    if(NULL == p_data || data_len <= 0)
    {
        return -1;
    }

    id_conn_map_iter_t iter = m_id_conn_map.find(connection_id);
    if(iter == m_id_conn_map.end())
    {
	    set_err(NETIOSERVER_IFACE_ESEND, "not found connection %d", connection_id);
        return -1;
    }

    int empty_buffer_len = sizeof(iter->second.send_buffer) - iter->second.buffer_data_len;
    if(empty_buffer_len < data_len)
    {
        return 0;
    }

    int remain_data_len = data_len;/**< 剩余的待发送的数据的长度*/
    int bytes_sent = 0;
    if(iter->second.buffer_data_len == 0)
    {
        /** 底层缓冲中没有数据，那么就直接发送数据*/
        bytes_sent = send(iter->second.sock_fd, p_data, remain_data_len, 0);
        if(bytes_sent > 0)
        {
           remain_data_len -= bytes_sent;
        }
        // baron add
        else
        {
            bytes_sent = 0;
        }
    }

    if(remain_data_len == 0)
    {
        return data_len;
    }

    memcpy(iter->second.send_buffer + iter->second.buffer_data_len, p_data + bytes_sent, remain_data_len);
    iter->second.buffer_data_len += remain_data_len;
    return data_len;
}

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
int c_net_io_server::get_connection_info(const int connection_id, char *server_ip, int ip_buffer_len,
                                  int *p_port, int *p_connected_time, int *p_connection_fd)
{
    if(!m_inited)
    {
        return -1;
    }

    id_conn_map_iter_t iterator = m_id_conn_map.find(connection_id);
    if(iterator == m_id_conn_map.end())
    {
	    set_err(NETIOSERVER_IFACE_EQUERY, "not found connection %d", connection_id);
        return -1;
    }

    if(server_ip != NULL && ip_buffer_len >= 16)
    {
        strcpy(server_ip,iterator->second.peer_ip);
    }

    if(p_port != NULL)
    {
        *p_port = iterator->second.peer_port;
    }

    if(p_connected_time != NULL)
    {
        *p_connected_time = iterator->second.connected_time;
    }

    if(p_connection_fd != NULL)
    {
        *p_connection_fd = iterator->second.sock_fd;
    }

    return 0;
}

/**
 * @brief  枚举所有的连接ID
 * @param  p_recv_buffer 存放所有的连接信息id
 * @param  buffer_len 存放连接缓冲区的长度
 * @param p_data_len
 * @return  0success -1failed
 */
int c_net_io_server::enum_connections(int *p_recv_buffer, int buffer_len, int *p_data_len)
{
    if(!m_inited)
    {
        return -1;
    }

    if(NULL == p_recv_buffer || buffer_len < 1)
    {
        return -1;
    }

    int data_len = 0;

    id_conn_map_const_iter_t iter = m_id_conn_map.begin();
    for(; iter != m_id_conn_map.end(); iter++)
    {
        if(data_len >= buffer_len)
        {
            break;
        }
        *(p_recv_buffer + data_len) = iter->first;
        data_len++;
    }

    if(p_data_len != NULL)
    {
        *p_data_len = data_len;
    }

    return 0;
}

/**
 * @brief  关闭连接
 * @param   connection_id 要关闭的连接ID
 * @param   b_notify 关闭连接后是否通知上层
 * @return  0success -1failed
 */
int c_net_io_server::close_connection(const int connection_id, bool b_notify)
{
    if(!m_inited)
    {
        return -1;
    }

    id_conn_map_iter_t iter = m_id_conn_map.find(connection_id);
    if(iter == m_id_conn_map.end())
    {
	    set_err(NETIOSERVER_IFACE_ECLOSE, "not found connection %d", connection_id);
        return -1;
    }
    else
    {
        if(b_notify)
        {
            //通知上层连接已关闭
            if(m_p_events)
            {
                m_p_events->on_connection_closed((void*)dynamic_cast<c_net_io_server*>(this), iter->first, iter->second.sock_fd);
            }
        }

        close(iter->second.sock_fd);
        m_id_conn_map.erase(iter);

        return 0;
    }
}

/**
 * @brief  获取链接id
 * @param  无
 * @return 无
 */
int c_net_io_server::get_conn_id()
{
    int temp_id = m_connect_id;
    m_connect_id += m_connect_id_step;

    if(m_connect_id < 0)
    {
        m_connect_id = m_connect_id_base;
    }

    return temp_id;
}

/**
 * @brief  发送，接收网络数据
 * @param  const int time_interval, 超时限制值
 * @param  const const int flags, 控制网络操作的标志，可以为:NET_IO_SERVER_OP_ACCEPT,NET_IO_SERVER_OP_READ,NET_IO_SERVER_OP_WRITE
 * @return 成功返回0， 失败返回－1.
 */
int c_net_io_server::do_io(const int time_interval, const int flags)
{
    if(!m_inited)
    {
        return -1;
    }

    if(time_interval < 0)
    {
        return -1;
    }

    static epoll_event events[MAX_CLIENT_NUM];
    if((flags & NET_IO_SERVER_OP_READ) || (flags & NET_IO_SERVER_OP_ACCEPT))
    {
        int result = epoll_wait(m_epoll_fd, events, sizeof(events)/sizeof(epoll_event), time_interval);
        if(result > 0)
        {
            for(int index = 0; index < result; index++)
            {
                if(events[index].data.ptr == NULL || (index == 0 && m_fd_flag) )
                {
                    //新的连接到达
                    while(true)
                    {
                        sockaddr_in peer_addr;
                        memset(&peer_addr, 0, sizeof(peer_addr));
                        socklen_t addr_len = sizeof(peer_addr);
                        int peer_socket = accept(m_listen_socket, (struct sockaddr*)&peer_addr, &addr_len);
                        if(peer_socket >= 0)
                        {
                            if( !(flags & NET_IO_SERVER_OP_ACCEPT))
                            {
                                close(peer_socket);
                                continue;
                            }

                            static connection_info_t conn_info;
                            memset(&conn_info, 0, sizeof(conn_info));

                            conn_info.connection_id = get_conn_id();
                            conn_info.sock_fd = peer_socket;
                            strcpy(conn_info.peer_ip, inet_ntoa(peer_addr.sin_addr));
                            conn_info.peer_port = ntohs(peer_addr.sin_port);
                            conn_info.connected_time = (int)time(NULL);
                            conn_info.buffer_data_len = 0;
                            if(m_p_events)
                            {
                                if(-1 == m_p_events->on_new_connection((void*)dynamic_cast<c_net_io_server*>(this),
                                                conn_info.connection_id, conn_info.sock_fd, conn_info.peer_ip, conn_info.peer_port))
                                {
                                    close(peer_socket);
                                    continue;
                                }
                            }

                            int socket_flags = fcntl(peer_socket, F_GETFL);
                            socket_flags |= O_NONBLOCK;
                            fcntl(peer_socket, F_SETFL, socket_flags);

                            epoll_event event;
                            memset(&event, 0, sizeof(event));
                            event.events = EPOLLIN|EPOLLET;
                            pair<id_conn_map_iter_t, bool> id_pair = m_id_conn_map.insert(pair<int, connection_info_t>(conn_info.connection_id, conn_info));
                            if(id_pair.second)
			                {
			    	            event.data.ptr = &((id_pair.first)->second);
                            	if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, peer_socket, &event) < 0)
                            	{
					                set_err(NETIOSERVER_IFACE_EINIT, "epoll ctl failed(%s).", strerror(errno));
                                	close_connection(conn_info.connection_id, true);
                                	continue;
                            	}
			                }
			                else
			                {
				                set_err(NETIOSERVER_IFACE_EINIT, "it's not possible");
                                if(m_p_events)
                                {
                                    m_p_events->on_connection_closed((void*)dynamic_cast<c_net_io_server*>(this), conn_info.connection_id, peer_socket);
                                }
                                close(peer_socket);
                                continue;
			                }
                        }
                        else //accept 返回小于0
                        {
//                            if(errno == EMFILE || errno == ENFILE)
//                            {
//                                m_fd_flag = 1;
//                                break;
//                            }
//                            else if(errno == EAGAIN)
//                            {
//                                m_fd_flag = 0;
//                                break;
//                            }
//                            else if(errno == EINTR)
//                            {
//                                continue; // continue to accept
//                            }
//                            else
//                            {
//                                break;
//                            }

                            if(errno == EAGAIN)
                            {
                                m_fd_flag = 0;
                                break;
                            }
                            else if(errno == EINTR)
                            {
                                continue;
                            }
                            else
                            {
                                m_fd_flag = 1;
                                break;
                            }
                        }
                    }//while(true) to accept
                }

                if(events[index].data.ptr != NULL)
                {
                    //已经建立连接的socket
                    if(events[index].events & EPOLLIN)
                    {
                        while(true)
                        {
                            char recv_buffer[1024 * 4];
                            connection_info_t *p_conn =(connection_info_t*)events[index].data.ptr;
                            if(p_conn == NULL)
                            {
                                set_err(NETIOSERVER_IFACE_ERECV, "unknown error occured.");
                                break;
                            }

                            int bytes_recv = recv(p_conn->sock_fd, recv_buffer, sizeof(recv_buffer),0);
                            if(bytes_recv > 0)
                            {
                                if(m_p_events)
                                {
                                    if(m_p_events->on_recv_data((void*)dynamic_cast<c_net_io_server*>(this), p_conn->connection_id, p_conn->sock_fd, recv_buffer, bytes_recv) < 0)
                                    {
                                        //caller wish to close the connection
                                        close_connection(p_conn->connection_id,true);
                                        events[index].data.ptr = NULL;
                                        break;
                                    }
                                }

                                if(bytes_recv < (int)sizeof(recv_buffer))
                                {
                                    //没有更多的数据可接受
                                    break;
                                }
                            }
                            else if(bytes_recv == 0)
                            {
                                //对端关闭连接
                                close_connection(p_conn->connection_id, true);
                                events[index].data.ptr = NULL;
                                break;
                            }
                            else
                            {
                                if(errno == EAGAIN)
                                {
                                    //没有数据可读取了
                                    break;
                                }
                                else if(errno == EINTR)
                                {
                                    continue;
                                }
                                else
                                {
                                    close_connection(p_conn->connection_id, true);
                                    events[index].data.ptr = NULL;
                                    break;
                                }
                            }//bytes_recv == -1
                        }//while(true)
                    }//EPOLLIN

                    if((events[index].events & EPOLLERR) || (events[index].events & EPOLLHUP))
                    {
                        connection_info_t *p_conn =(connection_info_t*)events[index].data.ptr;
                        if(p_conn != NULL)
                        {
                            close_connection(p_conn->connection_id, true);
                        }
                    }//EPOLLERR EPOLLHUP
                }
            }//for(index < result; index++)
        }
        else if(result == -1)
        {
            if(errno == EINTR)
            {
                //do nothing , continue to send data
            }
            else
            {
                return -1;
            }
        }
        else
        {
            // do nothing, continue to send data
        }
    }//NET_IO_SERVER_OP_READ NET_IO_SERVER_OP_ACCEPT

    if(flags & NET_IO_SERVER_OP_WRITE)
    {
        //遍历每个连接，将各个连接缓冲区中的数据发送出去
        id_conn_map_iter_t id_iter = m_id_conn_map.begin();
        while(id_iter != m_id_conn_map.end())
        {
            if(id_iter->second.buffer_data_len > 0)
            {
                int bytes_sent = send(id_iter->second.sock_fd, id_iter->second.send_buffer, id_iter->second.buffer_data_len, 0);
                if(bytes_sent > 0)
                {
                    memmove(id_iter->second.send_buffer, id_iter->second.send_buffer + bytes_sent,
                            id_iter->second.buffer_data_len - bytes_sent);
                    id_iter->second.buffer_data_len -= bytes_sent;
                }
                else
                {
                    if( (errno != EAGAIN) && (errno != EINTR) )
                    {
                        //有错误发生
                     	id_conn_map_iter_t old_iter = id_iter;
			            id_iter++;
		                close_connection(old_iter->second.connection_id, true);
                        continue; //very important to continue
                    }
                }
            }//if buffer_data_len > 0

            id_iter++;
        }//while to iterate the while map
    }//NET_IO_SERVER_OP_WRITE

    return 0;
}

/**
 * @brief  反初始化
 * @param 无
 * @return 无
 */
int c_net_io_server::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    if(m_listen_socket >= 0)
    {
    	close(m_listen_socket);
        m_listen_socket = -1;
    }

    id_conn_map_iter_t iterator = m_id_conn_map.begin();
    while(iterator != m_id_conn_map.end())
    {
	    close_connection((iterator++)->first, true);
    }

    m_id_conn_map.clear();

    m_connect_id = 0;
    m_connect_id_base = 1;
    m_connect_id_step = 1;

    if(m_epoll_fd >= 0)
    {
    	close(m_epoll_fd);
    	m_epoll_fd = -1;
    }

    memset(m_listen_ip,0,sizeof(m_listen_ip));
    m_listen_port = 0;

    m_fd_flag = 0;
    m_inited = 0;

    return 0;
}

/**
 * @brief  释放本对象资源
 * @param  无
 * @return 无
 */
int c_net_io_server::release()
{
    delete this;
    return 0;
}

int c_net_io_server::get_last_errno()
{
	return m_errno;
}

const char* c_net_io_server::get_last_errstr()
{
	return m_errstr;
}

void c_net_io_server::set_err(int err_no, const char* msg, ...)
{
	m_errno = err_no;

	va_list arg;
	va_start(arg, msg);
	vsprintf(m_errstr, msg, arg);
	va_end(arg);
}
