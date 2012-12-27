#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <linux/unistd.h>
#include "daemon.h"
#include "config.h"
#include <libtaomee/log.h>

volatile int stop = 0;
volatile int restart = 0;
char **saved_argv = NULL;
static char *arg_start;
static char *arg_end;
static char *env_start;
static int backgd_mode = 0;
int pagesize = 4096;
static int status;

const char *const signame[] = {
	"SIG_0",
	"SIGHUP",	"SIGINT",	"SIGQUIT",	"SIGILL",
	"SIGTRAP",	"SIGABRT",	"SIGBUS",	"SIGFPE",
	"SIGKILL",	"SIGUSR1",	"SIGSEGV",	"SIGUSR2",
	"SIGPIPE",	"SIGALRM",	"SIGTERM",	"SIGSTKFLT",
	"SIGCHLD",	"SIGCONT",	"SIGSTOP",	"SIGTSTP",
	"SIGTTIN",	"SIGTTOU",	"SIGURG",	"SIGXCPU",
	"SIGXFSZ",	"SIGVTALRM",	"SIGPROF",	"SIGWINCH",
	"SIGIO",	"SIGPWR",	"SIGSYS",
	"SIGRT_0",	"SIGRT_1",	"SIGRT_2",	"SIGRT_3",
	"SIGRT_4",	"SIGRT_5",	"SIGRT_6",	"SIGRT_7",
	"SIGRT_8",	"SIGRT_9",	"SIGRT_10",	"SIGRT_11",
	"SIGRT_12",	"SIGRT_13",	"SIGRT_14",	"SIGRT_15",
	"SIGRT_16",	"SIGRT_17",	"SIGRT_18",	"SIGRT_19",
	"SIGRT_20",	"SIGRT_21",	"SIGRT_22",	"SIGRT_23",
	"SIGRT_24",	"SIGRT_25",	"SIGRT_26",	"SIGRT_27",
	"SIGRT_28",	"SIGRT_29",	"SIGRT_30",	"SIGRT_31",
	"SIGRT_32"
};
static void sigterm_handler(int signo)
{
	stop = 1;
	restart = 0;
	DEBUG_LOG ("SIG_TERM from pid=%d", getpid());
}

static void sighup_handler(int signo)
{
	restart = 1;
	stop = 1;
}

static void daemon_emergent_restart(int signo)
{
	int i;
	struct timespec tv;

	stop = 1;
	tv.tv_sec = 5;
	tv.tv_nsec = 0;
	nanosleep(&tv, NULL);
	for(i=3; i<MAXFDS; i++)
		close(i);

	ERROR_LOG ("Emergent Restarting...\n");
	if(signo) {
		sigset_t sset;
		sigaddset(&sset, signo);
		sigprocmask(SIG_UNBLOCK, &sset, &sset);
	}
	execv(prog_name, saved_argv);
	ERROR_LOG("Restart Failed.\n");
	exit(-1);
}

static void sigchld_handler(int signo, siginfo_t *si, void * p)
{
	char *corename;
	while (waitpid (-1, &status, WNOHANG|__WALL) > 0);

	switch (si->si_code) {
		case SI_USER:
		case SI_TKILL:
			DEBUG_LOG("SIGCHLD from pid=%d uid=%d, IGNORED",
					si->si_pid, si->si_uid);
			return; /* someone send use fake SIGCHLD */
		case CLD_KILLED:
			DEBUG_LOG("child %d killed by signal %s",
					si->si_pid, signame[WTERMSIG(si->si_status)]);
			stop = 1;
			if (WTERMSIG(si->si_status) == SIGABRT)
				restart = 1;
			break;
		case CLD_TRAPPED:
			DEBUG_LOG("child %d trapped", si->si_pid);
			return;
		case CLD_STOPPED:
			DEBUG_LOG("child %d stopped", si->si_pid);
			if(si->si_pid > 1) kill(si->si_pid, SIGCONT);
			return;
		case CLD_CONTINUED:
			DEBUG_LOG("child %d continued", si->si_pid);
			return;
		case CLD_DUMPED:
			DEBUG_LOG("child %d coredumped by signal %s",
					si->si_pid, signame[WTERMSIG(si->si_status)]);
			chmod("core", 700);
			chown("core", 0, 0);
			corename = alloca(40);
			sprintf(corename, "core.%d", si->si_pid);
			rename("core", corename);
			restart = 1;
			stop = 1;
			break;
	}

	daemon_emergent_restart (signo);
}

