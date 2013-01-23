/**
 * @file c_net_io_server.cpp
 * @brief 网络IO服务框架接口实现
 * @author baron baron@taomee.com
 * @version 2.0
 * @date 2010-12-16
 */

#include "c_net_io_server.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int net_io_server_create(i_net_io_server **pp_net_io_server)
{
    if (!pp_net_io_server)
    {
        return -1;
    }

    i_net_io_server *p_net_io_server = new (std::nothrow) c_net_io_server();

    if (!p_net_io_server)
    {
        return -1;
    }

    *pp_net_io_server = p_net_io_server;
    return 0;
}

c_net_io_server::c_net_io_server()
    :m_init_(0),
     m_id_(0),
     m_errno_(NET_IO_SERVER_OK),
     m_p_net_io_notifier_(NULL),
     m_flag_(0)
{
    bzero(m_errstr_, sizeof(m_errstr_));
    m_id_conn_map_.clear();
}

c_net_io_server::~c_net_io_server()
{
    uninit();
}

int c_net_io_server::init(const char *ip,
                          const int port,
                          const i_net_io_event_handler *p_event_handler,
                          const i_net_io_notifier *p_net_io_notifier,
                          const int id_base,
                          const int id_step)
{
    if (m_init_)
    {
        return -1;
    }

    if (!ip || port < 0 || port > 65535)
    {
        return -1;
    }

    if (id_base <= 0 || id_step <= 0)
    {
        return -1;
    }

    m_socket_listen_ = socket(AF_INET, SOCK_STREAM, 0);

    if (m_socket_listen_ < 0)
    {
        set_error_info_(
            NET_IO_SERVER_EINIT, "fail to obtain socket(%s)", strerror(errno));
        return -1;
    }

    sockaddr_in sockaddr_listen;
    bzero(&sockaddr_listen, sizeof(sockaddr_in));
    sockaddr_listen.sin_family = AF_INET;
    sockaddr_listen.sin_port = htons(port);
    sockaddr_listen.sin_addr.s_addr = inet_addr(ip);

    if (INADDR_NONE == sockaddr_listen.sin_addr.s_addr)
    {
        close(m_socket_listen_);
        set_error_info_(
            NET_IO_SERVER_EINIT, "fail to inet_addr(%s)", strerror(errno));
        return -1;
    }

    // 设置地址可重用
    int is_reuseaddr = 1;

    if (setsockopt(m_socket_listen_,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &is_reuseaddr,
                   sizeof(int)) != 0)
    {
        close(m_socket_listen_);
        set_error_info_(
            NET_IO_SERVER_EINIT, "fail to setsockopt(%s)", strerror(errno));
        return -1;
    }

    if (bind(m_socket_listen_,
             reinterpret_cast<struct sockaddr *>(&sockaddr_listen),
             sizeof(struct sockaddr)) != 0)
    {
        close(m_socket_listen_);
        set_error_info_(
            NET_IO_SERVER_EINIT, "fail to bind(%s)", strerror(errno));
        return -1;
    }

    // 设置SOCKET非阻塞模式
    int flags = fcntl(m_socket_listen_, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(m_socket_listen_, F_SETFL, flags);

    if (listen(m_socket_listen_, NET_IO_SERVER_LISTEN_BACKLOG) != 0)
    {
        close(m_socket_listen_);
        set_error_info_(
            NET_IO_SERVER_EINIT, "fail to listen(%s)", strerror(errno));
        return -1;
    }

    if ((m_epoll_fd_ = epoll_create(MAX_CLIENT_NUM)) < 0)
    {
        close(m_socket_listen_);
        return -1;
    }

    epoll_event event;
    bzero(&event, sizeof(epoll_event));
    event.events = EPOLLET | EPOLLIN;
    event.data.ptr = NULL;

    if (epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, m_socket_listen_, &event) != 0)
    {
        close(m_socket_listen_);
        close(m_epoll_fd_);
        set_error_info_(NET_IO_SERVER_EINIT,
                       "fail to EPOLL_CTL_ADD(%s)",
                       strerror(errno));
        return -1;
    }

    strncpy(m_ip_listen_, ip, 15);
    m_ip_listen_[15] = '\0';
    m_p_net_io_event_handler_
        = const_cast<i_net_io_event_handler *>(p_event_handler);

    if (p_net_io_notifier)
    {
        int efd = p_net_io_notifier->get_eventfd();

        if (efd < 0)
        {
            close(m_socket_listen_);
            close(m_epoll_fd_);
            set_error_info_(NET_IO_SERVER_EINIT,
                            "fail to get_fd by net_io_notifier");
            return -1;
        }

        bzero(&event, sizeof(epoll_event));
        event.events = EPOLLET | EPOLLIN;
        event.data.ptr = reinterpret_cast<void *>(-1);

        if (epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, efd, &event) != 0)
        {
            close(m_socket_listen_);
            close(m_epoll_fd_);
            set_error_info_(NET_IO_SERVER_EINIT,
                            "fail to EPOLL_CTL_ADD(%s)",
                            strerror(errno));
            return -1;
        }

        m_p_net_io_notifier_
            = const_cast<i_net_io_notifier *>(p_net_io_notifier);
    }

    m_id_ = m_id_base_ = id_base;
    m_id_step_ = id_step;
    m_init_ = 1;
    return 0;
}

