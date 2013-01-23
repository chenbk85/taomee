/**
 * @file acceptor_unit_test.cpp
 * @brief 接受器单元测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-18
 */
#include <iostream>

#include "acceptor.hpp"
#include "inet_address.h"
#include "sock_dgram.h"
#include "sock_acceptor.h"
#include "sock_stream.h"
#include "connections_pool.hpp"
#include "connection.hpp"

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

    connections_pool<connection<sock_stream> >* p_tcp_pool = connections_pool<connection<sock_stream> >::instance();
    if (!p_tcp_pool) {
        std::cerr << "connections_pool::instance() failed" << std::endl;
        return -1;
    }

    inet_address serv_addr("127.0.0.1", 8080);
    inet_address serv_addr1("127.0.0.1", 8081);
    inet_address serv_addr2("127.0.0.1", 8082);

    connections_pool<connection<sock_dgram> >* p_udp_pool = connections_pool<connection<sock_dgram> >::instance();
    if (!p_udp_pool) {
        std::cerr << "connections_pool::instance() failed" << std::endl;
        return -1;
    }

    acceptor<connection<sock_stream>, sock_acceptor> tcp_acceptor(serv_addr);
    tcp_acceptor.start();
    tcp_acceptor.enable_accepting();

    acceptor<connection<sock_stream>, sock_acceptor> tcp_acceptor1(serv_addr1);
    tcp_acceptor1.start();
    tcp_acceptor1.enable_accepting();

    acceptor<connection<sock_stream>, sock_acceptor> tcp_acceptor2(serv_addr2);
    tcp_acceptor2.start();
    tcp_acceptor2.enable_accepting();

    inet_address udp_addr("127.0.0.1", 8080);
    acceptor<connection<sock_dgram>, sock_dgram> udp_acceptor(udp_addr);
    udp_acceptor.start();

    while (true) {
        p_reactor->reactor_wait(1000);
    }

    return 0;
}

