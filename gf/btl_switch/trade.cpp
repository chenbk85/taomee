#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include "trade.hpp"
#include <map>
#include "utils.hpp"
#include "online.hpp"
using namespace std;
using namespace taomee;

static std::map<uint32_t, trade_info_t*> trade_info_map;

bool init_trade_info(uint32_t trade_svr_id, trade_info_t* p)
{
	p->trade_svr_id_ = trade_svr_id;
	p->room_list_ = new trade_room_info[MAX_TRADE_ROOM_COUNT];
	return true;
}

bool final_trade_info(trade_info_t* p)
{
	p->trade_svr_id_ = 0;
	delete [] p->room_list_;
	p->room_list_ = NULL;
	return true;
}


bool init_trade_info_group()
{
	return true;
}

void reset_trade_info(trade_info_t* p)
{
	
	for(uint32_t i =0; i< MAX_TRADE_ROOM_COUNT; i++)
	{
		trade_room_info* info = &(p->room_list_[i]);
		info->player_count_ = 0;
		/*
		std::list<uint32_t>::iterator pItr = info->player_id_list_.begin();
		for( ; pItr != info->player_id_list_.end(); ++pItr)
		{
			uint32_t player_id = (*pItr);
			player_info_t *pInfo = Online::get_player_info(player_id);
			if(pInfo)
			{
				pInfo->trade_svr_id = 0;
				pInfo->trade_room_index = -1;
			}
		}
		info->player_id_list_.clear();
		*/
	}
		
}

bool final_trade_info_group()
{
	std::map<uint32_t, trade_info_t*>::iterator pItr = trade_info_map.begin();
	for(; pItr != trade_info_map.end(); ++pItr)
	{
		trade_info_t* p = pItr->second;
		if( p ){
			final_trade_info(p);
			delete p;
			p = NULL;
		}
	}
	trade_info_map.clear();
	return true;
}

bool add_trade_info_to_group(uint32_t trade_svr_id)
{
	if( trade_info_map.find(trade_svr_id) == trade_info_map.end()){
		trade_info_t * p = new trade_info_t();
		init_trade_info(trade_svr_id, p);
		trade_info_map[trade_svr_id] = p;
	}
	return true;
}

bool del_trade_info_from_group(uint32_t trade_svr_id)
{
	std::map<uint32_t, trade_info_t*>::iterator pItr = trade_info_map.find(trade_svr_id);
	if(pItr == trade_info_map.end())return false;
	
	trade_info_t* p = pItr->second;
	if( p ){
		final_trade_info(p);
		delete p;
		p = NULL;
	}
	trade_info_map.erase( pItr );
	return true;
}

trade_info_t* get_trade_info_by_id(uint32_t trade_svr_id)
{
	std::map<uint32_t, trade_info_t*>::iterator pItr = trade_info_map.find(trade_svr_id);
	if( pItr == trade_info_map.end() ) return NULL;
	return pItr->second;
}

//////////////////////////class trade///////////////////////////////////

std::map<int, int> Trade::trade_fd_maps;



bool Trade::init()
{
	init_trade_info_group();
	trade_fd_maps.clear();
	return true;
}

bool Trade::final()
{
	final_trade_info_group();
	trade_fd_maps.clear();
	return true;
}

int Trade::report_trade_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct trade_info_req_t
	{
		uint32_t    trade_svr_id;	
		uint32_t    player_count[MAX_TRADE_ROOM_COUNT];
	}__attribute__((packed));
	
	CHECK_VAL(bodylen, sizeof(trade_info_req_t));
    trade_info_req_t* req = reinterpret_cast<trade_info_req_t*>(pkg->body);
    add_trade_info_to_group(req->trade_svr_id);
    trade_fd_maps[fdsess->fd] = req->trade_svr_id;

	trade_info_t* info = get_trade_info_by_id( req->trade_svr_id );
	if(info == NULL){
		DEBUG_LOG("report trade info fail trade_svr_id = %u", req->trade_svr_id);
		return 0;
	}
	for(uint32_t i =0; i < MAX_TRADE_ROOM_COUNT; i++)
	{
		info->room_list_[i].player_count_ = req->player_count[i];
	}	
	DEBUG_LOG("report trade info ok trade_svr_id = %u", req->trade_svr_id);
	return 0;
}

int  Trade::clear_trade_info(int fd)
{
	std::map<int, int>::iterator pItr = trade_fd_maps.find(fd);
	if(pItr == trade_fd_maps.end()){
		return -1;
	}
	uint32_t trade_svr_id = pItr->second;
	trade_info_t* info =  get_trade_info_by_id(trade_svr_id);
	reset_trade_info(info);
	trade_fd_maps.erase(pItr);	
	return 0;
}