int c_net_io_server::do_io(const int timeout, const int cmd)
{
    if (!m_init_)
    {
        return -1;
    }

    int epoll_ret = epoll_wait(m_epoll_fd_,
                               m_events_,
                               sizeof(m_events_) / sizeof(epoll_event),
                               timeout);

    if (m_p_net_io_event_handler_)
    {
        m_p_net_io_event_handler_->on_wakeup(
            dynamic_cast<i_net_io_server *>(this));
    }

    for (int i = 0; i < epoll_ret; ++ i)
    {
        // 事件响应于服务器套接字或者曾发生过满溢或者被notify
        if ((!m_events_[i].data.ptr)
            || (0 == i && (m_flag_ & NET_IO_SERVER_FLAG_RUNOUT)))
        {
            for (;;)
            {
                sockaddr_in peer_addr;
                bzero(&peer_addr, sizeof(struct sockaddr_in));
                socklen_t peer_addr_len = sizeof(sockaddr_in);
                int peer_socket
                    = accept(m_socket_listen_,
                             reinterpret_cast<struct sockaddr *>(&peer_addr),
                             &peer_addr_len);

                if (peer_socket >= 0)
                {
                    if (!(cmd & NET_IO_SERVER_CMD_ACCEPT))
                    {
                        // 若调用者不希望进行新的连接则accept后立即关闭
                        close(peer_socket);
                        continue;
                    }

                    bzero(&m_connection_info_, sizeof(connection_info_t));
                    m_connection_info_.id = genereate_id_();
                    m_connection_info_.fd = peer_socket;
                    strcpy(m_connection_info_.ip,
                           inet_ntoa(peer_addr.sin_addr));
                    m_connection_info_.port = ntohs(peer_addr.sin_port);
                    m_connection_info_.connect_time
                        = static_cast<int>(time(NULL));

                    if (m_p_net_io_event_handler_)
                    {
                        // 回调函数on_new_connection返回值决定是否进行连接
                        if (-1 == m_p_net_io_event_handler_->on_new_connection(
                                      dynamic_cast<i_net_io_server *>(this),
                                      m_connection_info_.id,
                                      m_connection_info_.fd,
                                      m_connection_info_.ip,
                                      m_connection_info_.port,
                                      &m_connection_info_.storage))
                        {
                            close(peer_socket);
                            continue;
                        }
                    }

                    std::pair<id_conn_map_iter_t, bool> ret
                        = m_id_conn_map_.insert(
                            std::pair<int, connection_info_t>(
                                m_connection_info_.id,
                                m_connection_info_));

                    if (ret.second)
                    {
                        int flags = fcntl(peer_socket, F_GETFL);
                        flags |= O_NONBLOCK;
                        fcntl(peer_socket, F_SETFL, flags);
                        epoll_event event;
                        event.events
                            = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                        event.data.ptr = &ret.first->second;

                        if (epoll_ctl(m_epoll_fd_,
                                      EPOLL_CTL_ADD,
                                      peer_socket,
                                      &event) != 0)
                        {
                            set_error_info_(NET_IO_SERVER_EIO,
                                           "fail to EPOLL_CTL_ADD(%s)",
                                           strerror(errno));
                            close_connection(m_connection_info_.id, true);
                        }
                    }
                    else
                    {
                        // id冲突了,极低概率事件
                        set_error_info_(
                            NET_IO_SERVER_EIO, "fatal error:id conflict");

                        if (m_p_net_io_event_handler_)
                        {
                            m_p_net_io_event_handler_->on_connection_closed(
                                dynamic_cast<c_net_io_server *>(this),
                                m_connection_info_.id,
                                m_connection_info_.fd,
                                m_connection_info_.ip,
                                m_connection_info_.port,
                                &m_connection_info_.storage);
                        }

                        close(peer_socket);
                    }
                }
                else
                {
                    if (EAGAIN == errno)
                    {
                        // 既然accept至EAGAIN状态,则将fd尽状态标志去掉
                        m_flag_ &= ~NET_IO_SERVER_FLAG_RUNOUT;
                        break;
                    }
                    else if (EINTR == errno)
                    {
                        // no thing to do ...
                    }
                    else
                    {
                        // 认为除EAGAIN以及EINTR错误状态的状态标示fd已耗尽
                        m_flag_ |= NET_IO_SERVER_FLAG_RUNOUT;
                        break;
                    }
                }
            } // for
        } // if

        if (m_p_net_io_notifier_
            && m_events_[i].data.ptr == reinterpret_cast<void *>(-1))
        {
            // 响应通知事件
            m_p_net_io_notifier_->got();
            continue;
        }

        if (m_events_[i].data.ptr)
        {
            connection_info_t *p_connection_info
                = reinterpret_cast<connection_info_t *>(m_events_[i].data.ptr);

            if ((m_events_[i].events & EPOLLERR)
                || (m_events_[i].events & EPOLLHUP)
                || (m_events_[i].events & EPOLLRDHUP))
            {
                // 标示fd出现错误或者fd被挂起或者对端关闭连接
                close_connection(p_connection_info->id, true);
                continue;
            }

            if (m_events_[i].events & EPOLLIN)
            {
                for (;;)
                {
                    if (!(cmd & NET_IO_SERVER_CMD_READ))
                    {
                        // 若用户未要求读操作,则不进行读操作,并且通过重新修改fd
                        // 相关事件,使之在ET模式下仍然可以在下一次被触发读
                        epoll_event event;
                        event.events
                            = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                        event.data.ptr = m_events_[i].data.ptr;
                        epoll_ctl(m_epoll_fd_,
                                  EPOLL_CTL_MOD,
                                  p_connection_info->fd,
                                  &event);
                        //epoll_ctl(m_epoll_fd_,
                        //          EPOLL_CTL_ADD,
                        //          p_connection_info->fd,
                        //          &event);
                        break;
                    }

                    int buffer_empty_len
                        = sizeof(p_connection_info->buffer_recv)
                               - p_connection_info->buffer_recv_len;
                    int recv_len
                        = recv(p_connection_info->fd,
                               p_connection_info->buffer_recv
                               + p_connection_info->buffer_recv_len,
                               buffer_empty_len,
                               0);

                    if (recv_len > 0)
                    {
                        if (m_p_net_io_event_handler_)
                        {
                            p_connection_info->buffer_recv_len += recv_len;
                            int left
                                = m_p_net_io_event_handler_->on_recv_data(
                                    this,
                                    p_connection_info->id,
                                    p_connection_info->fd,
                                    p_connection_info->ip,
                                    p_connection_info->port,
                                    p_connection_info->buffer_recv,
                                    p_connection_info->buffer_recv_len,
                                    &p_connection_info->storage);

                            if (left < 0
                                || left >=
                                    static_cast<int>(
                                        sizeof(
                                            p_connection_info->buffer_recv)))
                            {
                                // invalid value returned, close the connection
                                close_connection(p_connection_info->id, true);
                                m_events_[i].data.ptr
                                    = p_connection_info = NULL;
                            }
                            else
                            {
                                p_connection_info->buffer_recv_len = left;
                            }
                        }  // 否则p_connection_info->buffer_recv_len自然为0

                        //if (buffer_recv_len == buffer_empty_len)
                        if (recv_len >= buffer_empty_len && p_connection_info)
                        {
                            // 若不能保证本次已读完全数据或读fd发生EAGAIN
                            // 则重新修改fd相关事件,
                            // 使之在ET模式下仍然可以在下一次被触发读
                            epoll_event event;
                            event.events
                                = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                            event.data.ptr = m_events_[i].data.ptr;
                            epoll_ctl(m_epoll_fd_,
                                      EPOLL_CTL_MOD,
                                      p_connection_info->fd,
                                      &event);
                            //epoll_ctl(m_epoll_fd_,
                            //          EPOLL_CTL_ADD,
                            //          p_connection_info->fd,
                            //          &event);
                        }
                    }
                    else if (0 == recv_len)
                    {
                        // 对端关闭连接
                        close_connection(p_connection_info->id, true);
                        m_events_[i].data.ptr = p_connection_info = NULL;
                    }
                    else
                    {
                        if (EAGAIN == errno)
                        {
                            // 消耗此次读事件
                            // nothing to do ...
                        }
                        else if (EINTR == errno)
                        {
                            continue;
                        }
                        else
                        {
                            close_connection(p_connection_info->id, true);
                            m_events_[i].data.ptr = p_connection_info = NULL;
                        }
                    }

                    break;
                } // for
            } // if

            if (p_connection_info
                && m_events_[i].events & EPOLLOUT
                && p_connection_info->buffer_send_len > 0)
            {
                if (!(cmd & NET_IO_SERVER_CMD_WRITE))
                {
                    // 若用户未要求写操作,则不进行写操作,并且通过重新修改fd
                    // 相关事件,使之在ET模式下仍然可以在下一次被触发写
                    epoll_event event;
                    event.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                    event.data.ptr = m_events_[i].data.ptr;
                    //epoll_ctl(m_epoll_fd_,
                    //          EPOLL_CTL_ADD,
                    //          p_connection_info->fd,
                    //          &event);
                    epoll_ctl(m_epoll_fd_,
                              EPOLL_CTL_MOD,
                              p_connection_info->fd,
                              &event);
                    continue;
                }

                for (;;)
                {
                    int send_len = send(p_connection_info->fd,
                                        p_connection_info->buffer_send,
                                        p_connection_info->buffer_send_len,
                                        0);

                    // buffer_send_len 一定小于等于
                    // p_connection_info->buffer_send_len
                    if (send_len > 0)
                    {
                        p_connection_info->buffer_send_len -= send_len;

                        if (p_connection_info->buffer_send_len > 0)
                        {
                            // TODO:可以优化 不用每次都移动
                            memmove(p_connection_info->buffer_send,
                                    p_connection_info->buffer_send + send_len,
                                    p_connection_info->buffer_send_len);
                        }
                        else
                        {
                            // 写完
                            break;
                        }
                    }
                    else if (0 == send_len)
                    {
                        // 不应发生,因为若send_len,
                        // 应该在上一个发送周期中进入第一个分支,并break出循环
                        assert(false);
                        break;
                    }
                    else
                    {
                        if (EAGAIN == errno)
                        {
                            // 消耗此次写事件
                            break;
                        }
                        else if (EINTR == errno)
                        {
                            continue;
                        }
                        else
                        {
                            close_connection(p_connection_info->id, true);
                            m_events_[i].data.ptr = NULL;
                            break;
                        }
                    }
                } // for
            } // if
        } // if
    } // for 主循环,遍历事件

    return 0;
}

