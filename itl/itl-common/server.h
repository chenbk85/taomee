/** 
 * ========================================================================
 * @file server.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-15
 * Modify $Date: 2012-10-16 15:54:40 +0800 (二, 16 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_SERVER_H_2012_10_15
#define H_SERVER_H_2012_10_15

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>


#include "itl_timer.h"
#include "itl_proto.h"

typedef int (*dispatch_func_t)(int fd, const char * buf, uint32_t len);
typedef int (*on_connected_func_t)();


class c_server : public timer_head_t
{
    public:

        c_server();
        c_server(uint32_t auto_reconnect_interval);
        c_server(const char * ip, uint32_t port, uint32_t auto_reconnect_interval);

        ~c_server();

        int connect(const char * ip, uint32_t port);
        int connect();
        int disconnect();

        int send(const void * buf, uint32_t len);
        int send(const void * head, Cmessage * p_out);

        int dispatch(dispatch_func_t dispatch_func, const char * buf, uint32_t len);

        inline bool check_fd(int fd) const
        {
            return (fd >= 0 && m_fd == fd);
        }

        inline bool is_connected() const
        {
            return (-1 != m_fd);
        }

        inline const char * get_ip() const
        {
            return m_ip;
        }

        inline uint32_t get_port() const
        {
            return m_port;
        }

        // 设置自动重连间隔
        // 间隔为0表示不开启自动重连
        // 默认是不开启的
        int set_auto_reconnect(uint32_t interval);
        int connect_timely();
        int on_server_down();
        inline int set_on_connected_func(on_connected_func_t func)
        {
            m_on_connected_func = func;
            return 0;
        }

    private:


    private:

        int m_fd;
        char m_ip[INET_ADDRSTRLEN];
        uint32_t m_port;

    private:

        uint32_t m_auto_reconnect_interval;
        timer_struct_t * m_reconnect_timer;

    private:

        on_connected_func_t m_on_connected_func;


};



#endif