int  Trade::join_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct join_trade_req_t
	{
		uint32_t trade_svr_id;
		int32_t room_index;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(join_trade_req_t));
	join_trade_req_t* req = reinterpret_cast<join_trade_req_t*>(pkg->body);

	player_join_trade_room(pkg->sender_id, req->trade_svr_id, req->room_index);
	return 0;
}
int  Trade::leave_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct leave_trade_req_t
	{
		uint32_t trade_svr_id;
		int32_t room_index;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(leave_trade_req_t));
	leave_trade_req_t* req = reinterpret_cast<leave_trade_req_t*>(pkg->body);

	player_leave_trade_room(pkg->sender_id, req->trade_svr_id, req->room_index);		
	return 0;
}

int  Trade::change_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct change_trade_req_t
	{
		uint32_t old_trade_svr_id;
		int32_t old_room_index;
		uint32_t new_trade_svr_id;
		int32_t new_room_index;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(change_trade_req_t));
	change_trade_req_t* req = reinterpret_cast<change_trade_req_t*>(pkg->body);

	player_change_trade_room(pkg->sender_id, req->old_trade_svr_id, req->old_room_index, req->new_trade_svr_id, req->new_room_index);
	return 0;
}


int Trade::check_change_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct check_change_trade_req_t
	{
		uint32_t old_trade_svr_id;
		int32_t old_room_index;
		uint32_t new_trade_svr_id;
		int32_t new_room_index;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(check_change_trade_req_t));
	check_change_trade_req_t* req = reinterpret_cast<check_change_trade_req_t*>(pkg->body);

	bool ret = check_player_change_trade_room(pkg->sender_id, req->old_trade_svr_id, req->old_room_index, req->new_trade_svr_id, req->new_room_index);

	if( !ret){	
		uint32_t svr_id = 0;
		int32_t room_index = -1;
		ret = find_trade_room_by_condition(pkg->sender_id, svr_id, room_index);
		req->new_trade_svr_id = svr_id;
		req->new_room_index = room_index;	
	}

	int idx  = sizeof(svr_proto_t);
	pack_h(s_pkg_,  req->old_trade_svr_id, idx);
	pack_h(s_pkg_,  req->old_room_index, idx);
	pack_h(s_pkg_,  ret ? req->new_trade_svr_id : 0 , idx);
	pack_h(s_pkg_,  ret ? req->new_room_index : -1, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}



int Trade::auto_join_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct auto_join_trade_room_req_t
	{
		uint32_t trade_svr_id;
		int32_t  begin_room_index;
		int32_t  end_room_index;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(auto_join_trade_room_req_t));
	auto_join_trade_room_req_t* req = reinterpret_cast<auto_join_trade_room_req_t*>(pkg->body);

	uint32_t trade_svr_id = req->trade_svr_id;
	int32_t  trade_room_index = -1;

	DEBUG_LOG("begin auto join trade room on trade_svr = %u, player_id = %u",  req->trade_svr_id,  pkg->sender_id);
	find_trade_room_by_condition(pkg->sender_id, 
			                     req->trade_svr_id,  
								 req->begin_room_index,
								 req->end_room_index, 
								 trade_room_index);
	DEBUG_LOG("end auto join trade room on trade_svr = %u, player_id = %u",  req->trade_svr_id,  pkg->sender_id);
	
	if(trade_room_index == -1){
		DEBUG_LOG("begin auto join trade room player_id = %u", pkg->sender_id);
		find_trade_room_by_condition(pkg->sender_id, trade_svr_id, trade_room_index);
		DEBUG_LOG("end auto join trade room player_id = %u", pkg->sender_id);
	}

	int idx  = sizeof(svr_proto_t);
	pack_h(s_pkg_, trade_svr_id, idx);
	pack_h(s_pkg_, trade_room_index, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}




bool player_join_trade_room(uint32_t player_id, uint32_t trade_svr_id, int32_t trade_room_index)
{
	//player_info_t *pInfo = Online::get_player_info(player_id);
	//if(pInfo == NULL)return false;
	//if(pInfo->trade_svr_id != 0 || pInfo->trade_room_index != -1)return false;	

	if(  !(trade_room_index >= 0 && trade_room_index < MAX_TRADE_ROOM_COUNT) )return false;
	trade_info_t* info =  get_trade_info_by_id(trade_svr_id);
	if(info == NULL)return false;

	info->room_list_[trade_room_index].add_player_id(player_id);
	//pInfo->trade_svr_id = trade_svr_id;
	//pInfo->trade_room_index = trade_room_index;
	return true;
}

