#include "server_task.h"
#include "client_task.h"
#include "merge_task.h"

int main()
{
		std::map<int, task_data> server_task_datas;
		bool ret = load_server_task("server_tasks.xml", server_task_datas);
		if(ret == false)
		{
			printf("load server_tasks.xml fail\n");
			return 0;
		}
		else
		{
			printf("load server_tasks.xml ok\n");
		}

		std::map<int, client_task> client_task_datas;
		ret = load_client_task("client_tasks.xml", client_task_datas);
		if(ret == false)
		{
			printf("load client_tasks.xml fail\n");
			return 0;
		}
		else
		{
			printf("load server_tasks.xml ok\n");
		}

		ret = merge_task(client_task_datas, server_task_datas);
		if(ret == false)
		{
			printf("merge_task fail \n");
			return 0;
		}
		else
		{
			printf("load server_tasks.xml ok\n");
		}

		ret = save_client_task("merge_tasks.xml", client_task_datas);
		if(ret == false)
		{
			printf("merge_tasks save fail \n");
			return 0;
		}
		else
		{
			printf("merge_tasks save ok \n");	
		}


	return 0;
}