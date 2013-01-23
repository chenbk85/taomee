/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file net_io_server.cpp
 * @author baron <baron@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2011-04-06
 */

#include "net_io_server.h"

#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

//const static int MEM_BLOCK_SIZE = 4096;
const static int MEM_BLOCK_SIZE = 1;

int net_io_server_create(i_net_io_server **pp_net_io_server)
{
    if (pp_net_io_server == NULL) {
        return -1;
    }

    i_net_io_server *p_net_io_server = new (std::nothrow) net_io_server();

    if (!p_net_io_server) {
        return -1;
    }

    *pp_net_io_server = p_net_io_server;

    return 0;
}

net_io_server::net_io_server() : m_inited(false), m_id_(0), m_errno_(NET_IO_SERVER_OK),
                                 m_p_net_io_notifier_(NULL), m_flag_(0), 
                                 m_p_cont_mem(NULL), m_cont_mem_len(0)
{
    bzero(m_errstr_, sizeof(m_errstr_));
    m_id_conn_map_.clear();
    INIT_LIST_HEAD(&m_free_mem_head);
}

net_io_server::~net_io_server()
{
    if (m_inited) {
        uninit();
    }
}

int net_io_server::init(const char *ip, int port,
                        const i_net_io_event_handler *p_event_handler,
                        const i_net_io_notifier *p_net_io_notifier,
                        int id_base, int id_step)
{
    if (m_inited) {
        fprintf(stderr, "ERROR: m_inited\n");
        return -1;
    }

    if (!ip || port < 0 || port > 65535) {
        fprintf(stderr, "ERROR: !ip || port < 0 || port > 65535\n");
        return -1;
    }

    if (id_base <= 0 || id_step <= 0) {
        fprintf(stderr, "ERROR: id_base <= 0 || id_step <= 0\n");
        return -1;
    }

    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (m_listen_fd < 0) {
        set_error_info_(NET_IO_SERVER_EINIT, "fail to obtain socket(%s)", strerror(errno));
        return -1;
    }

    sockaddr_in sockaddr_listen;
    bzero(&sockaddr_listen, sizeof(sockaddr_in));
    sockaddr_listen.sin_family = AF_INET;
    sockaddr_listen.sin_port = htons(port);
    sockaddr_listen.sin_addr.s_addr = inet_addr(ip);

    if (sockaddr_listen.sin_addr.s_addr == INADDR_NONE) {
        close(m_listen_fd);
        set_error_info_(NET_IO_SERVER_EINIT, "fail to inet_addr(%s)", strerror(errno));
        return -1;
    }

    // 设置地址可重用
    int is_reuseaddr = 1;
    if (setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuseaddr, sizeof(int)) != 0) {
        close(m_listen_fd);
        set_error_info_(NET_IO_SERVER_EINIT, "fail to setsockopt(%s)", strerror(errno));
        return -1;
    }

    if (bind(m_listen_fd, reinterpret_cast<struct sockaddr *>(&sockaddr_listen),
             sizeof(struct sockaddr)) != 0) {
        close(m_listen_fd);
        set_error_info_(NET_IO_SERVER_EINIT, "fail to bind(%s)", strerror(errno));
        fprintf(stderr, "ERROR: bind: %s\n", strerror(errno));
        return -1;
    }

    // 设置SOCKET非阻塞模式
    int flags = fcntl(m_listen_fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(m_listen_fd, F_SETFL, flags);

    if (listen(m_listen_fd, NET_IO_SERVER_LISTEN_BACKLOG) != 0) {
        close(m_listen_fd);
        set_error_info_(NET_IO_SERVER_EINIT, "fail to listen(%s)", strerror(errno));
        return -1;
    }

    if ((m_epoll_fd = epoll_create(MAX_CLIENT_NUM)) < 0) {
        close(m_listen_fd);
        return -1;
    }

    epoll_event event;
    bzero(&event, sizeof(epoll_event));
    event.events = EPOLLET | EPOLLIN;
    event.data.ptr = NULL;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &event) != 0) {
        close(m_listen_fd);
        close(m_epoll_fd);
        set_error_info_(NET_IO_SERVER_EINIT, "fail to EPOLL_CTL_ADD(%s)", strerror(errno));
        return -1;
    }

    m_p_net_io_event_handler = const_cast<i_net_io_event_handler *>(p_event_handler);

    if (p_net_io_notifier) {
        int efd = p_net_io_notifier->get_eventfd();

        if (efd < 0) {
            close(m_listen_fd);
            close(m_epoll_fd);
            set_error_info_(NET_IO_SERVER_EINIT, "fail to get_fd by net_io_notifier");
            return -1;
        }

        bzero(&event, sizeof(epoll_event));
        event.events = EPOLLET | EPOLLIN;
        event.data.ptr = reinterpret_cast<void *>(-1);

        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, efd, &event) != 0) {
            close(m_listen_fd);
            close(m_epoll_fd);
            set_error_info_(NET_IO_SERVER_EINIT, "fail to EPOLL_CTL_ADD(%s)", strerror(errno));
            return -1;
        }

        m_p_net_io_notifier_ = const_cast<i_net_io_notifier *>(p_net_io_notifier);
    }

    INIT_LIST_HEAD(&m_free_mem_head);
    m_p_cont_mem = NULL;
    m_cont_mem_len = 0;
    
    m_id_ = m_id_base_ = id_base;
    m_id_step_ = id_step;
    m_inited = true;
    return 0;
}

