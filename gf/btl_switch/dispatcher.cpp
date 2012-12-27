#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" 
{
#include <stdint.h>
#include <libtaomee/log.h>
#include <async_serv/dll.h>
#include <libtaomee/project/utilities.h>
}


#include "online.hpp"
#include "switch.hpp"
#include "dispatcher.hpp"
#include "trade.hpp"
//#include "hunter_top.hpp"

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[5500];

/**
  * @brief initialize processing functions
  */
void init_funcs()
{
	final_funcs();
	funcs[ proto_online_svrinfo - 60001 ] = Online::report_online_info; 
	funcs[ proto_user_onoff     - 60001 ] = Online::report_user_onoff;
	
	funcs[ proto_battle_svrinfo - 60001 ] = Online::report_battle_info;
	funcs[ proto_create_room    - 60001 ] = Online::report_create_room; 
	funcs[ proto_destroy_room   - 60001 ] = Online::report_destroy_room;
	funcs[ proto_join_room      - 60001 ] = Online::report_join_room;
	funcs[ proto_leave_room     - 60001 ] = Online::report_leave_room;
	funcs[ proto_clear_all_rooms - 60001 ] = Online::clear_all_rooms;
	funcs[ proto_auto_join_room  - 60001 ] = Online::auto_join_room;
	funcs[ proto_room_hot_join   - 60001 ] = Online::report_room_hot_join;
	funcs[ proto_team_room_start - 60001 ] = Online::report_team_room_start;
	funcs[ proto_pvp_btl_over - 60001 ]  = Online::pvp_btl_over;

	funcs[ proto_create_pvp_room - 60001 ] = Online::report_create_pvp_room;
	funcs[ proto_destroy_pvp_room - 60001 ] = Online::report_destroy_pvp_room;
	funcs[ proto_join_pvp_room - 60001 ] = Online::report_join_pvp_room;
	funcs[ proto_auto_join_pvp_room - 60001 ] = Online::auto_join_pvp_room;
	funcs[ proto_leave_pvp_room     - 60001 ] = Online::report_leave_pvp_room;
	funcs[ proto_pvp_room_start    - 60001  ] = Online::report_pvp_room_start;
	funcs[proto_auto_join_contest - 60001] = Online::auto_join_16_hero_contest;
	funcs [proto_contest_pvp_room_info - 60001] = Online::get_contest_pvp_room_info;
	funcs [proto_list_contest_group - 60001 ] = Online::list_16_hero_contest_info;
	funcs [proto_leave_contest_group - 60001] = Online::leave_16_hero_contest;
//	funcs [proto_contest_contest_btl - 60001] = Online::contest_group_btl;

	funcs [proto_list_all_contest_player - 60001] = Online::list_all_contest_player;

	funcs [proto_list_passed_contest_info - 60001] = Online::list_16_hero_passed_contest;

	funcs [proto_guess_contest_champion - 60001] =  Online::guess_contest_champion;

	//other
	funcs [proto_consume_item - 60001] = Online::consume_item;
	funcs [proto_reset_global_data - 60001] = Online::reset_global_data;

	funcs[ proto_trade_svrinfo    - 60001  ] = Trade::report_trade_info;
	funcs[ proto_join_trade_room  - 60001  ] = Trade::join_trade_room;
	funcs[ proto_leave_trade_room - 60001  ] = Trade::leave_trade_room;
	funcs[ proto_check_change_trade_room - 60001 ] = Trade::check_change_trade_room;
	funcs[ proto_auto_join_trade_room - 60001 ] = Trade::auto_join_trade_room;

	//team contest
	funcs[proto_team_contest_auto_join - 60001] = Online::team_contest_auto_join;
	funcs[proto_team_contest_get_server_id - 60001] = Online::team_contest_get_server_id;



	funcs[ proto_get_ap_toplist - 60001 ] = Online::get_ap_toplist;
    //funcs[ proto_get_hunter_top_list - 60001 ] = Hunter_top::get_hunter_top_list;
    //funcs[ proto_set_hunter_top_list - 60001 ] = Hunter_top::set_hunter_top_list;
} 

void final_funcs()
{
	for(uint32_t i =0; i< sizeof(funcs)/sizeof(funcs[0]); i++)
	{
		funcs[i] = NULL;
	}
}
/**
  * @brief dispatch package to the corresponding processing functions
  * @param data package data
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
	uint32_t cmd = pkg->cmd - 60001;

	if (pkg->cmd == cli_get_version) {
		int idx = sizeof(svr_proto_t);
		char tmpbuf[255] = "20110810";
		pack(s_pkg_, tmpbuf, sizeof(tmpbuf), idx);
		init_proto_head(s_pkg_, 0, idx, cli_get_version, 0, 0);
		TRACE_LOG("get version %s", tmpbuf);
		return send_pkg_to_client(fdsess, s_pkg_, idx);
	}

//	if (is_common_player_cmd(pkg->cmd))


	if ((cmd > 5499) || (funcs[cmd] == 0)) 
	{
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return -1;
	}
	
	int ret =  funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
	return ret;
}

