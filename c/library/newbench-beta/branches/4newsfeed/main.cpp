/**
 * =====================================================================================
 *       @file  main.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2010 02:47:31 PM
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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <string>
#include <map>
using namespace std;

#include "i_ini_file.h"
#include "i_net_io_server.h"
#include "i_ring_queue.h"
#include "i_barrier_ring_queue.h"
#include "afutex.h"

#include "setproctitle.h"
#include "connect.h"
#include "work.h"
#include "daemon.h"
#include "util.h"
#include "newbench.h"
#include "c_net_io_notifier.h"

extern dll_func_t dll;

i_barrier_ring_queue *g_prcv_brq = NULL;
i_ring_queue *g_prcv_rq = NULL;
i_ring_queue *g_psnd_rq = NULL;

bench_config_t g_bench_conf;

/// 对环形队列互斥的锁
static char* g_rq_lock_shm = NULL;
static int g_rq_lock_shm_len = 0;
int* g_snd_rq_lock = NULL;
int* g_rcv_rq_lock = NULL;
afutex_opbuf_t* g_rcv_wait_op = NULL;

static i_ini_file *g_pnb_ini = NULL;
static char g_conf_buf[MAX_CONF_STR_LEN] = {0};

/// 为了兼容老 serverbench 配置信息读取函数而设立的数据结构
static map<string, string> g_bench_conf_map;

i_net_io_notifier *g_p_net_io_notifier = NULL;

/**
 * @brief  与serverbench之前版本兼容
 * @param
 * @return
 */
char* config_get_strval(const char* key)
{
    if (key == NULL)
    {
        return NULL;
    }

    map<string, string>::iterator itor = g_bench_conf_map.find(key);
    if (itor != g_bench_conf_map.end())
    {
        return (char*)((*itor).second.c_str());
    }

    if(g_pnb_ini->read_string("Backward", key, g_conf_buf, MAX_CONF_STR_LEN, NULL) != 0)
    {
        ERROR_LOG("Get %s config value failed!", key);
        return NULL;
    }

    g_bench_conf_map[key] = g_conf_buf;
    return (char*)(g_bench_conf_map[key].c_str());
}

int config_get_intval(const char* key, int def)
{
    if (key == NULL)
    {
        return def;
    }

    return g_pnb_ini->read_int("Backward", key, def);
}


/**
 * @brief  参数解析
 * @param
 * @return
 */
int parse_args(int argc, char* argv[])
{
    g_bench_conf.prog_name = strdup(argv[0]);
    if (argc != 2)
    {
        NB_BOOT_LOG(-1, "Usage: %s conf_file", g_bench_conf.prog_name);
        return -1;
    }
    g_bench_conf.current_dir = strdup(get_current_dir_name());
    g_bench_conf.saved_argv = (char**)malloc(sizeof(char *) * (argc + 1));
    if (g_bench_conf.saved_argv == NULL)
    {
        NB_BOOT_LOG(-1, "Failed to malloc space to save argv.");
        return -1;
    }
    dup_argv(argc, argv, g_bench_conf.saved_argv);

    if (create_ini_file_instance(&g_pnb_ini) != 0)
    {
        NB_BOOT_LOG(-1, "Create i_ini_file failed.");
        return -1;
    }

    if (g_pnb_ini->init(argv[1]) != 0)
    {
        NB_BOOT_LOG(-1, "Init ini file[%s] failed!", argv[1]);
        return -1;
    }

    NB_BOOT_LOG(0, "Load config file: %s", argv[1]);
    return 0;
}