int net_io_server::do_io(int timeout, int cmd)
{
    if (!m_inited) {
        return -1;
    }

    int epoll_ret = epoll_wait(m_epoll_fd, m_events_, sizeof(m_events_) / sizeof(epoll_event), timeout);

    if (m_p_net_io_event_handler) {
        m_p_net_io_event_handler->on_wakeup(dynamic_cast<i_net_io_server *>(this));
    }

    for (int i = 0; i < epoll_ret; ++ i) {
        // 事件响应于服务器套接字或者曾发生过满溢或者被notify
        if ((!m_events_[i].data.ptr) || (0 == i && (m_flag_ & NET_IO_SERVER_FLAG_RUNOUT))) {
            for (;;) {
                sockaddr_in peer_addr;
                bzero(&peer_addr, sizeof(struct sockaddr_in));
                socklen_t peer_addr_len = sizeof(struct sockaddr_in);
                int peer_socket = accept(m_listen_fd, reinterpret_cast<struct sockaddr *>(&peer_addr),
                                                           &peer_addr_len);

                if (peer_socket >= 0) {
                    if (!(cmd & NET_IO_SERVER_CMD_ACCEPT)) {
                        // 若调用者不希望进行新的连接则accept后立即关闭
                        close(peer_socket);
                        continue;
                    }

                    bzero(&m_conn_info, sizeof(conn_info_t));
                    m_conn_info.id = genereate_id();
                    m_conn_info.fd = peer_socket;
                    strcpy(m_conn_info.ip, inet_ntoa(peer_addr.sin_addr));
                    m_conn_info.port = ntohs(peer_addr.sin_port);
                    m_conn_info.connect_time = static_cast<int>(time(NULL));
                    m_conn_info.recv_buff_offset = 0;
                    m_conn_info.recv_buff_data_len = 0;
                    m_conn_info.send_buff_offset = 0;
                    m_conn_info.send_buff_data_len = 0;

                    if (m_p_net_io_event_handler) {
                        // 回调函数on_new_connection返回值决定是否进行连接
                        if (-1 == m_p_net_io_event_handler->on_new_connection(
                                      dynamic_cast<i_net_io_server *>(this),
                                      m_conn_info.id, m_conn_info.fd,
                                      m_conn_info.ip, m_conn_info.port,
                                      &m_conn_info.storage)) {
                            close(peer_socket);
                            continue;
                        }
                    }

                    std::pair<id_conn_map_iter_t, bool> ret = m_id_conn_map_.insert(
                            std::pair<int, conn_info_t>(m_conn_info.id, m_conn_info));

                    if (ret.second) {
                        int flags = fcntl(peer_socket, F_GETFL);
                        flags |= O_NONBLOCK;
                        fcntl(peer_socket, F_SETFL, flags);
                        epoll_event event;
                        event.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                        event.data.ptr = &ret.first->second;
                        INIT_LIST_HEAD(&(ret.first->second.recv_buff_head));
                        INIT_LIST_HEAD(&(ret.first->second.send_buff_head));

                        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, peer_socket, &event) != 0) {
                            set_error_info_(NET_IO_SERVER_EIO, "fail to EPOLL_CTL_ADD(%s)", strerror(errno));
                            close_connection(m_conn_info.id, true);
                        }
                    } else {
                        // id冲突了,极低概率事件
                        set_error_info_(NET_IO_SERVER_EIO, "fatal error:id conflict");

                        if (m_p_net_io_event_handler) {
                            m_p_net_io_event_handler->on_connection_closed(
                                dynamic_cast<net_io_server *>(this),
                                m_conn_info.id, m_conn_info.fd,
                                m_conn_info.ip, m_conn_info.port,
                                &m_conn_info.storage);
                        }

                        close(peer_socket);
                    }
                } else {                                   // peer_socket == -1
                    if (EAGAIN == errno) {
                        // 既然accept至EAGAIN状态,则将fd尽状态标志去掉
                        m_flag_ &= ~NET_IO_SERVER_FLAG_RUNOUT;
                        break;
                    } else if (EINTR == errno) {
                        // no thing to do ...
                    } else {
                        // 认为除EAGAIN以及EINTR错误状态的状态标示fd已耗尽
                        m_flag_ |= NET_IO_SERVER_FLAG_RUNOUT;
                        break;
                    }
                }
            } // for
        } // if

        if (m_p_net_io_notifier_ && m_events_[i].data.ptr == reinterpret_cast<void *>(-1)) {
            // 响应通知事件
            m_p_net_io_notifier_->got();
            continue;
        }

        if (m_events_[i].data.ptr) {
            conn_info_t *p_conn_info = reinterpret_cast<conn_info_t *>(m_events_[i].data.ptr);

            if ((m_events_[i].events & EPOLLERR) || (m_events_[i].events & EPOLLHUP) || 
                        (m_events_[i].events & EPOLLRDHUP)) {
                // 标示fd出现错误或者fd被挂起或者对端关闭连接
                close_connection(p_conn_info->id, true);
                continue;
            }

            if (m_events_[i].events & EPOLLIN) {
                if (!(cmd & NET_IO_SERVER_CMD_READ)) {
                    // 若用户未要求读操作,则不进行读操作,并且通过重新修改fd
                    // 相关事件,使之在ET模式下仍然可以在下一次被触发读
                    epoll_event event;
                    event.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                    event.data.ptr = m_events_[i].data.ptr;
                    epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, p_conn_info->fd, &event);
                    //epoll_ctl(m_epoll_fd,
                    //          EPOLL_CTL_ADD,
                    //          p_conn_info->fd,
                    //          &event);
                    continue;
                }

                int rv = recv_data(p_conn_info);
                if (rv > 0) {
                    if (p_conn_info->recv_buff_data_len > 0 && m_p_net_io_event_handler != NULL) {
                        char *p_merge_addr = merge_mem_block(&p_conn_info->recv_buff_head);
                        if (p_merge_addr == NULL) {
                            fprintf(stdout, "DEBUG: merge_mem_block\n");
                            close_connection(p_conn_info->id, true);
                            m_events_[i].data.ptr = p_conn_info = NULL;
                            continue;
                        }
                        char *p_data = p_merge_addr + p_conn_info->recv_buff_offset;
                        int data_len = p_conn_info->recv_buff_data_len;

                        int left_data_len = m_p_net_io_event_handler->on_recv_data(this,
                                    p_conn_info->id, p_conn_info->fd, p_conn_info->ip, p_conn_info->port,
                                    p_data, data_len, &p_conn_info->storage);
                        int consume_data_len = data_len - left_data_len;
                        for (int i = 0; i != ((consume_data_len + p_conn_info->recv_buff_offset) / MEM_BLOCK_SIZE); ++i) {
                            free_mem_block(p_conn_info->recv_buff_head.next);
                        }
                        p_conn_info->recv_buff_offset = (consume_data_len + p_conn_info->recv_buff_offset) % MEM_BLOCK_SIZE;
                        p_conn_info->recv_buff_data_len = left_data_len;
                        if (p_conn_info->recv_buff_offset != 0 && p_conn_info->recv_buff_data_len == 0) {
                            free_mem_block(p_conn_info->recv_buff_head.next);
                            p_conn_info->recv_buff_offset = 0;
                        }
                    }
                } else if (rv == 0) {
                    fprintf(stdout, "DEBUG: connection closed by peer party\n");
                    close_connection(p_conn_info->id, true);
                    m_events_[i].data.ptr = p_conn_info = NULL;
                } else {
                    fprintf(stderr, "ERROR: recv_data\n");
                    close_connection(p_conn_info->id, true);
                    m_events_[i].data.ptr = p_conn_info = NULL;
                }
            } // if

            if (p_conn_info && m_events_[i].events & EPOLLOUT && p_conn_info->send_buff_data_len > 0) {
                if (!(cmd & NET_IO_SERVER_CMD_WRITE)) {
                    // 若用户未要求写操作,则不进行写操作,并且通过重新修改fd
                    // 相关事件,使之在ET模式下仍然可以在下一次被触发写
                    epoll_event event;
                    event.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                    event.data.ptr = m_events_[i].data.ptr;
                    //epoll_ctl(m_epoll_fd,
                    //          EPOLL_CTL_ADD,
                    //          p_conn_info->fd,
                    //          &event);
                    epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, p_conn_info->fd, &event);
                    continue;
                }

                if (send_data(p_conn_info) < 0) {
                    fprintf(stderr, "ERROR: send_data\n");
                    close_connection(p_conn_info->id, true);
                    m_events_[i].data.ptr = p_conn_info = NULL;
                }
            } // if
        } // if
    } // for 主循环,遍历事件

    return 0;
}

