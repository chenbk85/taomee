/**
 * =====================================================================================
 *       @file  c_newbench_io_event.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/10/2010 11:15:37 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT) , tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "i_ring_queue.h"
#include "i_barrier_ring_queue.h"
#include "afutex.h"

#include "newbench.h"
#include "dll.h"
#include "connect.h"
#include "util.h"
#include "i_net_io_server.h"
#include "c_newbench_io_event.h"

extern dll_func_t dll;
extern i_barrier_ring_queue *g_prcv_brq;
extern i_ring_queue *g_prcv_rq;
extern i_ring_queue *g_psnd_rq;

extern bench_config_t g_bench_conf;

extern int* g_rcv_rq_lock;
extern afutex_opbuf_t* g_rcv_wait_op;
extern int g_stop;


char buffer_send[MAX_PACKAGE_LEN];
int buffer_send_length;

/**
 * @brief  构造函数
 * @param
 * @return
 */
c_newbench_net_io_event::c_newbench_net_io_event()
{
}

/**
 * @brief  析构函数
 * @param
 * @return
 */
c_newbench_net_io_event::~c_newbench_net_io_event()
{
    uninit();
}


/**
 * @brief  初始化
 * @param
 * @return
 */
int c_newbench_net_io_event::init()
{
    // 因为 push 时需要加上头 ，所以分配的缓冲区要能容纳这个头
    m_push_buf_len = g_bench_conf.max_pkg_len + sizeof(shm_block_t);
    m_push_buf = (char *) malloc(m_push_buf_len);

    if (m_push_buf == NULL)
    {
        m_push_buf_len = 0;
        NB_BOOT_LOG(-1, "Malloc push buffer faile, size: %d", m_push_buf_len);
        return -1;
    }

    // 预先赋值结构体中的固定变量，减少每次on_recv_data执行的代码量
    memset(m_push_buf, 0, m_push_buf_len);
    m_push_mb = (shm_block_t*)m_push_buf;
    m_push_mb->skinfo.local_ip = inet_addr(g_bench_conf.bind_list->bind_ip);
    m_push_mb->skinfo.local_port = htons(g_bench_conf.bind_list->bind_port);
    m_push_mb->skinfo.type = g_bench_conf.bind_list->type;

    return 0;
}

/**
 * @brief  反初始化
 * @param
 * @return
 */
int c_newbench_net_io_event::uninit()
{
    if (m_push_buf != NULL)
    {
        free(m_push_buf);
        m_push_buf = NULL;
    }

    return 0;
}

int c_newbench_net_io_event::on_new_connection(void *p_net_io_server,
                                               const int connection_id,
                                               const int connection_fd,
                                               const char *peer_ip,
                                               const int peer_port,
                                               union net_io_storage *p_storage)
{
    if (dll.handle_open)
    {
        if (p_net_io_server == NULL)
        {
            return -1;
        }

        m_push_mb->skinfo.sockfd = connection_fd;
        m_push_mb->skinfo.remote_ip = inet_addr(peer_ip);
        m_push_mb->skinfo.remote_port = htons(peer_port);
        // default for m_push_mb->skinfo.storage
        // ...
        m_push_mb->skinfo.ptr_lookout = p_storage;
        m_push_mb->skinfo.connection_id = connection_id;

        if (dll.handle_open(NULL, 0, &m_push_mb->skinfo) != 0)
        {
            return -1;
        }

        *p_storage = m_push_mb->skinfo.storage;
    }

    return 0;
}

/**
* @brief 底层接收到数据时通知用户:将数据放入环形队列,并通知work进程去取数据
* @param   p_net_io_server
* @param   connection_id 指示哪个连接ID上接收到数据
* @param   p_data   指向接收到的数据
* @param   data_len 接收到的数据长度
* @return  0-success -1-failed
*/
int c_newbench_net_io_event::on_recv_data(void *p_net_io_server,
                                          const int connection_id,
                                          const int connection_fd,
                                          const char *ip,
                                          const int port,
                                          char *p_data,
                                          int data_len,
                                          union net_io_storage *p_storage)
{
    if (p_net_io_server == NULL || p_data == NULL)
    {
        ERROR_LOG("Parameter error on receive data!");
        return -1;
    }