int config_init()
{
    int ret_code;
    ///初始化log日志
    ret_code = g_pnb_ini->read_string("LogInfo", "log_dir", g_bench_conf.log_dir, MAX_CONF_STR_LEN, NULL);
    if (ret_code != 0)
    {
        NB_BOOT_LOG(-1, "Get log_dir config failed(%d).", ret_code);
        return -1;
    }
    g_bench_conf.log_level = g_pnb_ini->read_int("LogInfo", "log_level", log_lvl_trace);
    g_bench_conf.log_size = g_pnb_ini->read_int("LogInfo", "log_size", 1 << 30);
    g_bench_conf.log_maxfiles = g_pnb_ini->read_int("LogInfo", "log_maxfiles", 0);
    ret_code = g_pnb_ini->read_string("LogInfo", "log_prefix", g_bench_conf.log_prefix, MAX_CONF_STR_LEN, "");
    if (ret_code != 0)
    {
        NB_BOOT_LOG(-1, "Get log_prefix config failed(%d).", ret_code);
        return -1;
    }

    ///初始化ConnInfo
    ret_code = g_pnb_ini->read_string("ConnInfo", "bind_file", g_bench_conf.bind_file, MAX_CONF_STR_LEN, NULL);
    if (ret_code != 0)
    {
        NB_BOOT_LOG(-1, "Get bind_file config failed(%d)", ret_code);
        return -1;
    }
    ret_code = load_bind_file(g_bench_conf.bind_file, &g_bench_conf.bind_list);
    if (ret_code != 0)
    {
        NB_BOOT_LOG(-1, "Load bind file: %s", g_bench_conf.bind_file);
        return -1;
    }
    NB_BOOT_LOG(0, "Load bind file: %s", g_bench_conf.bind_file);
    g_bench_conf.use_barrier = g_pnb_ini->read_int("RunInfo", "use_barrier", 0) == 0 ? false : true;

    ///初始化WorkInfo
    ret_code = g_pnb_ini->read_string("WorkInfo", "dll", g_bench_conf.so_file, MAX_CONF_STR_LEN, NULL);
    if (ret_code != 0)
    {
        NB_BOOT_LOG(-1, "Get dll config failed(%d)!", ret_code);
        return -1;
    }
    g_bench_conf.shmq_length = g_pnb_ini->read_int("RunInfo", "shmq_length", 1 << 26);
    NB_CHECK_INT("shmq_length", g_bench_conf.shmq_length);

    g_bench_conf.pkg_timeout = g_pnb_ini->read_int("RunInfo", "pkg_timeout", 10);
    NB_CHECK_INT("pkg_timeout", g_bench_conf.pkg_timeout);//待定

    g_bench_conf.worker_num = g_pnb_ini->read_int("WorkInfo", "worker_num", 1);
    NB_CHECK_INT("worker_num", g_bench_conf.worker_num);

    g_bench_conf.max_pkg_len = g_pnb_ini->read_int("RunInfo", "max_pkg_len", MAX_PACKAGE_LEN);
    NB_CHECK_INT("max_pkg_len", g_bench_conf.max_pkg_len);

    g_bench_conf.avg_pkg_len = g_pnb_ini->read_int("RunInfo", "avg_pkg_len", 32);
    NB_CHECK_INT("avg_pkg_len", g_bench_conf.avg_pkg_len);

    NB_BOOT_LOG(0, "Max package length: %u B", g_bench_conf.max_pkg_len);

    ///初始化RunInfo
    ret_code = g_pnb_ini->read_string("RunInfo", "run_mode", g_bench_conf.run_mode, MAX_CONF_STR_LEN, "");
    if (ret_code != 0)
    {
        NB_BOOT_LOG(-1, "Get run_mode config failed(%d)!", ret_code);
        return -1;
    }

    return 0;
}

void config_dump()
{
    char buf[MAX_CONF_STR_LEN * 2 * 20] = {0};
    sprintf(buf, "Dump config info: \n");
    sprintf(buf + strlen(buf), "  [RunInfo]\n");
    sprintf(buf + strlen(buf), "    run_mode = %s\n", g_bench_conf.run_mode);
    sprintf(buf + strlen(buf), "    use_barrier = %u\n", g_bench_conf.use_barrier);
    sprintf(buf + strlen(buf), "    max_pkg_len = %u\n", g_bench_conf.max_pkg_len);
    sprintf(buf + strlen(buf), "    avg_pkg_len = %u\n", g_bench_conf.avg_pkg_len);
    sprintf(buf + strlen(buf), "    shmq_length = %u\n", g_bench_conf.shmq_length);
    sprintf(buf + strlen(buf), "    pkg_timeout = %u\n", g_bench_conf.pkg_timeout);

    sprintf(buf + strlen(buf), "  [LogInfo]\n");
    sprintf(buf + strlen(buf), "    log_dir = %s\n", g_bench_conf.log_dir);
    sprintf(buf + strlen(buf), "    log_level = %u\n", g_bench_conf.log_level);
    sprintf(buf + strlen(buf), "    log_size = %u\n", g_bench_conf.log_size);
    sprintf(buf + strlen(buf), "    log_maxfiles = %u\n", g_bench_conf.log_maxfiles);
    sprintf(buf + strlen(buf), "    log_prefix = %s\n", g_bench_conf.log_prefix);

    sprintf(buf + strlen(buf), "  [ConnInfo]\n");
    sprintf(buf + strlen(buf), "    bind_file = %s\n", g_bench_conf.bind_file);

    sprintf(buf + strlen(buf), "  [WorkInfo]\n");
    sprintf(buf + strlen(buf), "    dll = %s\n", g_bench_conf.so_file);
    sprintf(buf + strlen(buf), "    worker_num = %u", g_bench_conf.worker_num);

    DEBUG_LOG("%s", buf);

    DEBUG_LOG("[Bind file information]");
    bind_dump(g_bench_conf.bind_list);
}

