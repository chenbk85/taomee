/**
 * @file connection.h
 * @brief 连接处理模块
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-03
 */

#ifndef _H_CONNECTION_H_
#define _H_CONNECTION_H_

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "reactor.h"
#include "config.h"
#include "log.h"
#include "event_handler.h"
#include "ipc_traits.hpp"
#include "continuous_buffer.h"
#include "inet_address.h"
#include "connections_pool.hpp"

/**
 * @brief 连接类，通过模板配置成不同的连接类，由连接池(connections_pool)管理连接
 * 类
 */
template<typename ipc_stream_t>
class connection : public event_handler, boost::noncopyable {
public:
    typedef boost::function<int (connection<ipc_stream_t>*)> on_new_conn_callback_t;
    typedef boost::function<int (connection<ipc_stream_t>*, void*, int)> on_message_callback_t;
    typedef boost::function<int (connection<ipc_stream_t>*)> on_write_comple_callback_t;
    typedef boost::function<int (connection<ipc_stream_t>*)> on_close_conn_callback_t;

    /**<@连接状态*/
    enum conn_state_t {
        Kdisconnect,
        Kdisconnecting,
        Kconnected
    };

    /**
     * @brief connection构造函数
     */
    connection()
        : p_next(NULL),
          m_p_conns_pool_(connections_pool<connection<ipc_stream_t> >::instance()),
          m_p_reactor_(reactor::instance()),
          m_connection_id_(0),
          m_conn_state_(Kdisconnect),
          m_is_writing_(false)
    {

    }

    /**
     * @brief connection析构函数
     */
    virtual ~connection() {
        m_input_buf_.continuous_buffer_fini();
        m_output_buf_.continuous_buffer_fini();
    }

    /**
     * @brief connection初始化函数，由连接建立时被调用
     * @param p_remote_addr  连接的远端地址
     * @return -1failed, 0success
     */
    int open(const inet_address* p_remote_addr = NULL) {
        return do_open(p_remote_addr,
                       typename ipc_traits<ipc_stream_t>::ipc_category());
    }

    /**
     * @brief 读事件回调函数
     * @return 调用具体参数类型的读事件回调函数
     */
    virtual int handle_read() {
        return do_handle_read(typename ipc_traits<ipc_stream_t>::ipc_category());
    }

    /**
     * @brief 写事件回调函数
     * @return 调用具体参数类型的写事件回调函数
     */
    virtual int handle_write() {
        return do_handle_write(typename ipc_traits<ipc_stream_t>::ipc_category());
    }

    /**
     * @brief 错误事件回调函数
     * @return -1failed, 0success
     */
    virtual int handle_error();

    /**
     * @brief 获得当前连接的fd
     * @return 当前连接的fd
     */
    virtual int get_fd() { return m_ipc_stream_.get_fd(); }

    /**
     * @brief 关闭写端
     * @return NULL
     */
    void shutdown() {
        do_shutdown(typename ipc_traits<ipc_stream_t>::ipc_category());
    }

    /**
     * @brief 获得当前连接的连接id
     * @return 当前连接的连接id
     */
    const uint32_t get_connection_id() const { return m_connection_id_; }

    /**
     * @brief 设置当前连接的连接id
     * @return NULL
     */
    void set_connection_id(uint32_t conn_id) { m_connection_id_ = conn_id; }

    /**
     * @brief 获得当前连接的具体连接对象指针
     * @return 当前连接的具体连接指针
     */
    ipc_stream_t* get_ipc_conn() { return &m_ipc_stream_; }

    /**
     * @brief 获得当前实例化连接的参数的类型
     */
    const int get_protocol_type() const {
        return do_get_protocol_type(typename ipc_traits<ipc_stream_t>::ipc_category());
    }

    /**
     * @brief 发送数据，用于udp之类的需要对端地址才能发送数据的类型
     * @param remote_addr  对端地址
     * @param buf          发送数据缓冲区
     * @param len          发送数据长度
     * @return 0success, -1failed
     */
    int send_data(const inet_address& remote_addr, void* buf, int len);

