/**
 * @file daemon.cpp
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-01
 */
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sched.h>
#include <sys/stat.h>

#include <iostream>

#include "daemon.h"
#include "log.h"

extern char** g_saved_argv;
extern int g_saved_argc;

extern char* g_prog_name;

extern volatile sig_atomic_t g_stop;
extern volatile sig_atomic_t g_restart;
extern char* g_p_current_dir;
extern int g_work_num;

static char* g_p_arg_start = NULL;
static char* g_p_arg_end = NULL;
static char* g_p_env_start = NULL;

static void sigterm_handler(int signo)
{
    g_stop = 1;
    g_restart = 0;
}

static void sighup_handler(int signo)
{
    g_stop = 1;
    g_restart = 1;
}

static void sigchld_handler(int signo, siginfo_t *si, void * p)
{
    int status;
    char *corename;

    pid_t child_pid = 0;

    while ((child_pid = waitpid(-1, &status, WNOHANG | __WALL)) > 0) {
        switch (si->si_code) {
            case SI_USER:
            case SI_TKILL:
                DEBUG_LOG("SIGCHLD from pid=%d uid=%d, IGNORED",
                        child_pid, si->si_uid);
                return; /* someone send use fake SIGCHLD */
            case CLD_EXITED:
                DEBUG_LOG("child %d exited", child_pid);
                g_work_num--;
                break;
            case CLD_KILLED:
                DEBUG_LOG("child %d killed by signal %d",
                        child_pid, WTERMSIG(si->si_status));
                g_work_num--;
                break;
            case CLD_TRAPPED:
                DEBUG_LOG("child %d trapped", child_pid);
                return;
            case CLD_STOPPED:
                DEBUG_LOG("child %d stopped", child_pid);
                if(child_pid > 1) kill(child_pid, SIGCONT);
                return;
            case CLD_CONTINUED:
                DEBUG_LOG("child %d continued", child_pid);
                return;
            case CLD_DUMPED:
                DEBUG_LOG("child %d coredumped by signal %d",
                        child_pid, WTERMSIG(si->si_status));
                chmod("core", 777);
                //chown("core", 0, 0);
                corename = (char*) alloca(40);
                sprintf(corename, "core.%d", child_pid);
                rename("core", corename);
                restart_worker_proc(child_pid);
                break;
            default:
                break;
        }
    }
}

static int rlimit_set(i_ini_file* p_ini_file)
{
    struct rlimit rlim;
    struct rlimit rlim_new;

    if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
        rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
        if (setrlimit(RLIMIT_CORE, &rlim_new)!= 0) {
            rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
            setrlimit(RLIMIT_CORE, &rlim_new);
        }
    }

    if ((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_cur == 0) {
        std::cerr << "failed to ensure corefile creation" << std::endl;
        return -1;
    }

    int maxconns = p_ini_file->read_int("WorkInfo", "max_conns", 10000);

    if (getrlimit(RLIMIT_NOFILE, &rlim) != 0) {
        std::cerr << "failed to getrlimit number of files" << std::endl;
        return -1;
    } else {
        if ((int)rlim.rlim_cur < maxconns)
            rlim.rlim_cur = maxconns + 3;
        if (rlim.rlim_max < rlim.rlim_cur)
            rlim.rlim_max = rlim.rlim_cur;
        if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) {
            std::cerr << "failed to set rlimit for open files. "
                "Try running as root or requesting smaller maxconns value" << std::endl;
            return -1;
        }
    }

    return 0;
}

static int write_lock(int fd)
{
    struct flock file_lock;
    file_lock.l_type = F_WRLCK;
    file_lock.l_whence = SEEK_SET;
    file_lock.l_start = 0;
    file_lock.l_len = 0;

    return fcntl(fd, F_SETLK, &file_lock);
}

static int check_single_on(const char * process_name)
{
    int fd = 0;
    char buf[10];
    fd = open("./daemon.pid", O_WRONLY | O_CREAT, 0644);
    if(fd < 0) {
        return -1;
    }

    if(write_lock(fd) < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            std::cerr << process_name << "is alreadly running" << std::endl;
            close(fd);
            return 1;
        } else {
            std::cerr << "write_lock() error: " << strerror(errno) << std::endl;
            close(fd);
            return -1;
        }
    }

    if (ftruncate(fd, 0)) {
        close(fd);
        return -1;
    }

    int len = sprintf(buf, "%d", (int)getpid());
    if (write(fd, buf, len) != len) {
        close(fd);
        return -1;
    }
    return 0;
}


