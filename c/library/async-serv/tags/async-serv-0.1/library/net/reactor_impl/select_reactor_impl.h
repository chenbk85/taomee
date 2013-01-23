/**
 * @file select_reactor_impl.h
 * @brief 基于select的反应器实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-11
 */
#ifndef _H_SELECT_REACTOR_IMPL_H_
#define _H_SELECT_REACTOR_IMPL_H_

#include <sys/select.h>

#include "../reactor_impl.h"

class select_reactor_impl : public reactor_impl {
public:
    select_reactor_impl();
    virtual ~select_reactor_impl();

    virtual int reactor_impl_init();
    virtual int reactor_impl_fini();

    virtual int register_event(event_handler* h, int t);
    virtual int update_event(event_handler* h, int t);
    virtual int remove_event(event_handler* h);

    virtual int reactor_impl_wait(int timeout);

private:
    void** m_p_event_vec_;  /**<@管理event_handler的数组*/

    int m_max_fd_;  /**<@当前的最大fd*/

    typedef struct {
        fd_set old_rd_set, old_wr_set, old_err_set;
        fd_set rd_set, wr_set, err_set;
    } fd_struct_t;

    /**<@fd_set结构体，由于每次select后相应的注册事件都会失效，因此需要另一组fd_set来管理注册事件*/
    fd_struct_t m_fd_struct_;
};

#endif
