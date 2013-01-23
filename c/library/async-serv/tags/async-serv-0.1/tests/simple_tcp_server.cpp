/**
 * @file simple_server.cpp
 * @brief 简单的网络服务端测试程序
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-09
 */
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "sock_stream.h"
#include "sock_acceptor.h"
#include "inet_address.h"

static char g_rcvbuf[4096] = {0};

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3) {
        std::cout << "usage: server [ip] port" << std::endl;
        return -1;
    }

    ///初始化连接本地end point地址
    inet_address local_addr;

    if (argc == 2) {
        local_addr.set_port(atoi(argv[1]));
    } else if (argc == 3) {
        local_addr.set_ip_addr(argv[1]);
        local_addr.set_port(atoi(argv[2]));
    } else {
        //do nothing
    }


    ///初始化接收器
    sock_acceptor serv_acceptor(local_addr);

    if (serv_acceptor.open_listening() < 0) {
        std::cout << "serv_acceptor::open_listening failed" << std::endl;
        return -1;
    }

    while (true) {
        sleep(3);

        inet_address remote_addr;
        int cli_fd = -1;
        sock_stream serv_stream;

        int rv = serv_acceptor.accept(cli_fd, remote_addr);
        if (rv == ACCEPT_ERR) {
            std::cout << "serv_acceptor::accept failed" << std::endl;
            return -1;
        }  else if (rv == ACCEPT_EMFILE) {
            std::cout << "serv_acceptor::accept fd run out" << std::endl;
            continue;
        } else {
            // do nothing
        }

        serv_stream.set_fd(cli_fd);

        ///设置非阻塞
        //serv_stream.set_nonblocking();

        int len = serv_stream.read(g_rcvbuf, sizeof(g_rcvbuf));
        if (len < 0) {
            std::cout << "serv_stream::read failed, len: " << len << std::endl;
            continue;
        }

        serv_stream.write(g_rcvbuf, len);

    }

    return 0;
}