int net_io_server::send_data(int id, const char *p_data, int data_len)
{
    if (!m_inited) {
        return -1;
    }

    if (p_data == NULL || data_len <= 0) {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);
    if (it == m_id_conn_map_.end()) {
        set_error_info_(NET_IO_SERVER_ESEND, "fail to find connection info according by id(%d)", id);
        return -1;
    }

    conn_info_t *p_conn_info = reinterpret_cast<conn_info_t *>(&it->second);
    
    return send_data(p_conn_info, p_data, data_len);
}

int net_io_server::broadcast(int id, const char *p_data, int data_len, int is_atomic)
{
    if (!m_inited) {
        return -1;
    }

    int ret = 0;

    for (id_conn_map_iter_t it = m_id_conn_map_.begin(); it != m_id_conn_map_.end(); ++ it) {
        if (it->first != id) {
            if (is_atomic && send_data(reinterpret_cast<conn_info_t *>(&it->second), p_data, data_len) < 0) {
                ++ ret;
            } else if (!is_atomic && send_data(reinterpret_cast<conn_info_t *>(&it->second),
                         p_data,
                         data_len) != data_len) {
                ++ ret;
            } else {
                // nothing to do ...
            }
        }
    }

    return ret;
}

int net_io_server::close_connection(int id, bool need_notify)
{
    if (!m_inited) {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);

    if (it == m_id_conn_map_.end()) {
        set_error_info_(NET_IO_SERVER_ECLOSE, "fail to find connection info according by id(%d)", id);
        return -1;
    }

    if (m_p_net_io_event_handler && need_notify) {
        m_p_net_io_event_handler->on_connection_closed(dynamic_cast<net_io_server *>(this),
            id, it->second.fd, it->second.ip, it->second.port, &it->second.storage);
    }

    close(it->second.fd);

    struct list_head *list;
    struct list_head *temp;
    list_for_each_safe (list, temp, &it->second.recv_buff_head) {
        free_mem_block(list);
    }
    list_for_each_safe (list, temp, &it->second.send_buff_head) {
        free_mem_block(list);
    }

    m_id_conn_map_.erase(it);

    return 0;
}

