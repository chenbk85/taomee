#ifndef _RING_TASK_H_
#define _RING_TASK_H_

#include<libtaomee++/random/random.hpp>
#include"fwd_decl.hpp"
#include <vector>
#include <map>
using namespace std;
using namespace taomee;

/*
typedef struct task_pair
{
	task_pair()
	{
		slave_task_id = 0;
		index = 0;
	}
	uint32_t slave_task_id;
	uint32_t index;
}task_pair;
*/

typedef struct slave_task_group
{
	slave_task_group()
	{
		group_id = 0;
		rate = 0;
	}
	int find_slave_index(uint32_t task_id)
	{
		for(uint32_t i=0; i< slave_task_ids.size(); i++)
		{
			if( slave_task_ids[i] == task_id)return i;
		}
		return -1;
	}	
	uint32_t group_id;
	uint32_t rate;
	std::vector<uint32_t> slave_task_ids;
}slave_task_group;


typedef struct ring_task_data
{
	ring_task_data()
	{
		master_task_id = 0;
		slave_factor = 0;
	}
	~ring_task_data()
	{
		std::map<uint32_t,  slave_task_group*>::iterator pItr = m_groups.begin();
		for(; pItr != m_groups.end(); ++pItr)
		{
			slave_task_group* p_group = pItr->second;
			delete p_group;
		}
	}
	bool add_task_group(slave_task_group* group)
	{
		if(is_task_group_exist(group->group_id))return false;
		m_groups[group->group_id] = group;
		m_group_ids.push_back(group->group_id);
		return true;
	}
	uint32_t rand_get_group_id()
	{
		if(m_group_ids.size() == 0)return 0;
		int index = ranged_random (0, m_group_ids.size()- 1);
		return m_group_ids[index];
	}
	bool is_task_group_exist(uint32_t group_id)
	{
		return m_groups.find(group_id) != m_groups.end();
	}
	slave_task_group* get_task_group_by_id(uint32_t group_id)
	{
		std::map<uint32_t,  slave_task_group*>::iterator pItr = m_groups.find(group_id);
		if(pItr == m_groups.end())return NULL;
		return pItr->second;
	}
	uint32_t master_task_id;
	uint32_t slave_factor ;
	std::map<uint32_t,  slave_task_group*> m_groups;
	std::vector<uint32_t>  m_group_ids;	
}ring_task_data;



class ring_task_mgr
{
public:
	bool init(const char* xml);
	bool final();
	ring_task_data* get_ring_task_data_by_id(uint32_t task_id);
	bool add_ring_task_data(ring_task_data* data);
	bool is_ring_task_data_exist(uint32_t task_id);
private:
	std::map<uint32_t,  ring_task_data*> m_data_maps;
};


typedef struct player_ring_task_data
{
	player_ring_task_data()
	{
		task_id = 0;
		task_type  = 0;
		master_task_id = 0;
		task_group_id = 0;
	}
	uint32_t task_id;         //任务ID
	uint32_t task_type;       //任务类型5:大环任务 6:大环子任务
	uint32_t master_task_id;  //task_type = 5时，该字段无意义， task_type = 6时，该字段表示对应的大环任务ID
	uint32_t task_group_id;   //task_type = 5时，该字段表示大环任务下小环子任务所属的任务组ID，task_type = 6时，该字段无意义
}player_ring_task_data;

typedef struct player_ring_task_history_data
{
	player_ring_task_history_data()
	{
		task_id = 0;
		day_count = 0;
		last_tm = 0;
	}
	bool is_time_in_same_day(time_t cur_tm)
	{
		return last_tm / (60*60*24) == cur_tm / (60*60*24);
	}
	uint32_t task_id;
	uint32_t day_count;
	uint32_t last_tm;
}player_ring_task_history_data;


bool init_player_ring_task(player_t* p);
bool final_player_ring_task(player_t* p);


int get_master_ring_task_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int send_player_master_ring_task_count(player_t* p);

uint32_t  random_gen_slave_group_id(player_t*p,  uint32_t task_id);
player_ring_task_data* get_player_ring_task(player_t*p,  uint32_t task_id);
bool add_player_ring_task(player_t* p, uint32_t task_id, uint32_t type, uint32_t master_task_id, uint32_t task_group_id);
bool del_player_ring_task(player_t* p, uint32_t task_id, uint32_t type);
bool del_player_ring_task(player_t* p, uint32_t task_id);
bool is_player_ring_task_exist(player_t* p, uint32_t task_id);
bool finish_player_ring_task(player_t* p, uint32_t task_id);

bool add_player_ring_task_history(player_t* p, uint32_t task_id, uint32_t op_time);
uint32_t get_player_ring_task_day_count(player_t* p, uint32_t task_id);
player_ring_task_history_data* get_player_ring_task_history(player_t*p,  uint32_t task_id);


bool is_master_ring_task(uint32_t task_id);
bool is_slave_ring_task(uint32_t task_id);
uint32_t get_factor_by_task_id(player_t* p, uint32_t task_id);

int get_treasure_task_cnt(player_t * p); 


////////////////////db interface////////////////////////////
typedef struct get_ring_task_list_rsp_t
{
	uint32_t count;
	player_ring_task_data datas[];
}get_ring_task_list_rsp_t;

int db_get_ring_task_list(player_t* p);
int db_get_ring_task_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

typedef struct get_ring_task_history_list_rsp_t
{
	uint32_t count;
	player_ring_task_history_data datas[];
}get_ring_task_history_list_rsp_t;

int db_get_ring_task_history_list(player_t* p);
int db_get_ring_task_history_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/////////以下三个函数没有DB  CALLBACK//////////////////////////
int db_replace_ring_task_data(player_t* p, player_ring_task_data* p_data);
int db_delete_ring_task_data(player_t*p,  uint32_t task_id, uint32_t task_type);
int db_replace_ring_task_history_data(player_t* p, player_ring_task_history_data* p_data);

#endif




