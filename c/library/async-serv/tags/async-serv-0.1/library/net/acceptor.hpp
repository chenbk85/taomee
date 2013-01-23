/**
 * @file acceptor.hpp
 * @brief 接受器模块
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-04
 */

#ifndef _H_ACCEPTOR_H_
#define _H_ACCEPTOR_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "inet_address.h"
#include "connections_pool.hpp"
#include "reactor.h"
#include "config.h"
#include "ipc_traits.hpp"
#include "event_handler.h"

/**
 * @brief 接受器，此接受器拥有多个连接
 * @param connection_t   连接类参数
 * @param ipc_acceptor_t 具体接受方式参数
 */
template<typename connection_t, typename ipc_acceptor_t>
class acceptor : public event_handler, boost::noncopyable {
public:
    typedef boost::function<int (connection_t*)> on_new_conn_callback_t;
    typedef boost::function<int (connection_t*, void*, int)> on_message_callback_t;
    typedef boost::function<int (connection_t*)> on_write_comple_callback_t;
    typedef boost::function<int (connection_t*)> on_close_conn_callback_t;

    /**
     * @brief 接受器构造函数
     * @param local_addr  本地监听地址
     */
    acceptor(const inet_address& local_addr)
        : m_ipc_acceptor_(local_addr),
          m_p_reactor_(reactor::instance()),
          m_p_conns_pool_(connections_pool<connection_t>::instance()),
          m_first_accept_(0) { }

    virtual ~acceptor() { }

    /**
     * @brief 接受器初始化监听
     * @return -1failed, 0success
     */
    int start();

    /**
     * @brief 将监听套接字放入反应器中以进行监听，只针对tcp有用
     * @return 调用特化版本do_enable_accepting
     */
    int enable_accepting() {
        return do_enable_accepting(typename ipc_traits<ipc_acceptor_t>::ipc_category());
    }

    /**
     * @brief 设置新连接建立回调函数
     * @param cb 回调函数
     * @return NULL
     */
    void set_new_conn_callback(const on_new_conn_callback_t& cb) {
        m_new_conn_cb_ = cb;
    }

    void set_on_message_callback(const on_message_callback_t& cb) {
        m_message_cb_ = cb;
    }

    void set_on_write_comple_callback(const on_write_comple_callback_t& cb) {
        m_write_comple_cb_ = cb;
    }

    void set_on_close_callback(const on_close_conn_callback_t& cb) {
        m_close_conn_cb_ = cb;
    }

    /**
     * @brief 读事件回调函数
     * @return 调用特化版本handle_read
     */
    virtual int handle_read() {
        return do_handle_read(typename ipc_traits<ipc_acceptor_t>::ipc_category());
    }

    /**
     * @brief 错误事件回调函数，只有epoll才会有错误事件
     * @return -1failed, 0success
     */
    virtual int handle_error();

    /**
     * @brief 获得接受器fd
     * @return 被动监听fd
     */
    virtual int get_fd() { return m_ipc_acceptor_.get_fd(); }

private:
    /**
     * @brief tcp特化版本enable_accepting
     */
    int do_enable_accepting(sock_acceptor_tag);

    /**
     * @brief udp特化版本enable_accepting
     */
    int do_enable_accepting(sock_dgram_tag);

    /**
     * @brief tcp特化版本读事件回调函数
     */
    int do_handle_read(sock_acceptor_tag);

    /**
     * @brief udp特化版本读事件回调函数，udp无须accept，因此读事件回调函数为空
     */
    int do_handle_read(sock_dgram_tag) { return 0; }

private:
    /**<@具体接受器对象*/
    ipc_acceptor_t m_ipc_acceptor_;

    /**<@反应器指针*/
    reactor* m_p_reactor_;

    /**<@连接池指针*/
    connections_pool<connection_t>* m_p_conns_pool_;

    /**<@标志位，标志该接受器是否已经注册过*/
    int m_first_accept_;