bool player_leave_trade_room(uint32_t player_id, uint32_t trade_svr_id, int32_t trade_room_index)
{
	//player_info_t *pInfo = Online::get_player_info(player_id);
	//if(pInfo == NULL)return false;
	//if(pInfo->trade_svr_id == 0 || pInfo->trade_room_index == -1) return false;

	if(  !(trade_room_index >= 0 && trade_room_index < MAX_TRADE_ROOM_COUNT) )return false;
	trade_info_t* info =  get_trade_info_by_id(trade_svr_id);
	if(info == NULL)return false;
	info->room_list_[trade_room_index].del_player_id(player_id);
	//pInfo->trade_svr_id = 0;
	//pInfo->trade_room_index = -1;
	return true;
}

bool player_change_trade_room(uint32_t player_id, uint32_t old_trade_svr_id, int32_t old_room_index, uint32_t new_trade_svr_id,  int32_t new_room_index)
{
	if(old_room_index == new_room_index && old_trade_svr_id == new_trade_svr_id ) return true;
	//player_info_t *pInfo = Online::get_player_info(player_id);
	//if(pInfo == NULL)return false;
	//if(pInfo->trade_svr_id != old_trade_svr_id ||  pInfo->trade_room_index != old_room_index) return false;

	if(  !(old_room_index >= 0 && old_room_index < MAX_TRADE_ROOM_COUNT) )return false;
	if(  !(new_room_index >= 0 && new_room_index < MAX_TRADE_ROOM_COUNT) )return false;

	trade_info_t* info =  get_trade_info_by_id(old_trade_svr_id);
	info->room_list_[old_room_index].del_player_id(player_id);
	
	info = get_trade_info_by_id(new_trade_svr_id);
	if(info == NULL)return false;
	info->room_list_[new_room_index].add_player_id(player_id);

	//pInfo->trade_room_index = new_room_index;
	//pInfo->trade_svr_id =  new_trade_svr_id;
	return true;
}


bool check_player_change_trade_room(uint32_t player_id, uint32_t old_trade_svr_id, int32_t old_room_index, uint32_t new_trade_svr_id, int32_t new_room_index)
{
	if(old_room_index == new_room_index && old_trade_svr_id == new_trade_svr_id ) return true;
	//player_info_t *pInfo = Online::get_player_info(player_id);
	//if(pInfo == NULL)return false;
	//if(pInfo->trade_svr_id != old_trade_svr_id ||  pInfo->trade_room_index != old_room_index) return false;
	if(  !(new_room_index >= 0 && new_room_index < MAX_TRADE_ROOM_COUNT) )return false;

	trade_info_t* info =  get_trade_info_by_id(new_trade_svr_id);
	if(info == NULL)return false;
	if(  !info->room_list_[new_room_index].check_player_limit() )return false;
	return true;
}


bool find_trade_room_by_condition(uint32_t player_id,  uint32_t& trade_svr_id, int32_t& trade_room_index)
{
	trade_svr_id = 0;
	trade_room_index = -1;
	//player_info_t *pInfo = Online::get_player_info(player_id);
	//if( pInfo == NULL)return false;

	std::map<uint32_t, trade_info_t*>::iterator pItr = trade_info_map.begin();
	for(; pItr != trade_info_map.end(); ++pItr)
	{
		trade_info_t* info = pItr->second;
		if(info == NULL)continue;
		for( uint32_t j = 0; j < MAX_TRADE_ROOM_COUNT; j++)
		{
			if( info->room_list_[j].check_player_limit() ){
				trade_svr_id = info->trade_svr_id_;
				trade_room_index = j;
				return true;
			}
		}
	}

	return true;
}

bool find_trade_room_by_condition(uint32_t player_id, uint32_t  trade_svr_id,  int32_t begin_room_index, int32_t end_room_index, int32_t& new_room_index)
{
	new_room_index = -1;

	//player_info_t *pInfo = Online::get_player_info(player_id);
	//if(pInfo == NULL)return false;
	
	trade_info_t* info =  get_trade_info_by_id(trade_svr_id);
	if(info == NULL)return false;

	if( !(begin_room_index >= 0 && begin_room_index < MAX_TRADE_ROOM_COUNT) )return false;
	if( !(end_room_index >= 0 && end_room_index < MAX_TRADE_ROOM_COUNT)) return false;
	if(  end_room_index <= begin_room_index ) return false;

	for( int32_t i = begin_room_index; i < end_room_index; i++)
	{
		if(  info->room_list_[i].check_player_limit()){
			new_room_index = i;
			return true;
		}
	}

	return true;
}


