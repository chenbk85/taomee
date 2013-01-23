/**
 * @file c_net_io_notifier.h
 * @brief 网络IO框架通知机制
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2010-12-16
 */

#ifndef __C_NET_IO_NOTIFIER__
#define __C_NET_IO_NOTIFIER__

#include "i_net_io_server.h"

#include <stdio.h>
#include <stdarg.h>

/**
 * @brief 生成net_io_notifier实例
 *
 * @param pp_net_io_notifier 指向指向pp_net_io_notifier类型的指针的指针
 *
 * @return 0-成功 -1-失败
 */
int net_io_notifier_create(i_net_io_notifier **pp_net_io_notifier);

class c_net_io_notifier: public i_net_io_notifier
{
public:
    /**
     * @brief 构造函数
     */
    c_net_io_notifier();

    /**
     * @brief 析构函数
     */
    ~c_net_io_notifier();

    /**
     * @brief 初始化
     *
     * @param fd 文件描述符
     *
     * @return 0-成功 -1-失败
     */
    int init();

    /**
     * @brief 反初始化
     *
     * @return 0-成功 -1-失败
     */
    int uninit();

    /**
     * @brief 抛出事件
     *
     * @return
     */
    int popup();

    /**
     * @brief 确定接收时间
     *
     * @return
     */
    int got();

    /**
     * @brief 获取用于监听的文件描述父
     *
     * @return 返回私有文件描述符-成功 -1-失败
     */
    int get_eventfd() const;

    /**
     * @brief 释放本对象资源
     */
    int release();

private:
    /**
     * @brief 标记是否初始化
     */
    int m_init_;

    /**
     * @brief 通知机制使用的文件描述符
     */
    int m_wfd_;

    /**
     * @brief 通知机制使用的文件描述符
     */
    int m_rfd_;

    /**
     * @brief 原子操作对象整型内存单元
     */
    int *m_p_mutex_;
};

inline int c_net_io_notifier::get_eventfd() const
{
    if (!m_init_)
    {
        return -1;
    }

    return m_rfd_;
}

#endif //!__C_NET_IO_NOTIFIER__