int c_net_io_server::send_data(const int id, char *p_data, const int data_len)
{
    if (!m_init_)
    {
        return -1;
    }

    if (!p_data || data_len <= 0)
    {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);

    if (it == m_id_conn_map_.end())
    {
        set_error_info_(NET_IO_SERVER_ESEND,
                        "fail to find connection info according by id(%d)",
                        id);
        return -1;
    }

    connection_info_t *p_connection_info
        = reinterpret_cast<connection_info_t *>(&it->second);
    int send_len = 0;
    int left = p_connection_info->buffer_send_len;

    while (left > 0)
    {
        // 则发送原本留在发送缓冲区中的数据
        send_len = send(p_connection_info->fd,
                        p_connection_info->buffer_send
                        + p_connection_info->buffer_send_len
                        - left,
                        left,
                        0);

        if (send_len < 0)
        {
            if (EAGAIN == errno)
            {
                break;
            }
            else if (EINTR == errno)
            {
                continue;
            }
            else
            {
                close_connection(id, true);
                return -1;
            }
        }
        else
        {
            left -= send_len;
        }
    }

    if (left > 0)
    {
        // 若上次发送未发完全,则整理发送缓冲区,将数据向前靠齐
        memmove(p_connection_info->buffer_send,
                p_connection_info->buffer_send
                + p_connection_info->buffer_send_len
                - left,
                left);
        p_connection_info->buffer_send_len = left;
        left = data_len;
    }
    else
    {
        // 这里表明当前发送缓冲区已空
        // 继续发送新数据
        p_connection_info->buffer_send_len = 0;
        left = data_len;

        while (left > 0)
        {
            send_len = send(p_connection_info->fd,
                            p_data + data_len - left,
                            left,
                            0);

            if (send_len < 0)
            {
                if (EAGAIN == errno)
                {
                    break;
                }
                else if (EINTR == errno)
                {
                    continue;
                }
                else
                {
                    close_connection(id, true);
                    return -1;
                }
            }
            else
            {
                left -= send_len;
            }
        }
    }

    if (left > 0)
    {
        // 将新数据缓冲区剩余数据拷贝至输出缓冲区中
        int buffer_empty_len
            = static_cast<int>(sizeof(p_connection_info->buffer_send))
                - p_connection_info->buffer_send_len;

        if (buffer_empty_len > 0)
        {
            if (left <= buffer_empty_len)
            {
                memcpy(p_connection_info->buffer_send
                       + p_connection_info->buffer_send_len,
                       p_data + data_len - left,
                       left);
                p_connection_info->buffer_send_len += left;
                left = 0;
            }
            else
            {
                memcpy(p_connection_info->buffer_send
                       + p_connection_info->buffer_send_len,
                       p_data + data_len - left,
                       buffer_empty_len);
                p_connection_info->buffer_send_len
                    = static_cast<int>(sizeof(p_connection_info->buffer_send));
                left -= buffer_empty_len;
            }
        }
    }

    return data_len - left;
}

