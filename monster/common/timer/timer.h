
#ifndef _TIMER_H
#define _TIMER_H

#include <time.h>

extern "C" {
#include "libtaomee/list.h"
}

/// 超时回调函数
typedef int (* timeout_callback_t)(void *owner, void *arg);

/**
 * @brief timer_event_t 定时器时间
 */
struct timer_event_t
{
    /**
     * @brief 链表结点，用于加入超时链表
     */
    list_head_t timer_node;
    /**
     * @brief 超时时间戳,>expire_time将触发回调函数
     */
    time_t expire_time;
    /**
     * @brief 回调函数
     */
    timeout_callback_t func;
    /**
     * @brief 回调函数作用对象
     */
    void *owner;
    /**
     * @brief 回调函数其他参数
     */
    void *arg;
};

int timer_init(list_head_t *timer);

int timer_uninit(list_head_t *timer);

int event_init(timer_event_t *event);

int timer_add_event(list_head_t *timer, timer_event_t *event,
        time_t expire_time, timeout_callback_t func,
        void *owner, void *arg);

int timer_check_event(list_head_t *timer);

int timer_del_event(timer_event_t *event);

int is_event_works(timer_event_t *event);

#endif // end of _TIMER_H
