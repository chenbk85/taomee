#ifndef _MAP_DATA_HPP
#define _MAP_DATA_HPP

#include <boost/intrusive/list.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
}

#include "pea_common.hpp"
#include "fwd_decl.hpp"

#include <vector>
#include <map>
using namespace std;


typedef struct map_npc_info
{
	map_npc_info()
	{
		memset(this, 0, sizeof(*this));	
	}
	uint32_t npc_id;
	uint32_t x;
	uint32_t y;
}map_npc_info;

typedef struct map_transport_info
{
	map_transport_info()
	{
		memset(this, 0, sizeof(*this));
	}
	uint32_t port_id;
	uint32_t x;
	uint32_t y;
	uint32_t to_map_id;
	uint32_t to_map_x;
	uint32_t to_map_y;
}map_transport_info;


typedef struct  map_data
{
	map_data()
	{
		map_id = 0;
		memset(map_name, 0, sizeof(map_name));
		born_x = 0;
		born_y = 0;
	}

	bool add_npc_info(map_npc_info* info)
	{
		map<uint32_t, map_npc_info*>::iterator pItr = map_npcs.find(info->npc_id);
		if(pItr == map_npcs.end())return false;
		map_npcs[info->npc_id] = info;
		return true;
	}

	bool add_transport_info(map_transport_info* info)
	{
		map<uint32_t, map_transport_info*>::iterator pItr = map_ports.find(info->port_id);	
		if(pItr == map_ports.end())return false;
		map_ports[info->port_id] = info;
		return true;
	}

	bool final()
	{
		map<uint32_t, map_npc_info*>::iterator pItr = map_npcs.begin();
		
		for(; pItr != map_npcs.end(); ++pItr)
		{
			map_npc_info* info = pItr->second;
			if(info == NULL)continue;
			delete info;
			info = NULL;
		}
	
		map<uint32_t, map_transport_info*>::iterator pItr2 = map_ports.begin();
		
		for(; pItr2 != map_ports.end(); ++pItr2)
		{
			map_transport_info* info2 = pItr2->second;
			if(info2 == NULL)continue;
			delete info2;
			info2 = NULL;
		}

	
		return true;
	}
	uint32_t map_id;
	char     map_name[1024];
	uint32_t born_x;
	uint32_t born_y;
	map<uint32_t, map_npc_info*> map_npcs;
	map<uint32_t, map_transport_info*> map_ports;
}map_data;

class map_data_mgr
{
private:
	map_data_mgr(){}
	~map_data_mgr(){}
public:
	static map_data_mgr* get_instance();
	bool   init_xml(const char* xml);
	bool   final();
public:
	bool add_map_data(map_data* data);
	bool is_map_data_exist(uint32_t map_id);
	map_data* get_map_data_by_id(uint32_t map_id);
public:
	map<uint32_t, map_data*>  datas;
};

#endif
