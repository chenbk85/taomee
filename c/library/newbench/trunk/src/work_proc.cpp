/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file work_proc.cpp
 * @author tonyliu <tonyliu@taomee.com>
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#include "i_ring_queue.h"
#include "i_barrier_ring_queue.h"
#include "newbench_io_event.h"
#include "newbench_util.h"
#include "afutex.h"

#include "dll.h"
#include "setproctitle.h"
#include "daemon.h"
#include "util.h"
#include "work_proc.h"
#include "newbench.h"

extern volatile int g_stop;
extern dll_func_t dll;
extern i_barrier_ring_queue *g_prcv_brq;
extern i_ring_queue *g_prcv_rq;
extern i_ring_queue *g_psnd_rq;

extern int* g_rcv_rq_lock;
extern int* g_snd_rq_lock;
extern afutex_opbuf_t* g_rcv_wait_op;

static char *g_p_send_buff = NULL;
static int g_send_buff_len = 0;

int work_proc_close_conn(shm_block_t* mb)
{
    mb->length = sizeof(shm_block_t);
    mb->type = FIN_BLOCK;

    AFUTEX_LOCK_ERR_CHK(g_snd_rq_lock);
    int ret = g_psnd_rq->push_data((char*)mb, sizeof(shm_block_t), true);
    AFUTEX_UNLOCK_ERR_CHK(g_snd_rq_lock);

    if (ret != sizeof(shm_block_t)) {
        TRACE_LOG("ERROR: Push data(len: %zd) error (ret: %d, last err: %s, data: %d, empty: %d)", sizeof(shm_block_t), ret,
                g_psnd_rq->get_last_errstr(), g_psnd_rq->get_data_len(), g_psnd_rq->get_empty_buffer_len());
    } else {
        g_p_net_io_notifier->popup();
    }

    return 0;
}

int send_data(int connection_id, const char *p_data, int data_len, int /* is_atomic */, int is_broadcast)
{
    TRACE_LOG("connection_id: %d data_len: %d, is_broadcast: %d", connection_id, data_len, is_broadcast);

    if (g_send_buff_len < (int)sizeof(shm_block_t) + data_len) {
        g_p_send_buff = (char *)realloc(g_p_send_buff, sizeof(shm_block_t) + data_len);
        TRACE_LOG("work_proc: realloc: %p %d", g_p_send_buff, (int)sizeof(shm_block_t) + data_len);
        if (g_p_send_buff == NULL) {
            TRACE_LOG("ERROR: g_p_send_buff == NULL");
            return -1;
        }
        g_send_buff_len = sizeof(shm_block_t) + data_len;
    }

    shm_block_t *mb = (shm_block_t *)g_p_send_buff;
    mb->blk_id = connection_id;
    mb->type = is_broadcast ? BROADCAST_BLOCK : DAT_BLOCK;
    mb->length = data_len + sizeof(shm_block_t);
    memcpy(mb->data, p_data, data_len);
    mb->skinfo.ptr_lookout = NULL;

    AFUTEX_LOCK_ERR_CHK(g_snd_rq_lock);
    int ret = g_psnd_rq->push_data((char *)mb, mb->length, true);
    AFUTEX_UNLOCK_ERR_CHK(g_snd_rq_lock);

    if (ret != mb->length) {
        TRACE_LOG("ERROR: Push data(len: %d) error (ret: %d, data: %d, empty: %d)",
                  mb->length,
                  g_psnd_rq->get_last_errno(),
                  g_psnd_rq->get_data_len(),
                  g_psnd_rq->get_empty_buffer_len());

        return -1;
    }

    g_p_net_io_notifier->popup();
    return 0;
}

int close_connection(int connection_id)
{
    TRACE_LOG("close_connection: connection_id: %d", connection_id);
    if (g_send_buff_len < (int)sizeof(shm_block_t)) {
        g_p_send_buff = (char *)realloc(g_p_send_buff, sizeof(shm_block_t));
        TRACE_LOG("work_proc: realloc: %p %d", g_p_send_buff, (int)sizeof(shm_block_t));
        if (g_p_send_buff == NULL) {
            TRACE_LOG("ERROR: g_p_send_buff == NULL");
            return -1;
        }
        g_send_buff_len = sizeof(shm_block_t);
    }
    
    shm_block_t *mb = (shm_block_t *)g_p_send_buff;
    mb->blk_id = connection_id;
    mb->type = FIN_BLOCK;
    mb->length = sizeof(shm_block_t);

    AFUTEX_LOCK_ERR_CHK(g_snd_rq_lock);
    int ret = g_psnd_rq->push_data((char *)mb, mb->length, true);
    AFUTEX_UNLOCK_ERR_CHK(g_snd_rq_lock);

    if (ret != mb->length) {
        TRACE_LOG("ERROR: Push data(len: %d) error (ret: %d, data: %d, empty: %d)",
                  mb->length, g_psnd_rq->get_last_errno(), g_psnd_rq->get_data_len(),
                  g_psnd_rq->get_empty_buffer_len());

        return -1;
    }

    g_p_net_io_notifier->popup();
    return 0;
}

