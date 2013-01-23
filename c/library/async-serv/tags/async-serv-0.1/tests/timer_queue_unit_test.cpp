/**
 * @file timer_queue_unit_test.cpp
 * @brief 定时器单元测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-15
 */
#include "time.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "timer.h"
#include "reactor.h"

void test_timer(void* arg)
{
    char* p = (char*)arg;
    std::cout << "time " << p << " now " << time(NULL) << std::endl;
}

int main(int argc, char** argv)
{
    reactor* p_rt = reactor::instance();
    if (!p_rt) {
        std::cerr << "reactor::instance() failed" << std::endl;
        return -1;
    }

    if (p_rt->reactor_init()) {
        std::cerr << "reactor_init failed" << std::endl;
        return -1;
    }

    boost::shared_ptr<timer> test_timer1(new timer(boost::bind(&test_timer, _1),
                                                   (void*)"timer_1",
                                                   time(NULL) + 10,
                                                   TIMER_REPEAT,
                                                   1));

    timer_id_t id = test_timer1->register_timer();

    std::cout << "timer_id " << id << std::endl;

    boost::shared_ptr<timer> test_timer2(new timer(boost::bind(&test_timer, _1),
                                                   (void*)"timer_2",
                                                   time(NULL) + 15,
                                                   TIMER_ONCE));

    timer_id_t id_2 = test_timer2->register_timer();
    std::cout << "timer_id" << id_2 << std::endl;

    int loop = 50;
    while (loop > 0) {
        reactor::instance()->reactor_wait(500);
        loop--;
    }

    p_rt->reactor_fini();

    delete p_rt;
    return 0;
}

