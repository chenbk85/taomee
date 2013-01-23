#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "benchapi.h"
#include "dll.h"
#include "notifier.h"
#include "dispatcher.h"
#include "shmq.h"
#include "daemon.h"
#include "tsc.h"
#include "worker.h"

static const char version[] __initdata = "3g.1.1.0";
extern char **saved_argv;
char *prog_name;
char *current_dir;

void __init show_banner ()
{
	char feature[256];
	int pos = 0;
	
#ifdef DEBUG	
	pos = sprintf (feature + pos, "-DDEBUG -g");
#endif
#ifdef EPOLL_MODE
	pos = sprintf (feature + pos, "-DEPOLL_MODE ");
#endif
	INFO_LOG ("ServerBench %s, report bugs to <jasonwang@taomee.com>", version);
	INFO_LOG ("Compiled at %s %s, flag: %s\n", __DATE__, __TIME__, pos ? feature : "");
}

void __init show_usage ()
{
	INFO_LOG ("Usage: %s conf dll [option]\n", prog_name);
	exit (0);
}

static __init int parse_args (int argc, char **argv)
{
	prog_name = strdup(argv[0]);
	current_dir = strdup (get_current_dir_name ());
	show_banner ();
	if (argc < 3 || !strcmp (argv[1], "--help") || !strcmp (argv[1], "-h")) 
		show_usage ();

	BOOT_LOG (0, "parse parameters of command line");
}

int main (int argc, char* argv[])
{
	pid_t pid;

	parse_args (argc, argv);

	if (config_init (argv[1]) == -1) {
		BOOT_LOG(-1, "Failed to Parse File '%s'", argv[1]);
	}

	pagesize_init ();       // ???
	tsc_init ();    // ???
	
	load_bind_file (config_get_strval ("bind_file"));
	register_plugin (argv[2]);

	pipe_create ();
	shmq_create ();

	daemon_start (argc, argv);
	if (dll.handle_init)
		boot_log (dll.handle_init (argc, argv, PROC_MAIN), 0, "call main process handle_init"); 

	log_init (config_get_strval ("log_dir"), config_get_intval ("log_level", APP_TRACE),
		config_get_intval ("log_size", 1<<30), config_get_strval ("log_prefix"));

	daemon_set_title ("%s:[MAIN]", prog_name);
	if ((pid = fork ()) < 0) {
		BOOT_LOG (-1, "fork child process");
	//parent process	
	} else if (pid > 0) {
		rd_pipe_close ();
		worker_procs_spawn (pid, argc, saved_argv);
		children_monitor (argc, saved_argv);
		shmq_destory ();
		daemon_stop ();
	//child process	
	} else {
		wr_pipe_close ();
		conn_loop ();
	}
	
	return 0;
}
