/**
 * @file timer_queue.cpp
 * @brief 定时器队列管理类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-12
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "timer_queue.h"
#include "event_handler.h"


int timer_queue::timer_queue_init()
{
    m_p_timer_queue_ = (timer_queue_t*) ::calloc(1, sizeof(timer_queue_t));
    if (!m_p_timer_queue_) {
        ERROR_LOG("timer_queue_init failed");
        return -1;
    }

    INIT_LIST_HEAD(&m_p_timer_queue_->timer_list_head);
    m_p_timer_queue_->timer_num = 0;

    return 0;
}

int timer_queue::timer_queue_fini()
{
    if (m_p_timer_queue_) {
        struct list_head* p = NULL;
        timer_node_t* t = NULL;
        list_for_each(p, &m_p_timer_queue_->timer_list_head) {
            t = list_entry(p, timer_node_t, timer_list_node);
            struct list_head tmp = *p;
            list_del(p);
            ::free(t);
            t = NULL;
            p = &tmp;
        }

        ::free(m_p_timer_queue_);
        m_p_timer_queue_ = NULL;
    }

    return 0;
}

timer_id_t timer_queue::register_timer(event_handler* h,
                                       int t,
                                       u_int expired_time,
                                       u_int time_interval)
{
    timer_node_t* node = (timer_node_t*) ::calloc(1, sizeof(timer_node_t));
    if (!node) {
        ERROR_LOG("timer_node_t calloc failed");
        return NULL;
    }

    node->timer_handler = h;
    node->expired_time = expired_time;
    node->type = t;
    node->time_interval = time_interval;

    INIT_LIST_HEAD(&node->timer_list_node);

    m_p_timer_queue_->timer_num++;

    ///将新节点加入链表
    if (list_empty(&m_p_timer_queue_->timer_list_head)) {
        list__add(&node->timer_list_node, &m_p_timer_queue_->timer_list_head);
    } else {
        struct list_head* p = NULL;
        timer_node_t* t = NULL;
        list_for_each(p, &m_p_timer_queue_->timer_list_head) {
            t = list_entry(p, timer_node_t, timer_list_node);
            if (t->expired_time >= node->expired_time) {
                list__add(&node->timer_list_node, t->timer_list_node.prev);
                goto out;
            }
        }
        list__add_tail(&node->timer_list_node, &m_p_timer_queue_->timer_list_head);
    }

out:
    return (timer_id_t)node;
}

int timer_queue::remove_timer(timer_id_t* timer_id)
{
    ///从链表中摘掉定时器节点
    struct list_head* p = NULL;

    ///从参数中获得的timer_id有可能已经失效，需要遍历判断
    list_for_each(p, &m_p_timer_queue_->timer_list_head) {
        timer_node_t* node = list_entry(p, timer_node_t, timer_list_node);
        if (node == (timer_node_t*)(*timer_id)) {
            list_del(&node->timer_list_node);

            ::free(node);
            node = NULL;
            *timer_id = NULL;
            m_p_timer_queue_->timer_num--;
            break;
        }
    }

    return 0;
}

int timer_queue::timer_wait()
{
    while (1) {
        if (list_empty(&m_p_timer_queue_->timer_list_head)) {
            return 0;
        }

       timer_node_t* node = list_entry(m_p_timer_queue_->timer_list_head.next,
                                       timer_node_t,
                                       timer_list_node);
        if (!node)
            return -1;

        u_int now = time(NULL);
        if (node->expired_time > now) {
            break;
        } else {
            /*从定时器链表中摘除*/
            list_del(&node->timer_list_node);

            if (node->type == TIMER_REPEAT) {
                node->expired_time += node->time_interval;
            } else if (node->type == TIMER_REPEAT_EXACT) {
                node->expired_time = time(NULL) + node->time_interval;
            }

            (node->timer_handler)->handle_timer();

            if (node->type == TIMER_ONCE) {
                ::free(node);
                node = NULL;
                m_p_timer_queue_->timer_num--;
            } else if (node->type == TIMER_REPEAT
                    || node->type == TIMER_REPEAT_EXACT) {
                ///调整定时器链表顺序，使定时器时间始终按从小到大排列
                timer_list_adjust(node);
            }
        }
    }

    return 0;
}

int timer_queue::timer_list_adjust(timer_node_t* node)
{
    if (list_empty(&m_p_timer_queue_->timer_list_head)) {
        list__add(&node->timer_list_node, &m_p_timer_queue_->timer_list_head);
    } else {
        timer_node_t* p = NULL;
        list_for_each_entry(p, &m_p_timer_queue_->timer_list_head, timer_list_node) {
            if (p->expired_time >= node->expired_time) {
                list__add(&node->timer_list_node, p->timer_list_node.prev);
                return 0;
            }
        }
        list__add_tail(&node->timer_list_node, &m_p_timer_queue_->timer_list_head);
    }
    return 0;
}

