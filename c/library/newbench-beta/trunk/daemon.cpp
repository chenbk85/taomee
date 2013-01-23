#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>

#include "newbench.h"
#include "util.h"
#include "daemon.h"

extern bench_config_t g_bench_conf;

volatile int g_stop = 0;
volatile int g_restart = 0;

static int g_backgd_mode = 0;

#define MAXFDS 100000 /**<TODO*/

void sigterm_handler(int /* signo */)
{
    g_stop = 1;
    g_restart = 0;
}

void sighup_handler(int /* signo */)
{
    g_restart = 1;
    g_stop = 1;
}

void dup_argv(int argc, char **argv, char **saved_argv)
{
    if (!saved_argv)
    {
        return;
    }
    saved_argv[argc] = NULL;
    while (--argc >= 0)
    {
        saved_argv[argc] = strdup(argv[argc]);
    }
}

void free_argv(char **saved_argv)
{
    if (saved_argv == NULL)
    {
        return;
    }

    for (char **argv = saved_argv; *argv; argv++)
    {
        free (*argv);
    }
    free (saved_argv);
    saved_argv = NULL;
}

void rlimit_reset()
{
    struct rlimit rlim;

    /* raise open files */
    rlim.rlim_cur = MAXFDS;
    rlim.rlim_max = MAXFDS;
    setrlimit(RLIMIT_NOFILE, &rlim);

    /* allow core dump */
    rlim.rlim_cur = 1<<29;
    rlim.rlim_max = 1<<29;
    setrlimit(RLIMIT_CORE, &rlim);
}

int daemon_start(int /* argc */, char** /* argv */)
{
    struct sigaction sa;
    sigset_t sset;

    rlimit_reset();

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = sighup_handler;
    sigaction(SIGHUP, &sa, NULL);

    signal(SIGPIPE,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);

    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);

    if (!strcasecmp ("background", g_bench_conf.run_mode))
    {
        daemon (1, 1);
        g_backgd_mode = 1;
        DEBUG_LOG("switch to daemon mode");
    }

    return 0;
}

void daemon_stop(void)
{
    if (!g_backgd_mode)
    {
        printf("Server stopping...\n");
    }

    if (g_restart && g_bench_conf.prog_name && g_bench_conf.saved_argv)
    {
        WARN_LOG("%s", "Server restarting...");
        chdir(g_bench_conf.current_dir);
        execv(g_bench_conf.prog_name, g_bench_conf.saved_argv);
        WARN_LOG("%s", "Restart Failed...");
    }

    if (g_bench_conf.prog_name != NULL)
    {
        free(g_bench_conf.prog_name);
        g_bench_conf.prog_name = NULL;
    }
}
