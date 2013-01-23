/**
 * @file timer_test.cpp
 * @brief 各种类型的定时器的测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-07
 */
#include <iostream>

#include "reactor.h"
#include "timer.h"

void test_timer_once(void* arg)
{
    std::cout << "TIMER ONCE EXPIRED" << std::endl;
}

void test_timer_repeat(void* arg)
{
    std::cout << "TIMER REPEAT EXPIRED" << std::endl;
}

void test_timer_repeat_exact(void* arg)
{
    std::cout << "TIMER REPEAT_EXACT EXPIRED " << std::endl;
}

int main()
{
    timer timer_once(test_timer_once,
                     NULL,
                     time(NULL) + 1,
                     TIMER_ONCE);
    timer timer_repeat(test_timer_repeat,
                       NULL,
                       time(NULL) + 1,
                       TIMER_REPEAT,
                       1);
    timer timer_repeat_exact(test_timer_repeat_exact,
                             NULL,
                             time(NULL) + 1,
                             TIMER_REPEAT_EXACT,
                             2);

    reactor::instance()->reactor_init();

    timer_once.register_timer();
    timer_repeat.register_timer();
    timer_repeat_exact.register_timer();

    int i = 1000;

    while (i-- > 0) {
        reactor::instance()->reactor_wait(500);
        if (i <= 950) {
            timer_repeat.remove_timer();
        }

        if (i <= 920) {
            timer_repeat_exact.remove_timer();
        }
    }


    return 0;

}
