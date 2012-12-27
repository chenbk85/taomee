#ifndef ONLINE_DAEMON_H_
#define ONLINE_DAEMON_H_

// headers since C89
#include <stdlib.h>

#define MAXFDS	18000

volatile extern int stop;
volatile extern int restart;
extern int pagesize;

extern char *prog_name;
extern char *current_dir;
extern char **saved_argv;

extern void daemon_stop (void);
extern int daemon_start ();
extern void daemon_set_title (const char *fmt, ...);
extern void pagesize_init ();

static inline void free_argv(void)
{
	char **argv;
	for ( argv = saved_argv; *argv; ++argv ) {
		free (*argv);
	}
	free(saved_argv);
	saved_argv = NULL;
}

#endif // ONLINE_DAEMON_H_
