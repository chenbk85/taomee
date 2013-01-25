/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file utils.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-03-09
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "utils.h"

#define LOCKFILE "./daemon.pid"

int mysignal(int sig, void(*signal_handler)(int))
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = signal_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	return sigaction(sig, &act, 0);
}

static int lockfile(int fd)
{
	struct flock fl = {0};

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

extern char **environ;
static int prog_argc = -1;
static char **pp_prog_argv = NULL;
static char *p_prog_last_argv = NULL;

void init_proc_title(int argc, char *argv[]) 
{
	char **old_environ = environ;

	int environ_count = 0;
	while (old_environ[environ_count] != NULL) {
		++environ_count;
	}

	int i = 0;
	char **p = NULL;
	if ((p = (char **) malloc((environ_count + 1) * sizeof(char *))) != NULL) {
		for (i = 0; old_environ[i] != NULL; i++) {
			size_t envp_len = strlen(old_environ[i]);

			p[i] = (char *)malloc(envp_len + 1);
			if (p[i] != NULL) {
				strncpy(p[i], old_environ[i], envp_len + 1);
			}
		}

		p[i] = NULL;
		environ = p;
	}

	pp_prog_argv = argv;
	prog_argc = argc;

	for (i = 0; i < prog_argc; ++i) {
		if (!i || (p_prog_last_argv + 1 == argv[i])) {
			p_prog_last_argv = argv[i] + strlen(argv[i]);
		}
	}

	for (i = 0; old_environ[i] != NULL; ++i) {
		if ((p_prog_last_argv + 1) == old_environ[i]) {
			p_prog_last_argv = old_environ[i] + strlen(old_environ[i]);
		}
	}
}

void uninit_proc_title() 
{
	if (environ) {
		unsigned int i;

		for (i = 0; environ[i] != NULL; ++i) {
			free(environ[i]);
		}
		free(environ);
		environ = NULL;
	}
}

void set_proc_title(const char *fmt, ...) 
{
	int maxlen = (p_prog_last_argv - pp_prog_argv[0]) - 2;

	if (fmt == NULL) {
		return;
	}

	va_list msg;
	va_start(msg, fmt);
	memset(pp_prog_argv[0], 0, maxlen);
	vsnprintf(pp_prog_argv[0], maxlen, fmt, msg);
	va_end(msg);

	int i = 0;
	for (i = 1; i < prog_argc; ++i) {
		pp_prog_argv[i] = NULL;
	}
}

int get_proc_title(char *buf, size_t bufsz) 
{
	if (buf == NULL || bufsz == 0) {
		return strlen(pp_prog_argv[0]);
	}

	strncpy(buf, pp_prog_argv[0], bufsz);
	
	return strlen(buf);
}
