#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "net.h"
#include "util.h"
#include "shmq.h"
#include "daemon.h"
#include <libtaomee/log.h>
#include "service.h"
#include "mcast.h"

static const char version[] = "1.0.0";
extern char **saved_argv;
char *prog_name;
char *current_dir;
struct epinfo epi;

void show_banner ()
{
	char feature[256];
	int pos = 0;

#ifdef DEBUG
	pos = sprintf (feature + pos, "-DDEBUG -g ");
#endif
#ifdef EPOLL_MODE
	pos = sprintf (feature + pos, "-DEPOLL_MODE ");
#endif
	INFO_LOG ("Game Server v%s (C) 2007-2008 TAOMEE.COM, report bugs to <andy@taomee.com>", version);
	INFO_LOG ("Compiled at %s %s, flag: %s\n", __DATE__, __TIME__, pos ? feature : "");
}

void show_usage ()
{
	INFO_LOG ("Usage: %s conf\n", prog_name);
	exit (-1);
}

static void parse_args (int argc, char **argv)
{
	prog_name = strdup(argv[0]);
	current_dir = get_current_dir_name ();
	show_banner ();
	if (argc < 2 || !strcmp (argv[1], "--help") || !strcmp (argv[1], "-h"))
		show_usage ();
}

int main (int argc, char* argv[])
{
	pid_t pid;

	parse_args (argc, argv);
	config_init (argv[1]);

	pipe_create ();
	shmq_create ();

	daemon_start (argc, argv);

	log_init(config_get_strval("log_dir"), config_get_intval("log_level", log_lvl_trace),
				config_get_intval("log_size", 1<<30), config_get_intval("max_log_files", 100),
				config_get_strval("log_prefix"));

	if ((pid = fork ()) < 0)
		BOOT_LOG (-1, "fork child process");
	//parent process
	else if (pid > 0) {
		close (pipe_handles[3]);
		close (pipe_handles[0]);

		//setup_timer();

		net_init (MAXFDS, pipe_handles[2]);
		net_start (config_get_strval("bind_ip"), (uint16_t)config_get_intval("bind_port", 0));
		while (!stop)
			net_loop (-1, RCVBUFSZ, 1);
	//child process
	} else {
		close (pipe_handles[1]);
		close (pipe_handles[2]);

		net_init (MAXFDS, pipe_handles[0]);

		if (handle_init () != 0)
			goto out;

		while (!stop || !handle_fini ())
			net_loop (1000, RCVBUFSZ, 0);
	}
out:
	net_exit ();
	shmq_destroy ();
	if (pid > 0) daemon_stop ();
	return 0;
}

