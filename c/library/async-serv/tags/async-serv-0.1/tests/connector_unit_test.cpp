/**
 * @file connector_unit_test.hpp
 * @brief 连接器单元测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-17
 */
#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "connector.hpp"
#include "connection.hpp"
#include "connections_pool.hpp"
#include "inet_address.h"
#include "sock_dgram.h"
#include "sock_stream.h"
#include "sock_connector.h"

int main(int argc, char** argv)
{
    reactor* p_reactor = reactor::instance();
    if (!p_reactor) {
        std::cerr << "reactor::instance failed" << std::endl;
        return -1;
    }

    if (p_reactor->reactor_init()) {
        std::cerr << "reactor::reactor_init failed" << std::endl;
        return -1;
    }

    connections_pool<connection<sock_stream> >* p_pool = connections_pool<connection<sock_stream> >::instance();
    if (!p_pool) {
        std::cerr << "connections_pool::instance() failed" << std::endl;
        return -1;
    }

    inet_address serv_addr("127.0.0.1", 80);

    connections_pool<connection<sock_dgram> >* p_udp_pool = connections_pool<connection<sock_dgram> >::instance();
    if (!p_udp_pool) {
        std::cerr << "connections_pool::instance() failed" << std::endl;
        return -1;
    }

    printf("sock_stream conn pool[%p], sock_dgram conn pool[%p]\n", p_pool, p_udp_pool);
    fflush(stdout);

    connector<connection<sock_dgram>, sock_dgram > udp_conn;
    udp_conn.start();

    connector<connection<sock_stream>, sock_connector> tcp_conn(serv_addr);
    tcp_conn.start();

    while(1) {
        p_reactor->reactor_wait(500);
    }

    return 0;
}