int nb_init()
{
    if (config_init() != 0)
    {
        return -1;
    }

    log_init(g_bench_conf.log_dir,
             (log_lvl_t)g_bench_conf.log_level,
             g_bench_conf.log_size,
             0,
             g_bench_conf.log_prefix);

    /// 加载SO文件
    if (register_plugin(g_bench_conf.so_file) != 0)
    {
        NB_BOOT_LOG(-1, "Load dll file: %s ", g_bench_conf.so_file);
        return -1;
    }
    NB_BOOT_LOG(0, "Load dll file: %s", g_bench_conf.so_file);
    if (g_bench_conf.use_barrier && dll.handle_dispatch == NULL)
    {
        NB_BOOT_LOG(0, "Warnning: handle_dispatch not found, pkg filter disabled.");
    }

    /// 检查SO中必须实现的接口是否已经实现
    if (dll.handle_input == NULL)
    {
        NB_BOOT_LOG(-1, "dll handle_input is null!");
        return -1;
    }

    if (dll.handle_process == NULL)
    {
        NB_BOOT_LOG(-1, "dll handle_process is null!");
        return -1;
    }

    ///初始化数据包接受环形队列
    if (g_bench_conf.use_barrier)
    {///barrier ring queue
        if (create_barrier_ring_queue_instance(&g_prcv_brq) != 0)
        {
            NB_BOOT_LOG(-1, "Create i_barrier_ring_queue failed.");
            return -1;
        }

        /// 注：平均包长中要包含shm_block_t的长度，因为每个数据包都会加上这个头以后
        ///     再压入过滤器！
        if (g_prcv_brq->init(g_bench_conf.shmq_length, g_bench_conf.avg_pkg_len + sizeof(shm_block_t)) != 0)
        {
            NB_BOOT_LOG(-1, "Init barrier failed: %s", g_prcv_brq->get_last_errstr());
            return -1;
        }
        NB_BOOT_LOG(0, "Init barrier ring queue, size %u KB, avg_pkg_len %u B + %zd B",
            g_bench_conf.shmq_length/1024, g_bench_conf.avg_pkg_len, sizeof(shm_block_t));
    }
    else
    { ///common ring queue
        if (create_variable_queue_instance(&g_prcv_rq, sizeof(int32_t)) != 0)
        {
            NB_BOOT_LOG(-1, "Create receive i_ring_queue failed.");
            return -1;
        }

        if (g_prcv_rq->init(g_bench_conf.shmq_length) != 0)
        {
            NB_BOOT_LOG(-1, "Init receive i_ring_queue failed: %d", g_prcv_rq->get_last_errno());
            return -1;
        }
        NB_BOOT_LOG(0, "Init receive ring queue, size %u KB",
            g_bench_conf.shmq_length/1024);
    }

    ///初始化数据包发送环形队列
    if (create_variable_queue_instance(&g_psnd_rq, sizeof(int32_t)) != 0)
    {
        NB_BOOT_LOG(-1, "Create send i_ring_queue failed.");
        return -1;
    }
    if (g_psnd_rq->init(g_bench_conf.shmq_length) != 0)
    {
        NB_BOOT_LOG(-1, "Init send i_ring_queue failed: %d", g_psnd_rq->get_last_errno());
        return -1;
    }
    NB_BOOT_LOG(0, "Init send ring queue: %d KB", g_bench_conf.shmq_length/1024);

    ///初始化环形队列互斥的锁
    g_rq_lock_shm_len = getpagesize();
    if ((u_int)g_rq_lock_shm_len < sizeof(int) * 2 + sizeof(afutex_opbuf_t))
    {
        g_rq_lock_shm_len = sizeof(int) * 2 + sizeof(afutex_opbuf_t);
    }
    g_rq_lock_shm = (char *)mmap(NULL, g_rq_lock_shm_len, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (g_rq_lock_shm == MAP_FAILED)
    {
        NB_BOOT_LOG(-1, "Mmap ring queue lock shm fail.");
        return -1;
    }
    g_rcv_rq_lock = (int*)g_rq_lock_shm;
    g_snd_rq_lock = (int*)(g_rq_lock_shm + sizeof(int));
    g_rcv_wait_op = (afutex_opbuf_t*)(g_rq_lock_shm + sizeof(int) * 2);

    afutex_mutex_init(g_rcv_rq_lock);
    afutex_mutex_init(g_snd_rq_lock);
    afutex_opbuf_init(g_rcv_wait_op);

    extern i_net_io_server *pnb_io_server;
    if (net_io_server_create(&pnb_io_server) != 0 || (pnb_io_server == NULL))
    {
        NB_BOOT_LOG(-1, "net_io_server_create failed.");
        return -1;
    }

    if (net_io_notifier_create(&g_p_net_io_notifier))
    {
        NB_BOOT_LOG(-1, "fail to create net_io_notifier!");
        return -1;
    }

    if (g_p_net_io_notifier->init())
    {
        NB_BOOT_LOG(-1, "fail to init net_io_notifier!");
        return -1;
    }

    config_dump();
    return 0;
}

int nb_uninit()
{
    /// 收回共享内存
    if (g_rq_lock_shm != NULL && g_rq_lock_shm_len > 0)
    {
        munmap(g_rq_lock_shm, g_rq_lock_shm_len);
    }

    NB_CLASS_FREE(g_prcv_brq);
    NB_CLASS_FREE(g_prcv_rq);
    NB_CLASS_FREE(g_psnd_rq);
    free_bind_file(g_bench_conf.bind_list);
    NB_CLASS_FREE(g_pnb_ini);
    NB_COMMON_FREE(g_bench_conf.prog_name);
    NB_COMMON_FREE(g_bench_conf.current_dir);
    free_argv(g_bench_conf.saved_argv);

    return 0;
}

int main(int argc, char* argv[])
{
    memset(&g_bench_conf, 0, sizeof(bench_config_t));
    if (parse_args(argc, argv) != 0)
    {
       return -1;
    }

    if (nb_init() != 0)
    {
        return -1;
    }

    if (dll.handle_init)
    {
        NB_BOOT_LOG(dll.handle_init(argc, argv, PROC_MAIN), "Call main process handle_init");
    }

    daemon_start(argc, argv);
    initproctitle(argc, argv);
    setproctitle("%s:[MAIN]", g_bench_conf.prog_name);

    if (g_bench_conf.use_barrier && dll.handle_dispatch == NULL)
    {
        ERROR_LOG("Warning: handle_dispatch not found, pkg filter disabled.");
    }

    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        NB_BOOT_LOG(-1, "Fork child process failed: %s", strerror(errno));
        return -1;
    }
    else if(pid > 0)
    {///父进程
        ///查看CONN进程是否已启动，等待 1 秒
        usleep (100000);
        if (kill(pid, 0) == 0 || errno != ESRCH)
        {
            pid_t *wpids = (pid_t*) calloc(g_bench_conf.worker_num, sizeof(pid_t));
            if (spawn_work_proc(argc, g_bench_conf.saved_argv, wpids,
                    g_bench_conf.worker_num, g_bench_conf.use_barrier) != 1)
            {
                ///父进程返回
                monitor_work_proc(argc, g_bench_conf.saved_argv, wpids,
                                g_bench_conf.worker_num, g_bench_conf.use_barrier);
                daemon_stop();
            }
            else
            {
                ///子进程返回
            }
            free(wpids);
        }
    }
    else
    {///子进程
        conn_proc(argc, g_bench_conf.saved_argv);
    }

    nb_uninit();
    ///只能增加资源释放相关代码
    return 0;
}