int c_net_io_server::send_data_atomic(const int id,
                                      char *p_data,
                                      const int data_len)
{
    if (!m_init_)
    {
        return -1;
    }

    if (!p_data || data_len <= 0)
    {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);

    if (it == m_id_conn_map_.end())
    {
        set_error_info_(NET_IO_SERVER_ESEND,
                        "fail to find connection info according by id(%d)",
                        id);
        return -1;
    }

    connection_info_t *p_connection_info
            = reinterpret_cast<connection_info_t *>(&it->second);
    int send_len = 0;
    int left = p_connection_info->buffer_send_len;

    while (left > 0)
    {
        // 则发送原本留在发送缓冲区中的数据
        send_len = send(p_connection_info->fd,
                        p_connection_info->buffer_send
                        + p_connection_info->buffer_send_len
                        - left,
                        left,
                        0);

        if (send_len < 0)
        {
            if (EAGAIN == errno)
            {
                break;
            }
            else if (EINTR == errno)
            {
                continue;
            }
            else
            {
                close_connection(id, true);
                return -1;
            }
        }
        else
        {
            left -= send_len;
        }
    }

    if (left > 0)
    {
        // 若上次发送未发完全,则整理发送缓冲区,将数据向前靠齐
        memmove(p_connection_info->buffer_send,
                p_connection_info->buffer_send
                + p_connection_info->buffer_send_len
                - left,
                left);
    }

    p_connection_info->buffer_send_len = left;

    if (data_len >
            static_cast<int>(sizeof(p_connection_info->buffer_send))
                - p_connection_info->buffer_send_len)
    {
        // 若输出缓冲区不够放入新书据,则不进行新书据发送或拷贝,
        // 以保证发送操作原子性
        return 0;
    }

    left = data_len;

    if (0 == p_connection_info->buffer_send_len)
    {
        // 这里表明当前发送缓冲区已空
        // 继续发送新数据
        while (left > 0)
        {
            send_len = send(p_connection_info->fd,
                            p_data + data_len - left,
                            left,
                            0);

            if (send_len < 0)
            {
                if (EAGAIN == errno)
                {
                    break;
                }
                else if (EINTR == errno)
                {
                    continue;
                }
                else
                {
                    close_connection(id, true);
                    return -1;
                }
            }
            else
            {
                left -= send_len;
            }
        }
    }

    if (left > 0)
    {
        // 将新数据缓冲区剩余数据拷贝至输出缓冲区中
        // 由于事先判断过,在此一定可以将所有数据拷贝至发送缓冲区
        memcpy(p_connection_info->buffer_send
               + p_connection_info->buffer_send_len,
               p_data + data_len - left,
               left);
        p_connection_info->buffer_send_len += left;
    }

    return data_len;
}

