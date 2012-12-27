#include <libtaomee/project/utilities.h>
#include "use_item_history.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "global_data.hpp"
#include "player.hpp"
#include "login.hpp"
using namespace std;
using namespace taomee;


bool init_player_use_item_data(player_t* p)
{
	p->m_use_item_data_maps = new std::map<uint32_t, use_item_data>();
	return true;
}

bool final_player_use_item_data(player_t* p)
{
	delete p->m_use_item_data_maps;
	p->m_use_item_data_maps = NULL;
	return true;
}


bool add_player_use_item_data(player_t*p,  uint32_t item_id, uint32_t item_count)
{
	std::map<uint32_t, use_item_data>::iterator pItr = p->m_use_item_data_maps->find(item_id);
	if(pItr == p->m_use_item_data_maps->end()){
		use_item_data data;
		data.item_id = item_id;
		data.item_count = item_count;
		data.last_tm = time(NULL);
		(*p->m_use_item_data_maps)[item_id] = data;
	}else {
		use_item_data* pdata= &(pItr->second);
		pdata->item_count += item_count;
		pdata->last_tm = time(NULL);
	}
	return true;
}

bool is_player_item_data_exist(player_t*p, uint32_t item_id)
{
	return p->m_use_item_data_maps->find(item_id) != p->m_use_item_data_maps->end();
}

use_item_data* get_player_use_item_data(player_t* p, uint32_t item_id)
{
	std::map<uint32_t, use_item_data>::iterator pItr = p->m_use_item_data_maps->find(item_id);
	if(pItr == p->m_use_item_data_maps->end())return NULL;
	return &(pItr->second);
}

uint32_t   get_player_use_item_data_times(player_t* p, uint32_t item_id)
{
	use_item_data* pdata = get_player_use_item_data(p, item_id);
	if(pdata == NULL)return 0;
	return pdata->item_count;
}

int  load_player_use_item_data(player_t* p, get_use_item_list_rsp_t* rsp)
{
	for(uint32_t i=0; i< rsp->count; i++)
	{
		use_item_data* pdata = &(rsp->datas[i]);
		(*(p->m_use_item_data_maps))[pdata->item_id] = *pdata;
	}
	return 0;
}

int db_get_use_item_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_use_item_list , 0, 0);
}
int db_replace_use_item(player_t* p, uint32_t item_id, uint32_t item_count, uint32_t last_tm)
{
	int idx = 0;
	pack_h(dbpkgbuf, item_id, idx);
	pack_h(dbpkgbuf, item_count, idx);
	pack_h(dbpkgbuf, last_tm, idx);
	return send_request_to_db(NULL, p->id, p->role_tm,  dbproto_replace_use_item, dbpkgbuf, idx);
}

int db_get_use_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_use_item_list_rsp_t* rsp = reinterpret_cast<get_use_item_list_rsp_t*>(body);
	load_player_use_item_data(p, rsp);	
	return 0;
}


