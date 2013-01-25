/*
 * =========================================================================
 *
 *        Filename: pvp.h
 *
 *        Version:  1.0
 *        Created:  2011-10-18 17:46:26
 *        Description:  
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  PVP_H
#define  PVP_H
#include    "onlinehome.h"
#include <libtaomee++/proto/proto_base.h>
#include  <set>
#include  <map>
#include  <list>

struct stru_user_score_info {
	uint32_t userid;
	//uint32_t level;
	char nick[16];
	uint32_t score;
	uint32_t is_alive;
};

class Cgame_item {
	private:

	public:
		//对战超时：游戏对战开始结束时间，对应end_time_homeid_map
		uint32_t start_time;
		uint32_t end_time;

		//等待其他用户进入游戏超时：用户长时间没有进来，对应no_entry_time_homeid_map
		uint32_t add_user_time_out_and_start_game_time;

		//未进入地图超时：已经有足够人数，但是有人没有进来，对应wait_time_out_homeid_map
		uint32_t time_out_and_start_game_time;
		
		mapid_t homeid;
		uint32_t opt_level;
		//已收到mapid但还未进入地图的人
		//std::set<userid_t> need_userid_set;
		//保存积分等信息
		std::map<userid_t, stru_user_score_info> user_score_map;
		//实际进入的人
		std::set<userid_t> userid_set;
};

/*struct  stru_pre_pvp_game_item{
	uint32_t userid;
	uint32_t add_time;
};*/

struct stru_pre_user_info {
	uint32_t opt_level;
	uint32_t onlineid;
};


class Cpvp {
	private:
		static const uint32_t opt_level_count=20;
		static const uint32_t level_divider = 5; //差距在5级之内的都可以加入pvp


		static const uint32_t total_battle_timeout = 60*20;//对战超时
		static const uint32_t battle_timeout_sync = 60;//对战超时：每60秒同步一次对战超时
		
		static const uint32_t no_entry_timeout = 120;//等待其他用户进入游戏超时

		static const uint32_t wait_timeout = 20;//未进入地图超时

		std::map<mapid_t,Cgame_item>homeid_game_map;

		//对战超时：用于搜索超时游戏(对战结束时间超时)
		std::multimap <uint32_t,mapid_t  > end_time_homeid_map;

		//等待其他用户进入游戏超时：一直没有人进入游戏超时
		std::multimap <uint32_t,mapid_t  > no_entry_time_homeid_map;

		//未进入地图超时：已匹配人数足够，但有人没有进入地图
		std::multimap <uint32_t,mapid_t  > wait_time_out_homeid_map;

		//userid->用户信息
		std::map<uint32_t, stru_pre_user_info> user_info_map;
		//用于产生mapid
		uint32_t next_dupid;

		mapid_t games[opt_level_count];

		void start_new_game(Cgame_item& game_item);
		void do_battle_time_out_game(mapid_t homeid);
		void do_no_entry_time_out_game(mapid_t homeid);
		void do_wait_time_out_game(mapid_t homeid);
		void add_user_to_user_info_map(userid_t uid,
			char nick[16], uint32_t onlineid);

		void send_msg_to_all_users(Cmessage* msg,mapid_t homeid,
			uint32_t opid);
		void send_msg_to_a_user(Cmessage* msg,mapid_t homeid,
			userid_t uid, uint32_t opid);
		void send_noti_pvp_winner(mapid_t homeidi,
			userid_t winner, userid_t loser);

		void print_userid_sets(mapid_t homeid);

	public:
		uint32_t game_user_count;//一场游戏的人数
		Cpvp(){
			memset(games, '\0', sizeof(games) * sizeof(mapid_t));
		}
		//Cgame_item * get_game(mapid_t homeid);

		//void add_game(mapid_t homeid,Cgame_item &  game);
		//void check_start_game();
		void check_time_outs();

		void del_user( mapid_t homeid,userid_t userid, userid_t rival_winner);
		//进入游戏地图了
		void cd_map( mapid_t homeid,uint32_t userid );
		
		void get_map_id(userid_t uid, char nick[16],
			uint32_t level, uint32_t onlineid,uint32_t pvp_type);	
		bool is_in(uint32_t uid);
};


#endif  /*PVP_H*/