int c_net_io_server::close_connection(const int id, bool need_notify)
{
    if (!m_init_)
    {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);

    if (it == m_id_conn_map_.end())
    {
        set_error_info_(NET_IO_SERVER_ECLOSE,
                        "fail to find connection info according by id(%d)",
                        id);
        return -1;
    }

    if (m_p_net_io_event_handler_ && need_notify)
    {
        m_p_net_io_event_handler_->on_connection_closed(
            dynamic_cast<c_net_io_server *>(this),
            id,
            it->second.fd,
            it->second.ip,
            it->second.port,
            &it->second.storage);
    }

    close(it->second.fd);
    m_id_conn_map_.erase(it);
    return 0;
}

int c_net_io_server::get_connection_info(const int id,
                                         char *ip,
                                         int ip_buffer_len,
                                         int *p_port,
                                         int *p_time,
                                         int *p_fd,
                                         union net_io_storage **pp_storage)
{
    if (!m_init_)
    {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);

    if (it == m_id_conn_map_.end())
    {
        set_error_info_(NET_IO_SERVER_ECLOSE,
                        "fail to find connection info according by id(%d)",
                        id);
        return -1;
    }

    if (ip != NULL && ip_buffer_len >= static_cast<int>(sizeof(it->second.ip)))
    {
        strcpy(ip, it->second.ip);
    }

