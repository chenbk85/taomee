#include "Common.h"
#include "Options.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/resource.h>

init_factory app_init = NULL;
fini_factory app_fini = NULL;
handle_socket_factory app_socket = NULL;
handle_stat_factory app_stat = NULL;
handle_accept_factory app_accept = NULL;
bool stopped = false;

int load_dll(const char* dll_name)
{
	void *handle;
	char *error = NULL;

	handle = dlopen (dll_name, RTLD_NOW);
	if ((error = dlerror()) != NULL || !handle)
		goto err_entry;

	app_init = (init_factory) dlsym (handle, "init");
	if ((error = dlerror()) != NULL)
		goto err_entry;

	app_socket = (handle_socket_factory) dlsym (handle, "handle_socket");
	if ((error = dlerror()) != NULL)
		goto err_entry;


	app_accept = (handle_accept_factory) dlsym (handle, "handle_accept");
	if ((error = dlerror()) != NULL)
		goto err_entry;
/*
	app_stat = (handle_stat_factory) dlsym (handle, "handle_stat");
	if ((error = dlerror()) != NULL)
		goto err_entry;
*/
	app_fini = (fini_factory) dlsym (handle, "fini");
	if ((error = dlerror()) != NULL)
		goto err_entry;

	return 0;
err_entry:
	printf("open %s error: %s\n" ,dll_name, error);
	return -1;
}

static void sigterm_handler(int signo) 
{
	stopped = true;
}

void daemon_start ()
{
	struct sigaction sa;
	sigset_t sset;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigterm_handler;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGPIPE,SIG_IGN);	

	sigemptyset(&sset);
	sigaddset(&sset, SIGSEGV);
	sigaddset(&sset, SIGBUS);
	sigaddset(&sset, SIGABRT);
	sigaddset(&sset, SIGILL);
	sigaddset(&sset, SIGCHLD);
	sigaddset(&sset, SIGFPE);
	sigprocmask(SIG_UNBLOCK, &sset, &sset);

	daemon (1, 1);
}

void init_fdlimit ()
{
	struct rlimit rlim;

	/* raise open files */
	rlim.rlim_cur = 20000;
	rlim.rlim_max = 20000;
	setrlimit(RLIMIT_NOFILE, &rlim);

	/* allow core dump */
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);
}
