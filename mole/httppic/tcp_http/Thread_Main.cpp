#include "Thread_Main.h"
#include "Log.h"
#include "Net.h"
#include "Common.h"
#include "myepoll.h"
#include "Net.h"
#include "Options.h"
#include <stdio.h>
#include <vector>
using namespace std;

pthread_t *thread_tid = NULL;
//atomic_t process_cn;

void *thread_main(void *arg)
{
	int active_fd, ret_code; 
	while (true)
	{
		if (g_handle->child_wait (&active_fd) != 0)
			continue;

		if (active_fd > 0)
		{
//			atomic_inc (&process_cn);
			ret_code = app_socket (active_fd);
			LOG (TRACE, "app_socket return %d", ret_code);

			if (ret_code == 0)
				g_handle->add_epoll_events (active_fd);
			else
				close (active_fd);

//			atomic_dec (&process_cn);
		}
	}
}

void thread_make ()
{
//	process_cn.counter = 0;
	thread_tid = (pthread_t*)calloc (ini.worker_proc_num, sizeof (pthread_t));
	if (thread_tid == NULL) {
		fprintf (stderr, "thread_make::calloc error\n");
		exit (-1);
	}

	for (int i = 0; i < ini.worker_proc_num; i++)
		pthread_create (&thread_tid[i], NULL, &thread_main, (void*)i);
}

