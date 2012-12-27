#include "Net.h"
#include "Common.h"
#include "Options.h"
#include "Log.h"
#include "Thread_Main.h"
#include <unistd.h>
#include <stdio.h>

char dll_file[256] = {0};
char ini_file[256] = {0};

Log *g_pLog;
Epoll_handle *g_handle;
Option option;

void print_version ()
{
	printf ("Copyright@Tencent Inc. ISD Dep, All right reserved\n"); 
	printf ("TCP Pool 1.1, 2005-12-09\n"); 
}
int parse_args (int argc, char *argv[])
{
	if (argc < 3)
	{
		printf ("usage: Tcp_Pool config_file dll_file [option]\n");
		return -1;
	}
	strncpy (ini_file ,argv[1], sizeof (ini_file));
	strncpy (dll_file ,argv[2], sizeof (dll_file));

	if(access (ini_file, F_OK) != 0)
	{
		printf("get config file:%s error\n", ini_file);
		return -1;
	}

	if(option.init (ini_file) != 0)
	{
		printf ("initalize config file:%s error\n", ini_file);
		return -1;
	}

	option.print ();
	return 0;
}

int main(int argc,char* argv[])
{
	print_version ();
	if (parse_args (argc, argv))
		return -1;
	printf ("parse args \t\t\t[ok]\n");

	g_pLog = new Log (ini.log_dir, ini.log_priority, ini.log_size,
			ini.log_num, ini.log_prename);
	printf ("initalize log \t\t\t[ok]\n");

	if (load_dll (dll_file))
		return -1;

	init_fdlimit ();
	daemon_start ();
	printf ("daemon start\t\t\t[ok]\n");

	if (app_init (argc, argv) != 0)
	{
		fprintf (stderr, "error in execute so init funciotn\n");
		return -1;
	}
	printf ("initalize application \t\t[ok]\n");
	
	g_handle = new Epoll_handle ();
	if (g_handle->open () != 0)
		return -1;
	thread_make ();
	printf ("initalize thread pool \t\t[ok]\n");

	while (!stopped)
	{
		g_handle->wait_ready ();
	/*
		if (app_stat != NULL)
			app_stat (g_handle->active_cn (), g_handle->idle_cn (), process_cn.counter);
	*/
	}

	for (int i = 0; i < ini.worker_proc_num; i++)
		pthread_join (thread_tid[i], NULL);

	app_fini ();

	delete g_pLog;
	return 0;
}