int net_io_server::get_connection_info(int id, char *ip, int ip_buffer_len, int *p_port, int *p_time,
                                         int *p_fd, union net_io_storage **pp_storage)
{
    if (!m_inited) {
        return -1;
    }

    id_conn_map_iter_t it = m_id_conn_map_.find(id);

    if (it == m_id_conn_map_.end()) {
        set_error_info_(NET_IO_SERVER_ECLOSE, "fail to find connection info according by id(%d)", id);
        return -1;
    }

    if (ip != NULL && ip_buffer_len >= static_cast<int>(sizeof(it->second.ip))) {
        strcpy(ip, it->second.ip);
    }

    if (p_port != NULL) {
        *p_port = it->second.port;
    }

    if (p_time != NULL) {
        *p_time = it->second.connect_time;
    }

    if (p_fd != NULL) {
        *p_fd = it->second.fd;
    }

    if (pp_storage != NULL) {
        *pp_storage = &it->second.storage;
    }

    return 0;
}

int net_io_server::enum_connections(int *p_buffer, int length, int *p_buffer_len)
{
    if (!m_inited) {
        return -1;
    }

    if (!p_buffer || length < 1) {
        return -1;
    }

    int data_len = 0;

    for (id_conn_map_iter_t it = m_id_conn_map_.begin(); (data_len < length) && (it != m_id_conn_map_.end()); ++ it) {
        *(p_buffer + data_len) = it->first;
        ++ data_len;
    }

    if (p_buffer_len) {
        *p_buffer_len = data_len;
    }

    return 0;
}

