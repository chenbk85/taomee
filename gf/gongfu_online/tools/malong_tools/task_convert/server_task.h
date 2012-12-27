#ifndef _TASK_H_
#define _TASK_H_

#include <vector>
#include <map>
using namespace std;

typedef struct out_item_data
{
	out_item_data()
	{
		memset(this, 0, sizeof(out_item_data));
	}
	int give_type;
	int role_type;
	char name[1024];
	int give_id;
	int count;
}out_item_data;

typedef struct out_items
{
	out_items()
	{
		id = 0;
	}
	out_item_data* get_by_item_id(unsigned int item_id)
	{
		for(unsigned int i =0; i< datas.size(); i++)
		{
			out_item_data* p_data = &datas[i];
			if(p_data->give_id == item_id)
			{
				return p_data;
			}
		}
		return NULL;
	}
	int id;
	std::vector<out_item_data> datas;
}out_items;


typedef struct in_item_data
{
	in_item_data()
	{
		memset(this, 0, sizeof(in_item_data));
	}
	int give_type;
	unsigned int give_id;
	char name[1024];
	int count;
	int monster_id;
	int stage;
	int drop_odds;
}in_item_data;

typedef struct in_items
{
	in_items()
	{
		id = 0;
	}	
	in_item_data* get_by_item_id(unsigned int item_id)
	{
		for(unsigned int i =0; i< datas.size(); i++)
		{
			if(datas[i].give_id == item_id)
			{
				return &datas[i];
			}
		}
		return NULL;
	}

	int id;
	std::vector<in_item_data> datas;
}in_items;

typedef struct task_data
{
	task_data()
	{
		task_id = 0;
		memset(name, 0, sizeof(name));
		type = 0;
		need_role = 0;
		vip_limit = 0;
		diffculty = 0;
		master_task_id = 0;
	}
	in_item_data* get_in_by_item_id(unsigned int item_id)
	{
		in_item_data* p_data = NULL;
		std::map<int,  in_items>::iterator pItr = in_maps.begin();
		for(; pItr != in_maps.end(); ++pItr)
		{
			in_items* p_in = &(pItr->second);
			p_data = p_in->get_by_item_id(item_id);
			if(p_data)
			{
				return p_data;
			}
		}
		return NULL;
	}
	out_item_data* get_out_by_item_id(unsigned int item_id)
	{
		out_item_data* p_data = NULL;
		std::map<int,  out_items>::iterator pItr = out_maps.begin();
		for(; pItr != out_maps.end(); ++pItr)
		{
			out_items* p_out = &(pItr->second);
			p_data = p_out->get_by_item_id(item_id);
			if(p_data)
			{
				return p_data;
			}
		}
		return NULL;

	}


	int task_id;
	char name[1024];
	int type;
	char need_level;
	int  need_role;
	int  vip_limit;
	int  diffculty;
	int  master_task_id;
	std::map<int, out_items> out_maps;
	std::map<int,  in_items> in_maps;
}task_data;

bool load_server_task(const char* xml_name,  std::map<int, task_data>& task_datas);


#endif





