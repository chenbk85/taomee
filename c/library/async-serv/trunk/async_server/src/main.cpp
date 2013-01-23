#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <arpa/inet.h>

#include "log.h"
#include "shmq.h"
#include "config.h"
#include "global.h"
#include "plugin.h"
#include "mem_pool.h"

#define VERSION "1.0"

int fork_conn();
int fork_work(uint32_t channel);

static void sigterm_handler(int sig)
{
    g_stop = 1;
}

static inline int rlimit_reset()
{
    // 上调打开文件数的限制
    struct rlimit rl = {0};
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        printf("ERROR: getrlimit.\n");
        return -1;
    }
    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rl) != 0 )
    {
        printf("ERROR: setrlimit.\n");
        return -1;
    }

    // 允许产生CORE文件
    if (getrlimit(RLIMIT_CORE, &rl) != 0)
    {
        printf("ERROR: getrlimit.\n");
        return -1;
    }

    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_CORE, &rl) != 0) {
        printf("ERROR: setrlimit.\n");
        return -1;
    }

    return 0;
}

static void daemon_start()
{
    rlimit_reset();

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    signal(SIGPIPE,SIG_IGN);

    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGFPE);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);
    daemon(1, 1);
}

int check_single()
{
    int fd = -1;
    char buf[16] = {0};

    fd = open(config_get_strval("pid_file", "./pid"), O_RDWR|O_CREAT, 0644);
    if (fd < 0)
        BOOT_LOG(-1, "check single failed");

    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &fl) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            BOOT_LOG(-1, "service is running");
            return -2;
        }
        BOOT_LOG(-1, "service is running");
    }

    if (ftruncate(fd, 0) != 0)
        BOOT_LOG(-1, "check single failed");

    snprintf(buf, sizeof(buf), "%d", (int)getpid());
    if (write(fd, buf, strlen(buf)) == -1)
        BOOT_LOG(-1, "check single failed");

    return 0;
}

int main(int argc, char* argv[])
{
    arg_start = argv[0];
    arg_end = argv[argc-1] + strlen(argv[argc - 1]) + 1;
    env_start = environ[0];

    if (argc != 2)
        exit(-1);

    daemon_start();

    INFO_LOG ("async_server %s, report bugs to <cliu.hust@hotmail.com>", VERSION);

    load_config_file(argv[1]);

    check_single();

    if (config_get_strval("proc_name", NULL))
    {
        set_title("%s-MAIN", config_get_strval("proc_name", NULL));
    }
    else
    {
        set_title("%s-MAIN", arg_start);
    }

    load_work_file(config_get_strval("work_conf", ""));

    if (-1 == log_init(config_get_strval("log_dir", "./"), (log_lvl_t)config_get_intval("log_level", 8),
                       config_get_intval("log_size", 33554432),
                       config_get_intval("log_maxfiles", 100),
                       "main_")) {
        BOOT_LOG(-1, "log init");
    }

    init_warning_system();

    plugin_load(config_get_strval("plugin_file", ""));


    shmq_init(g_work_confs.size(), config_get_intval("shmq_size", 8388608));

    int max_connect = config_get_intval("max_connect", 10000);
    if (max_connect <= 4096)
        max_connect = 4096;

    g_max_connect = max_connect;

    int max_pkg_len = config_get_intval("max_pkg_len", 16384);
    if (max_pkg_len <= 4096)
        max_pkg_len = 4096;

    g_max_pkg_len = max_pkg_len;


    const char * bind_ip = config_get_strval("bind_ip", NULL);
    if (NULL == bind_ip)
    {
        BOOT_LOG(-1, "unspecified bind_ip");
        return -1;
    }

    if (0 != get_ip_by_name(bind_ip, g_bind_ip))
    {
        strncpy(g_bind_ip, bind_ip, 16);
    }


    if (g_plugin.plugin_init) 
    {
        if (g_plugin.plugin_init(PROC_MAIN) != 0)
            BOOT_LOG(-1, "plugin_init init failed PROC_MAIN");
    }


    for (uint32_t i = 0; i < g_work_confs.size(); ++i)
        g_work_confs[i].pid = fork_work(i);

    pid_t conn_pid = fork_conn();

    while (!g_stop) {
        int status = 0;
        pid_t p = waitpid(-1, &status, 0);
        if(-1 == p)
            continue;

        if (WEXITSTATUS(status) == 10) {
            if (p == conn_pid) {
                conn_pid = -1;
            } else {
                for (uint32_t i = 0; i < g_work_confs.size(); ++i) {
                    if (g_work_confs[i].pid == p) {
                        g_work_confs[i].pid = -1;
                        break;
                    }
                }
            }
        } else {
            if (p == conn_pid) {
                conn_pid = fork_conn();
                send_warning_msg("conn core", 0, 0, 0, config_get_strval("bind_ip", "0.0.0.0"));
                ERROR_LOG("conn core");
            } else {
                for (uint32_t i = 0; i < g_work_confs.size(); ++i) {
                    if (g_work_confs[i].pid == p) {
                        ERROR_LOG("work core, id: %u, pid: %d", g_work_confs[i].id, p);
                        g_work_confs[i].pid = fork_work(i);
                        send_warning_msg("work core", g_work_confs[i].id, 0, 0, config_get_strval("bind_ip", "0.0.0.0"));
                        break;
                    }
                }
            }
        }
    }

    for (uint32_t i = 0; i < g_work_confs.size(); ++i) {
        if (g_work_confs[i].pid != -1)
            kill(g_work_confs[i].pid, SIGTERM);
    }

    if (conn_pid != -1)
        kill(conn_pid, SIGTERM);

    while (true) {
        int ret = 0;
        for (uint32_t i = 0; i < g_work_confs.size(); ++i) {
            if (g_work_confs[i].pid != -1)
                ret = 1;
        }

        if (ret || conn_pid != -1) {
            int status = 0;
            pid_t p = waitpid(-1, &status, 0);
            if(-1 == p)
                continue;

            if (p == conn_pid) {
                conn_pid = -1;
            } else {
                for (uint32_t i = 0; i < g_work_confs.size(); ++i) {
                    if (g_work_confs[i].pid == p) {
                        g_work_confs[i].pid = -1;
                        break;
                    }
                }
            }
        } else {
            break;
        }
    }

    if (g_plugin.plugin_fini)
        g_plugin.plugin_fini(PROC_MAIN);
}