int net_io_server::uninit()
{
    if (!m_inited) {
        return -1;
    }

    if (m_listen_fd >= 0) {
        close(m_listen_fd);
    }

    if (m_epoll_fd >= 0) {
        close(m_epoll_fd);
    }

    for (id_conn_map_iter_t it = m_id_conn_map_.begin(); it != m_id_conn_map_.end(); ++ it) {
        close_connection(it->second.id, true);
    }

    m_id_conn_map_.clear();

    mem_block_t *p_mem_block = NULL;
    mem_block_t *p_mem_temp = NULL;
    list_for_each_entry_safe (p_mem_block, p_mem_temp, &m_free_mem_head, list) {
        free(p_mem_block->p_addr);
        list_del(&p_mem_block->list);
    }

    if (m_p_cont_mem != NULL) {
        free(m_p_cont_mem);
    }

    m_errno_ = NET_IO_SERVER_OK;
    bzero(m_errstr_, sizeof(m_errstr_));
    m_flag_ = 0;
    m_inited = false;

    return 0;
}

int net_io_server::release()
{
    delete this;
    return 0;
}

int net_io_server::genereate_id()
{
    int id = m_id_;

    if ((m_id_ += m_id_step_) < 0) {
        m_id_ = m_id_base_;
    }

    return id;
}

void net_io_server::set_error_info_(int error_no, const char *msg, ...)
{
    m_errno_ = error_no;
    va_list args;
    va_start(args, msg);
    vsprintf(m_errstr_, msg, args);
    va_end(args);
}

int net_io_server::get_last_errno()
{
    return m_errno_;
}

const char *net_io_server::get_last_errstr()
{
    return m_errstr_;
}
    
