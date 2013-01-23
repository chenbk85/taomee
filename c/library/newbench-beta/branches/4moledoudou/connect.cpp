/**
 * =====================================================================================
 *       @file  connect.cpp
 *      @brief  conn进程
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2010 10:58:59 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "i_net_io_server.h"
#include "i_ring_queue.h"
#include "i_barrier_ring_queue.h"
#include "c_newbench_io_event.h"
#include "afutex.h"

#include "setproctitle.h"
#include "newbench.h"
#include "daemon.h"
#include "dll.h"
#include "util.h"
#include "connect.h"

extern volatile int g_stop;
extern dll_func_t dll;
extern i_barrier_ring_queue *g_prcv_brq;
extern i_ring_queue *g_prcv_rq;
extern i_ring_queue *g_psnd_rq;
extern bench_config_t g_bench_conf;
extern int* g_rcv_rq_lock;
i_net_io_server *pnb_io_server = NULL;

extern i_net_io_notifier *g_p_net_io_notifier;

/**
 * @brief  创建conn进程
 * @param
 * @return 0:success, -1:fail
 */
int conn_proc(int argc, char **argv)
{
    //建立TCP连接
    if (g_bench_conf.bind_list == NULL)
    {
        NB_BOOT_LOG(-1, "Bind info is null.");
        return -1;
    }

    //暂时仅支持一个端口
    struct bind_config *bc = g_bench_conf.bind_list;
    if (bc->type != SOCK_STREAM)
    {
        NB_BOOT_LOG(-1, "Warning: cannot support UDP now.");
        return -1;
    }

    c_newbench_net_io_event nb_event;
    if (nb_event.init() != 0)
    {
        NB_BOOT_LOG(-1, "Init c_newbench_net_io_event object failed.");
        return -1;
    }

    if (pnb_io_server->init(bc->bind_ip,
                            bc->bind_port,
                            &nb_event,
                            g_p_net_io_notifier,
                            1,
                            1) != 0)
    {
        NB_BOOT_LOG(-1, "Bind [%s:%u]: %s", bc->bind_ip,
                bc->bind_port, pnb_io_server->get_last_errstr());
        return -1;
    }

    NB_BOOT_LOG(0, "Listen on %s:%u", bc->bind_ip, bc->bind_port);

    int psnd_data_len = g_bench_conf.max_pkg_len + sizeof(shm_block_t);
    char *psnd_data = (char *)malloc(psnd_data_len);
    if (psnd_data == NULL)
    {
        NB_BOOT_LOG(-1, "Malloc package send buffer fail, size: %d B", g_bench_conf.max_pkg_len);
        return -1;
    }

    if (dll.handle_init && dll.handle_init(argc, argv, PROC_CONN) != 0)
    {
        NB_BOOT_LOG(-1, "Conn handle init fail.");
        return -1;
    }

    setproctitle("%s:[CONN]", g_bench_conf.prog_name);

    shm_block_t* mb = (shm_block_t*)psnd_data;

    while(!g_stop)
    {
        // 每次阻塞1毫秒, 因后面要发送数据
        // TODO:时间应该是可配的
        if (pnb_io_server->do_io(4,
                                 NET_IO_SERVER_CMD_ACCEPT
                                 | NET_IO_SERVER_CMD_READ
                                 | NET_IO_SERVER_CMD_WRITE) != 0)
        {
            ERROR_LOG("do_io fail: %s", pnb_io_server->get_last_errstr());
        }

        // 发送发送环形队列中的数据包, 发干净为止
        while (!g_stop)
        {
            // 取出数据包
            int ret = g_psnd_rq->pop_data(psnd_data, psnd_data_len, 0);

            if (ret == 0)
            {
                break;
            }
            else if (ret < 0)
            {
                // 发生错误
                ERROR_LOG("Pop data error, ret: %d, err: %s", ret, g_psnd_rq->get_last_errstr());
                break;
            }
            else
            {
                if (ret != mb->length || mb->length > psnd_data_len)
                {
                    // 出错, 打印日志以后退出发送
                    ERROR_LOG("pop data len %d != pkg len %d or > psnd_data_len", mb->length, psnd_data_len);
                    break;
                }
            }

            if (mb->type == FIN_BLOCK)
            {
                pnb_io_server->close_connection(mb->blk_id, true);
                continue;
            }

            TRACE_LOG("common ring queue pop data length: %d", mb->length);

            // 发送数据
            int bytes_sent = 0;
            int data_len = mb->length - sizeof(shm_block_t);

            if (data_len)
            {
                if (mb->type == BROADCAST_BLOCK)
                {
                    ret = pnb_io_server->broadcast(
                            mb->blk_id,
                            (char*)mb->data + bytes_sent,
                            data_len - bytes_sent,
                            1);

                    if (ret)
                    {
                        // 广播报文为全部成功
                        ERROR_LOG("broadcast not success at all, %d failed",
                                  ret);
                    }
                }
                else
                {
                    ret = pnb_io_server->send_data_atomic(
                            mb->blk_id,
                            (char*)mb->data + bytes_sent,
                            data_len - bytes_sent);

                    if (ret != data_len - bytes_sent)
                    {
                        // 发送数据失败时丢弃这个包
                        ERROR_LOG("Send to conn(%d) fail, len: %d, discard this pkg.", mb->blk_id, mb->length);
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
    }

    free(psnd_data);

    if (dll.handle_fini)
    {
        dll.handle_fini(PROC_CONN);
    }

    return 0;
}