    if (p_port != NULL)
    {
        *p_port = it->second.port;
    }

    if (p_time != NULL)
    {
        *p_time = it->second.connect_time;
    }

    if (p_fd != NULL)
    {
        *p_fd = it->second.fd;
    }

    if (pp_storage != NULL)
    {
        *pp_storage = &it->second.storage;
    }

    return 0;
}

int c_net_io_server::enum_connections(int *p_buffer,
                                      int length,
                                      int *p_buffer_len)
{
    if (!m_init_)
    {
        return -1;
    }

    if (!p_buffer || length < 1)
    {
        return -1;
    }

    int data_len = 0;

    for (id_conn_map_iter_t it = m_id_conn_map_.begin();
         (data_len < length) && (it != m_id_conn_map_.end());
         ++ it)
    {
        *(p_buffer + data_len) = it->first;
        ++ data_len;
    }

    if (p_buffer_len)
    {
        *p_buffer_len = data_len;
    }

    return 0;
}

int c_net_io_server::uninit()
{
    if (!m_init_)
    {
        return -1;
    }

    if (m_socket_listen_ >= 0)
    {
        close(m_socket_listen_);
    }

    for (id_conn_map_iter_t it = m_id_conn_map_.begin();
         it != m_id_conn_map_.end();
         ++ it)
    {
        close(it->second.fd);
    }

    m_id_conn_map_.clear();

    if (m_epoll_fd_ >= 0)
    {
        close(m_epoll_fd_);
    }

    m_errno_ = NET_IO_SERVER_OK;
    bzero(m_errstr_, sizeof(m_errstr_));
    m_flag_ = 0;
    m_init_ = 0;

    return 0;
}

int c_net_io_server::release()
{
    delete this;
    return 0;
}
