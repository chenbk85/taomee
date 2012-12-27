#include "ring_task.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "cli_proto.hpp"
#include "global_data.hpp"
#include "player.hpp"
#include "login.hpp"
using namespace taomee;

static bool init_player_ring_task_list(player_t*p, get_ring_task_list_rsp_t* rsp);
static bool init_player_ring_task_history_list(player_t* p, get_ring_task_history_list_rsp_t* rsp);

bool ring_task_mgr::init(const char* xml)
{
	if(xml == NULL)return false;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr master_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the ring task xml file is not exist"));
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
    if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the ring task xml file content is empty"));
		return false;						    
	}
	slave_task_group malong;

	master_node = root->xmlChildrenNode;
	while(master_node)
	{
		ring_task_data* p_data = new ring_task_data();
		get_xml_prop_def(p_data->master_task_id,  master_node, "taskId", 0);
		get_xml_prop_def(p_data->slave_factor,    master_node, "factor", 0);
		const task_t* p_task = get_task(p_data->master_task_id);
		if(p_task == NULL)
		{
			char err[1024] = { 0 };
			sprintf(err, "the task_id:%u is not exist ring_task", p_data->master_task_id);
			throw XmlParseError(std::string(err));
			return false;
		}	


		xmlNodePtr group_node = master_node->xmlChildrenNode;	
		while( group_node )
		{
			slave_task_group* p_group = new slave_task_group();
			get_xml_prop_def(p_group->group_id, group_node, "group", 0);
			get_xml_prop_def(p_group->rate, group_node, "rate", 0);

			xmlNodePtr 	slave_node = group_node->xmlChildrenNode;
			while(slave_node)
			{
				uint32_t slave_task_id = 0;
				get_xml_prop_def(slave_task_id, slave_node, "taskId", 0);
				const task_t* p_task = get_task(slave_task_id);
				if(p_task == NULL)
				{
					char err[1024] = {0};
					sprintf(err, "the task_id:%u is not exist ring_task", slave_task_id);
					throw XmlParseError(std::string(err));
					return false;
				}
				
				p_group->slave_task_ids.push_back(slave_task_id);
				slave_node = slave_node->next;
			}	
			p_data->add_task_group(p_group);
			group_node = group_node->next;
		}	
		get_ring_task_mgr()->add_ring_task_data(p_data);
		master_node = master_node->next;
	}
	return true;	
}

bool ring_task_mgr::final()
{
	std::map<uint32_t,  ring_task_data*>::iterator pItr = m_data_maps.begin();
	for(; pItr != m_data_maps.end(); ++pItr)
	{
		ring_task_data* pdata = pItr->second;
		delete pdata;
	}
	return true;
}

int get_master_ring_task_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return send_player_master_ring_task_count(p);
}

