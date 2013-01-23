/**
 * @file simple_udp_server.cpp
 * @brief 简单udp服务程序测试代码
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-10
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>

#include "sock_dgram.h"
#include "inet_address.h"

static char g_sndbuf[4096] = {0};
static char g_rcvbuf[4096] = {0};

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3) {
        std::cout << "usage: serv [ip] port" << std::endl;
        return 0;
    }

    inet_address local_addr;
    if (argc == 2) {
        local_addr.set_port(atoi(argv[1]));
    } else if (argc == 3) {
        local_addr.set_ip_addr(argv[1]);
        local_addr.set_port(atoi(argv[2]));
    } else {
        //do nothing
    }

    ///实例化udp收发类
    sock_dgram udp_dgram(local_addr);

    if (udp_dgram.open_listening() < 0) {
        std::cerr << "sock_dgram::open_listening failed" << std::endl;
        return -1;
    }

    while (true) {
        sleep(3);
        inet_address remote_addr;
        int len = 0;
        if ((len = udp_dgram.read(remote_addr, g_rcvbuf, sizeof(g_rcvbuf))) < 0) {
            std::cerr << "sock_dgram::read failed: " << strerror(errno) << std::endl;
            continue;
        }

        udp_dgram.write(remote_addr, g_rcvbuf, len);
    }

    return 0;
}



