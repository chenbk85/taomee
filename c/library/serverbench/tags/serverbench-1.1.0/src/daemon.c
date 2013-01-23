#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include "daemon.h"
#include "benchapi.h"

volatile int stop = 0;
volatile int restart = 0;
char **saved_argv = NULL;
static char *arg_start;
static char *arg_end;
static char *env_start;
static int backgd_mode = 0;
int pagesize __initdata = 4096;

static void sigterm_handler(int signo) 
{
	stop = 1;
	restart = 0;
}

static void sighup_handler(int signo) 
{
	restart = 1;
	stop = 1;
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
	if (setrlimit(RLIMIT_NOFILE, &rlim) == -1) {
		ALERT_LOG("INIT FD RESOURCE FAILED");
	}

	/* allow core dump */
	rlim.rlim_cur = 1<<29;
	rlim.rlim_max = 1<<29;
	if (setrlimit(RLIMIT_CORE, &rlim) == -1) {
		ALERT_LOG("INIT CORE FILE RESOURCE FAILED");
	}
}

int daemon_start (int argc, char** argv)
{
	struct sigaction sa;
	sigset_t sset;
	const char *style;

	rlimit_reset ();

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
        unsigned int i;
        int n;  
        u_char c; 

        for (i = 1; i < (unsigned) pagesize_init ; i <<= 1) {
                if ((n = mincore ((void *)i, 1, &c)) >= 0) 
			break;
                if (errno == ENOMEM) 
			break;
        }       
        if (i < (unsigned) pagesize_init)
                pagesize = i;
}
