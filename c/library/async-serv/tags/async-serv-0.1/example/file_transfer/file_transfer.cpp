/**
 * @file file_transfer.cpp
 * @brief 文本传输示例，测试大包发送性能
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-26
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <string>

#include "reactor.h"
#include "sock_acceptor.h"
#include "sock_stream.h"
#include "acceptor.hpp"
#include "connection.hpp"

static int g_file_fd = -1;
static char g_sndbuf[8192] = {0};
std::string g_file_path;

int on_new_conn(connection<sock_stream>* p_conn)
{

    if ((g_file_fd = open(g_file_path.c_str(), O_RDONLY)) < 0) {
        std::cerr << "open failed " << strerror(errno) << std::endl;
        return -1;
    }

    int len = 0;
    if ((len = read(g_file_fd, g_sndbuf, sizeof(g_sndbuf))) < 0) {
        std::cout << "read failed" << std::endl;
        return -1;
    }

    if (p_conn->send_data(g_sndbuf, len) < 0)
        return -1;

    return 0;
}

int on_write_comple(connection<sock_stream>* p_conn)
{
    int len = 0;
    if ((len = read(g_file_fd, g_sndbuf, sizeof(g_sndbuf))) <= 0) {
        close(g_file_fd);
        g_file_fd = -1;
        p_conn->shutdown();
        return -1;
    } else {
        if (p_conn->send_data(g_sndbuf, len) < 0) {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "usage: file_name" << std::endl;
        return 0;
    }

    g_file_path = argv[1];


    inet_address local_addr(8080);

    acceptor<connection<sock_stream>, sock_acceptor> tcp_acceptor(local_addr);
    reactor* p_reactor = reactor::instance();
    p_reactor->reactor_init();

    if (tcp_acceptor.start() < 0)
        return -1;

    tcp_acceptor.set_new_conn_callback(on_new_conn);
    tcp_acceptor.set_on_write_comple_callback(on_write_comple);

    tcp_acceptor.enable_accepting();

    while(1) {
        p_reactor->reactor_wait(500);
    }

    return 0;

}


