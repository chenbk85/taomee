#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "log.h"
#include "shmq.h"
#include "config.h"
#include "plugin.h"
#include "global.h"
#include "reactor.h"
#include "tcp_accept.h"
#include "shm_sevent.h"
#include "reactor.h"

int fork_conn()
{
    if (g_stop)
        return -1;

    pid_t pid = fork();
    if (pid < 0)
    {
        BOOT_LOG(-1, "conn fork failed(%s).", strerror(errno));
        return pid;
    }
    if (pid != 0)
        return pid;

    if (config_get_strval("proc_name", NULL))
    {
        set_title("%s-CONN", config_get_strval("proc_name", NULL));
    }
    else
    {
        set_title("%s-CONN", arg_start);
    }

    log_fini();
    if (-1 == log_init(config_get_strval("log_dir", "./"), (log_lvl_t)config_get_intval("log_level", 8),
                       config_get_intval("log_size", 33554432),
                       config_get_intval("log_maxfiles", 100),
                       "conn_")) {
        BOOT_LOG(-1, "log init");
    }

    if (g_reactor.init(g_max_connect) == -1)
        exit(10);

    for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i) {
        set_noblock(g_shm_queue_mgr.recv_queue[i].pipe[1]);
        close(g_shm_queue_mgr.recv_queue[i].pipe[0]);
        set_noblock(g_shm_queue_mgr.send_queue[i].pipe[0]);
        close(g_shm_queue_mgr.send_queue[i].pipe[1]);
        free(g_shm_queue_mgr.recv_queue[i].pull_buf);
        c_sevent *sevent = new c_sevent();
        if (!sevent->start(i))
            BOOT_LOG(10, "event start PROC_CONN");
    }

    for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ) {
        if (g_shm_queue_mgr.recv_queue[i].head->inited == 1
            && g_shm_queue_mgr.send_queue[i].head->inited == 1) {
            i++;
        } else {
            usleep(100000);
            if (g_stop)
                exit(10);
        }
    }

    if (g_plugin.plugin_init) {
        if (g_plugin.plugin_init(PROC_CONN) == -1)
            BOOT_LOG(10, "plugin_init init failed PROC_CONN");
    }

    c_tcp_accept tcp_accept;
    tcp_accept.start(g_bind_ip, config_get_intval("bind_port", 0));

    while (!g_stop) {
        g_reactor.handle_event();
        for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i)
            pruge_recv(i);
    }

    if (g_plugin.plugin_fini)
        g_plugin.plugin_fini(PROC_CONN);

    g_reactor.fini();

    exit(10);
}
