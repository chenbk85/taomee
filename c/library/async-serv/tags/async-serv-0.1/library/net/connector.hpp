/**
 * @file connector.hpp
 * @brief 连接器模块
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-04
 */

#ifndef _H_CONNECTOR_H_
#define _H_CONNECTOR_H_

#include <string.h>
#include <errno.h>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "event_handler.h"
#include "ipc_traits.hpp"
#include "config.h"
#include "inet_address.h"
#include "connections_pool.hpp"
#include "reactor.h"


/**
 * @brief 此连接器拥有一个连接(是否需要支持重连机制FIXME.可以用定时器实现)
 * @param connection_t      连接类参数
 * @param ipc_connector_t   具体连接方式参数
 */
template<typename connection_t, typename ipc_connector_t>
class connector : public event_handler, boost::noncopyable {
public:
    typedef boost::function<int (connection_t*)> on_new_conn_callback_t;
    typedef boost::function<int (connection_t*, void*, int)> on_message_callback_t;
    typedef boost::function<int (connection_t*)> on_write_comple_callback_t;
    typedef boost::function<int (connection_t*)> on_close_conn_callback_t;

    typedef boost::function<int (connector*)> on_conn_fail_callback_t;

    ///连接器，需要对端连接地址的连接器
    connector(const inet_address& serv_addr)
        : m_ipc_connector_(serv_addr),
          m_p_reactor_(reactor::instance()),
          m_p_conns_pool_(connections_pool<connection_t>::instance()),
          m_p_conn_(NULL) { }

    ///连接器，不需要对端连接地址
    connector()
        : m_p_reactor_(reactor::instance()),
          m_p_conns_pool_(connections_pool<connection_t>::instance()),
          m_p_conn_(NULL) { }

    virtual ~connector() { }

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

    void set_on_conn_fail_callback(const on_conn_fail_callback_t cb) {
        m_conn_fail_cb_ = cb;
    }

    /**
     * @brief 初始化连接器，初始化主动连接，若连接成功，则不放入反应器，若连接正
     * 在进行中，则放入反应器等待
     * @return -1failed, 0success
     */
    int start() {
        return do_start(typename ipc_traits<ipc_connector_t>::ipc_category());
    }

    /**
     * @brief 连接器回调写事件函数，用来判断主动连接是否成功，若连接成功，则进行
     * 回调，通知so使用者
     * @return 0success, -1failed
     */
    virtual int handle_write() {
        return do_handle_write(typename ipc_traits<ipc_connector_t>::ipc_category());
    }

    /**
     * @brief 连接器回调错误事件，用来判断连接是否出错
     * @return 0success, -1failed
     */
    virtual int handle_error();

    /**
     * @brief 获得此连接器的fd
     * @return fd
     */
    virtual int get_fd() { return m_ipc_connector_.get_fd(); }

    connection_t* get_connection() const { return m_p_conn_; }

private:
    /**
     * @brief tcp特化版本写事件回调函数
     */
    int do_handle_write(sock_connector_tag);

    /**
     * @brief udp特化版本写事件回调函数，空函数
     */
    int do_handle_write(sock_dgram_tag) { return 0; }

    /**
     * @brief tcp特化版本的start
     */
    int do_start(sock_dgram_tag);

    /**
     * @brief udp特化版本的start
     */
    int do_start(sock_connector_tag);

private:
    /**<@连接器具体连接实例*/
    ipc_connector_t m_ipc_connector_;

    /**<@反应器指针*/
    reactor* m_p_reactor_;

    /**<@连接池指针*/
    connections_pool<connection_t>* m_p_conns_pool_;

    /**<@连接对象指针，每个连接器只可能拥有一个连接流对象，因此只需维护一个指针
     * 即可*/
    connection_t* m_p_conn_;

    on_new_conn_callback_t m_new_conn_cb_;
    on_message_callback_t m_message_cb_;
    on_write_comple_callback_t m_write_comple_cb_;
    on_close_conn_callback_t m_close_conn_cb_;

    on_conn_fail_callback_t m_conn_fail_cb_;

} __attribute__((aligned(sizeof(long))));


