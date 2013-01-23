/**
 * @file connections_pool.h
 * @brief 连接池管理类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-15
 */
#ifndef _H_CONNECTIONS_POOL_H_
#define _H_CONNECTIONS_POOL_H_

#include <stdint.h>
#include <stdlib.h>

#include <new>
#include <boost/noncopyable.hpp>

#include "log.h"

#define HASHMAP_SIZE 98317

template<typename connection_t>
class connections_pool : boost::noncopyable{
public:

    ~connections_pool() {
        m_p_conns_pool_ = NULL;
    }

    /**
     * @brief 从连接池中获得新连接
     * @return success: 新连接的指针; failed: NULL
     */
    connection_t* get_new_connection();

    /**
     * @brief 释放该连接
     * @param p_conn  所需释放的连接指针
     * @return -1failed, 0success
     */
    int free_connection(connection_t* p_conn);

    /**
     * @brief 释放该连接
     * @param conn_id  所需释放的连接id
     * @return -1failed, 0success
     */
    int free_connection(uint32_t conn_id);

    /**
     * @brief 通过连接id找出此连接的指针
     * @param conn_id  连接id
     * @return success：连接指针; failed：NULL
     */
    connection_t* search_connection(uint32_t conn_id);

    /**
     * @brief 获得此连接池的实例指针
     * @return 此连接池的实例指针
     */
    static connections_pool<connection_t>* instance();

    /**
     * @brief 删除此连接池中所有的连接
     * @return NULL
     */
    void destroy_conns_pool();

    /**
     * @brief 获得此连接池的类型
     * @return 连接池的类型
     */
    const uint32_t get_typeid() const { return m_type_id_; }

protected:
    connections_pool(): m_hashmap_size_(HASHMAP_SIZE),
                        m_pp_hashmap_(NULL),
                        m_p_free_conn_(NULL),
                        m_type_id_(0),
                        m_total_conn_num_(0)
    {

    }

private:
    /**<@连接池指针*/
    static connections_pool<connection_t>* m_p_conns_pool_;

    /**<@连接池中hash表大小*/
    const int m_hashmap_size_;

    /**<@hash表*/
    connection_t** m_pp_hashmap_;

    /**<@free链表*/
    connection_t* m_p_free_conn_;

    /**<@连接池分配的id*/
    static uint32_t m_allocate_conn_id_;

    /**<@连接池类型*/
    uint32_t m_type_id_;

    /**<@连接池中的连接总数*/
    uint32_t m_total_conn_num_;
};


template<typename connection_t>
connections_pool<connection_t>* connections_pool<connection_t>::m_p_conns_pool_ = NULL;

template<typename connection_t>
uint32_t connections_pool<connection_t>::m_allocate_conn_id_ = 0;

template<typename connection_t>
connections_pool<connection_t>* connections_pool<connection_t>::instance()
{
    m_p_conns_pool_ = (m_p_conns_pool_ == NULL) ?
        new (std::nothrow) connections_pool<connection_t>() : m_p_conns_pool_;
    return m_p_conns_pool_;
}

template<typename connection_t>
connection_t* connections_pool<connection_t>::get_new_connection()
{
    ///若没有创建hash表，则创建新的hash表
    if (!m_pp_hashmap_) {
        m_pp_hashmap_ = (connection_t**) ::calloc(m_hashmap_size_, sizeof(connection_t*));
        if (!m_pp_hashmap_) {
            ERROR_LOG("m_pp_hashmap_ calloc failed");
            return NULL;
        }
    }

    connection_t* p_new_conn = NULL;
    ///若空闲链表上有节点，则先从空闲链表上摘除
    if (m_p_free_conn_) {
        p_new_conn = m_p_free_conn_;
        m_p_free_conn_ = m_p_free_conn_->p_next;
    } else {
        p_new_conn = new (std::nothrow) connection_t();
        if (!p_new_conn) {
            ERROR_LOG("new connection_t() failed");
            return NULL;
        }
    }

    ///若没有获得类型，则先获得类型
    if (!m_type_id_)
        m_type_id_ = p_new_conn->get_protocol_type();

    uint32_t conn_id =
        m_type_id_ | (connections_pool<connection_t>::m_allocate_conn_id_++ << 8);
    p_new_conn->set_connection_id(conn_id);

    ///插入hash表
    connection_t** p = &m_pp_hashmap_[conn_id % m_hashmap_size_];
    p_new_conn->p_next = *p;
    *p = p_new_conn;
    m_total_conn_num_++;

    return p_new_conn;
}

template<typename connection_t>
int connections_pool<connection_t>::free_connection(connection_t* p_conn)
{
    if (!p_conn)
        return -1;

    uint32_t conn_id = p_conn->get_connection_id();
    connection_t* p = m_pp_hashmap_[conn_id % m_hashmap_size_];
    connection_t **prev = &m_pp_hashmap_[conn_id % m_hashmap_size_];

    while (p) {
        if (p->get_connection_id() == conn_id) {
            *prev = p->p_next;
            m_total_conn_num_--;

            ///将节点放入free链表
            p->p_next = m_p_free_conn_;
            m_p_free_conn_ = p;
            break;
        } else {
            prev = &p->p_next;
            p = p->p_next;
        }
    }
    return 0;
}

template<typename connection_t>
int connections_pool<connection_t>::free_connection(uint32_t conn_id)
{
    connection_t* p = m_pp_hashmap_[conn_id % m_hashmap_size_];
    connection_t **prev = &m_pp_hashmap_[conn_id % m_hashmap_size_];

    while (p) {
        if (p->get_connection_id() == conn_id) {
            *prev = p->p_next;
            m_total_conn_num_--;

            ///将节点放入free链表
            p->p_next = m_p_free_conn_;
            m_p_free_conn_ = p;
            break;
        } else {
            prev = &p->p_next;
            p = p->p_next;
        }
    }
    return 0;
}

template<typename connection_t>
connection_t* connections_pool<connection_t>::search_connection(uint32_t conn_id)
{
    connection_t* p = m_pp_hashmap_[conn_id % m_hashmap_size_];
    while (p) {
        if (p->get_connection_id() == conn_id) {
            return p;
        } else {
            p = p->p_next;
        }
    }
    return NULL;
}

template<typename connection_t>
void connections_pool<connection_t>::destroy_conns_pool()
{
    if (m_pp_hashmap_) {
        for (int i = 0; i < m_hashmap_size_; i++) {
            connection_t* t = m_pp_hashmap_[i];
            while (t) {
                connection_t* f = t;
                t = t->p_next;
                delete f;
            }
        }
    }

    connection_t* t = m_p_free_conn_;
    while (t) {
        connection_t* f = t;
        t = t->p_next;
        delete f;
    }
    m_p_free_conn_ = NULL;

    if (m_pp_hashmap_) {
        ::free(m_pp_hashmap_);
        m_pp_hashmap_ = NULL;
    }

    m_total_conn_num_ = 0;
    connections_pool<connection_t>::m_allocate_conn_id_ = 0;
}

#endif
