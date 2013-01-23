/**
 * @file reactor.h
 * @brief 反应器类(singleton模式)
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-03
 */

#ifndef _H_REACTOR_H_
#define _H_REACTOR_H_

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>

#include "config.h"
#include "reactor_impl.h"
#include "timer_queue.h"
#include "event_handler.h"

class reactor_impl;
class timer_impl;
class timer_queue;

/**
 * @brief 反应器，无法拷贝，单件模式
 * @brief 为了使得反应器能及时唤醒，而不是阻塞在事件等待中，反应器加入了管道通知
 */
class reactor : public event_handler, boost::noncopyable {
public:
    typedef boost::function<void()> functor_t;

    ~reactor() {
        m_p_instance_ = NULL;
    }

    /**
     * @brief 反应器初始化
     * @return -1failed, 0success
     */
    int reactor_init();

    /**
     * @brief 反应器反初始化
     * @return -1failed, 0success
     */
    int reactor_fini();

    /**
     * @brief 将所提交的事件排队
     * @param func  所提交的事件
     * @return NULL
     */
    void queue_in_loop(const functor_t func);

    /**
     * @brief 管道读事件回调函数
     * @return -1failed, 0success
     */
    virtual int handle_read();

    /**
     * @brief 获得管道fd
     * @return fd
     */
    virtual int get_fd() {
        return m_wake_fd_[0];
    }

    /**
     * @brief 通过管道唤醒阻塞在事件等待中的进程
     * @return NULL
     */
    void wakeup();

    /**
     * @brief 向反应器注册事件
     * @param h  事件指针，此事件将注册到反应器中等待反应器有事件时回调
     * @param t  事件类型
     * @return -1failed, 0success
     */
    int register_event(event_handler* h, int t);

    /**
     * @brief 向反应器注册定时事件
     * @param h  定时事件指针
     * @param t  定时事件类型
     * @param expired_time 定时器超时时间
     * @param time_interval 定时器响应时间间隔
     * @return NULL failed, success return timer_id_t
     */
    timer_id_t register_timer(event_handler* h,
                              int t,
                              u_int expired_time,
                              u_int time_interval);

    /**
     * @brief 更新已注册事件
     * @param h  事件指针
     * @param t  事件类型
     * @return -1failed, 0success
     */
    int update_event(event_handler* h, int t);

    /**
     * @brief 删除已注册事件
     * @param h  事件指针
     * @return -1failed, 0success
     */
    int remove_event(event_handler* h);

    /**
     * @brief 删除已注册定时器事件
     * @param id  定时器ID
     * @return -1failed, 0success
     */
    int remove_timer(timer_id_t* id);

    /**
     * @brief 反应器等待事件
     * @param timeout  所等待的超时时间(ms)
     * @return -1failed, 0success
     */
    int reactor_wait(int imeout);

    /**
     * @brief 获取反应器实例指针
     * @return 反应器对象实例指针
     */
    static reactor* instance();

protected:
    reactor();

private:
    static reactor* m_p_instance_;  /**<@反应器实例指针*/

    std::vector<functor_t> m_pending_func_;

    boost::scoped_ptr<reactor_impl> m_reactor_impl_;  /**<@具体反应器指针*/
    boost::scoped_ptr<timer_queue> m_timer_queue_impl_;  /**<@具体定时器指针*/

    int m_wake_fd_[2];  /**<@管道fd*/
    char m_buffer_[4096];  /**<@管道buffer*/
};


#endif