/*****************************************************************************/
template<typename connection_t, typename ipc_connector_t>
int connector<connection_t, ipc_connector_t>::do_start(sock_connector_tag)
{
    ///初始化新连接
    int rt = m_ipc_connector_.open_connecting();
    if (rt < 0) {
        return -1;
    } else if (rt == CONN_TEMP_UNAVAIL) {
        ///连接暂时不可用，可以选择重连或是返回错误，暂时处理为返回错误
        //FIXME.
        return -1;
    } else if (rt == IS_CONNECTING) {
        ///正在连接，将连接器注册到反应器内
        if (m_p_reactor_->register_event(this, WRITE_MASK | ONESHOT_MASK) < 0) {
            return -1;
        }
        return 0;
    }

    ///已经完成连接
    m_p_conn_ = m_p_conns_pool_->get_new_connection();
    if (!m_p_conn_) {
        ERROR_LOG("connections_pool::get_new_connection() failed");
        m_ipc_connector_.close();
        return -1;
    }

    ///将fd转移到连接中，并关闭连接器中的fd
    (m_p_conn_->get_ipc_conn())->set_fd(m_ipc_connector_.get_fd());
    m_ipc_connector_.set_fd(-1);

    if (m_new_conn_cb_)
        m_p_conn_->set_new_conn_callback(m_new_conn_cb_);
    if (m_message_cb_)
        m_p_conn_->set_on_message_callback(m_message_cb_);
    if (m_write_comple_cb_)
        m_p_conn_->set_on_write_comple_callback(m_write_comple_cb_);
    if (m_close_conn_cb_)
        m_p_conn_->set_on_close_callback(m_close_conn_cb_);

    if (m_p_conn_->open(&(m_ipc_connector_.get_serv_addr())) < 0)
        return -1;

    return 0;
}

template<typename connection_t, typename ipc_connector_t>
int connector<connection_t, ipc_connector_t>::do_start(sock_dgram_tag)
{
    ///初始化新连接
    int rt = m_ipc_connector_.open_connecting();
    if (rt < 0) {
        return -1;
    }

    ///已经完成连接
    m_p_conn_ = m_p_conns_pool_->get_new_connection();
    if (!m_p_conn_) {
        ERROR_LOG("connections_pool::get_new_connection() failed");
        m_ipc_connector_.close();
        return -1;
    }

    ///将fd转移到连接中，并关闭连接器中的fd
    (m_p_conn_->get_ipc_conn())->set_fd(m_ipc_connector_.get_fd());
    m_ipc_connector_.set_fd(-1);

    if (m_new_conn_cb_)
        m_p_conn_->set_new_conn_callback(m_new_conn_cb_);
    if (m_message_cb_)
        m_p_conn_->set_on_message_callback(m_message_cb_);
    if (m_write_comple_cb_)
        m_p_conn_->set_on_write_comple_callback(m_write_comple_cb_);
    if (m_close_conn_cb_)
        m_p_conn_->set_on_close_callback(m_close_conn_cb_);

    if (m_p_conn_->open() < 0)
        return -1;

    return 0;
}

template<typename connection_t, typename ipc_connector_t>
int connector<connection_t, ipc_connector_t>::do_handle_write(sock_connector_tag)
{
    int opt = 0;
    int optlen = 0;

    if (m_ipc_connector_.get_options(SOL_SOCKET, SO_ERROR, &opt, &optlen) < 0) {
        ERROR_LOG("sock::get_options failed, err: %s", ::strerror(errno));
        goto err;
    }
    if (opt) {
        ERROR_LOG("sockgetopt SO_ERROR is not null, connect failed");
        goto err;
    }

    m_p_reactor_->remove_event(this);

    m_p_conn_ = m_p_conns_pool_->get_new_connection();
    if (!m_p_conn_) {
        ERROR_LOG("connections_pool::get_new_connection() failed");
        goto err;
    }

    ///将fd转移到连接中，并关闭连接器中的fd
    (m_p_conn_->get_ipc_conn())->set_fd(m_ipc_connector_.get_fd());
    m_ipc_connector_.set_fd(-1);

    if (m_new_conn_cb_)
        m_p_conn_->set_new_conn_callback(m_new_conn_cb_);
    if (m_message_cb_)
        m_p_conn_->set_on_message_callback(m_message_cb_);
    if (m_write_comple_cb_)
        m_p_conn_->set_on_write_comple_callback(m_write_comple_cb_);
    if (m_close_conn_cb_)
        m_p_conn_->set_on_close_callback(m_close_conn_cb_);

    if (m_p_conn_->open(&(m_ipc_connector_.get_serv_addr())) < 0)
        goto err;

    return 0;

err:
    m_p_reactor_->remove_event(this);
    m_ipc_connector_.close();

    if (m_conn_fail_cb_)
        m_conn_fail_cb_(this);
    return -1;
}

template<typename connection_t, typename ipc_connector_t>
int connector<connection_t, ipc_connector_t>::handle_error()
{
    m_p_reactor_->remove_event(this);
    m_ipc_connector_.close();

    if (m_conn_fail_cb_)
        m_conn_fail_cb_(this);
    return 0;
}

#endif
