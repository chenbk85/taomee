// =====================================================================================
//
//       Filename:  online.hpp
// 
//    	 Description: specify the communication between online and switch 
// 
//       Version:  1.0
//       Created:  03/11/2009 08:35:48 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================


#ifndef SWITCH_ONLINE_HPP_
#define SWITCH_ONLINE_HPP_

#include <map>
#include <set>
#include <fstream>
#include <list>
#include <libtaomee++/memory/mempool.hpp>

using namespace std;

extern "C" 
{
#include <arpa/inet.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <string.h>
#include <glib.h>
}

#include "switch.hpp"

#define MAX_ONLINE_INFO_COUNT 2000
#define MAX_NICK_SIZE 16
extern const uint32_t MAX_PLAYER_INFO_COUNT; 

inline bool is_vaild_online_id(uint32_t online_id)
{
	if (online_id > 0 && online_id < MAX_ONLINE_INFO_COUNT) {
		return true;
	}
	return false;
}

enum pvp_lv_t {
	pvp_lv_0 = 0,
	pvp_lv_1 = 1,
	pvp_lv_2 = 2,
    pvp_monster_ = 3,
    pvp_monster_game = 4,
    pvp_monster_practice = 5,
	pvp_contest_advance = 6,
	pvp_contest_advance_practive = 7,
    pvp_contest_final = 8,
	pvp_16_contest = 9,

	pvp_red_blue_1 = 10,
	pvp_red_blue_2 = 11,
	pvp_red_blue_3 = 12,

	pvp_summon_mode = 14,
	pvp_dragon_ship = 23,
    pvp_lv_max,
};



extern  uint8_t       s_pkg_[pkg_size];
/** 
  * @brief online server related information 
  */
typedef struct online_info_t 
{
	online_info_t()
	{
		domain = 0;
		online_id = 0;
		memset(online_ip, 0, sizeof(online_ip));
		online_port = 0;
		fdsess = NULL;
		player_id_map = new std::map<uint32_t, uint32_t>;
	}

	~online_info_t()
	{
		delete player_id_map;
	}

	uint16_t	domain;
	uint32_t	online_id;
	char		online_ip[16];
	in_port_t	online_port;
	std::map<uint32_t, uint32_t> *  player_id_map;
	fdsession_t*    fdsess;
} online_info_t;

struct player_info_t {
	userid_t	usrid;
	uint32_t	role_tm;
	uint32_t	online_id;
	uint32_t    battle_svr_id;
	int32_t     room_index;
	uint32_t    btl_mode;
	uint32_t    stage_id;
	uint32_t    diffcult;
	uint32_t    pvp_lv;
	uint64_t    session_id; //英雄盛典的唯一标识符
	int32_t     trade_room_index;
	uint32_t    trade_svr_id;
	uint32_t    seq;
	fdsession_t * fdsess;
};

player_info_t * alloc_new_player(uint32_t id, 
								 uint32_t tm, 
								 uint32_t type, 
								 uint32_t online);

void init_player_info(player_info_t * p);
void free_player_info(void * p);


// =====================================================================================

class Online 
{
public:
	static online_info_t*    p_online_infos;
	static std::map<int, int> online_fd_maps; //online fd --->online id

	static GHashTable * p_player_infos;

	static std::map<int, int> battle_fd_maps; //battle fd --->battle id	


public:
	static void init();
	static void final();


	static int  get_online_id_by_fd(int fd);
	static bool is_guest(uint32_t userid);
	static player_info_t * add_player( uint32_t user_id, uint32_t role_tm, uint32_t role_type, uint32_t online_id);
	static int  del_player(player_info_t * p_info);
	static player_info_t* get_player_info(uint32_t user_id);
	static bool           check_player_online(uint32_t user_id);
	
	/***************** Communication with Online Server  *************************/
	static int  report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  clear_online_info(int fd);

	static void  remove_player_from_online_map(uint32_t online_id, uint32_t user_id);

	static int  report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	
	static int  room_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  auto_join_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int  auto_join_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int  get_hunter_top_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  get_ap_toplist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	/***************** Communication with Battle Server  *************************/
	static int report_battle_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int clear_battle_info(int fd);

	static int report_create_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_destroy_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_join_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_leave_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int clear_all_rooms(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_room_hot_join(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_team_room_start(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
		
	static int report_create_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_destroy_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_join_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_leave_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int report_pvp_room_start(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int pvp_btl_over(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);

	static int get_contest_pvp_room_info(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);

	static int set_hunter_top_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int auto_join_16_hero_contest(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
	static int leave_16_hero_contest(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
	static int list_16_hero_contest_info(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);

	static int list_16_hero_passed_contest(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);

//	static int contest_group_btl(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
	static int list_all_contest_player(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);  
	static int guess_contest_champion(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
	static int team_contest_auto_join(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
	static int team_contest_get_server_id(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);


	//other
	static int consume_item(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
	static int reset_global_data(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);

	//version 
	static int get_get_version(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess);
};


#endif // SWITCH_ONLINE_HPP_