    TRACE_LOG("Recv from conn id: %d, sock fd: %d, data length: %d, sto: %lld",
              connection_id,
              connection_fd,
              data_len,
              static_cast<long long>(p_storage->u64));

    m_push_mb->blk_id = connection_id;
    m_push_mb->skinfo.sockfd = connection_fd;
    m_push_mb->skinfo.remote_ip = inet_addr(ip);
    m_push_mb->skinfo.remote_port = htons(port);
    m_push_mb->skinfo.storage = *p_storage;
    m_push_mb->skinfo.ptr_lookout = p_storage;
    m_push_mb->skinfo.connection_id = connection_id;

    int pkg_len = 0;
    int mimi_id = 0;
    int barr_flag = 0;
    char *p_push_data = p_data;

    char *puser_data;
    int sndlen;
    i_net_io_server *p_net_io_server2
        = reinterpret_cast<i_net_io_server *>(p_net_io_server);

    do
    {
        sndlen = 0;

        pkg_len = dll.handle_input(p_push_data,
                                   data_len,
                                   &puser_data,
                                   &sndlen,
                                   &m_push_mb->skinfo);

        if (sndlen > 0)
        {
            int ret = p_net_io_server2->send_data_atomic(
                        connection_id,
                        puser_data,
                        sndlen);

            if (ret != sndlen)
            {
                ERROR_LOG("fail to send_data_atomic for conn[%d] length[%d]", connection_id, sndlen);
            }
        }

        if (pkg_len < 0)
        {
            /// handle_input 返回值小于0时关闭连接
            ERROR_LOG("dll handle_input(...) return %d", pkg_len);
            return -1;
        }
        else if ((pkg_len == 0) || (pkg_len > data_len))
        {
            *p_storage = m_push_mb->skinfo.storage;

            /// 数据长度不够一个包，继续接收
            memmove(p_data, p_push_data, data_len);
            return data_len;
        }
        else
        {
            /// pkg_len > 0 && pkg_len < data_len, 接收到了完整的包，处理这个包
            *p_storage = m_push_mb->skinfo.storage;
        }

        m_push_mb->length = sizeof(shm_block_t) + pkg_len;

        if (m_push_mb->length > m_push_buf_len)
        {
            /// 数据包溢出，说明平台中的缓冲区与 net_io_server 的缓冲区不匹配
            /// 可能是最大包长配置有问题，直接关闭这个连接，可以起到一定的警告使用者的目的
            ERROR_LOG("IP:Port[%s:%d] recv data len %d > max size %d!",
                      ip, port, pkg_len, g_bench_conf.max_pkg_len);
            return -1;
        }

        memcpy(m_push_mb->data, p_push_data, pkg_len);

        if (g_bench_conf.use_barrier)
        {
            mimi_id = 0;
            barr_flag = 0;

            if (dll.handle_dispatch == NULL)
            {
                ERROR_LOG("Warning: handle_dispatch not found, pkg filter disabled.");
            }
            else
            {
                if ((barr_flag = dll.handle_dispatch(p_push_data, pkg_len, 0, &mimi_id)) < 0)
                {
                    ERROR_LOG("dll handle_dispatch failed!");
                    return -1;
                }
            }

            AFUTEX_LOCK_ERR_CHK(g_rcv_rq_lock);
            int ret = g_prcv_brq->push((char*)m_push_mb, m_push_mb->length, mimi_id, barr_flag);
            AFUTEX_UNLOCK_ERR_CHK(g_rcv_rq_lock);

            if (ret != 0)
            {
                /// 如果 push 失败，则丢弃这个包
                ERROR_LOG("Barrier push data failed, discard pkg (len: %d): %s.", pkg_len, g_prcv_brq->get_last_errstr());
            }
            else
            {
                TRACE_LOG("Barrier ring queue push data length: %d, flag: %d, key: %d", m_push_mb->length, barr_flag, mimi_id);
            }
        }
        else
        {
            if (g_prcv_rq->push_data(m_push_buf, m_push_mb->length, true) != m_push_mb->length)
            {
                /// 如果 push 失败，则丢弃这个包
                ERROR_LOG("Common ring queue push failed(ret: %d, data: %d, empty: %d), discard pkg (len: %d)",
                        g_prcv_rq->get_last_errno(), g_prcv_rq->get_data_len(), g_prcv_rq->get_empty_buffer_len(), pkg_len);
            }
            else
            {
                TRACE_LOG("Common ring queue push data length: %d", m_push_mb->length);
            }
        }

        p_push_data += pkg_len;
        data_len -= pkg_len;

    } while (data_len > 0);

