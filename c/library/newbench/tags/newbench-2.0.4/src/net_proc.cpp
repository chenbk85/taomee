/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file net_proc.cpp
 * @author tonyliu <tonyliu@taomee.com>
 */

#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
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
#include "net_proc.h"

extern volatile int g_stop;
extern i_barrier_ring_queue *g_prcv_brq;
extern i_ring_queue *g_prcv_rq;
extern i_ring_queue *g_psnd_rq;
extern int* g_rcv_rq_lock;
static i_net_io_server *g_p_net_io_server = NULL;

int net_proc(int argc, char **argv)
{
    //建立TCP连接
    if (g_bench_conf.bind_list == NULL) {
        print_prompt(false, "Bind info is null.");
        tell_parent('\xFF');
        return -1;
    }

    //暂时仅支持一个端口
    struct bind_config *bc = g_bench_conf.bind_list;
    if (bc->type != SOCK_STREAM) {
        print_prompt(false, "Warning: cannot support UDP now.");
        tell_parent('\xFF');
        return -1;
    }

    newbench_net_io_event nb_event;
    if (nb_event.init() != 0) {
        print_prompt(false, "Init newbench_net_io_event object failed.");
        tell_parent('\xFF');
        return -1;
    }
    
    if (net_io_server_create(&g_p_net_io_server) != 0 || (g_p_net_io_server == NULL)) {
        print_prompt(false, "net_io_server_create failed.");
        tell_parent('\xFF');
        return -1;
    }

    if (g_p_net_io_server->init(bc->bind_ip, bc->bind_port, &nb_event, g_p_net_io_notifier, 1, 1) != 0) {
        print_prompt(false, "Bind [%s:%u]: %s", bc->bind_ip, bc->bind_port, g_p_net_io_server->get_last_errstr());
        tell_parent('\xFF');
        return -1;
    }

    print_prompt(true, "Listen on %s:%u", bc->bind_ip, bc->bind_port);

    int psnd_data_len = g_bench_conf.max_pkg_len + sizeof(shm_block_t);
    char *psnd_data = (char *)malloc(psnd_data_len);
    if (psnd_data == NULL) {
        print_prompt(false, "Malloc package send buffer fail, size: %d B", g_bench_conf.max_pkg_len);
        tell_parent('\xFF');
        return -1;
    }

    setproctitle("-%s:[NET]", g_bench_conf.prog_name);
    
    if (dll.handle_init && dll.handle_init(argc, argv, PROC_CONN) != 0) {
        print_prompt(false, "Conn handle init fail.");
        tell_parent('\xFF');
        return -1;
    }

    tell_parent('\x00');
    shm_block_t* mb = (shm_block_t*)psnd_data;

    while(!g_stop) {
        // 每次阻塞1毫秒, 因后面要发送数据
        // TODO:时间应该是可配的
        if (g_p_net_io_server->do_io(4, NET_IO_SERVER_CMD_ACCEPT | NET_IO_SERVER_CMD_READ | NET_IO_SERVER_CMD_WRITE) != 0) {
            TRACE_LOG("ERROR: do_io fail: %s", g_p_net_io_server->get_last_errstr());
        }

        // 发送环形队列中的数据包, 发干净为止
        while (!g_stop) {
            // 取出数据包
            int ret = g_psnd_rq->pop_data(psnd_data, psnd_data_len, 0);

            if (ret == 0) {
                break;
            } else if (ret < 0) {
                // 发生错误
                TRACE_LOG("ERROR: Pop data error, ret: %d, err: %s", ret, g_psnd_rq->get_last_errstr());
                break;
            } else {
                if (ret != mb->length || mb->length > psnd_data_len) {
                    // 出错, 打印日志以后退出发送
                    TRACE_LOG("ERROR: pop data len %d != pkg len %d or > psnd_data_len", mb->length, psnd_data_len);
                    break;
                }
            }

            TRACE_LOG("g_psnd_rq->pop_data: length: %d type: %d blk_id: %d", (int)mb->length, (int)mb->type, (int)mb->blk_id);
            
            if (mb->type == FIN_BLOCK) {
                g_p_net_io_server->close_connection(mb->blk_id, true);
                TRACE_LOG("close_connection: %d", mb->blk_id);
                continue;
            }

            // 发送数据
            int data_len = mb->length - sizeof(shm_block_t);

            if (data_len > 0) {
                if (mb->type == BROADCAST_BLOCK) {
                    ret = g_p_net_io_server->broadcast(mb->blk_id, (char*)mb->data, data_len, 1);
                    if (ret) {
                        // 广播报文为全部成功
                        TRACE_LOG("ERROR: broadcast not success at all, %d failed", ret);
                    }
                } else {
                    ret = g_p_net_io_server->send_data(mb->blk_id, (char*)mb->data, data_len);
                    if (ret != data_len) {
                        // 发送数据失败时丢弃这个包
                        TRACE_LOG("ERROR: send_data: blk_id: %d data_len: %d ret: %d", 
                                    mb->blk_id, data_len, ret);
                        g_p_net_io_server->close_connection(mb->blk_id, true);
                        TRACE_LOG("close_connection: %d", mb->blk_id);
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

    g_p_net_io_server->uninit();
    g_p_net_io_server->release();
    g_p_net_io_server = NULL;

    free(psnd_data);

    if (dll.handle_fini) {
        dll.handle_fini(PROC_CONN);
    }

    return 0;
}

pid_t spawn_net_proc(int argc, char **argv)
{
    pid_t pid = fork();
    if (pid < 0) {
        print_prompt(false, "Fork child processs failed: %s", strerror(errno));
        return -1;
    } else if (pid == 0) {                        // child
        net_proc(argc, argv);
        exit(0);
    }
  
    char status = 0;
    if (wait_child(&status) != 0) {
        return -1;
    }
    if (status != 0) {
        return -1;
    }

    return pid;
}