int send_player_master_ring_task_count(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	int idx2 = idx;
	int count = 0;
	std::map<uint32_t, player_ring_task_history_data>::iterator pItr = p->m_ring_task_history_maps->begin();
	uint32_t cur_time = time(NULL);
	
	pack(pkgbuf, count, idx);
	for(;  pItr != p->m_ring_task_history_maps->end(); ++pItr)
	{
		player_ring_task_history_data* p_data = &(pItr->second);
		if( !p_data->is_time_in_same_day(cur_time))	continue;
		pack(pkgbuf,  p_data->task_id, idx);
		pack(pkgbuf,  p_data->day_count, idx);
		count++;
	}	
	pack(pkgbuf, count, idx2);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

ring_task_data*  ring_task_mgr::get_ring_task_data_by_id(uint32_t task_id)
{
	std::map<uint32_t,  ring_task_data*>::iterator pItr = m_data_maps.find(task_id);
	if(pItr == m_data_maps.end())return NULL;
	return pItr->second;
}

bool ring_task_mgr::add_ring_task_data(ring_task_data* data)
{
	if(is_ring_task_data_exist(data->master_task_id)){
		return false;
	}
	m_data_maps[data->master_task_id] = data;
	return true;
}

bool ring_task_mgr::is_ring_task_data_exist(uint32_t task_id)
{
	return m_data_maps.find(task_id) != m_data_maps.end();
}

bool init_player_ring_task(player_t* p)
{
	p->m_ring_task_maps = new map<uint32_t, player_ring_task_data>();
	p->m_ring_task_history_maps = new map<uint32_t, player_ring_task_history_data>();
	return true;
}
 
bool final_player_ring_task(player_t* p)
{
	delete p->m_ring_task_maps ;
	p->m_ring_task_maps = NULL;

	delete p->m_ring_task_history_maps;
	p->m_ring_task_history_maps = NULL;
	return true;
}


bool add_player_ring_task(player_t* p, uint32_t task_id, uint32_t type, uint32_t master_task_id, uint32_t task_group_id)
{
	if(is_player_ring_task_exist(p, task_id))return false;
	player_ring_task_data data;		
	data.task_id = task_id;
	data.task_type = type;
	data.master_task_id = master_task_id;
	data.task_group_id  =  task_group_id;
	(*p->m_ring_task_maps)[task_id] = data;
	return true;
}

bool del_player_ring_task(player_t* p, uint32_t task_id)
{
	map<uint32_t, player_ring_task_data>::iterator pItr = p->m_ring_task_maps->find(task_id);
	if(pItr != p->m_ring_task_maps->end()){
		p->m_ring_task_maps->erase(pItr);
		return true;
	}
	return false;
}

bool del_player_ring_task(player_t* p, uint32_t task_id, uint32_t type)
{
	if(type == slave_ring_task){
		return del_player_ring_task(p, task_id);
	}
	else if(type == master_ring_task)
	{
		ring_task_data* p_ring_data = get_ring_task_mgr()->get_ring_task_data_by_id(task_id);
		if(p_ring_data == NULL)return false;
		player_ring_task_data* pdata = get_player_ring_task(p, task_id);
		if(pdata == NULL)return false;
		slave_task_group* p_group = p_ring_data->get_task_group_by_id(pdata->task_group_id);
		if(p_group == NULL)return false;
		//delete master task
		del_player_ring_task(p, task_id);
		//delete slave task belong to master
		for(uint32_t i =0; i< p_group->slave_task_ids.size(); i++)
		{
			uint32_t slave_task_id = p_group->slave_task_ids[i];
			del_player_ring_task(p, slave_task_id);
		}
	}
	return true;
}

bool is_player_ring_task_exist(player_t* p, uint32_t task_id)
{
	map<uint32_t, player_ring_task_data>::iterator pItr = p->m_ring_task_maps->find(task_id);
	return pItr != p->m_ring_task_maps->end();	
}


player_ring_task_data* get_player_ring_task(player_t*p,  uint32_t task_id)
{
	map<uint32_t, player_ring_task_data>::iterator pItr = p->m_ring_task_maps->find(task_id);
	if(pItr == p->m_ring_task_maps->end())return NULL;
	return &(pItr->second);
}

uint32_t  random_gen_slave_group_id(player_t*p,  uint32_t task_id)
{
	ring_task_data* p_data = get_ring_task_mgr()->get_ring_task_data_by_id(task_id);
	if(p_data == NULL)return 0;
	return p_data->rand_get_group_id();	
}

bool finish_player_ring_task(player_t* p, uint32_t task_id)
{
	player_ring_task_data* pdata = get_player_ring_task(p, task_id);
	if(pdata == NULL)return false;
	del_player_ring_task(p, task_id, pdata->task_type);
	uint32_t cur_time = time(NULL);
	add_player_ring_task_history(p, task_id, cur_time);
	return true;
}

bool add_player_ring_task_history(player_t* p, uint32_t task_id, uint32_t op_time)
{
	std::map<uint32_t, player_ring_task_history_data>::iterator pItr = p->m_ring_task_history_maps->find(task_id);
	if(pItr == p->m_ring_task_history_maps->end())
	{
		player_ring_task_history_data data;
		data.task_id = task_id;
		data.day_count = 1;
		data.last_tm = op_time;
		(*p->m_ring_task_history_maps)[task_id] = data;
	}
	else
	{
		time_t cur_tm = time(NULL);
		player_ring_task_history_data* pdata = &pItr->second;
		if( pdata->is_time_in_same_day(cur_tm))
		{
			pdata->day_count ++;
			pdata->last_tm = op_time;
		}
		else
		{
			pdata->day_count = 1;
			pdata->last_tm = op_time;
		}	
	}
	return true;
}
uint32_t get_player_ring_task_day_count(player_t* p, uint32_t task_id)
{
	std::map<uint32_t, player_ring_task_history_data>::iterator pItr = p->m_ring_task_history_maps->find(task_id);
	if(pItr == p->m_ring_task_history_maps->end())return 0;
	time_t cur_tm = time(NULL);
	player_ring_task_history_data* pdata = &pItr->second;
	if( !pdata->is_time_in_same_day(cur_tm))return 0;
	return pdata->day_count;
}

player_ring_task_history_data* get_player_ring_task_history(player_t*p,  uint32_t task_id)
{
	std::map<uint32_t, player_ring_task_history_data>::iterator pItr = p->m_ring_task_history_maps->find(task_id);
	if(pItr == p->m_ring_task_history_maps->end()) return NULL;
	return &(pItr->second);
}

uint32_t get_factor_by_task_id(player_t* p, uint32_t task_id)
{
	/*
	if( is_master_ring_task(task_id) )
	{
		ring_task_data* pdata = get_ring_task_mgr()->get_ring_task_data_by_id(task_id);
		if(pdata == NULL)return 0;
		return pdata->master_task_id;
	}
	*/
	if( is_slave_ring_task(task_id))
	{
		player_ring_task_data* p_user_data =  get_player_ring_task(p, task_id);
		if(p_user_data == NULL)return 0;
		ring_task_data* pdata = get_ring_task_mgr()->get_ring_task_data_by_id( p_user_data->master_task_id);
		if(pdata == NULL)return 0;
		p_user_data = get_player_ring_task(p, p_user_data->master_task_id);
		if(p_user_data == NULL) return 0;
		slave_task_group* p_group = pdata->get_task_group_by_id(p_user_data->task_group_id);
		if(p_group == NULL)return 0;
		int index = p_group->find_slave_index(task_id);
		if(index == -1)return 0;
		return index * pdata->slave_factor;
	}
	return 0;
}


bool is_master_ring_task(uint32_t task_id)
{
	const  task_t* p_task = get_task(task_id);
	if(p_task == NULL)return false;
	return p_task->type == master_ring_task;
}
bool is_slave_ring_task(uint32_t task_id)
{
	const  task_t* p_task = get_task(task_id);
    if(p_task == NULL)return false;
    return p_task->type == slave_ring_task;	
}

///////////////////////////////////db interface///////////////////////////////////////////
bool init_player_ring_task_list(player_t* p, get_ring_task_list_rsp_t* rsp)
{
	for(uint32_t i =0; i< rsp->count; i++)
	{
		player_ring_task_data* p_data = &(rsp->datas[i]);
		(*p->m_ring_task_maps)[p_data->task_id] = *p_data;
	}
	return true;
}

int db_get_ring_task_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_get_ring_task_list, 0, 0);
}