/**
 * @brief  启动work进程
 * @param
 * @return
 */
int work_proc(int argc, char **argv)
{
    setproctitle("-%s:[WORK]", g_bench_conf.prog_name);
    
    if (dll.handle_init != NULL && dll.handle_init(argc, argv, PROC_WORK) != 0) {
        print_prompt(false, "Worker process handle_init failed.");
        tell_parent('\xFF');
        return -1;
    }

    /// 由于环形队列中有头 shm_block_t，所以需要给头预留空间
    int prcv_data_len = g_bench_conf.max_pkg_len + sizeof(shm_block_t);
    char *prcv_data = (char *)malloc(prcv_data_len);

    if (prcv_data == NULL) {
        print_prompt(false, "Malloc work proc pop buffer fail, size: %d B", prcv_data_len);
        tell_parent('\xFF');
        return -1;
    }
    
    tell_parent('\x00');
    shm_block_t *mb = (shm_block_t *)prcv_data;
    int sndlen = 0;
    int ret = 0;
    char *puser_data = NULL;

    if (g_bench_conf.use_barrier == true) {
        if (g_prcv_brq == NULL) {
            print_prompt(false, "Use barrier mode, but i_barrier_ring_queue is null.");
            goto work_proc_end;
        }

        while (! g_stop) {
            if (dll.handle_schedule) {
                dll.handle_schedule();
            }

            if (!g_prcv_brq->is_able_pop()) {
                /// 等待数据
                struct timespec tv = {0, 200000};
                nanosleep(&tv, NULL);
                continue;
            }

            AFUTEX_LOCK_ERR_CHK(g_rcv_rq_lock);
            ret = g_prcv_brq->pop(prcv_data, prcv_data_len);
            AFUTEX_UNLOCK_ERR_CHK(g_rcv_rq_lock);
            if (ret < 0) {
                TRACE_LOG("ERROR: Barrier pop error: %d", g_prcv_brq->get_last_errno());
                sleep(1);  /// 防止出错以后打印日志速度太快
                continue;
            } else if (ret == 0) {
                /// 没有取出任何数据
                continue;
            } else {
                if(ret != mb->length || (u_int)mb->length <= sizeof(shm_block_t) || 
                   mb->length > prcv_data_len) {
                    /// 数据包包长有误，关闭这个连接
                    TRACE_LOG("ERROR: Pop data len(%d) err, pkg len(%d), prcv_data_len(%d), shm_block_t(%zd), close conn: %d.",
                            ret, mb->length, prcv_data_len, sizeof(shm_block_t), mb->blk_id);
                    work_proc_close_conn(mb);
                    continue;
                }
            }

            TRACE_LOG("Pop barrier ring queue len: %d", mb->length);

            sndlen = 0;
            ret = dll.handle_process(mb->data, mb->length - sizeof(shm_block_t), &puser_data, &sndlen, &mb->skinfo);

            if (ret != 0) {
                work_proc_close_conn(mb);
                TRACE_LOG("Handle_process fail, close conn: %d", mb->blk_id);
            } else if(sndlen + sizeof(shm_block_t) > (u_int)prcv_data_len) {
                work_proc_close_conn(mb);
                TRACE_LOG("ERROR: Handle_process sndlen(%d) > buf len, close conn: %d", sndlen, mb->blk_id);
            } else {
                if (sndlen > 0) {
                    mb->length = sndlen + sizeof(shm_block_t);
                    mb->type = DAT_BLOCK;
                    memcpy(mb->data, puser_data, sndlen);

                    AFUTEX_LOCK_ERR_CHK(g_snd_rq_lock);
                    ret = g_psnd_rq->push_data(prcv_data, mb->length, true);
                    AFUTEX_UNLOCK_ERR_CHK(g_snd_rq_lock);

                    if (ret != mb->length) {
                        TRACE_LOG("ERROR: Push data(len: %d) error (ret: %d, data: %d, empty: %d)", mb->length,
                                  g_psnd_rq->get_last_errno(), g_psnd_rq->get_data_len(), g_psnd_rq->get_empty_buffer_len());
                    } else {
                        TRACE_LOG("ring queue push data len: %d", mb->length);
                        g_p_net_io_notifier->popup();
                    }
                }
            }
        }
    } else {
        if (g_prcv_rq == NULL) {
            print_prompt(false, "Use nonbarrier mode, but i_ring_queue object is null.");
            goto work_proc_end;
        }

        while (!g_stop) {
            if (dll.handle_schedule != NULL) {
                dll.handle_schedule();
            }

            if (g_prcv_rq->get_data_len() <= 0) {
                struct timespec tv = {0, 200000};
                nanosleep (&tv, NULL);
                continue;
            }

            AFUTEX_LOCK_ERR_CHK(g_rcv_rq_lock);
            ret = g_prcv_rq->pop_data(prcv_data, prcv_data_len, 0);
            AFUTEX_UNLOCK_ERR_CHK(g_rcv_rq_lock);
            if (ret < 0) {
                TRACE_LOG("ERROR: Common ring queue pop fail(ret: %d): %s", ret, g_prcv_rq->get_last_errstr());
                sleep(1); /// 防止日志打印过快
                continue;
            } else if (ret == 0) {
                /// 数据还没准备好
                continue;
            } else {
                if(ret != mb->length || (u_int)mb->length <= sizeof(shm_block_t) || mb->length > prcv_data_len) {
                    /// 数据包包长有误，关闭这个连接
                    TRACE_LOG("ERROR: Pop data len(%d) err, pkg len(%d), prcv_data_len(%d), shm_block_t(%zd), close conn: %d.",
                            ret, mb->length, prcv_data_len, sizeof(shm_block_t), mb->blk_id);
                    work_proc_close_conn(mb);
                    continue;
                }
            }
            TRACE_LOG("Common ring queue pop len: %d", mb->length);

            sndlen = 0;
            ret = dll.handle_process(mb->data, mb->length - sizeof(shm_block_t), &puser_data, &sndlen, &mb->skinfo);
            if (ret != 0) {
                work_proc_close_conn(mb);
                TRACE_LOG("Handle process fail, close conn: %d", mb->blk_id);
            } else if(sndlen + sizeof(shm_block_t) > (u_int)prcv_data_len) {
                work_proc_close_conn(mb);
                TRACE_LOG("ERROR: Handle_process sndlen(%zd) > buf len %d, close conn: %d",
                        sndlen + sizeof(shm_block_t), prcv_data_len, mb->blk_id);
            } else {
                if (sndlen > 0) {
                    mb->length = sndlen + sizeof(shm_block_t);
                    mb->type = DAT_BLOCK;
                    memcpy(mb->data, puser_data, sndlen);

                    AFUTEX_LOCK_ERR_CHK(g_snd_rq_lock);
                    ret = g_psnd_rq->push_data(prcv_data, mb->length, true);
                    AFUTEX_UNLOCK_ERR_CHK(g_snd_rq_lock);

                    if (ret != mb->length) {
                        TRACE_LOG("ERROR: Push data(len: %d) error (ret: %d, data: %d, empty: %d)", mb->length,
                                g_psnd_rq->get_last_errno(), g_psnd_rq->get_data_len(), g_psnd_rq->get_empty_buffer_len());
                    } else {
                        TRACE_LOG("Common ring queue push len: %d", mb->length);
                        g_p_net_io_notifier->popup();
                    }
                }
            }
        }
    }

work_proc_end:
    free(prcv_data);
    if (g_p_send_buff != NULL) {
        free(g_p_send_buff);
        g_p_send_buff = NULL;
        g_send_buff_len = 0;
    }
    if (dll.handle_fini != NULL) {
        dll.handle_fini(PROC_WORK);
    }
    return 0;
}

pid_t spawn_work_proc(int argc, char **argv)
{
    char status = 0;
    pid_t pid = fork();
    if (pid < 0) {
        print_prompt(false, "Fork child processs failed: %s", strerror(errno));
        return -1;
    } else if (pid == 0) {                   // child
        work_proc(argc, argv);
        exit(0);
    }

    if (wait_child(&status) != 0) {
        return -1;
    }
    if (status != 0) {
        return -1;
    }

    return pid;
}
