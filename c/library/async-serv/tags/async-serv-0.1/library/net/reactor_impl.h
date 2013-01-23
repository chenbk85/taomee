/**
 * @file reactor_impl.h
 * @brief 具体反应器实现抽象基类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-10
 */
#ifndef _H_REACTOR_IMPL_H_
#define _H_REACTOR_IMPL_H_

#include "config.h"

class event_handler;

class reactor_impl {
public:
    virtual ~reactor_impl() { }

    /**
     * @brief 具体反应器初始化
     * @return -1failed, 0success
     */
    virtual int reactor_impl_init() = 0;

    /**
     * @brief 具体反应器反初始化
     * @return -1failed, 0success
     */
    virtual int reactor_impl_fini() = 0;

    /**
     * @brief 向具体反应器注册事件
     * @param h  事件指针，此事件将注册到反应器中等待反应器有事件时回调
     * @param t  事件类型
     * @return -1failed, 0success
     */
    virtual int register_event(event_handler* h, int t) = 0;

    /**
     * @brief 向具体反应器更新已注册事件
     * @param h  事件指针
     * @param t  事件类型
     * @return -1failed, 0success
     */
    virtual int update_event(event_handler* h, int t) = 0;

    /**
     * @brief 从具体反应器删除已注册事件
     * @param h  事件指针
     * @return -1failed, 0success
     */
    virtual int remove_event(event_handler* h) = 0;

    /**
     * @brief 在具体反应器中等待事件
     * @param timeout  所等待的超时时间(ms) -1不等待；0无限等待；timeout有限等待
     * 时间
     * @return -1failed, 0success
     */
    virtual int reactor_impl_wait(int timeout) = 0;

    /**
     * @brief 根据环境变量的设置实例化具体反应器对象，默认是epoll
     * return 反应器对象指针
     */
    static reactor_impl* new_reactor_impl();
};

#endif
