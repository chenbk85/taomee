#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <confuse.h>

#include "daemon.h"

void init_daemon(cfg_bool_t bg, cfg_bool_t debug)
{
	int fd;
	pid_t pid, sid;

	// shield some signals
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGSEGV, SIG_IGN);
	//signal(SIGHUP,  SIG_IGN); /* reread cfg */
	//signal(SIGINT,  SIG_IGN); /* stop to quit */

	if (bg == cfg_true) {
		pid = fork();

		if (pid < 0) {
			exit(EXIT_FAILURE);
		} else if (pid > 0) {
			exit(EXIT_SUCCESS);
		}

		sid = setsid();
		if (sid < 0) {
			exit(EXIT_FAILURE);
		}
	}

	fd = (debug == cfg_true) ? 3 : 0;
	for(; fd < NOFILE; ++fd) {
		close(fd);
	}

	umask(0);
}
