#ifndef _CLIENT_TASK_H_
#define _CLIENT_TASK_H_

#include <vector>
#include <map>
using namespace std;

typedef struct selected_data
{
	selected_data()
	{
		memset(this, 0, sizeof(selected_data));
	}
	int id;
	int role;
	char name[1024];
	int count;
	int give_type;
}selected_data;

typedef struct normal_data
{
	normal_data()
	{
		memset(this, 0, sizeof(normal_data));	
	}
	int exp;
	int cash;
}normal_data;

typedef struct rewards
{
	std::vector<selected_data>  select;
	std::vector<selected_data>  fix;
 	normal_data				    normal;
}rewards;


typedef struct pro_data
{
	pro_data()
	{
		memset(this, 0, sizeof(pro_data));
	}
	int id;
	int parent;
	int visible;
	char doc[1024];
	char proc[1024];
	char params[1024];
	char tran[1024];
	char alert[1024];
	//////
	int stage;
	unsigned int monster_id;
	int give_type;
	int drop_odds;

}pro_data;

typedef struct condition
{
	std::vector<pro_data> condition_datas;
}condition;


typedef struct client_task
{
	client_task()
	{
		task_id = 0;
		vip_limit = 0;
		master_task_id = 0;
		memset(name, 0, sizeof(name));
		parent = 0;
		type = 0;
		memset(alert, 0, sizeof(alert));
		int task_level = 0;
		int role_level = 0;
		memset(role_type, 0, sizeof(role_type));
		memset(start, 0, sizeof(start));
		memset(end, 0, sizeof(end));
		memset(start_tran, 0, sizeof(start_tran));
		memset(end_tran, 0, sizeof(end_tran));
		next_accept = 0;
		show_accept_animation = 0;
		memset(doc, 0, sizeof(doc));
		memset(end_doc, 0, sizeof(end_doc));
	}

	int  task_id;
	char name[1024];
	int  parent;
	int  type;
	char alert[1024];
	int  task_level;
	int  role_level;
	char role_type[1024];
	char start[1024];
	char end[1024];
	char start_tran[1024];
	char end_tran[1024];
	int  next_accept;
	int  show_accept_animation;
	int  vip_limit;
	int  master_task_id;

	condition      condition_data;
	rewards        rewards_data;

	char doc[1024];
	char end_doc[1024];
}client_task;

bool load_client_task(const char* xml_name,  std::map<int, client_task>& task_datas);

bool save_client_task(const char* xml_name,  std::map<int, client_task>& task_datas);

#endif