#include "buy_item_limit.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "cli_proto.hpp"
#include "player.hpp"
#include "login.hpp"
#include "kill_boss.hpp"
#include "home_impl.hpp"
using namespace taomee;

/*
buy_item_limit_mgr* buy_item_limit_mgr::getInstance()
{
	static buy_item_limit_mgr obj;
	return &obj;
}
*/


bool buy_item_limit_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr limit_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the buy_item_limit xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the buy_item_limit xml file content is empty"));
		return false;
	}

	uint32_t item_id = 0;
	uint32_t limit_type = 0;
	uint32_t limit_count = 0;
	
	limit_node = root->xmlChildrenNode;
	while(limit_node)
	{
		get_xml_prop_def( item_id,  limit_node,  "item_id", 0);
		get_xml_prop_def( limit_type, limit_node,  "limit_type", 0);
		get_xml_prop_def( limit_count, limit_node,  "limit_count", 0);		
		if( item_id == 0 || limit_count == 0 || limit_type == 0){
			throw XmlParseError(std::string("the limit data error!"));
			return false;
		}
	
		if(  is_buy_item_limit_exist(item_id) ){
			throw XmlParseError(std::string("the limit item_id has existed!"));
			return false;
		}

		buy_item_limit_data* pdata = new buy_item_limit_data();
		pdata->item_id = item_id;
		pdata->item_limit_type = limit_type;
		pdata->item_limit_count = limit_count;
		
		add_buy_item_limit_data(pdata);
		
		limit_node = limit_node->next;
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool buy_item_limit_mgr::final()
{
	std::map<uint32_t, buy_item_limit_data*>::iterator pItr = m_data_map.begin();
	for( ; pItr != m_data_map.end(); ++pItr)
	{
		buy_item_limit_data* pdata = pItr->second;
		if(pdata == NULL)continue;
		delete pdata;
		pdata = NULL;
	}
	return true;
}


buy_item_limit_data* buy_item_limit_mgr::get_limit_data_by_item_id(uint32_t item_id)
{
	std::map<uint32_t, buy_item_limit_data*>::iterator pItr = m_data_map.find(item_id);
	if( pItr == m_data_map.end())return NULL;
	return pItr->second;
}

bool buy_item_limit_mgr::add_buy_item_limit_data( buy_item_limit_data* pdata )
{
	if( pdata == NULL ||  is_buy_item_limit_exist( pdata->item_id) )return false;
	m_data_map[pdata->item_id] = pdata;
	return false;
}

bool buy_item_limit_mgr::is_buy_item_limit_exist(uint32_t item_id)
{
	std::map<uint32_t, buy_item_limit_data*>::iterator pItr = m_data_map.find(item_id);
	return pItr != m_data_map.end();
}


bool init_player_buy_item_limit_data(player_t* p)
{
	p->m_limit_data_maps = new std::map<uint32_t,  player_item_limit_data>();
	return true;
}


bool final_player_buy_item_limit_data(player_t* p)
{
	delete p->m_limit_data_maps;
	p->m_limit_data_maps = NULL;
	return true;
}

player_item_limit_data* get_player_item_limit_data_by_item_id(player_t* p, uint32_t item_id)
{
	std::map<uint32_t,  player_item_limit_data>::iterator pItr = p->m_limit_data_maps->find(item_id);
	if(pItr == p->m_limit_data_maps->end())return NULL;
	return &(pItr->second);
}

bool check_player_buy_item_limit_data( buy_item_limit_mgr* mgr,  player_t* p, uint32_t item_id, uint32_t item_count)
{
	buy_item_limit_data* pdata = mgr->get_limit_data_by_item_id(item_id);
	if(pdata == NULL)return true;
	
	player_item_limit_data* ppdata = get_player_item_limit_data_by_item_id(p, item_id);
	if(ppdata == NULL){
		if( pdata->item_limit_count >= item_count){
			return true;
		}else {
			return false;
		}
	}

	if( ppdata->check_data_expiry()){
		if( pdata->item_limit_count >= item_count){
			return true;
		}else{
			return false;
		}
	}

	if( (ppdata->item_count + item_count) <= pdata->item_limit_count){
		return true;
	}
	return false;
}


bool add_player_buy_item_limit_data(buy_item_limit_mgr* mgr,  player_t* p, uint32_t item_id, uint32_t item_count)
{
	buy_item_limit_data* pdata = mgr->get_limit_data_by_item_id(item_id);
	if( pdata == NULL)return true;
	
	player_item_limit_data* ppdata = get_player_item_limit_data_by_item_id(p, item_id);
	if(ppdata == NULL){
		player_item_limit_data data;
		data.item_id = item_id;
		data.item_limit_type = pdata->item_limit_type;
		data.last_time = time(NULL);
		data.item_count = item_count;
		(*(p->m_limit_data_maps))[item_id] = data;
		return true;
	}
		
	if( ppdata->check_data_expiry() ){
		ppdata->item_count = item_count;
		ppdata->last_time = time(NULL);
		return true;
	}
	
	ppdata->item_count += item_count;
	ppdata->last_time = time(NULL);
	return true;	
}

bool init_player_buy_item_limit_list(player_t* p, get_buy_item_limit_list_rsp_t* rsp)
{
	for( uint32_t i = 0; i < rsp->count; i++)
	{
		player_item_limit_data* pdata = &rsp->data[i];
		(*(p->m_limit_data_maps))[pdata->item_id] = *pdata;
	}
	return true;
}


int get_buy_item_limit_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return send_buy_item_limit_list(p);
}

int send_buy_item_limit_list(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	int total_count = 0;
	int total_count_idx = idx;
	pack(pkgbuf, total_count, idx);

	std::map<uint32_t,  player_item_limit_data>::iterator pItr = p->m_limit_data_maps->begin();
	for(;  pItr != p->m_limit_data_maps->end(); ++pItr)
	{
		player_item_limit_data* pdata = &(pItr->second);
		if( pdata->check_data_expiry())continue;
		pack(pkgbuf,  pdata->item_id, idx);
		pack(pkgbuf,  pdata->item_count, idx);
		total_count ++;
	}
	pack(pkgbuf, total_count, total_count_idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


///////////////////////////////////////db interface///////////////////////////////
int db_get_buy_item_limit_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_buy_item_limit_list, 0, 0);
}

int db_get_buy_item_limit_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_buy_item_limit_list_rsp_t* rsp = reinterpret_cast<get_buy_item_limit_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_buy_item_limit_list_rsp_t) + rsp->count*sizeof(player_item_limit_data));
	init_player_buy_item_limit_list(p, rsp);
	if ( p->waitcmd == cli_proto_login ){
		return db_get_player_home_data(p);
		//return db_kill_boss_list(p);
	}
	return 0;
}

int db_set_buy_item_limit_data(player_t* p, player_item_limit_data* pdata)
{
	int idx = 0;
	pack_h(dbpkgbuf, pdata->item_id, idx);
	pack_h(dbpkgbuf, pdata->item_limit_type, idx);
	pack_h(dbpkgbuf, pdata->last_time, idx);
	pack_h(dbpkgbuf, pdata->item_count, idx);

	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_buy_item_limit_data, dbpkgbuf, idx);
}