    /**
     * @brief 发送数据，用于tcp之类的流式连接
     * @param buf  发送数据缓冲区
     * @param len  发送数据长度
     * @return 0success, -1failed
     */
    int send_data(void* buf, int len);

    /**
     * @brief 设置连接连接时的回调函数
     * @param cb  回调函数
     * @return NULL
     */
    void set_new_conn_callback(const on_new_conn_callback_t& cb) {
        m_new_conn_cb_ = cb;
    }

    /**
     * @brief 设置收到消息时的回调函数
     * @param cb  消息回调函数
     * @return NULL
     */
    void set_on_message_callback(const on_message_callback_t& cb) {
        m_message_cb_ = cb;
    }

    /**
     * @brief 设置关闭连接时的回调函数
     * @param cb  关闭连接回调函数
     * @return NULL
     */
    void set_on_close_callback(const on_close_conn_callback_t& cb) {
        m_close_cb_ = cb;
    }

    /**
     * @brief 设置发送完成回调函数
     * @param cb  发送完成回调函数
     * @return NULL
     */
    void set_on_write_comple_callback(const on_write_comple_callback_t& cb) {
        m_write_comple_cb_ = cb;
    }

    /**
     * @brief 获取对端地址
     * @return remote_addr
     */
    const inet_address& get_remote_addr() const { return m_remote_addr_; }

    /**
     * @brief 重置当前连接
     * @return NULL
     */
    void recycle_connection();

private:
    /**
     * @brief 针对socket的do_open特化版本
     */
    int do_open(const inet_address* p_remote_addr, sock_tag);

    /**
     * @brief 针对vpipe的do_open特化版本
     */
    int do_open(const inet_address* p_remote_addr, vpipe_tag);

    /**
     * @brief 针对流式连接的handle_read特化版本
     */
    int do_handle_read(ipc_tag);

    /**
     * @brief 针对报文式连接的handle_read特化版本
     */
    int do_handle_read(sock_dgram_tag);

    /**
     * @brief 针对流式连接的handle_write特化版本
     */
    int do_handle_write(ipc_tag);

    /**
     * @brief 针对报文式连接的handle_write特化版本
     */
    int do_handle_write(sock_dgram_tag) { return 0; }

    /**
     * @brief 获得tcp类型实例化参数
     */
    const int do_get_protocol_type(sock_stream_tag) const {
        return TYPE_STREAM;
    }

    /**
     * @brief 获得udp类型实例化参数
     */
    const int do_get_protocol_type(sock_dgram_tag) const {
        return TYPE_DGRAM;
    }

    /**
     * @brief 获得socketpair类型实例化参数
     */
    const int do_get_protocol_type(vpipe_sockpair_tag) const {
        return TYPE_SOCKPAIR;
    }

    /**
     * @brief 针对udp和socketpair特化的shutdown函数，空函数，不做任何调用
     * @return NULL
     */
    void do_shutdown(ipc_tag) { }

    /**
     * @brief 针对tcp的shutdown函数，关闭写端连接
     * @return NULL
     */
    void do_shutdown(sock_stream_tag);

public:

    /**<@该类在hash表中的next指针*/
    connection<ipc_stream_t>* p_next;

private:

    /**<@连接池指针*/
    connections_pool<connection<ipc_stream_t> >* m_p_conns_pool_;

    /**<@反应器指针*/
    reactor* m_p_reactor_;

    /**<@实际连接类型的对象*/
    ipc_stream_t m_ipc_stream_;

    /**<@该类对象的连接id*/
    uint32_t m_connection_id_;

    /**<@该连接当前的状态*/
    conn_state_t m_conn_state_;

    /**<@接收数据buffer*/
    continuous_buffer m_input_buf_;

    /**<@发送数据buffer*/
    continuous_buffer m_output_buf_;

    /**<@连接建立回调函数*/
    on_new_conn_callback_t m_new_conn_cb_;

    /**<@收到数据回调函数*/
    on_message_callback_t m_message_cb_;

    /**<@关闭连接回调函数*/
    on_close_conn_callback_t m_close_cb_;

