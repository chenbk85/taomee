#include <dlfcn.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "worker.h"
#include "tsc.h"
#include "daemon.h"
#include "notifier.h"
#include "benchapi.h"
#include "config.h"
#include "net.h"
#include "shmq.h"
#include "dll.h"

static pid_t *wpids = NULL;
static int proc_cnt = 0;
static long long mb_timeout;
void __attribute__ ((noreturn)) worker_run (int argc, char **argv)
{
	int ret_code;
	char *user_data;
	struct shm_block *mb;
	
	daemon_set_title ("%s:[WORKER]", prog_name);

	if (dll.handle_init != NULL)
		if (dll.handle_init (argc, argv, PROC_WORK))
			boot_log (-1, 0, "worker process handle_init");
	
	while (!stop) {
		if (unlikely (shmq_pop (&recvq, &mb, SLEEP_MASK | LOCKED_MASK) != 0))
			continue;
		rdtscll (current_tsc);
		if (likely (!check_timeout (mb->skinfo.recvtm, mb_timeout))) {
			int rcvlen = mb->length - sizeof (shm_block_t);
			int sndlen = 0;

			ret_code = dll.handle_process (mb->data, rcvlen, &user_data, &sndlen, &mb->skinfo); 
			TRACE_LOG ("handle_process return %d, rcvlen=%d, sndlen=%d, fd=%d", 
					ret_code, rcvlen, sndlen, mb->skinfo.sockfd);

			//close connection
			if (unlikely (ret_code == -1)) {
				mb->type = FIN_BLOCK;
				mb->length = sizeof (shm_block_t);
				shmq_push (&sendq, mb, NULL, SLEEP_MASK | LOCKED_MASK);
				write_pipe ();
			} else if (unlikely (sndlen > SHM_BLOCK_LEN_MAX)) {
				ERROR_LOG ("handle_process return invalid sndlen=%d", sndlen);
				/// 改成非动态分配版本
				//free (user_data);
			} else if (likely (sndlen > 0)) {
				mb->length = sndlen + sizeof (shm_block_t);
				if (ret_code == 1)
					mb->type = FIN_BLOCK;
				else
					mb->type = DAT_BLOCK;
				shmq_push (&sendq, mb, user_data, SLEEP_MASK | LOCKED_MASK);

				/// 改成非动态分配版本
				//free (user_data);
				write_pipe ();
			}
		}
		free (mb);
	}

	if (dll.handle_fini)
		dll.handle_fini (PROC_WORK);
	exit (0);		
}

void children_monitor (int argc, char** argv)
{
	int i, result, sec = 10;
	pid_t pid, old_pid;
	int sleep_ok;
	
	while (!stop) {
		struct timeval tv = {sec, 0};
		sleep_ok = select (0, NULL, NULL, NULL, &tv); 

		for (i = 0; i < proc_cnt && sleep_ok == 0; i++) {
			if (unlikely (wpids[i] != 0)) {
				result = kill (wpids[i], 0);
				if (result == 0 || errno != ESRCH)
					continue;
			}

			if (!stop) {
				old_pid = wpids[i];
				if ((pid = fork ()) > 0)
					wpids[i] = pid;
				else if (pid == 0)
					worker_run (argc, argv);
				else
					wpids[i] = 0;

				TRACE_LOG ("pid=%d has been exit, fork new process %d", old_pid, pid); 
			}
		}

		if (dll.handle_timer)
			dll.handle_timer (&sec);
	}

	if (dll.handle_fini)
		dll.handle_fini (PROC_MAIN);

	//wait for children end
	for (i = 0; i < proc_cnt; i++) {
		if (wpids[i] != 0) {
			result = kill (wpids[i], 0);
			if (result == 0 || errno != ESRCH) { 
				i --;
				usleep (50000);
			}
		}
	}
}

int worker_procs_spawn (pid_t conn_pid, int argc, char **argv)
{
	int i, conn_stat;

	//check conn process has been exited
	usleep (100000);
	conn_stat = kill (conn_pid, 0);
	if (conn_stat != 0 && errno == ESRCH)
		exit (-1);

	mb_timeout = config_get_intval ("pkg_timeout", 0) * tscsec;
	proc_cnt = config_get_intval ("worker_num", 0);
	wpids = (pid_t *) calloc (sizeof (pid_t), proc_cnt);
	
	for (i = 0; i < proc_cnt; i++) {
		if ((wpids[i] = fork ()) < 0) {
			ERROR_RETURN (("fork failed, %s", strerror (errno)), -1);
		} else if (wpids[i] == 0) {
			worker_run (argc, argv);
		}
	}

	BOOT_LOG (0, "fork %d worker processes", proc_cnt); 
}