int net_io_server::alloc_mem_block(struct list_head *head)
{
    fprintf(stdout, "DEBUG: alloc_mem_block\n");
    if (list_empty(&m_free_mem_head)) {
        mem_block_t *p_mem_block = (mem_block_t *)malloc(sizeof(mem_block_t) + MEM_BLOCK_SIZE);
        memset(p_mem_block, 0, sizeof(mem_block_t) + MEM_BLOCK_SIZE);
        if (p_mem_block == NULL) {
            fprintf(stderr, "ERROR: malloc\n");
            return -1;
        }
        p_mem_block->p_addr = ((char *)p_mem_block) + sizeof(mem_block_t);
        list_add_tail(&p_mem_block->list, &m_free_mem_head);
    }

    list_move_tail(m_free_mem_head.next, head);

    return 0;
}
   
int net_io_server::free_mem_block(struct list_head *list)
{
    fprintf(stdout, "DEBUG: free_mem_block\n");
    list_move_tail(list, &m_free_mem_head);
    
    return 0;
}
    
char * net_io_server::get_first_mem_block_addr(struct list_head *head)
{
    return list_entry(head->next, mem_block_t, list)->p_addr;
}
    
char * net_io_server::get_last_mem_block_addr(struct list_head *head)
{
    return list_entry(head->prev, mem_block_t, list)->p_addr;
}
    
int net_io_server::recv_data(conn_info_t *p_conn_info)
{
    fprintf(stdout, "DEBUG: recv_data\n");
    int rv = -1;
    int recv_data_len = 0;

    for (;;) {
        // 检查是否需要分配新的内存块
        int used_len = p_conn_info->recv_buff_offset + p_conn_info->recv_buff_data_len;
        if (used_len % MEM_BLOCK_SIZE == 0) {
            if (alloc_mem_block(&p_conn_info->recv_buff_head) != 0) {
                fprintf(stderr, "ERROR: alloc_mem_block\n");
                return -1;
            }
        }
        int idle_len = MEM_BLOCK_SIZE  - used_len % MEM_BLOCK_SIZE;
        char *p_mem_block_addr = get_last_mem_block_addr(&p_conn_info->recv_buff_head);

        rv = recv(p_conn_info->fd, p_mem_block_addr + (used_len % MEM_BLOCK_SIZE), idle_len, 0);
        if (rv > 0) {
            p_conn_info->recv_buff_data_len += rv;
            recv_data_len += rv;
            continue;
        } else if (rv == 0) {
            fprintf(stdout, "DEBUG: connection close by peer party\n");
            if (used_len % MEM_BLOCK_SIZE == 0) {
                free_mem_block(p_conn_info->recv_buff_head.prev);
            }
            return 0;
        } else if (rv == -1 && errno == EINTR) {
            if (used_len % MEM_BLOCK_SIZE == 0) {
                free_mem_block(p_conn_info->recv_buff_head.prev);
            }
            continue;
        } else if (rv == -1 && errno == EAGAIN) {
            if (used_len % MEM_BLOCK_SIZE == 0) {
                free_mem_block(p_conn_info->recv_buff_head.prev);
            }
            break;
        } else {
            fprintf(stderr, "ERROR: recv: %s\n", strerror(errno));
            if (used_len % MEM_BLOCK_SIZE == 0) {
                free_mem_block(p_conn_info->recv_buff_head.prev);
            }
            return -1;
        }
    }

    return recv_data_len;
}
    