    /**<@发送完成回调函数*/
    on_write_comple_callback_t m_write_comple_cb_;

    /**<@连接远端地址*/
    inet_address m_remote_addr_;

    /**<@指示该连接是否正在写操作*/
    bool m_is_writing_;

} __attribute__((aligned(sizeof(long))));


/************************************************************************/
template<typename ipc_stream_t>
int connection<ipc_stream_t>::do_open(const inet_address* p_remote_addr, sock_tag)
{
    m_conn_state_ = Kconnected;

    if (p_remote_addr)
        m_remote_addr_ = *p_remote_addr;

    if (m_input_buf_.continuous_buffer_init() < 0 ||
            m_output_buf_.continuous_buffer_init() < 0)
        goto err;

    if (m_p_reactor_->register_event(this, READ_MASK) < 0)
        goto err;

    if (m_new_conn_cb_) {
        if (m_new_conn_cb_(this) < 0) {
            goto err;
        }
    }

    return 0;
err:
    recycle_connection();
    return -1;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::do_open(const inet_address* p_remote_addr, vpipe_tag)
{
    //if (m_ipc_stream_.open() < 0)  goto err;
    m_conn_state_ = Kconnected;

    if (m_input_buf_.continuous_buffer_init() < 0 ||
            m_output_buf_.continuous_buffer_init() < 0)
        goto err;

    if (m_p_reactor_->register_event(this, READ_MASK) < 0)
        goto err;

    if (m_new_conn_cb_) {
        if (m_new_conn_cb_(this) < 0) {
            goto err;
        }
    }

    return 0;
err:
    recycle_connection();
    return -1;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::do_handle_read(ipc_tag)
{
    while (true) {
        ssize_t rt = m_ipc_stream_.read(m_input_buf_.writeable_addr(),
                                        m_input_buf_.writeable_bytes());
        if (rt < 0 && errno == EINTR) {

        } else if (rt < 0 && errno == EAGAIN) {
            break;
        } else if (rt == 0) {
            goto err;
        } else if (rt > 0) {
            m_input_buf_.move_writeable(rt);
            ///如果写buferr还有空余。则无须读到EAGAIN
            if (m_input_buf_.writeable_actual_bytes() > 0)
                break;
        } else {
            ERROR_LOG("socket[%d] stream read failed, err: %s",
                    m_ipc_stream_.get_fd(), ::strerror(errno));
            goto err;
        }
    }

    if (m_message_cb_) {
        int rt = m_message_cb_(this,
                               m_input_buf_.readable_addr(),
                               m_input_buf_.readable_bytes());
        if (rt < 0) {
            goto err;
        } else {
            m_input_buf_.move_readable(m_input_buf_.readable_bytes() - rt);
        }
    }

    return 0;
err:
    recycle_connection();
    return -1;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::do_handle_read(sock_dgram_tag)
{
    while (true) {
        ssize_t rt = m_ipc_stream_.read(m_remote_addr_,
                                        m_input_buf_.writeable_addr(),
                                        m_input_buf_.writeable_bytes());
        if (rt < 0 && errno == EINTR) {

        } else if (rt < 0 && errno == EAGAIN) {
            break;
        } else if (rt >= 0) {
            m_input_buf_.move_writeable(rt);
        } else {
            ERROR_LOG("socket[%d] udp read failed, err: %s", m_ipc_stream_.get_fd(), ::strerror(errno));
            goto err;
        }
    }

    ///调用接收数据回调函数
    if (m_message_cb_) {
        m_message_cb_(this,
                      m_input_buf_.readable_addr(),
                      m_input_buf_.readable_bytes());

        ///udp报文是独立，当没有收到完整报文时此报文已经丢失
        ///所以对于udp接收缓存余下的数据是没有意义的
        ///因此在每次发完后复位接收缓存
        m_input_buf_.reset();
    }

    return 0;
err:
    recycle_connection();
    return -1;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::do_handle_write(ipc_tag)
{
    if (m_output_buf_.readable_bytes() == 0)
        return 0;

    ssize_t rt = m_ipc_stream_.write(m_output_buf_.readable_addr(),
                                     m_output_buf_.readable_bytes());
    if (rt < 0 && (errno == EINTR || errno == EAGAIN)) {
        return 0;
    } else if (rt > 0) {
        m_output_buf_.move_readable(rt);
    } else {
        ERROR_LOG("socket[%d] stream write failed, err: %s",
                m_ipc_stream_.get_fd(), ::strerror(errno));
        goto err;
    }

    if (m_output_buf_.readable_bytes() == 0) {
        m_is_writing_ = false;
        if (m_write_comple_cb_)
            m_p_reactor_->queue_in_loop(boost::bind(m_write_comple_cb_, this));

        if (m_conn_state_ == Kdisconnecting)
            m_ipc_stream_.close_writer();

        ///如果数据确实已发送完毕，则不再关注写事件
        if (m_output_buf_.readable_bytes() == 0)
            m_p_reactor_->update_event(this, READ_MASK);
    }

    return 0;
err:
    recycle_connection();
    return -1;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::handle_error()
{
    recycle_connection();
    return 0;
}

template<typename ipc_stream_t>
void connection<ipc_stream_t>::recycle_connection()
{
    ///关闭事件回调函数
    if (m_close_cb_)
        m_close_cb_(this);

    m_p_conns_pool_->free_connection(this);
    m_input_buf_.resize();
    m_output_buf_.resize();
    m_p_reactor_->remove_event(this);
    m_ipc_stream_.close();
    m_connection_id_ = 0;
    m_conn_state_ = Kdisconnect;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::send_data(const inet_address& remote_addr, void* buf, int len)
{
    if (m_conn_state_ == Kdisconnect || m_conn_state_ == Kdisconnecting)
        return 0;

    ssize_t rt = m_ipc_stream_.write(remote_addr, buf, len);
    if (rt > 0 && rt != len) {
        ERROR_LOG("failed to send a whole package, send_len[%d], pkg_len[%d]", rt, len);
        return -1;
    } else if (rt < 0) {
        ERROR_LOG("socket[%d] stream write failed, err: %s",
                m_ipc_stream_.get_fd(), ::strerror(errno));
        return -1;
    }

    if (m_write_comple_cb_)
        m_p_reactor_->queue_in_loop(boost::bind(m_write_comple_cb_, this));

    return 0;
}

template<typename ipc_stream_t>
int connection<ipc_stream_t>::send_data(void* buf, int len)
{
    if (m_conn_state_ == Kdisconnect || m_conn_state_ == Kdisconnecting)
        return 0;

    ssize_t rt = m_ipc_stream_.write(buf, len);
    if (rt < 0 && (errno == EINTR || errno == EAGAIN)) {
        goto out;
    } else if (rt > 0 && rt != len) {
        goto out;
    } else if (rt > 0 && rt == len) {
        if (m_write_comple_cb_)
            m_p_reactor_->queue_in_loop(boost::bind(m_write_comple_cb_, this));
        return 0;
    } else {
        ERROR_LOG("socket[%d] stream write failed, err: %s",
                m_ipc_stream_.get_fd(), ::strerror(errno));
        goto err;
    }

out:
    if (rt < 0) rt = 0;
    ///发送缓冲不够，需要扩大发送缓冲
    if ((int)m_output_buf_.writeable_bytes() < len - rt) {
        if (m_output_buf_.make_writeable_bytes(len - rt) < 0) {
            return -1;
        }
    }
    ::memcpy(m_output_buf_.writeable_addr(), (char*)buf + rt, len - rt);
    m_output_buf_.move_writeable(len - rt);
    m_is_writing_ = true;

    if (m_p_reactor_->update_event(this, READ_MASK | WRITE_MASK) < 0) {
        return -1;
    }

    return 0;

err:
//    recycle_connection();  //should be consider
    return -1;
}

template<typename ipc_stream_t>
void connection<ipc_stream_t>::do_shutdown(sock_stream_tag)
{
    if (m_conn_state_ == Kconnected) {
        m_conn_state_ = Kdisconnecting;
        if (!m_is_writing_) {
            m_ipc_stream_.close_writer();
        }
    }
}

#endif
