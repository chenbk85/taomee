/**
 * @file net_if.h
 * @brief 网络接口函数
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-05
 */

#ifndef _H_NET_IF_H_
#define _H_NET_IF_H_

#include <string>
#include <stdint.h>
#include "stdlib.h"

enum timer_type_t {
    kTIMER_ONCE = 1,
    kTIMER_REPEAT = 2,
    kTIMER_REPEAT_EXACT = 4
};


void* tcp_connect_to_svr(const std::string& ip_addr, u_short port);

void* create_udp_connect();

int tcp_net_send(uint32_t conn_id, void* data, const int len);

int udp_net_send(uint32_t conn_id, const std::string& remote_ip, u_short remote_port,
        void* data, const int len);

void tcp_close_serv_connection(void* connector_handler);
void udp_close_serv_connection(void* connector_handler);

void tcp_close_client_connection(uint32_t conn_id);

void* register_timer(void on_timer(void*),
                          void* arg,
                          u_int expired_time,
                          timer_type_t type,
                          u_int time_interval = 0);

void remove_timer(void* timer_handler);


#endif
