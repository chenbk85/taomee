/*
 * =========================================================================
 *
 *        Filename: Cgame.h
 *
 *        Version:  1.0
 *        Created:  2011-08-31 07:59:52
 *        Description:  
 *
 *        Author:  xxx (xxxj@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */


#ifndef  CGAME_H
#define  CGAME_H
#include "util.h" 
#include "Ctimeval.h"
#include  "./proto/pop_online.h" 
class Cgame_user_info {
	private:

	public:
		uint32_t	userid;
		bool		can_start;	//是否可以开始
		uint32_t	opt_count;	//操作的次数

		// 跳伞小游戏的数据
		TimeVal		jump_time;
		TimeVal		open_time;
		int			open_y;
		int			open_v;
		uint32_t	state;		//抢跳、加速、降落
};
enum enum_game_stat{
	GAME_START,	
	GAME_OPT,	
	GAME_END,	
	ENUM_GAME_STAT_END,	
};

class Cgame_item {
	private:
		enum enum_game_stat game_stat;//游戏状态
		uint32_t gameid;
		uint32_t start_userid;
		uint32_t cur_opt_userid;
		uint32_t step_timeout;//每一步的超时时间
		uint32_t start_time;
		uint32_t game_map_id;
		std::map<uint32_t , Cgame_user_info> user_map;
		bool game_user_opt_deal(uint32_t userid, cli_noti_game_opt_out* p_out, cli_game_opt_in* p_in);
		
		uint32_t timer_seq;
		TimeVal get_land_time();

		bool start_jump;
		bool is_start_timer;

	public:
		void init( uint32_t game_map_id,uint32_t gameid, uint32_t src_userid,uint32_t obj_userid);
		//返回游戏是不是开始了
		bool set_user_can_start(uint32_t userid);
		bool user_opt( uint32_t userid, cli_game_opt_in *p_in);
		void noti_others( uint32_t userid, uint32_t cmdid, Cmessage *c_in=NULL);
		bool game_end(uint32_t userid,uint32_t win_flag  ,bool del_game_flag, bool except_self = true);
		void noti_all( uint32_t cmdid, Cmessage *c_in, uint32_t except_uid = 0);
		bool get_game_user(game_user_t& game_user, uint32_t userid);
		void do_timer_func(uint32_t* data);
};

struct game_wait_t {
	uint32_t userid;
	uint32_t side;
};

class Cgame_map {
	private:
		uint32_t cur_use_game_map_id;
		std::map <uint32_t, Cgame_item > game_item_map;
		uint32_t gen_game_map_id(){
			this->cur_use_game_map_id++;
			while( this->cur_use_game_map_id ==0
				|| this->game_item_map.find(this->cur_use_game_map_id)!=this->game_item_map.end()
				 ){
				this->cur_use_game_map_id++;
			}
			return this->cur_use_game_map_id;
		}

	public:
		std::map <uint32_t, game_wait_t> game_wait_map;
		uint32_t add(uint32_t gameid, uint32_t src_userid, uint32_t obj_userid )
		{
			DEBUG_LOG("ADDGAME\t[%u %u %u]", gameid, src_userid, obj_userid);
			uint32_t game_map_id= this->gen_game_map_id();
			Cgame_item 	 game_item;
			game_item.init(game_map_id ,gameid, src_userid, obj_userid );
			this->game_item_map[game_map_id]=game_item;
			return game_map_id;
		}

		//返回是否开始了
		bool   set_user_can_start(uint32_t game_map_id, uint32_t userid );
		//用户操作
		bool user_opt(  uint32_t game_map_id, uint32_t userid, cli_game_opt_in *p_in)
		{
			std::map <uint32_t, Cgame_item >::iterator it;
			it=this->game_item_map.find(game_map_id);
			if(it!=this->game_item_map.end()){
				return it->second.user_opt( userid,p_in );
			}
			return false;
		}

		void noti_all(uint32_t game_map_id, uint32_t cmdid, Cmessage* c_in, uint32_t except_uid = 0);
		void noti_map_game_stat(uint32_t game_map_id, uint32_t state);

		bool game_end( uint32_t game_map_id, uint32_t userid,uint32_t win_flag ,bool del_game_flag );
		bool game_user_left( uint32_t game_map_id, uint32_t userid);
		bool get_game_user(uint32_t game_map_id, game_user_t& game_user, uint32_t userid);

		void do_timer_func(uint32_t game_map_id, uint32_t* data);
};

struct stru_session_md5 {
	uint32_t v1;
	uint32_t v2;
	uint32_t time;
	uint32_t v3;
};

#define MDKEY	"fjalej@#jj"

class Cgame_session {
	public:
		static void make_session(char* session, uint32_t v1, uint32_t v2, uint32_t v3);
		static bool check_session(char* session, uint32_t v1, uint32_t v2, uint32_t v3);
};

class Cgame_conf_map {
	public:
		enum {
			max_game_level			= 20,
			default_min_game_time	= 300,
		};

		uint32_t calc_game_level(uint32_t game_point);
		bool init();
		bool add_game(uint32_t gameid, int32_t winpt, int32_t losept, uint32_t min_time);
		// 根据游戏ID、在线时长及胜负获取小游戏积分
		int32_t get_game_point(uint32_t gameid, uint32_t day_oltime, bool win);
		// 获取小游戏最短游戏时间，默认为5分钟
		uint32_t get_min_game_time(uint32_t gameid);

	private:

		class game_reward {
			public:
				int32_t		winpt;
				int32_t		losept;
				uint32_t	min_time;
		};
		std::map<uint32_t, game_reward> reward;
		uint32_t game_level_point[max_game_level];
};

#endif  /*CGAME_H*/
