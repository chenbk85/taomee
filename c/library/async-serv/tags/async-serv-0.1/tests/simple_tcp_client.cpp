/**
 * @file simple_client.cpp
 * @brief 简单客户端测试代码
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-08
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "sock_connector.h"
#include "sock_stream.h"
#include "inet_address.h"

static char g_sndbuf[4096] = {0};
static char g_rcvbuf[4096] = {0};

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "usage: client ip_addr port" << std::endl;
        return 0;
    }

    ///初始化连接对端end point地址
    inet_address serv_addr(argv[1], atoi(argv[2]));

    ///初始化socket连接类
    sock_connector client_conn(serv_addr);
    ///初始化socket收发类
    sock_stream client_stream;

    if (client_conn.open_connecting() < 0) {
        std::cout << "sock_connector::open_connecting failed" << std::endl;
        return -1;
    }

    ///设置非阻塞
    //client_conn.set_nonblocking();

    ///发起主动连接
    int rv;
    if ((rv = client_conn.open_connecting()) == IS_CONNECTING) {
        std::cout << "is connecting" << std::endl;
        return -1;
    } else if (rv == CONN_TEMP_UNAVAIL) {
        std::cout << "connection temperally unavailable" << std::endl;
        return -1;
    } else if (rv == CONN_ERR) {
        std::cout << "connection error" << std::endl;
        return -1;
    } else {
        std::cout << "connect success" << std::endl;
    }

    client_stream.set_fd(client_conn.get_fd());
    client_conn.set_fd(-1);
    int len = sprintf(g_sndbuf, "process[%d] send data.\r\n", getpid());

    ///发送数据
    if (client_stream.write(g_sndbuf, len) < 0) {
        std::cout << "sock_stream::write() failed" << std::endl;
        return -1;
    }

    ///接收数据
    if ((len = client_stream.read(g_rcvbuf, sizeof(g_rcvbuf))) < 0) {
        std::cout << "sock_stream::read() failed" << std::endl;
        return -1;
    }

    std::cout << g_rcvbuf << std::endl;

    return 0;
}

