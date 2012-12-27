#ifndef _TRADE_H_
#define _TRADE_H_

#include <stdint.h>
#include <stdio.h>
#include <map>
#include <list>
#include <algorithm>
extern "C"
{
#include <arpa/inet.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <string.h>
}
#include "switch.hpp"


#define MAX_TRADE_ROOM_COUNT (420)
#define MAX_ROOM_PLAYER_COUNT (25)

typedef struct trade_room_info
{
	trade_room_info()
	{
		room_id_ = 0;
		player_count_ = 0;
	}
	void add_player_id(uint32_t player_id)
	{
		//player_id_list_.push_back(player_id);
		player_count_++;
	}
	void del_player_id(uint32_t player_id)
	{
		//std::list<uint32_t>::iterator pItr = std::find( player_id_list_.begin(),  player_id_list_.end(), player_id);
		//if(pItr != player_id_list_.end()){
			//player_id_list_.erase(pItr);
		//}
		player_count_--;
	}
	bool is_player_id_exist(uint32_t player_id)
	{
		//return std::find( player_id_list_.begin(),  player_id_list_.end(), player_id) != player_id_list_.end();
		return true;
	}
	bool check_player_limit()
	{
		//return player_id_list_.size() < MAX_ROOM_PLAYER_COUNT ;
		return player_count_ < MAX_ROOM_PLAYER_COUNT;
	}
	uint32_t  room_id_;
	//std::list<uint32_t> player_id_list_; 
	uint32_t  player_count_;
}trade_room_info;

typedef struct trade_info_t
{
	trade_info_t()
	{
		trade_svr_id_ = 0;
		room_list_ = NULL;
	}
	uint32_t trade_svr_id_;
	trade_room_info* room_list_;
}trade_info_t;

bool init_trade_info(uint32_t trade_svr_id, trade_info_t* p);
bool final_trade_info(trade_info_t* p);
void reset_trade_info(trade_info_t* p);


bool init_trade_info_group();
bool final_trade_info_group();
bool add_trade_info_to_group(uint32_t trade_svr_id);
bool del_trade_info_from_group(uint32_t trade_svr_id);
trade_info_t* get_trade_info_by_id(uint32_t trade_svr_id);




class Trade
{
public:
	static std::map<int, int> trade_fd_maps; //trade fd --->trade id 
public:
	static bool init();
	static bool final();
	static int  get_trade_id_by_fd(int fd);	
public:
	static int  report_trade_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  clear_trade_info(int fd);

	static int  join_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  leave_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  change_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  check_change_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  auto_join_trade_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
};


bool player_join_trade_room(uint32_t player_id, uint32_t trade_svr_id, int32_t trade_room_index);
bool player_leave_trade_room(uint32_t player_id, uint32_t trade_svr_id, int32_t trade_room_index);
bool player_change_trade_room(uint32_t player_id, uint32_t old_trade_svr_id, int32_t old_room_index, uint32_t new_trade_svr_id,  int32_t new_room_index);

bool check_player_change_trade_room(uint32_t player_id, uint32_t old_trade_svr_id, int32_t old_room_index, uint32_t new_trade_svr_id, int32_t new_room_index);

bool find_trade_room_by_condition(uint32_t player_id,  uint32_t& trade_svr_id, int32_t& trade_room_index);
bool find_trade_room_by_condition(uint32_t player_id, uint32_t  trade_svr_id,  int32_t begin_room_index, int32_t end_room_index,  int32_t& new_room_index);
#endif