static void dup_argv(int argc, char **argv) {
	saved_argv = malloc (sizeof (char *) * (argc + 1));
	if (!saved_argv)
		return;
	saved_argv[argc] = NULL;
	while (--argc >= 0) {
		saved_argv[argc] = strdup (argv[argc]);
	}
}

static void free_argv(void) {
	char **argv;
	for (argv = saved_argv; *argv; argv++)
		free (*argv);
	free (saved_argv);
	saved_argv = NULL;
}

static void rlimit_reset ()
{
	struct rlimit rlim;

	/* raise open files */
	rlim.rlim_cur = MAXFDS;
	rlim.rlim_max = MAXFDS;
	setrlimit(RLIMIT_NOFILE, &rlim);

	/* allow core dump */
	rlim.rlim_cur = 1<<30;
	rlim.rlim_max = 1<<30;
	setrlimit(RLIMIT_CORE, &rlim);
}

int daemon_start (int argc, char** argv)
{
	struct sigaction sa;
	sigset_t sset;
	const char *style;

	rlimit_reset ();

	memset(&sa, 0, sizeof(sa));
	signal(SIGPIPE,SIG_IGN);
	signal(SIGCHLD,SIG_IGN);

	sa.sa_handler = sigterm_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);

	sa.sa_handler = sighup_handler;
	sigaction(SIGHUP, &sa, NULL);
//	sigaction(SIGCHLD, &sa, NULL);
/*
	sa.sa_flags = SA_RESTART|SA_SIGINFO;
	sa.sa_sigaction = sigchld_handler;
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGILL, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGFPE, &sa, NULL);
*/
	sigemptyset(&sset);
	sigaddset(&sset, SIGSEGV);
	sigaddset(&sset, SIGBUS);
	sigaddset(&sset, SIGABRT);
	sigaddset(&sset, SIGILL);
	sigaddset(&sset, SIGCHLD);
	sigaddset(&sset, SIGFPE);
	sigprocmask(SIG_UNBLOCK, &sset, &sset);

	arg_start = argv[0];
	arg_end = argv[argc-1] + strlen (argv[argc - 1]) + 1;
	env_start = environ[0];
	dup_argv(argc, argv);

	style = config_get_strval ("run_mode");
	if (!style || !strcasecmp ("background", style)) {
		daemon (1, 1);
		backgd_mode = 1;
		BOOT_LOG (0, "switch to daemon mode");
	}
	return 0;
}

void daemon_stop(void)
{
	if (!backgd_mode)
		printf ("Server stopping...\n");

	if (restart && prog_name && saved_argv) {
		WARN_LOG ("%s", "Server restarting...");
		chdir (current_dir);
		execv (prog_name, saved_argv);
		WARN_LOG ("%s", "Restart Failed...");
	}

	free_argv ();
	free (prog_name);
	free (current_dir);
	prog_name = NULL;
}

void daemon_set_title (const char* fmt, ...)
{
	char title [64];
	int i, tlen;
	va_list ap;

	va_start(ap, fmt);
	vsnprintf (title, sizeof (title) - 1, fmt, ap);
	va_end (ap);

	tlen = strlen (title) + 1;
	if (arg_end - arg_start < tlen && env_start == arg_end) {
		char *env_end = env_start;
		for(i=0; environ[i]; i++) {
			if(env_end == environ[i]) {
				env_end = environ[i] + strlen (environ[i]) + 1;
				environ[i] = strdup(environ[i]);
			} else
				break;
		}
		arg_end = env_end;
		env_start = NULL;
	}

	i = arg_end - arg_start;
	if (tlen == i) {
		strcpy (arg_start, title);
	} else if (tlen < i) {
		strcpy (arg_start, title);
		memset (arg_start + tlen, 0, i - tlen);
	} else {
		stpncpy(arg_start, title, i - 1)[0] = '\0';
	}
}

void pagesize_init (void)
{
/*
		long i;
		int n;
		u_char c;

		for (i = 1; i < (long) pagesize_init ; i <<= 1) {
				if ((n = mincore ((void *)i, 1, &c)) >= 0)
			break;
				if (errno == ENOMEM)
			break;
		}
		if (i < (long) pagesize_init)
				pagesize = i;
*/
		pagesize = sysconf(_SC_PAGESIZE);		
}

