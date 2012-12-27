#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>

#include "bindconf.h"
#include "util.h"
#include "shmq.h"
#include "daemon.h"
#include "net.h"
#include "service.h"

static const char version[] = "1.2.0";

char *prog_name;
char *current_dir;
struct epinfo epi;

void show_banner()
{
	char feature[256];
	int pos = 0;

#ifdef DEBUG
	pos = sprintf(feature + pos, "-DDEBUG -g ");
#endif
#ifdef EPOLL_MODE
	pos = sprintf(feature + pos, "-DEPOLL_MODE ");
#endif
	INFO_LOG("Online Server v%s (C) 2007-2008 TAOMEE.COM, report bugs to <andy@taomee.com>", version);
	INFO_LOG("Compiled at %s %s, flag: %s\n", __DATE__, __TIME__, pos ? feature : "");
}

void show_usage()
{
	INFO_LOG("Usage: %s conf\n", prog_name);
	exit(-1);
}

static void parse_args(int argc, char **argv)
{
	prog_name = strdup(argv[0]);
	current_dir = get_current_dir_name ();
	show_banner();
	if (argc < 2 || !strcmp (argv[1], "--help") || !strcmp (argv[1], "-h"))
		show_usage();
}

int main(int argc, char* argv[])
{
	pid_t pid;
	
	parse_args(argc, argv);

	if (config_init(argv[1]) == -1) {
		BOOT_LOG(-1, "Failed to Parse File '%s'", argv[1]);
	}

	if (config_init("common.conf") == -1) {
			BOOT_LOG(-1, "Failed to Parse File '%s'", argv[1]);
	}

	daemon_start(argc, argv);
	load_bind_file();
	log_init_ex( config_get_strval("log_dir"),
				config_get_intval("log_level", log_lvl_trace),
				config_get_intval("log_size", 1<<30),
				config_get_intval("max_log_files", 0),
				config_get_strval("log_prefix") ,
				config_get_intval("log_save_next_file_interval_min", 0) );

    //如果使用脏词检测逻辑，则开启父进程的脏词更新逻辑.
    if (config_get_intval("tm_dirty_use_dirty_logical", 1) == 1) {
        if (tm_dirty_daemon( config_get_strval("tm_dirty_local_dirty_file_path"),
                    config_get_strval("tm_dirty_server_addr"),
                    config_get_intval("tm_dirty_update_cycle", 600),
                    NULL) == -1) {
            BOOT_LOG(-1, "FAILED TO RUN TM_DIRTY_DAEMON");
        }   
    }   

	net_init(MAXFDS, MAXFDS);

	bind_config_t* bc = get_bind_conf();
//--------------------------------
	net_start(config_get_strval("online_ip"), config_get_intval("online_port", 443), bc->configs);
//--------------------------------

//	bind_config_elem_t* bc_elem;
	int i = 0;
	for ( ; i != bc->bind_num; ++i ) {
		bind_config_elem_t* bc_elem = &(bc->configs[i]);
		shmq_create(bc_elem);

		if ( (pid = fork ()) < 0 ) {
			BOOT_LOG(-1, "fork child process");
		} else if (pid > 0) { //parent process
			close_shmq_pipe(bc, i, 0);
			do_add_conn(bc_elem->sendq.pipe_handles[0], PIPE_TYPE_FD, 0, bc_elem);
			//net_start(bc_elem->bind_ip, bc_elem->bind_port, bc_elem);
		} else { //child process
			run_worker_process(bc, i, i + 1);
		}
	}

	while (!stop) {
		net_loop(-1, PAGESIZE, 1);
	}

	net_exit();
	shmq_destroy(0, bc->bind_num);
	daemon_stop();

	return 0;
}