int db_get_ring_task_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_ring_task_list_rsp_t* rsp = reinterpret_cast< get_ring_task_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_ring_task_list_rsp_t) + rsp->count*sizeof(player_ring_task_data));
	init_player_ring_task_list(p, rsp);
	return db_get_ring_task_history_list(p);
}

bool init_player_ring_task_history_list(player_t* p, get_ring_task_history_list_rsp_t* rsp)
{
	for(uint32_t i =0; i< rsp->count; i++)
	{
		uint32_t cur_time = get_now_tv()->tv_sec;
		player_ring_task_history_data* p_data = &(rsp->datas[i]);

		const task_t * tsk = get_task(p_data->task_id);

		if (tsk->type == treasure_task && !p_data->is_time_in_same_day(cur_time)) {
			continue;
		} else {
			(*p->m_ring_task_history_maps)[p_data->task_id] = *p_data;
		}
	}
	return true;
}

int db_get_ring_task_history_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_get_ring_task_history_list, 0, 0);
}

int db_get_ring_task_history_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_ring_task_history_list_rsp_t* rsp = reinterpret_cast< get_ring_task_history_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_ring_task_history_list_rsp_t) + rsp->count*sizeof(player_ring_task_history_data));
	init_player_ring_task_history_list(p, rsp);
	return db_get_swap_action(p);
}


int db_replace_ring_task_data(player_t* p, player_ring_task_data* p_data)
{
	int idx = 0;
	pack_h(dbpkgbuf, p_data->task_id, idx);
	pack_h(dbpkgbuf, p_data->task_type, idx);
	pack_h(dbpkgbuf, p_data->master_task_id, idx);
	pack_h(dbpkgbuf, p_data->task_group_id, idx);
	return send_request_to_db(NULL, p->id, p->role_tm,  dbproto_replace_ring_task, dbpkgbuf, idx);
}
int db_delete_ring_task_data(player_t*p,  uint32_t task_id, uint32_t task_type)
{
	int idx = 0;
	pack_h(dbpkgbuf, task_id, idx);
	pack_h(dbpkgbuf, task_type, idx);
	return send_request_to_db(NULL, p->id, p->role_tm,  dbproto_delete_ring_task , dbpkgbuf, idx);
}

int db_replace_ring_task_history_data(player_t* p, player_ring_task_history_data* p_data)
{
	int idx = 0;
	pack_h(dbpkgbuf, p_data->task_id, idx);
	pack_h(dbpkgbuf, p_data->day_count, idx);
	pack_h(dbpkgbuf, p_data->last_tm, idx);
	return send_request_to_db(NULL, p->id, p->role_tm,  dbproto_replace_ring_task_history , dbpkgbuf, idx);
}

int get_treasure_task_cnt(player_t * p) 
{
	int cnt = 0;
	std::map<uint32_t, player_ring_task_history_data>::iterator pItr = p->m_ring_task_history_maps->begin();
	
	while (pItr != p->m_ring_task_history_maps->end()) {
		const task_t * tsk = get_task(pItr->first);
		if (tsk->type == treasure_task) {
			time_t cur_tm = time(NULL);
			player_ring_task_history_data* pdata = &pItr->second;
			if (pdata->is_time_in_same_day(cur_tm)) {
				cnt += pdata->day_count;
			}
		}
		++pItr;

	}
	return cnt;
}