char * net_io_server::merge_mem_block(struct list_head *head)
{
    if (single_mem_block(head)) {
        return list_entry(head->next, mem_block_t, list)->p_addr;
    }

    int idle_cont_len = m_cont_mem_len;
    int used_cont_len = 0;

    mem_block_t *p_mem_block = NULL;
    list_for_each_entry (p_mem_block, head, list) {
        if (idle_cont_len == 0) {
            m_p_cont_mem = (char *)realloc(m_p_cont_mem, m_cont_mem_len + MEM_BLOCK_SIZE);
            if (m_p_cont_mem == NULL) {
                return NULL;
            }
            m_cont_mem_len += MEM_BLOCK_SIZE;
            idle_cont_len += MEM_BLOCK_SIZE;
        }
        memcpy(m_p_cont_mem + used_cont_len, p_mem_block->p_addr, MEM_BLOCK_SIZE);
        idle_cont_len -= MEM_BLOCK_SIZE;
        used_cont_len += MEM_BLOCK_SIZE;
    }

    return m_p_cont_mem;
}

bool net_io_server::single_mem_block(list_head *head)
{
    return head->next->next == head;
}
    
int net_io_server::send_data(conn_info_t *p_conn_info, const char *p_data, int data_len)
{
    if (send_data(p_conn_info) == -1) {
        fprintf(stderr, "ERROR: send_data\n");
        return -1;
    }

    int send_data_len = 0;
    int left_data_len = data_len;

    if (p_conn_info->send_buff_data_len == 0) {
        while (left_data_len > 0) { 
            int rv = send(p_conn_info->fd, p_data + send_data_len, left_data_len, 0);
            if (rv > 0) {
                send_data_len += rv;
                left_data_len -= rv;
            } else if (rv == -1 && errno == EINTR) {
                continue;
            } else if (rv == -1 && errno == EAGAIN) {
                break;
            } else {
                fprintf(stderr, "ERROR: send: %s\n", strerror(errno));
                return -1;
            }
        }
    }

    while (left_data_len > 0) {
        // 检查是否需要分配新的内存块
        int used_len = p_conn_info->send_buff_offset + p_conn_info->send_buff_data_len;
        if (used_len % MEM_BLOCK_SIZE == 0) {
            if (alloc_mem_block(&p_conn_info->send_buff_head) != 0) {
                fprintf(stderr, "ERROR: alloc_mem_block\n");
                return -1;
            }
        }
        int idle_len = MEM_BLOCK_SIZE  - used_len % MEM_BLOCK_SIZE;
        char *p_mem_block_addr = get_last_mem_block_addr(&p_conn_info->send_buff_head);
        int copy_len = std::min(idle_len, left_data_len);
        memcpy(p_mem_block_addr + used_len % MEM_BLOCK_SIZE, p_data + send_data_len, copy_len);
        left_data_len -= copy_len;
        p_conn_info->send_buff_data_len += copy_len;
    }

    return send_data_len;
}
    
int net_io_server::send_data(conn_info_t *p_conn_info)
{
    fprintf(stdout, "DEBUG: send_data\n");
    int rv = -1;
    int send_data_len = 0;

    while (p_conn_info->send_buff_data_len > 0) {
        char *p_first_mem_block_addr = get_first_mem_block_addr(&p_conn_info->send_buff_head);
        int first_mem_block_data_len = 0;

        if (single_mem_block(&p_conn_info->send_buff_head)) {
            first_mem_block_data_len = p_conn_info->send_buff_data_len;
        } else {
            first_mem_block_data_len = MEM_BLOCK_SIZE - p_conn_info->send_buff_offset;
        }

        rv = send(p_conn_info->fd, p_first_mem_block_addr + p_conn_info->send_buff_offset, 
                    first_mem_block_data_len, 0);

        if (rv >= 0) {
            if (rv == first_mem_block_data_len) {
                free_mem_block(p_conn_info->send_buff_head.next);
                p_conn_info->send_buff_offset = 0;
            }
            p_conn_info->send_buff_data_len -= rv;
            send_data_len += rv;
            continue;
        } else if (rv == -1 && errno == EINTR) {
            continue;
        } else if (rv == -1 && errno == EAGAIN) {
            break;
        } else {
            fprintf(stderr, "ERROR: send: %s\n", strerror(errno));
            return -1;
        }
    }

    return send_data_len;
}
