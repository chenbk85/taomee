#include <libtaomee/project/utilities.h>
#include "kill_boss.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "global_data.hpp"
#include "player.hpp"
#include "login.hpp"
#include "ring_task.hpp"
using namespace std;
using namespace taomee;


static int load_player_kill_boss_data(player_t* p,  get_kill_boss_list_rsp_t* rsp);


int load_player_kill_boss_data(player_t* p,  get_kill_boss_list_rsp_t* rsp)
{
	for(uint32_t i =0; i< rsp->count; i++)
	{
		kill_boss_element* pdata = &(rsp->datas[i]);
		add_player_kill_boss_data(p, pdata->stage_id, pdata->boss_id, pdata->kill_time, pdata->pass_cnt);	
	}
	return 0;
}



bool init_player_kill_boss_data(player_t* p)
{
	p->m_kill_boss_datta_maps = new std::map<uint32_t, kill_boss_data>();
	return true;
}
bool final_player_kill_boss_data(player_t* p)
{
	delete p->m_kill_boss_datta_maps;
	p->m_kill_boss_datta_maps = NULL;
	return true;
}

bool add_player_kill_boss_data(player_t* p, kill_boss_data* pdata)
{
	bool ret = is_player_kill_boss_data_exist(p, pdata->stage_id_, pdata->diffcult_);
	if(ret)return false;

	uint32_t id = pdata->diffcult_*10000 + pdata->stage_id_;	
	(*(p->m_kill_boss_datta_maps))[ id ] = *pdata;
	return true;
}

bool add_player_kill_boss_data(player_t* p, uint32_t stage_id, uint32_t boss_id, uint32_t kill_time,uint32_t pass_cnt)
{
	kill_boss_data data;
	data.stage_id_ = stage_id %10000;
	data.diffcult_ = stage_id /10000;
	data.boss_id_  = boss_id;
	data.kill_time_ = kill_time;
	data.pass_cnt_ = pass_cnt;
	return add_player_kill_boss_data(p, &data);
}

bool is_player_finish_stage(player_t* p, uint32_t stage_id, uint32_t diffcult)
{
    uint32_t id = 0;
    for (uint32_t i = diffcult; i <= 6; i++) {
        id = i * 10000 + stage_id;
        if (p->m_kill_boss_datta_maps->find(id) != p->m_kill_boss_datta_maps->end()) {
            return true;
        }
    }
    return false;
}

bool is_player_get_stage_score(player_t* p, uint32_t stage_id, uint32_t diff, uint32_t score)
{
    uint32_t cnt = 0;
    for (uint32_t i = diff; i > 0; i--) {
        uint32_t player_score = find_player_stage_grade(p, stage_id, i);
        if (player_score != 0 && player_score <= score) {
            //return true;
            cnt++;
        }
    }
    if (cnt == diff) return true;
    return false;
}

int is_player_kill_boss_data_exist(player_t* p, uint32_t stage_id, uint32_t diffcult)
{
	uint32_t id = diffcult*10000 + stage_id;
	return p->m_kill_boss_datta_maps->find(id) != p->m_kill_boss_datta_maps->end();
}

int is_player_kill_boss_data_exist(player_t* p, uint32_t stage_id)
{
	return p->m_kill_boss_datta_maps->find(stage_id) != p->m_kill_boss_datta_maps->end();
}

kill_boss_data* get_player_kill_boss_data(player_t* p, uint32_t stage_id, uint32_t diffcult)
{
	uint32_t id = diffcult*10000 + stage_id;
	std::map<uint32_t, kill_boss_data>::iterator pItr = p->m_kill_boss_datta_maps->find(id);
	if(pItr == p->m_kill_boss_datta_maps->end()){
		return NULL;
	}
	return &(pItr->second);
}

uint32_t get_player_kill_boss_count(player_t* p, uint32_t diffcult)
{
	uint32_t count = 0;
	std::map<uint32_t, kill_boss_data>::iterator pItr = p->m_kill_boss_datta_maps->begin();
	for(; pItr !=  p->m_kill_boss_datta_maps->end(); ++pItr)
	{
		kill_boss_data* pdata = &(pItr->second);
		if( pdata->diffcult_ == diffcult && pdata->stage_id_ < 900){
			count++;
		}
	}
	return 0;
}

int db_kill_boss_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_kill_boss_list, 0, 0);
}

int db_update_kill_boss_list(player_t* p,  uint32_t stage_id, 
		uint32_t boss_id, uint32_t kill_time, uint32_t pass_cnt)
{
	int idx = 0;
	pack_h(dbpkgbuf, stage_id, idx);
	pack_h(dbpkgbuf, boss_id, idx);
	pack_h(dbpkgbuf, kill_time, idx);
	pack_h(dbpkgbuf, pass_cnt, idx);
	return send_request_to_db(NULL, p->id, p->role_tm,  dbproto_replace_kill_boss, dbpkgbuf, idx);
}

int db_get_kill_boss_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_kill_boss_list_rsp_t *rsp = reinterpret_cast<get_kill_boss_list_rsp_t*>(body); 
	CHECK_VAL_EQ(bodylen, sizeof(get_kill_boss_list_rsp_t) + rsp->count*sizeof(kill_boss_element));
	load_player_kill_boss_data(p, rsp);	
	return db_get_ring_task_list(p);
}

