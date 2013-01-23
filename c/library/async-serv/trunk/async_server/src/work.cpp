#include <time.h>
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
#include "shm_revent.h"

int fork_work(uint32_t channel)
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
        set_title("%s-WORK", config_get_strval("proc_name", NULL));
    }
    else
    {
        set_title("%s-WORK", arg_start);
    }

    g_link_flags = (uint32_t *)malloc(g_max_connect * sizeof(uint32_t));
    for (uint32_t i = 0; i < g_max_connect; ++i)
        g_link_flags[i] = 0;

    g_work_channel = channel;
    g_work_id = g_work_confs[channel].id;
    strcpy(g_work_name, g_work_confs[channel].name);

    log_fini();
    char *name = (char *)malloc(128);
    sprintf(name, "%d_%s_", g_work_id, g_work_name);
    if (-1 == log_init(config_get_strval("log_dir", "./"), (log_lvl_t)config_get_intval("log_level", 8),
                       config_get_intval("log_size", 33554432),
                       config_get_intval("log_maxfiles", 100),
                       name)) {
        BOOT_LOG(10, "log init");
    }

    free(name);

    if (g_reactor.init(g_max_connect) == -1)
        exit(10);

    if (g_plugin.plugin_init) {
        if (g_plugin.plugin_init(PROC_WORK) == -1)
            BOOT_LOG(10, "plugin_init init failed PROC_WORK");
    }

    g_shm_queue_mgr.recv_queue[channel].head->idle = 1;
    g_shm_queue_mgr.recv_queue[channel].head->inited = 1;
    g_shm_queue_mgr.send_queue[channel].head->inited = 1;

    for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i) {
        free(g_shm_queue_mgr.send_queue[i].pull_buf);
        if (i == channel)
            continue;

        free(g_shm_queue_mgr.recv_queue[i].pull_buf);
        close(g_shm_queue_mgr.recv_queue[i].pipe[0]);
        close(g_shm_queue_mgr.recv_queue[i].pipe[1]);
        close(g_shm_queue_mgr.send_queue[i].pipe[0]);
        close(g_shm_queue_mgr.send_queue[i].pipe[1]);
    }

    set_noblock(g_shm_queue_mgr.recv_queue[channel].pipe[0]);
    close(g_shm_queue_mgr.recv_queue[channel].pipe[1]);
    set_noblock(g_shm_queue_mgr.send_queue[channel].pipe[1]);
    close(g_shm_queue_mgr.send_queue[channel].pipe[0]);

    c_revent *revent = new c_revent();
    if (!revent->start(channel))
        exit(10);

    time_t old_time = time(NULL);
    while (!g_stop) {
        g_reactor.handle_event();
        pruge_send(channel);

        time_t cur_time = time(NULL);
        if (cur_time > old_time && g_plugin.time_event) {
            old_time = cur_time;
            g_plugin.time_event();
        }
    }

    while (true) {
        struct shm_block_t sb;
        uint8_t *buf;
        if (recv_pull(channel, &sb, &buf) != 0)
            break;

        if (sb.type == PROTO_BLOCK) {
            if (g_link_flags[sb.fd] != 0)
                g_plugin.proc_pkg_cli(sb.fd, (char *)buf, sb.len);
        } else if (sb.type == CLOSE_BLOCK) {
            if (g_link_flags[sb.fd] != 0) {
                if (g_plugin.link_down_cli)
                    g_plugin.link_down_cli(sb.fd);

                g_link_flags[sb.fd] = 0;
            }
        } else if (sb.type == LOGIN_BLOCK) {
            g_link_flags[sb.fd] = sb.id;
            if (g_plugin.link_up_cli)
                g_plugin.link_up_cli(sb.fd, *(uint32_t *)buf);
        }
    }

    if (g_plugin.plugin_fini)
        g_plugin.plugin_fini(PROC_WORK);

    g_reactor.fini();

    exit(10);
}
