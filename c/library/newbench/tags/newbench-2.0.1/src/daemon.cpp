/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/resource.h>

#include <iostream>
#include <iomanip>

#include "newbench_so.h"
#include "newbench.h"
#include "util.h"
#include "daemon.h"

using namespace std;

#define LOCKFILE "./daemon.pid"


volatile int g_stop = 0;
volatile int g_restart = 0;
static bool g_backgd_mode = false;

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

void dup_argv(int argc, const char * const argv[], char **saved_argv)
{
    if (!saved_argv) {
        return;
    }
    saved_argv[argc] = NULL;
    while (--argc >= 0) {
        saved_argv[argc] = strdup(argv[argc]);
    }
}

void free_argv(char **saved_argv)
{
    if (saved_argv == NULL) {
        return;
    }

    for (char **argv = saved_argv; *argv; argv++) {
        free (*argv);
    }
    free (saved_argv);
    saved_argv = NULL;
}

int rlimit_reset()
{
    // 上调打开文件数的限制
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
        cerr << "ERROR: getrlimit." << endl;
        return -1;
    }
    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rl) != 0 ) {
        cerr << "ERROR: setrlimit." << endl;
        return -1;
    }

    // 允许产生CORE文件
    if (getrlimit(RLIMIT_CORE, &rl) != 0) {
        cerr << "ERROR: getrlimit." << endl;
        return -1;
    }
    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_CORE, &rl) != 0) {
        cerr << "ERROR: setrlimit." << endl;
        return -1;
    }

    return 0;
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

    daemon (1, 1);
    g_backgd_mode = true;
    DEBUG_LOG("switch to daemon mode");
    
    if (already_running()) {
        print_prompt(false, "Already running");
        return -1;
    }

    return 0;
}

void daemon_stop(void)
{
    if (!g_backgd_mode) {
        printf("Server stopping...\n");
    }

    if (g_restart && g_bench_conf.prog_name && g_bench_conf.saved_argv) {
        WARN_LOG("%s", "Server restarting...");
        chdir(g_bench_conf.current_dir);
        execv(g_bench_conf.prog_name, g_bench_conf.saved_argv);
        WARN_LOG("%s", "Restart Failed...");
    }

    if (g_bench_conf.prog_name != NULL) {
        free(g_bench_conf.prog_name);
        g_bench_conf.prog_name = NULL;
    }
}

static int lockfile(int fd)
{
    struct flock fl;
    memset(&fl, 0, sizeof(fl));

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

int already_running()
{
    int     fd = -1;
    char    buf[16] = {0};

    fd = open(LOCKFILE, O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if (fd < 0) {
        return -1;
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        return -1;
    }
    if (ftruncate(fd, 0) != 0) {
        return -1;
    }
    sprintf(buf, "%d", (int)getpid());
    if (write(fd, buf, strlen(buf)+1) == -1) {
        return -1;
    }

    return 0;
}
