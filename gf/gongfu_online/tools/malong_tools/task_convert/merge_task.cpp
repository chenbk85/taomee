#include "merge_task.h"


bool merge_task(std::map<int, client_task>& client_tasks, std::map<int, task_data>& server_tasks)
{
	std::map<int, client_task>::iterator pItr = client_tasks.begin();
	for(; pItr != client_tasks.end(); ++pItr)
	{
		client_task* p_client_task = &(pItr->second);
		std::map<int, task_data>::iterator pItr2 = server_tasks.find(p_client_task->task_id);
		if(pItr2 == server_tasks.end())
		{
			printf("task_id %d in client_task not in server task", p_client_task->task_id);
			return false;
		}
		task_data* p_server_task  = &(pItr2->second);
		p_client_task->vip_limit = p_server_task->vip_limit;
		p_client_task->master_task_id = p_server_task->master_task_id;

		for(unsigned int i =0; i< p_client_task->condition_data.condition_datas.size(); i++)
		{
			pro_data* p_pro_data = &p_client_task->condition_data.condition_datas[i];
			if( strcmp(p_pro_data->proc, "TaskXML_ItemPay") == 0)
			{
				unsigned int item_id = 0;
				unsigned int item_count = 0;
				sscanf(p_pro_data->params, "%u,%u", &item_id, &item_count);
				//////////////找到服务器task与之匹配的in_item项///////////////////////
				in_item_data* p_data = p_server_task->get_in_by_item_id(item_id);
				if(p_data == NULL)
				{
					printf("task_id:%u, in_item %u error", p_client_task->task_id,  item_id);
					continue;
				}
				p_pro_data->give_type = p_data->give_id;
				p_pro_data->monster_id = p_data->monster_id;
				p_pro_data->stage = p_data->stage;
				p_pro_data->drop_odds = p_data->drop_odds;
			}
		}
		
		for(unsigned int i =0; i< p_client_task->rewards_data.fix.size(); i++)
		{
			selected_data* p_fix_data = &p_client_task->rewards_data.fix[i];
			//////////////找到服务器task与之匹配的out_item项///////////////////////
			out_item_data* p_data = p_server_task->get_out_by_item_id(p_fix_data->id);
			if(p_data == NULL)
			{
				printf("task_id:%u, out_item %u error", p_client_task->task_id,  p_fix_data->id);
				return false;
			}
			p_fix_data->give_type = p_data->give_type;
		}

		for(unsigned int i =0; i< p_client_task->rewards_data.select.size(); i++)
		{
			selected_data* p_select_data = &p_client_task->rewards_data.select[i];
			//////////////找到服务器task与之匹配的out_item项///////////////////////
			out_item_data* p_data = p_server_task->get_out_by_item_id(p_select_data->id);
			if(p_data == NULL)
			{
				printf("task_id:%u, out_item %u error", p_client_task->task_id,  p_select_data->id);
				return false;
			}
			p_select_data->give_type = p_data->give_type;

		}
	}
	return true;
}