    return 0;
}

/**
* @brief 底层关闭连接时通知用户
* @param  p_net_io_server
* @param  connection_id 指示关闭的是哪个连接
* @return  0-success -1-failed
*/
int c_newbench_net_io_event::on_connection_closed(void * /* p_net_io_server */,
                                                  const int connection_id,
                                                  const int connection_fd,
                                                  const char *ip,
                                                  const int port,
                                                  union net_io_storage *p_storage)
{
    TRACE_LOG("Close connection on conn id: %d, sock fd: %d", connection_id, connection_fd);

    if (dll.handle_close)
    {
        m_push_mb->skinfo.sockfd = connection_fd;
        m_push_mb->skinfo.remote_ip = inet_addr(ip);
        m_push_mb->skinfo.remote_port = htons(port);
        m_push_mb->skinfo.storage = *p_storage;

        m_push_mb->skinfo.connection_id = connection_id;

        if (dll.handle_close(&m_push_mb->skinfo) != 0)
        {
            ERROR_LOG("dll handle_close failed!");
            return -1;
        }
    }

    return 0;
}

int c_newbench_net_io_event::on_wakeup(void *p_net_io_server)
{
    i_net_io_server *p_net_io_server2
        = reinterpret_cast<i_net_io_server *>(p_net_io_server);
    shm_block_t *mb = reinterpret_cast<shm_block_t *>(buffer_send);

    while (!g_stop)
    {
        int ret = g_psnd_rq->pop_data(buffer_send, MAX_PACKAGE_LEN, 0);

        if (ret == 0)
        {
            // 无发送
            break;
        }
        else if (ret < 0)
        {
            ERROR_LOG("fail to pop data[%d]", ret);
            break;
        }
        else
        {
            if (ret != mb->length || mb->length > MAX_PACKAGE_LEN)
            {
                // 异常错误
                ERROR_LOG("exception: data length[%d] != package length or \
                           > MAX_PACKAGE_LEN", mb->length);
                break;
            }
        }

        if (mb->type == FIN_BLOCK)
        {
            p_net_io_server2->close_connection(mb->blk_id, true);
            continue;
        }

        int length_send
            = mb->length - static_cast<int32_t>(sizeof(shm_block_t));

        if (length_send > 0)
        {
            if (mb->type == BROADCAST_BLOCK)
            {
                ret = p_net_io_server2->broadcast(
                        mb->blk_id,
                        mb->data,
                        length_send,
                        1);

                if (ret)
                {
                    ERROR_LOG("broadcast not success at all, %d failed", ret);
                }
            }
            else
            {
                ret = p_net_io_server2->send_data_atomic(mb->blk_id,
                                                         mb->data,
                                                         length_send);

                if (ret != length_send)
                {
                    ERROR_LOG("fail send_data_atomic for conn[%d] length[%d] ret[%d]",
                              mb->blk_id, length_send, ret);
                }
                else
                {
                    if (mb->skinfo.ptr_lookout)
                    {
                        // 传回storage
                        *reinterpret_cast<union net_io_storage *>(mb->skinfo.ptr_lookout)
                            = mb->skinfo.storage;
                    }
                }
            }
        }
    }

    return 0;
}
