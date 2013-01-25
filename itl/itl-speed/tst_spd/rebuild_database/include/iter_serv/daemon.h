#ifndef ITERATIVE_SERVER_DAEMON_H_
#define ITERATIVE_SERVER_DAEMON_H_

// headers since C89
#include <stdlib.h>

extern int max_fd_num;

volatile extern int stop;
volatile extern int restart;

extern char*	prog_name;
extern char*	current_dir;
extern char**	saved_argv;

int  daemon_start(int argc, char** argv);
void daemon_stop(void);
void daemon_set_title(const char* fmt, ...);

static inline void
free_argv(void)
{
	char** argv;
	for ( argv = saved_argv; *argv; ++argv ) {
		free(*argv);
	}
	free(saved_argv);
	saved_argv = NULL;
}

#endif // ITERATIVE_SERVER_DAEMON_H_
