/**
 * @file timer.h
 * @brief 定时器类实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-11
 */

#ifndef _H_TIMER_H_
#define _H_TIMER_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "config.h"
#include "event_handler.h"
#include "reactor.h"


/**
 * @brief 定时器类
 */
class timer : public event_handler, boost::noncopyable {
public:

    /**<@定义定时器回调函数原型*/
    typedef boost::function<void (void*)> on_timer_callback_t;

    /**
     * @brief 定时器构造函数
     * @param cb  定时器回调函数
     * @param arg 定时器回调函数参数
     * @param expired_time 定时器到期时间
     * @param type 定时器类型
     * @param time_interval  定时器响应时间间隔(只对repeat类型的定时器才有效)
     */
    timer(const on_timer_callback_t& cb, void* arg, u_int expired_time, int type, u_int time_interval = 0)
        : m_timer_cb_(cb),
          m_cb_arg_(arg),
          m_timer_id_(NULL),
          m_type_(type),
          m_expired_time_(expired_time),
          m_time_interval_(time_interval),
          m_p_reactor_(reactor::instance())
    {

    }


    virtual ~timer() { }

    /**
     * @brief 定时器回调函数，超时后反应器会调用定时器中此函数
     */
    virtual int handle_timer();

    /**
     * @brief 获得定时器ID
     * @return 定时器ID
     */
    const timer_id_t get_timer_id() const { return m_timer_id_; }

    /**
     * @brief 向反应器注册定时器
     * @return -1failed, 0success
     */
    timer_id_t register_timer();

    /**
     * @brief 从反应器中删除定时器
     * @return NULL
     */
    void remove_timer();

private:
    on_timer_callback_t m_timer_cb_;
    void* m_cb_arg_;

    timer_id_t m_timer_id_;
    int m_type_;
    u_int m_expired_time_;
    u_int m_time_interval_;

    reactor* m_p_reactor_;

} __attribute__((aligned(sizeof(long))));

#endif

