/**
 * @file http_inspect.cpp
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-09
 */
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "http_serv.h"
#include "reactor.h"
#include "inet_address.h"
#include "http_inspector.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "usage: [ip] port" << std::endl;
        return 0;
    }

    std::vector<pid_t> pid_vec;
    pid_vec.push_back(getpid());

    http_inspector http_inspector_inst(pid_vec);

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

    http_serv_inst.set_http_callback(http_inspector_inst);

    if (http_serv_inst.start() < 0) {
        std::cout << "http serv start failed" << std::endl;
        return -1;
    }

    while (true) {
        reactor::instance()->reactor_wait(500);
    }

    return 0;
}
