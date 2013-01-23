/**
 * @file timer.cpp
 * @brief 定时器实现类实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-12
 */

#include "log.h"
#include "timer.h"

int timer::handle_timer()
{
    if (m_timer_cb_)
        m_timer_cb_(m_cb_arg_);

    return 0;
}

timer_id_t timer::register_timer()
{

    if (m_timer_id_) {
        ERROR_LOG("timer already registered");
        return NULL;
    }

    if ((m_timer_id_ = m_p_reactor_->register_timer(this, m_type_, m_expired_time_, m_time_interval_)) == NULL) {
        ERROR_LOG("reactor::register_timer failed");
        return NULL;
    }

    return m_timer_id_;
}

void timer::remove_timer()
{
    if (m_timer_id_)
        m_p_reactor_->remove_timer(&m_timer_id_);
}

