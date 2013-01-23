/**
 * @file http_server_test.cpp
 * @brief http服务端测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */
#include <stdlib.h>
#include <iostream>

#include "http_serv.h"
#include "reactor.h"
#include "inet_address.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "usage: [ip] port" << std::endl;
        return 0;
    }

    inet_address local_addr;
    if (argc == 2) {
        local_addr.set_port(atoi(argv[1]));
    } else {
        local_addr.set_ip_addr(argv[1]);
        local_addr.set_port(atoi(argv[2]));
    }

    if (reactor::instance()->reactor_init() < 0) {
        std::cerr << "reactor init failed" << std::endl;
        return -1;
    }

    http_serv http_serv_inst(local_addr);
    if (http_serv_inst.start() < 0) {
        std::cout << "http serv start failed" << std::endl;
        return -1;
    }

    while (true) {
        reactor::instance()->reactor_wait(500);
    }

    return 0;
}
