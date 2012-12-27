#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "timer.h"
extern "C" {
#include "libtaomee/list.h"
#include "libtaomee/log.h"
}

int timer_init(list_head_t *timer)
{
    if (timer == NULL) {
        return -1;
    }

    INIT_LIST_HEAD(timer);

    return 0;
}

int timer_uninit(list_head_t *timer)
{
    if (timer == NULL) {
        return -1;
    }

    list_head_t *pos, *next;
    list_for_each_safe(pos, next, timer) {
        timer_del_event((timer_event_t *)pos);
    }

    INIT_LIST_HEAD(timer);

    return 0;
}

int event_init(timer_event_t *event)
{
    if (event == NULL) {
        return -1;
    }

    event->expire_time = 0;
    event->func = NULL;
    event->owner = NULL;
    event->arg = NULL;
    INIT_LIST_HEAD(&event->timer_node);

    return 0;
}

int timer_add_event(list_head_t *timer, timer_event_t *event,
        time_t expire_time, timeout_callback_t func,
        void *owner, void *arg)
{
    if (timer == NULL || event == NULL) {
        return -1;
    }

    event->expire_time = expire_time;
    event->func = func;
    event->owner = owner;
    event->arg = arg;
    list_add_tail(&event->timer_node, (list_head_t *)timer);

    return 0;
}

int timer_check_event(list_head_t *timer)
{
    if (timer == NULL) {
        return -1;
    }

    time_t current_time = time(NULL);
    list_head_t *pos, *next;
    list_for_each_safe(pos, next, (list_head_t *)timer) {
        timer_event_t *event = (timer_event_t *)pos;
        if (event->expire_time >= current_time) {
            break;
        }
        list_del_init(pos);
        if (event->func) {
            event->func(event->owner, event->arg);
        }
    }

    return 0;
}

int timer_del_event(timer_event_t *event)
{
    if (event == NULL) {
        return -1;
    }

    list_del_init(&event->timer_node);
    event->expire_time = 0;
    event->func = NULL;
    event->owner = NULL;
    event->arg = NULL;

    return 0;
}

int is_event_works(timer_event_t *event)
{
    if (event == NULL) {
        return 0;
    }

    if (event->timer_node.prev == &event->timer_node
            && event->timer_node.next == &event->timer_node) {
        return 0;
    }

    return 1;
}
