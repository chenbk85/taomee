/**
 * @file reactor_unit_test.cpp
 * @brief 反应器单元测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-15
 */


#include <iostream>

#include "reactor.h"

int main(int argc, char** argv)
{
    ::setenv("USE_SELECT_REACTOR", "1", 0);
    reactor* p_reactor = reactor::instance();
    if (!p_reactor) {
        std::cerr << "reactor initialize failed" << std::endl;
        return -1;
    }

    if (p_reactor->reactor_init()) {
        std::cerr << "reactor_init failed" << std::endl;
        return -1;
    }

    while(true) {
        p_reactor->reactor_wait(1000);
    }

    p_reactor->reactor_fini();

    delete p_reactor;

    return 0;
}
