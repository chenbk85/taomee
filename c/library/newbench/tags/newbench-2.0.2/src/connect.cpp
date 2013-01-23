/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
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
#include "newbench_io_event.h"
#include "afutex.h"

#include "newbench_so.h"
#include "newbench_util.h"
#include "newbench.h"
#include "daemon.h"
#include "dll.h"
#include "util.h"
#include "connect.h"

extern volatile int g_stop;
extern i_barrier_ring_queue *g_prcv_brq;
extern i_ring_queue *g_prcv_rq;
extern i_ring_queue *g_psnd_rq;
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
    if (g_bench_conf.bind_list == NULL) {
        print_prompt(false, "Bind info is null.");
        return -1;
    }

    //暂时仅支持一个端口
    struct bind_config *bc = g_bench_conf.bind_list;
    if (bc->type != SOCK_STREAM) {
        print_prompt(false, "Warning: cannot support UDP now.");
        return -1;
    }

    newbench_net_io_event nb_event;
    if (nb_event.init() != 0) {
        print_prompt(false, "Init newbench_net_io_event object failed.");
        return -1;
    }

    if (pnb_io_server->init(bc->bind_ip, bc->bind_port, &nb_event, g_p_net_io_notifier, 1, 1) != 0) {
        print_prompt(false, "Bind [%s:%u]: %s", bc->bind_ip,
                bc->bind_port, pnb_io_server->get_last_errstr());
        return -1;
    }

    print_prompt(true, "Listen on %s:%u", bc->bind_ip, bc->bind_port);

    int psnd_data_len = g_bench_conf.max_pkg_len + sizeof(shm_block_t);
    char *psnd_data = (char *)malloc(psnd_data_len);
    if (psnd_data == NULL) {
        print_prompt(false, "Malloc package send buffer fail, size: %d B", g_bench_conf.max_pkg_len);
        return -1;
    }

    setproctitle("%s:[CONN]", g_bench_conf.prog_name);
    
    if (dll.handle_init && dll.handle_init(argc, argv, PROC_CONN) != 0) {
        print_prompt(false, "Conn handle init fail.");
        return -1;
    }

    shm_block_t* mb = (shm_block_t*)psnd_data;

    while(!g_stop) {
        // 每次阻塞1毫秒, 因后面要发送数据
        // TODO:时间应该是可配的
        if (pnb_io_server->do_io(4, NET_IO_SERVER_CMD_ACCEPT | NET_IO_SERVER_CMD_READ | NET_IO_SERVER_CMD_WRITE) != 0) {
            ERROR_LOG("do_io fail: %s", pnb_io_server->get_last_errstr());
        }

        // 发送发送环形队列中的数据包, 发干净为止
        while (!g_stop) {
            // 取出数据包
            int ret = g_psnd_rq->pop_data(psnd_data, psnd_data_len, 0);

            if (ret == 0) {
                break;
            } else if (ret < 0) {
                // 发生错误
                ERROR_LOG("Pop data error, ret: %d, err: %s", ret, g_psnd_rq->get_last_errstr());
                break;
            } else {
                if (ret != mb->length || mb->length > psnd_data_len) {
                    // 出错, 打印日志以后退出发送
                    ERROR_LOG("pop data len %d != pkg len %d or > psnd_data_len", mb->length, psnd_data_len);
                    break;
                }
            }

            if (mb->type == FIN_BLOCK) {
                pnb_io_server->close_connection(mb->blk_id, true);
                continue;
            }

            TRACE_LOG("common ring queue pop data length: %d", mb->length);

            // 发送数据
            int bytes_sent = 0;
            int data_len = mb->length - sizeof(shm_block_t);

            if (data_len) {
                if (mb->type == BROADCAST_BLOCK) {
                    ret = pnb_io_server->broadcast(mb->blk_id, (char*)mb->data + bytes_sent, data_len - bytes_sent, 1);

                    if (ret) {
                        // 广播报文为全部成功
                        ERROR_LOG("broadcast not success at all, %d failed", ret);
                    }
                } else {
                    ret = pnb_io_server->send_data(mb->blk_id, (char*)mb->data + bytes_sent, data_len - bytes_sent);

                    if (ret != data_len - bytes_sent) {
                        // 发送数据失败时丢弃这个包
                        ERROR_LOG("ret: %d != data_len: %d - bytes_sent: %d", ret, data_len, bytes_sent);
                        ERROR_LOG("Send to conn(%d) fail, len: %d, discard this pkg.", mb->blk_id, mb->length);
                        pnb_io_server->close_connection(mb->blk_id, true);
                    } else {
                        if (mb->skinfo.ptr_lookout) {
                            // 传回storage
                            reinterpret_cast<union net_io_storage *>(mb->skinfo.ptr_lookout)->u64
                                = mb->skinfo.storage.u64;
                        }
                    }
                }
            }
        }
    }

    pnb_io_server->uninit();

    free(psnd_data);

    if (dll.handle_fini) {
        dll.handle_fini(PROC_CONN);
    }

    return 0;
}