int daemon_start(int argc, char** argv, i_ini_file* p_ini_file)
{
    g_saved_argc = argc;

    g_saved_argv = (char**) calloc(argc + 1, sizeof(char*));
    if (!g_saved_argv) {
        return -1;
    }

    while (--argc >= 0) {
        g_saved_argv[argc] = strdup(argv[argc]);
    }

    g_prog_name = strdup(argv[0]);

    if (!(g_p_current_dir = get_current_dir_name())) {
        std::cerr << "get_current_dir_name failed" << std::endl;
        return -1;
    }

    if (rlimit_set(p_ini_file) < 0) {
        return -1;
    }

    struct sigaction sa;
    sigset_t sset;

    memset(&sa, 0, sizeof(sa));
    signal(SIGPIPE,SIG_IGN);

    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = sighup_handler;
    sigaction(SIGHUP, &sa, NULL);

    sa.sa_flags = SA_RESTART|SA_SIGINFO;
    sa.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);

    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);

    if (check_single_on(g_prog_name)) {
        return -1;
    }

    g_p_arg_start = argv[0];
    g_p_arg_end = argv[g_saved_argc - 1] + strlen (argv[g_saved_argc - 1]) + 1;
    g_p_env_start = environ[0];

    char run_mode[128] = {0};
    if (p_ini_file->read_string("WorkInfo", "run_mode", run_mode, sizeof(run_mode), NULL) != 0) {
        std::cerr << "failed to read run_mode from config file" << std::endl;
        return -1;
    }

    if (strncasecmp(run_mode, "background", 10) == 0) {
        ///后台模式运行
        if (daemon(1, 0) < 0) {
            std::cerr << "daemon failed, err: " << strerror(errno) << std::endl;
            return -1;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    return 0;

}

void daemon_stop()
{

    int work_num = g_work_num;
    while (work_num-- > 0) {
        usleep(200000);
    }

    if (g_restart && g_prog_name && g_saved_argv) {
        chdir(g_p_current_dir);
        execv(g_prog_name, g_saved_argv);
    }

    char** argv = NULL;
    for (argv = g_saved_argv; *argv; ++argv) {
        free(*argv);
        *argv = NULL;
    }
    free(g_saved_argv);
    g_saved_argv = NULL;
    g_saved_argc = 0;

    free(g_prog_name);
    free(g_p_current_dir);
}

void daemon_set_title(const char* fmt, ...)
{
    char title[64];
    int i, tlen;
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(title, sizeof(title) - 1, fmt, ap);
    va_end(ap);

    tlen = strlen(title) + 1;
    if (g_p_arg_end - g_p_arg_start < tlen && g_p_env_start == g_p_arg_end) {
        char *env_end = g_p_env_start;
        for (i = 0; environ[i]; i++) {
            if (env_end == environ[i]) {
                env_end = environ[i] + strlen(environ[i]) + 1;
                environ[i] = strdup(environ[i]);
            } else {
                break;
            }
        }
        g_p_arg_end = env_end;
        g_p_env_start = NULL;
    }

    i = g_p_arg_end - g_p_arg_start;
    if (tlen == i) {
            strcpy(g_p_arg_start, title);
    } else if (tlen < i) {
            strcpy(g_p_arg_start, title);
            memset(g_p_arg_start + tlen, 0, i - tlen);
    } else {
            stpncpy(g_p_arg_start, title, i - 1)[0] = '\0';
    }
}

int set_cpu_affinity(int proc_num, pid_t pid, u_int id)
{
    int cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    int cpu_div = 0;
    if (proc_num % cpu_num) {
        cpu_div = proc_num / cpu_num + 1;
    } else {
        cpu_div = proc_num /cpu_num;
    }
    int cpu_ser = id / cpu_div;

    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(cpu_ser, &cpu_mask);

    return sched_setaffinity(pid, sizeof(cpu_mask), &cpu_mask);
}
