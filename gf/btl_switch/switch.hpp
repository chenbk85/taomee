// =====================================================================================
//
//       Filename:  switch.hpp
// 
//    	 Description: define the neccessary switch related vriables and dll interface
// 
//       Version:  1.0
//       Created:  03/11/2009 08:25:18 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================

#ifndef  SWITCH_HPP_
#define  SWITCH_HPP_
extern "C"
{
#include <stdint.h>
#include <libtaomee/project/types.h>
}

/**
  * @brief Define the size of package
  */
enum
{
    pkg_size    = 8192
};

/**
  * @brief Define base home online id
  */
enum 
{
	home_online_base = 5001
};


enum
{
	cli_get_version		 = 30001,
	///////////////////for online server///////////////////////
	proto_online_svrinfo = 60001,
	proto_user_onoff     = 61002,
	proto_room_list      = 61003,
	proto_auto_join_room = 61004,
	proto_auto_join_pvp_room = 61005,

	proto_auto_join_trade_room = 61006,
	proto_check_change_trade_room = 61007,	
	proto_get_ap_toplist = 61008,
	proto_contest_pvp_room_info = 61009,

	proto_auto_join_contest = 61010,
	proto_list_contest_group = 61011,
	proto_leave_contest_group = 61012,
	proto_contest_auto_msg  = 61013,
	proto_contest_enter_room_msg = 61014,
	proto_contest_contest_btl = 61015,
	proto_contest_all_over = 61016,
	proto_player_win_contest = 61017,
	proto_guess_contest_champion = 61018,
	proto_list_all_contest_player = 61019,
	proto_list_passed_contest_info = 61020,

	proto_team_contest_auto_join = 61031,
	proto_team_contest_get_server_id = 61032,
//	proto_contest_over = 61015,

	////////////////////for battle server///////////////////////
	proto_battle_svrinfo = 62001,
	proto_create_room    = 62002,
	proto_destroy_room   = 62003,
	proto_join_room      = 62004,
	proto_leave_room     = 62005,
	proto_clear_all_rooms = 62006,
	proto_room_hot_join   = 62007,
	proto_team_room_start = 62008,


	proto_create_pvp_room = 63001,
	proto_destroy_pvp_room = 63002,
	proto_join_pvp_room = 63003,
	proto_leave_pvp_room = 63004,
	proto_pvp_room_start = 63005,
	proto_pvp_btl_over  = 63006,

	proto_consume_item	 = 63101,
	proto_reset_global_data = 63102,



//	proto_contest_pvp_room_info = 61009,


	///////////////////for trade server///////////////////////
	proto_trade_svrinfo   = 64001,
	proto_join_trade_room = 64002,
	proto_leave_trade_room = 64003,
	proto_change_trade_room = 64004,

    ////////////////////for hunter top////////////////////////////////
    proto_get_hunter_top_list = 65001,
    proto_set_hunter_top_list = 65002

};


#pragma pack(1)
/**
  * @brief Define the communication protocol between the switch
  *        and online server(or adminer server, login server) 
  */
struct svr_proto_t
{
	uint32_t    len; 
    uint32_t    seq; 
    uint16_t    cmd; 
    uint32_t    ret; 
    userid_t    sender_id; 
    uint8_t     body[];
};

#pragma pack()


// =====================================================================================
/**
  * @brief Set the package head
  * @param buf package buffer
  * @param uid user id of the sender
  * @param cmd command to be excuted
  * @param ret return of error number
  * @param seq sequence number
  */
inline void
init_proto_head(void* buf, userid_t uid, uint32_t len, uint16_t cmd, uint32_t ret, uint32_t seq)
{
    svr_proto_t* header = reinterpret_cast<svr_proto_t*>(buf);
    header->len = len;
    header->seq = seq;
    header->cmd = cmd;
    header->ret = ret;
    header->sender_id = uid;
}

#endif   // ----- #end SWITCH_HPP_  ----- 

