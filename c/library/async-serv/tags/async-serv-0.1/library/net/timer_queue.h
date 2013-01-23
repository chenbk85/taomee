/**
 * @file timer_queue.h
 * @brief 定时器队列管理类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-12
 */
#ifndef _H_TIMER_QUEUE_H_
#define _H_TIMER_QUEUE_H_

#include <unistd.h>
#include <stdlib.h>

#include <boost/noncopyable.hpp>

#include "config.h"
#include "list.h"

class event_handler;

/**
 * @brief 定时器采用优先级队列管理，到期时间近的定时器排在队首，久的排在队尾。同
 * 时以timer_id为key串到红黑树中，便于快速定位
 */
class timer_queue : boost::noncopyable{
public:
    typedef struct {
        struct list_head timer_list_node;   /**<@定时器链表节点*/
        event_handler* timer_handler;   /**<@事件回调句柄*/
        u_int expired_time;    /**<@定时器过期时间*/
        int type;       /**<@定时器类型*/
        u_int time_interval;   /**<@定时器定时时间间隔*/
    } timer_node_t;

    timer_queue() : m_p_timer_queue_(NULL) { }
    ~timer_queue() { }

    /**
     * @brief 定时器队列初始化
     * @return -1failed, 0success
     */
    int timer_queue_init();

    /**
     * @brief 定时器队列反初始化
     * @return -1failed, 0success
     */
    int timer_queue_fini();

    /**
     * @brief 向定时器队列注册定时器
     * @param h  事件回调句柄
     * @param t  定时器类型
     * @param expired_time  定时器到期时间
     * @param time_interval 定时器定时时间间隔
     * @return NULL failed, timer_id_t 定时器id
     */
    timer_id_t register_timer(event_handler* h,
                              int t,
                              u_int expired_time,
                              u_int time_interval);

    /**
     * @brief 从定时器队列中删除定时器
     * @param timer_id  定时器id
     * @return -1failed, 0success
     */
    int remove_timer(timer_id_t* timer_id);

    /**
     * @brief 定时器队列轮询判断时候有超时定时器
     * @return -1failed, 0success
     */
    int timer_wait();

private:

    /**
     * @brief 调整定时器队列，使其按照过期时间排序
     * @return -1failed, 0success
     */
    int timer_list_adjust(timer_node_t* node);

private:

    typedef struct {
        struct list_head timer_list_head; /**<@定时器链表头节点*/
        int timer_num;  /**<@定时器队列中定时器个数*/
    } timer_queue_t;

    timer_queue_t* m_p_timer_queue_;
};

#endif