    /**<@被动新连接建立回调函数*/
    on_new_conn_callback_t m_new_conn_cb_;
    on_message_callback_t m_message_cb_;
    on_write_comple_callback_t m_write_comple_cb_;
    on_close_conn_callback_t m_close_conn_cb_;

} __attribute__((aligned(sizeof(long))));


/*****************************************************************************/
template<typename connection_t, typename ipc_acceptor_t>
int acceptor<connection_t, ipc_acceptor_t>::start()
{
    int rt = 0;
    if ((rt = m_ipc_acceptor_.open_listening()) < 0) {
        return -1;
    } else if (rt == IS_LISTENING) {
        ///tcp无法立即建立连接，需要监听后accept才能建立
        return 0;
    }

    return 0;
}

template<typename connection_t, typename ipc_acceptor_t>
int acceptor<connection_t, ipc_acceptor_t>::do_enable_accepting(sock_acceptor_tag)
{
    ///加锁
    if (m_first_accept_ == 0) {
        m_p_reactor_->register_event(this, READ_MASK/* | ONESHOT_MASK*/);
        m_first_accept_ = 1;
    } else {
        m_p_reactor_->update_event(this, READ_MASK/* | ONESHOT_MASK*/);
    }
    ///解锁

    return 0;
}

template<typename connection_t, typename ipc_acceptor_t>
int acceptor<connection_t, ipc_acceptor_t>::do_enable_accepting(sock_dgram_tag)
{
    connection_t* p_conn = m_p_conns_pool_->get_new_connection();
    if (!p_conn) {
        ERROR_LOG("connections_pool::get_new_connection failed");
        m_ipc_acceptor_.close();
        return -1;
    }
    ///对于udp之类没有监听fd的，创建fd后将fd转移到连接中
    (p_conn->get_ipc_conn())->set_fd(m_ipc_acceptor_.get_fd());
    m_ipc_acceptor_.set_fd(-1);

    if (m_new_conn_cb_)
        p_conn->set_new_conn_callback(m_new_conn_cb_);
    if (m_message_cb_)
        p_conn->set_on_message_callback(m_message_cb_);
    if (m_write_comple_cb_)
        p_conn->set_on_write_comple_callback(m_write_comple_cb_);
    if (m_close_conn_cb_)
        p_conn->set_on_close_callback(m_close_conn_cb_);

    if (p_conn->open())
        return -1;

    return 0;
}

template<typename connection_t, typename ipc_acceptor_t>
int acceptor<connection_t, ipc_acceptor_t>::do_handle_read(sock_acceptor_tag)
{
    connection_t* p_conn = NULL;
    while (true) {
        int cli_fd = -1;
        inet_address remote_addr;
        int rt = m_ipc_acceptor_.accept(cli_fd, remote_addr);
        switch (rt) {
            case ACCEPT_CONTINUE:
                p_conn = m_p_conns_pool_->get_new_connection();
                if (!p_conn) {
                    ERROR_LOG("connections_pool::get_new_connection failed");
                    ::close(cli_fd);
                    cli_fd = -1;
                    return 0;
                }

                (p_conn->get_ipc_conn())->set_fd(cli_fd);
                (p_conn->get_ipc_conn())->set_nonblocking();

                if (m_new_conn_cb_)
                    p_conn->set_new_conn_callback(m_new_conn_cb_);
                if (m_message_cb_)
                    p_conn->set_on_message_callback(m_message_cb_);
                if (m_write_comple_cb_)
                    p_conn->set_on_write_comple_callback(m_write_comple_cb_);
                if (m_close_conn_cb_)
                    p_conn->set_on_close_callback(m_close_conn_cb_);

                p_conn->open(&remote_addr);

                break;
            case ACCEPT_EINTR:
                break;
            case ACCEPT_ERR:
                m_p_reactor_->remove_event(this);
                m_ipc_acceptor_.close();
                return -1;
            default:
                return 0;
        }
    }
    return 0;
}

template<typename connection_t, typename ipc_acceptor_t>
int acceptor<connection_t, ipc_acceptor_t>::handle_error()
{
    m_p_reactor_->remove_event(this);
    m_ipc_acceptor_.close();
    return 0;
}

#endif
