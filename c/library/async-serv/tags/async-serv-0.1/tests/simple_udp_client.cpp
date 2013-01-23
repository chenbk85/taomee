/**
 * @file simple_udp_client.cpp
 * @brief udp客户端简单测试代码
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-10
 */
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <iostream>

#include "sock_dgram.h"
#include "inet_address.h"

static char g_sndbuf[4096] = {0};
static char g_rcvbuf[4096] = {0};

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "usage: client ip port" << std::endl;
        return 0;
    }

    ///初始化对端地址
    inet_address udp_serv_addr(argv[1], atoi(argv[2]));

    ///实例化udp收发类
    sock_dgram udp_dgram;

    if (udp_dgram.open_connecting() < 0) {
        std::cout << "sock_dgram::open_connecting failed" << std::endl;
        return -1;
    }

    ///设置非阻塞
    //udp_dgram.set_nonblocking();

    int len = sprintf(g_sndbuf, "process[%d] udp send data.\r\n", getpid());

    if (udp_dgram.write(udp_serv_addr, g_sndbuf, len) < 0) {
        std::cout << "sock_dgram::write failed: " << strerror(errno) << std::endl;
        return -1;
    }

    inet_address remote_addr;
    if (udp_dgram.read(remote_addr, g_rcvbuf, sizeof(g_rcvbuf)) < 0) {
        std::cout << "sock_dgram::read failed: " << strerror(errno) << std::endl;
        return -1;
    }

    std::cout << g_rcvbuf << std::endl;

    return 0;
}

