/**
 * =====================================================================================
 *       @file  c_newbench_io_event.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/10/2010 11:18:05 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu , tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef _C_NEWBENCH_IO_EVENT_H_
#define _C_NEWBENCH_IO_EVENT_H_

#include "connect.h"
#include "i_net_io_server.h"

/**
 * @brief  网络读写事件类
 */
class c_newbench_net_io_event: public i_net_io_event_handler
{
public:
    c_newbench_net_io_event();
    ~c_newbench_net_io_event();
    int init();
    int uninit();
    /**
     * @brief  底层监听到有新的连接到来时通知用户
     * @param   p_net_io_server
     * @param   connection_id  连接ID
     * @param   connection_fd
     * @param   peer_ip        对端IP
     * @param   peer_port     对端端口号
     * @return  0success -1failed
     */
    int on_new_connection(void *p_net_io_server,
                          const int connection_id,
                          const int connection_fd,
                          const char *peer_ip,
                          const int peer_port,
                          union net_io_storage *p_storage);

    /**
     * @brief 底层接收到数据时通知用户
     * @param   p_net_io_server
     * @param   connection_id 指示哪个连接ID上接收到数据
     * @param   connection_fd
     * @param   p_data   指向接收到的数据
     * @param   data_len 接收到的数据长度
     * @return  0success -1failed
     */
    int on_recv_data(void *p_net_io_server,
                     const int connection_id,
                     const int connection_fd,
                     const char *ip,
                     const int port,
                     char *p_data,
                     int data_len,
                     union net_io_storage *p_storage);

    /**
     * @brief 底层关闭连接时通知用户(连接关闭之前)
     * @param   p_net_io_server
     * @param   connection_id 指示关闭的是哪个连接
     * @param   connection_fd
     * @return  0success -1failed
     */
    int on_connection_closed(void *p_net_io_server,
                             const int connection_id,
                             const int connection_fd,
                             const char *ip,
                             const int port,
                             union net_io_storage *p_storage);

    /**
     * @brief 唤醒时的回调函数
     *
     * @param p_net_io_server
     *
     * @return
     */
    int on_wakeup(void *p_net_io_server);

private:
    bool m_use_barrier;
    u_int m_max_pkg_len;
    char *m_push_buf;
    int m_push_buf_len;
    shm_block_t* m_push_mb;
};

#endif
