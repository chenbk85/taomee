/**
 *============================================================
 *  @file      battle_impl.hpp
 *  @brief    declare class Battle
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_BATTLE_IMPL_HPP_
#define KFBTL_BATTLE_IMPL_HPP_

#include <algorithm>
#include <vector>

#include <boost/pool/object_pool.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

#include <libtaomee++/bitmanip/bitmanip.hpp>

//#include "mempool/mempool.hpp"
#include "moving_object.hpp"
#include "player.hpp"
#include "stage.hpp"
#include "battle_switch.hpp"
#include "map_summon_object.hpp"
#include "roll.hpp"
/**
  * @brief battle mode
  */
enum battle_mode_t {
	/*! 2 players with the same level will be matched and start a battle */
	btl_mode_lv_matching	= 1,
	/*! n players vs environment */
	btl_mode_pve			= 2,


	btl_mode_watch          = 3,

	btl_mode_temp_team_pvp  = 4,
};

/**
  * @brief battle difficulty
  */
enum battle_difficulty_t {
	btl_easy = 1,
	btl_normal,
};

/**
  * @brief battle difficulty
  */
enum pvp_lv_t {
	pvp_lv_0 = 0,
	pvp_lv_1 = 1,
	pvp_lv_2 = 2,
    pvp_monster_ = 3,
    pvp_monster_game = 4,
    pvp_monster_practice = 5,
	pvp_contest_advance = 6,
	pvp_contest_advance_practice = 7,
	pvp_contest_last = 8,
	pvp_16_contest = 9,
//	pvp_city_game_1 = 9,
	pvp_red_blue_1 = 10,
	pvp_red_blue_2 = 11,
	pvp_red_blue_3 = 12,
	pvp_team_score  = 13,
//	pvp_city_game_5 = 13,

	pvp_summon_mode = 14,
	pvp_ghost_game = 15,
	pvp_summon_mode_2 = 16,

	pvp_eve_of_16_fight_1 = 17, 
    pvp_eve_of_16_fight_2 = 18, 
    pvp_eve_of_16_fight_3 = 19, 
    pvp_eve_of_16_fight_4 = 20,

	pvp_team_contest = 21,
    pvp_temporary_team  = 22,

	pvp_dragon_ship   = 23
};


/**
  * @brief reasons that causes a battle to end
  */
enum end_battle_reason_t {
	/*! a normal ending */
	end_btl_normal	= 0,
	/*! some players offline */
	end_btl_offline	= 1,
	/*! some players timeout */
	end_btl_timeout	= 2,
	/*! a draw game */
	end_btl_draw	= 3,
	/*! system error */
	end_btl_syserr	= 4,
	/*! pvp match time out*/
	end_btl_pvp_timeout = 7,
	/*! some players escaped */
	end_btl_escape	= 5,
	/*! some players use boost plugin */
	end_boost_plugin = 6,
	/*! player stage lose*/
	end_stage_lose = 7,
};

/**
 *@brief pvp cost
 */
enum {
	pvp_lv1_const = 200,
	pvp_lv2_const = 300,
};

enum {
	pvp_type_radom = 0,
	pvp_type_invite,
	pvp_type_footprint,
};

enum player_action_in_room {
	room_player_ready = 0,
	room_player_cancel_ready = 1,
};

enum {
	battle_permit_hot_join = 0,
	battle_forbid_hot_join = 1,
};

enum {
	small_moon_chip = 1400123,
	big_moon_chip	= 1400122,
};

struct mechanism_del_item_t {
	uint32_t itemid;
	uint32_t itemcnt;
	mechanism_del_item_t() {
		itemid = 0;
		itemcnt = 0;
	}
};


class Battle;

typedef  void (*battle_logic_proc)(Battle* btl, struct timeval  next_tm);
typedef  void (*battle_init_logic_proc)(Battle* btl, void*);
typedef  void (*battle_final_logic_proc)(Battle* btl, void*);


///////用户自定义的关卡战斗数据//////////////////////////////
///////数据必须为堆上分配，自己编写初始化和释放的函数/////////
typedef void* (*battle_logic_data_alloc)();
typedef void  (*battle_logic_data_release)(void* p);

typedef struct battle_logic_data
{
	battle_logic_data()
	{
		data = NULL;
		p_alloc = NULL;
		p_release = NULL;
	}
	void* data;
	battle_logic_data_alloc     p_alloc;
	battle_logic_data_release   p_release;
}battle_logic_data;
//////////////////////////////////////////////////////////////
/**
  * @brief Battle
  */
//class Battle : public MemPool<Battle> {
class Battle : public taomee::MemPool, public taomee::EventableObject {
public:
	/**
	  * @brief a vector to hold all the players in a battle
	  */
	typedef std::vector<Player*>		PlayerVec;
	typedef std::list<Player*>			PlayerList;

public:
	/**
	  * @brief constructor
	  */
	Battle(Player* p, battle_mode_t btl_mode, const stage_t* stg, uint32_t difficulty, uint8_t in_team_btl = 0);
	/**
	  * @brief destructor
	  */
	~Battle();


	/**
	  * @brief add a player to the battle
	  */
	void add_player(Player* p, uint32_t seq);
	/**
	  * @brief delete a player from the battle
	  */
	void del_player(Player* p, end_battle_reason_t reason);

	void del_watcher(Player *p, end_battle_reason_t reason);

	//void watcher_leave(uint32_t id);
    uint32_t   get_friend_cnt(uint32_t team);

	uint32_t get_pvp_win_team();
	void add_buff_to_all_players(uint32_t buff_id, uint32_t creater_id);

	void del_buff_from_all_players(uint32_t buff_id);

	Player* get_player_in_btl(uint32_t player_id);

	Player* get_an_live_enemy(Player* p);
	/**
	  * @brief get a player from the battle
	  */
	Player* get_some_player();
    /**
     * @brief get dead player
     */
    Player* get_dead_player();

	Player * get_one_player();

//	void add_player_team_attr();

    /**
     * @brief get player by team
     */
    Player* get_player_by_team(uint32_t team);
	/**
	  * @brief call each time when a player is dead
	  */
	void on_player_dead(Player* p);
	/**
	  * @brief call each time when boss is dead
	  */
	void on_boss_dead();

	/**
	  * @brief set all monsters dead
	  */	
	void set_all_monsters_dead();
	/**
	  * @brief call each time when btl is over
	  */
	void on_btl_over(Player* p, bool flag = false);
    /**
     * @brief for over 939 stage
     */
    void over_pvp_939_stage(uint32_t winner_team);
	/**
	  * @brief call each time when a player use plugin
	  */
	void on_player_use_plugin(Player* p);

    /**
     * @brief notify client monster is killed
     */
    void notify_client_player_killed_monster(uint32_t uid, uint32_t mon_type);
	/**
	 * @brief set all players relive
	 */
	void set_all_players_relive();
	/**
	 *  @brief set all player skill cd time when pvp battle begin
	 */
	void set_all_player_skills_cd();

	void set_all_player_skills_ready();
    /**
     * @brief get lower player level
     */
    uint32_t get_lower_player_lv();
    /**
     * @brief Here,only for pvp_monster_
     */
    uint32_t get_left_player_lv();
    /**
     * @brief Here,only for pvp_monster_
     */
    uint32_t get_right_player_lv();
	/**
	  * @brief start the battle
	  */
	void start_battle(bool restart_flag = false);
	/**
	  * @brief set a player's battle status to 'ready', and notify the players
	  *           to start fighting if condition meets
	  */
	void set_player_ready(Player* p);

	/**
	  * @brief notify p that the battle start
	  */
	void set_battle_start(Player* p);

 	void do_after_start_btl();
	
	void reduce_players_exploit(uint32_t reduce_num);

	/*
	 * @brief add tmp buf in special occasion, for example in holiday
	 */
	void add_tmp_buf(Player* p);
	/**
	  * @brief notify p that the battle was ended and sync data with Online
	  */
	void send_btl_over_pkg(Player* p, bool win_flag = true);

    /**
     * @brief for pvp_monster game; Record game score
     */
    int save_pvp_taotai_game_data(Player* p, bool win_flag);
	/**
	  * @brief set player ready in a team btl
	  */
	void set_team_btl_player_ready(Player* p, bool is_ready = true);

	/**
	  *@brief set battle hot join flag
	  */
	void set_hot_join_flag(uint32_t flag)
		{ 
			TRACE_LOG("FLAG: %u", flag);
			hot_join_flag_ = flag;
			if (hot_join_flag_) {
				local_flg = true;
			} else {
				local_flg = false;
			}
		}

	bool is_permit_hot_join()
		{ return (hot_join_flag_ == battle_permit_hot_join);}
	/**
	  * @brief start team battle (team leader do it)
	  */
	int start_team_battle(Player* p);
	/**
	  * @brief the timer func for call monsters
	  */
	int	call_monsters_timer();


	/**
	  * @brief nodify player's state to room ,include enter,leave ...etc room
	  */	
	void nodi_player_state_to_room(player_action_in_room type, Player* p);

	void nodi_player_enter_state_to_room(Player* p);

	void nodi_player_leave_state_to_room(Player* p);

	void notify_team_member_enter_map(Player* p, uint32_t map_id);

	void notify_player_speed_change(Player* p);

	void enter_room(Player* p);

	void leave_room(Player* p);
	
	static int pack_join_none_btl_rsp(uint8_t* buf);
	
	int pack_join_btl_rsp(uint8_t* buf, Player* p);

	void monster_private_drop(Player* p, Player* killer);

	//drop money to players
	void drop_coins_on_monster_dead(Player *p, Player * killer);

	uint32_t get_dead_player_count();

	void notify_open_box_times(Player* p, uint32_t times);

	void notify_auto_del_item(Player* p, mechanism_del_item_t* p_item_arr, uint32_t item_count);	
	/**
	  * @brief return id of a battle
	  */
	uint32_t id() const
		{ return id_; }
	void set_id( uint32_t id)
		{ id_ = id; }
	uint32_t global_room_id() const
		{
			uint32_t server_id = get_server_id();
			TRACE_LOG("%u %u", server_id, id_ |  (server_id << 16 & 0xffff0000));
			return (id_ |  (server_id << 16 & 0xffff0000));
		}
	uint32_t get_players_num() const
		{ return players_.size(); }
	uint32_t sum_players_lv()
		{ 
			uint32_t lv_sum = 0;
			PlayerVec::iterator it = players_.begin(); 
			for (; it != players_.end(); ++it) {
				lv_sum += (*it)->lv;
			}
			return lv_sum;
		}
	void on_player_killed(Player* killer , Player* victim, bool can_get_exp = true)
		{
		
			if (is_summon_mon(killer->role_type)) {
				Player* owner = killer->summon_info.owner;
				if (owner) {
					owner->on_player_killed(victim, can_get_exp);
				}
			} else if (is_valid_uid(killer->id) && killer->my_summon && killer->my_summon->if_can_fight()) {
				killer->my_summon->on_player_killed(victim, can_get_exp);
			}

			killer->on_player_killed(victim, can_get_exp);
			
			
			if (is_summon_mon(killer->role_type) || is_valid_uid(killer->id)) {
				//for other
				Player* owner = is_summon_mon(killer->role_type) ? killer->summon_info.owner : 0;
				Player* expt_p = is_valid_uid(killer->id) ? killer : owner;
				PlayerVec::iterator it = players_.begin(); 
				for (; it != players_.end(); ++it) {
					if ((*it) != expt_p) {
						if (is_valid_uid((*it)->id) && (*it)->my_summon && (*it)->my_summon->if_can_fight()) {
							(*it)->my_summon->on_player_killed(victim, can_get_exp);
						}
						(*it)->on_player_killed(victim, can_get_exp);
					}
				}
			}
		}
	Player * get_one_other_player(Player * p) 
	{
		for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
			Player * t_p = *it;
			if (t_p->id != p->id) {
				return t_p;
			}
		}	
		return 0;
	}
	/**
	  * @brief return mode of a battle
	  */
	battle_mode_t battle_mode() const
		{ return btl_mode_; }
	/**
	  * @brief return if mode is pve
	  */
	bool is_battle_pve() const
		{ return (btl_mode_ == btl_mode_pve); }
	/**
	  * @brief return if mode is pvp
	  */
	bool is_battle_pvp() const
		{ return (btl_mode_ == btl_mode_lv_matching); }
	/**
	  * @brief return if mode is tower
	  */
	bool is_battle_tower() const
		{ return (difficulty() == stage_mode_tower); }
	
	bool is_battle_tmp_team_pvp() const
		{ return (btl_mode_ == btl_mode_temp_team_pvp); }

	/**
	  * @brief return mode of a battle
	  */
	const stage_t* stage() const
		{ return stage_; }

	/**
	  * @brief return difficulty of a battle
	  */
	uint32_t difficulty() const
		{ return btl_difficulty_; }

	/**
	  * @brief return pvp_lv
	  */
	uint8_t pvp_lv() const 
		{ return pvp_lv_; }

	/**
	  * @brief set pvp_lv
	  */
	void set_pvp_lv(uint8_t lv) 
		{ pvp_lv_ = lv; }
    /**
	  * @brief return pvp_type
	  */
	uint8_t pvp_type() const 
		{ return pvp_type_; }

	/**
	  * @brief set pvp_type
	  */
	void set_pvp_type(uint8_t type) 
		{ pvp_type_ = type; }


	/**
	  * @brief get a map whose id 'mid'
	  */
	map_t* get_map(uint16_t mid);

	/**
	  * @brief judge if a battle was started
	  */
	bool is_battle_started() const
		{ return taomee::test_bit_on(status_, battle_started); }
	/**
	  * @brief judge if a battle was ended
	  */
	bool is_battle_ended() const
		{ return taomee::test_bit_on(status_, battle_ended); }
	/**
	  * @brief judge if a battle was deletable
	  */
	bool is_battle_deletable() const
		{ return taomee::test_bit_on(status_, battle_deletable); }
	
	/**
	  * @brief set end reason
	  */
	void set_end_reason(end_battle_reason_t type)
		{ end_reason_ = type;}
	
	/**
	  * @brief set vinner
	  */
	void set_winner(userid_t id)
		{ winner_ = id;}

	uint32_t is_winner(Player *p)
	{
		if (winner_ == p->id) {
			return 1;
		}
		return 0;
	}

    uint32_t get_ready_num()
        { return ready_num_; }
	/**
	  * @brief get vinner
	  */
	userid_t get_winner()
		{ return winner_;}

	void set_player_num_limit(uint32_t limit_num)
		{ players_num_limit = limit_num; }

	uint32_t get_player_num_limit()
		{ return players_num_limit; }
	bool is_player_full() 
		{ return (players_.size() >= players_num_limit); }

	bool is_team_btl()
		{ return (in_team_btl_flg == 1);}

	void send_to_watcher(void * pkg, uint32_t len, Player * p = 0)
	{
		for (PlayerVec::iterator it = watchers_.begin(); it != watchers_.end(); ++it) {
				send_to_player(*it, pkg, len, 0);
		}

		if (p) {
			p->waitcmd = 0;
		}
	}

	int send_to_btl(void* pkg, uint32_t len, Player* p = 0, uint8_t complete = 1)
	{
		for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
			if (*it != p) {
				TRACE_LOG("%u %u %u", p ? p->id : 0, (*it)->id, p ? p->waitcmd : 0);
				send_to_player(*it, pkg, len, 0);
			}
		}
		if (p && complete) {
			TRACE_LOG("%u %u", p ? p->id : 0, p->waitcmd);
			p->waitcmd = 0;
		}

		//for watcher
		if (!watchers_.empty()) {
			btl_proto_t* btl_pkg = reinterpret_cast<btl_proto_t*>(pkg);   
			if (btl_pkg->cmd == btl_transmit_only) {
				cli_proto_t * c_pkg = reinterpret_cast<cli_proto_t*>(btl_pkg->body);
				uint16_t tmp_cmd = taomee::bswap(c_pkg->cmd);
				static uint8_t tm[8192];
				int idx = 0;
				pack(tm, pkg, len, idx);
				cli_proto_t * cc_pkg = reinterpret_cast<cli_proto_t*>(tm + sizeof(btl_proto_t));
				uint16_t hp_mp_cmd = cli_proto_contest_hpmp_change;
				uint16_t skill_cmd = cli_proto_contest_skill;
				switch (tmp_cmd) {
					case cli_proto_player_hpmp_chg :  
						cc_pkg->cmd = taomee::bswap(hp_mp_cmd);
					//	ERROR_LOG(">>>>>>> HP MP CHANGE TO WATCHERS_!");
						send_to_watcher(tm, idx);
						break;
					case cli_proto_players_hit :      
						cc_pkg->cmd = taomee::bswap(skill_cmd);
					//	ERROR_LOG(">>>>>>> USE SKILL TO WATCHERS_!");
						send_to_watcher(tm, idx);
						break;
					default:
						break;
				}
			}
		}
		return 0;
	}
	/**
	  * @brief find a player whose level is the same to p and start a battle
	  */
	void set_pvp_matched(Player* p, uint8_t type, Player* first_p);
	void set_tmp_team_pvp_team_side(Player* p);

	//************特殊的关卡逻辑************//
	void register_init_logic_function(uint32_t stage_id, battle_init_logic_proc proc);
	void unregister_init_logic_function( );

	void register_final_logic_function(uint32_t stage_id, battle_final_logic_proc proc);
	void unregister_final_logic_function( );


	void register_logic_function(uint32_t stage_id,  
								 battle_logic_proc proc,  
								 uint32_t time_val, 
								 battle_logic_data_alloc alloc = NULL,
								 battle_logic_data_release release = NULL
								 );

	void unregister_logic_function();
	
	
	void battle_section_routing(struct timeval  next_tm);

	void init_battle_logic();
	void final_battle_logic();	
	/*! battle special logic*/  
	
	uint32_t battle_section_data;
	struct timeval start_tm;  //记录关卡开始的时间
	struct timeval btl_section_routing_tm; //记录battle_section_data开始时间的	
	battle_logic_proc   p_proc;
	uint32_t time_frequency;

	battle_init_logic_proc 		p_init_proc;
	battle_final_logic_proc		p_final_proc;
	/////////////////特殊关卡自定义数据/////////////////////////
	battle_logic_data           btl_logic_data;

	void init_battle_logic_data( battle_logic_data_alloc alloc_func, battle_logic_data_release release_func);
	void final_battle_logic_data();	
	//关卡开始时间从btl_start开始
	uint32_t btl_start_tm;
	/*common flag for battle, see more at battle_logic for each battle stage
	 */
	uint32_t common_flag1_;
	uint32_t common_flag2_;
	uint32_t common_flag3_;
	uint32_t common_flag4_;
	uint32_t common_flag5_;
	uint32_t common_flag6_;
	//*****************************************//
	void battle_extra_logic_msg(Player * p);

	static void send_lv_match_rsp(Player* p, uint32_t battle_mode, uint8_t type, Player* first_player = 0, uint32_t all_ready = 0);
	static void send_tmp_team_match_rsp(Player* p, uint32_t battle_mode);
	static int pack_tmp_team_match_rsp(uint8_t* buf, uint32_t battle_mode, uint32_t _id);
	/**
	  * @brief send a rsp of level matching battle in timer
	*/
	static void send_lv_match_rsp_in_timer(Player* p, uint32_t battle_mode, Player* first_player);

	/**
	  * @brief create a battle. returns false and send an error code to client on failure
	  */
	static bool create_battle(Player* p, battle_mode_t btlmode, const stage_t* stg, uint32_t diff = 1, uint8_t in_team_btl_flg = 0);

	static void add_wait_player(Player* p);

	static void remove_wait_player(Player* p);

	static void remove_wait_player(PlayerList::iterator& it);

	static int pack_lv_match_rsp(uint8_t* buf, uint32_t battle_mode, userid_t first_id, uint8_t type, uint32_t btl_id, uint32_t all_ready = 0);

	/** 
	 * @brief 关卡中机关逻辑处理
	 * 
	 * @param mechanism 机关
	 * @param p 逻辑处理的请求者
	 */
	void proc_battle_mechanism_logic(Player * mechanism, Player * p);
	/**
	 * @brief 机关打开时各种逻辑处理
	 **/
	void proc_battle_mechanism_opened(Player * mechanism, Player *p);
public:
	
	/*! level matching battle player set */
	static std::list<Player*> s_player_list_;
	/*! players in this battle */
	PlayerVec		players_;

	PlayerVec       watchers_;
public:
	enum battle_status_t {
		battle_started		= 1,
		battle_ended		= 2,
		battle_deletable	= 3,

	};

	typedef std::map<uint32_t, const monster_t *> RanMonInfo;

	RanMonInfo raninfo;

	/**
	  * @brief inform the players to download the needed files and get ready to battle
	  * @param grp battle group
	  */
	void send_btl_ready_noti();

	void noti_online_all_player_joined_ready_to_start();
	/**
	  * @brief pvp battle count down
	  */
	void battle_count_down(Player* p);

	/**
	  * @brief set battle status
	  */
	void set_battle_status(battle_status_t status)
		{ status_ = taomee::set_bit_on(status_, status); }


	void create_entity();
	



	//--------------------------------------------------------

	/*! battle ID */
	uint32_t		id_;
	/*! stage of a battle */
	const stage_t*	stage_;
	/*! battle difficulty */
	uint32_t	btl_difficulty_;

	uint32_t    challenge_;

	/*! battle mode */
	battle_mode_t	btl_mode_;
	/*! bit 1: battle started */
	uint32_t		status_;

	uint32_t        watch_status_;
	/*! reason that a battle is ended */
	uint32_t		end_reason_;
	/*! winner */
	userid_t		winner_;
	uint32_t		winner_team;

	/*! pvp info*/
	uint8_t			pvp_lv_;
	uint8_t			pvp_type_;

	/*! initial map of a battle */
	map_t*			init_map_;
	/*! number of players that have been ready for battle */
	uint32_t		ready_num_;
	/*! number of players that have been ready in team battle */
	uint32_t		btl_team_ready_num_;	
	/*! number of maps in this battle */
	int				map_num_;
	/*! maps in this battle */
	map_t			maps_[stage_max_map_num];
	/*! dynamic path data for maps*/
	KfAstar*        dynamic_paths[stage_max_map_num];	

	bool  change_map_data(Player* p, KfAstar::Point& upper_left,  KfAstar::Point& lower_right,  int barrier)
	{
		KfAstar* p_star = get_cur_map_path(p);
		if(p_star == NULL)return false;
		return p_star->set_grids_walkability(upper_left, lower_right, barrier);
	}

	bool  change_map_data(map_t* cur_map, KfAstar::Point& upper_left,  KfAstar::Point& lower_right,  int barrier)
	{
		KfAstar* p_star = get_cur_map_path(cur_map);
		if(p_star == NULL)return false;
		return p_star->set_grids_walkability(upper_left, lower_right, barrier);
	}


	KfAstar*        get_cur_map_path(Player* p)
	{
		for(uint32_t i =0; i < stage_max_map_num; i++)
		{
			if(p->cur_map && p->cur_map->id == maps_[i].id)
			{
				return dynamic_paths[i];
			}
		}
		return NULL;
	}
	KfAstar*       get_cur_map_path(map_t* p_map)
	{
		for(uint32_t i =0; i < stage_max_map_num; i++)
		{
			if(p_map->id == maps_[i].id)
			{
				return dynamic_paths[i];
			}
		}
		return NULL;
	}


	/*! number of monsters in an battle */
	int				mon_num_;

	/*! group battle info*/
	uint8_t  in_team_btl_flg;
	
	bool  local_flg;

	uint32_t players_num_limit;
	uint32_t hot_join_flag_;


	/*pvp creater lv */
	int    _creater_lv;

	/*! for generating a unique battle ID */
	static uint32_t		s_base_id_;
	/*! for roll item */
public:	
	std::vector<battle_roll_data*>  m_roll_datas;
	uint32_t                        m_base_roll_id;
	std::list<battle_roll_data*>    m_prepare_roll_datas;
public:
	bool 		init_roll_data();
	bool 		final_roll_data();
	uint32_t	get_next_roll_id();
	
	battle_roll_data* get_battle_roll_data_by_roll_id(uint32_t roll_id);
	bool        add_roll_data(map_t* map, battle_roll_data* data);
	bool        add_roll_data(battle_roll_data* data);

	bool        del_roll_data( battle_roll_data* data);
	bool        del_player_from_roll(uint32_t player_id);

	bool        notify_team_roll_point(uint32_t roll_id, uint32_t player_id, int roll_point, uint32_t except_player_id = 0);
	bool        notify_team_roll_item(battle_roll_data* data);
	bool        notify_team_get_item(uint32_t roll_id, uint32_t player_id, uint32_t item_id, uint32_t unique, int sucess);
	
	bool        notify_team_map_roll_point(map_t* map, uint32_t roll_id, uint32_t player_id, int roll_point, uint32_t except_player_id = 0);
	bool        notify_team_map_roll_item(map_t* map, battle_roll_data* data);
	bool        notify_team_map_get_item(map_t* map, uint32_t roll_id, uint32_t player_id, uint32_t item_id, uint32_t unique, int sucess);


	bool        drop_item_to_team(map_t* map, uint32_t item_id, uint32_t x, uint32_t y);
	
    /*! synchro fumo tower information to db */
    int sent_finish_tower_info2db(Player* p);
	
	/*! for map summon object */
	std::list<map_summon_object*> map_summon_list;
	bool init_map_summon();
	bool final_map_summon();
	bool add_map_summon(map_summon_object* p, bool notify);
	bool del_map_summon(map_summon_object* p);
	bool del_map_summon_by_owner_id(uint32_t owner_id);
	bool del_map_all_summons_by_role_type(uint32_t role_type);
	void map_summon_routing(struct timeval  next_tm);
	uint32_t get_map_summon_count(uint32_t owner_id);
	/*! notify first kill boss */
	void notify_first_boss_kill(uint32_t stage_id, uint32_t diffcult, uint32_t boss_id);
};

inline void
Battle::add_player(Player* p, uint32_t seq = 1)
{
	p->btl = this;
    uint32_t exist_friend = this->get_friend_cnt(seq);
	players_.push_back(p);

	if (is_team_btl()) {
		do_stat_team_enter_leave_battle(stage()->id, difficulty(), 1, 0, 0 );
		if (is_battle_pvp()) {
			do_stat_enter_leave_battle(stage()->id, difficulty(), 1, 0, 0 );
		}
	} else {
		do_stat_enter_leave_battle(stage()->id, difficulty(), 1, 0, 0 );
	}
    uint16_t x_pos = stage_->init_x; 
    uint16_t y_pos = stage_->init_y; 
    if (seq == 2) {
        x_pos = stage_->sec_x;
        y_pos = stage_->sec_y;
        p->dir = dir_left;
		if (pvp_lv_ == pvp_dragon_ship) {
			p->dir = dir_right;
		}
    }
    y_pos += exist_friend * 50;
	TRACE_LOG("S M2 %u, %u", x_pos, y_pos);
	p->set_pos(Vector3D(x_pos, y_pos));
	p->do_enter_map(init_map_);
	
}

inline void 
Battle::del_watcher(Player * p, end_battle_reason_t reason)
{
	PlayerVec::iterator it = watchers_.begin();
    while (it !=  watchers_.end()) {
		if ((*it)->id == p->id) {
			watchers_.erase(it);
			return;
		}
		++it;
	}
}

inline uint32_t
Battle::get_pvp_win_team()
{
	uint32_t team_1 = 0;
	uint32_t team_1_num = 0;
	uint32_t team_2 = 0;
	uint32_t team_2_num = 0;
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
		Player* p = (*it);
		if (!team_1) {
			team_1 = p->team;
		}
		if (!(p->is_dead()) && team_1 == p->team) {
			team_1_num++;
		}
		if (!(p->is_dead()) && team_1 != p->team) {
			team_2_num++;
			team_2 = p->team;
		}
	}
	if (team_1_num && team_2_num) {
		return 0;
	}
	if (team_1_num) {
		return team_1;
	}
	if (team_2_num) {
		return team_2;
	}
	return 0;
}

inline void
Battle::del_player(Player* p, end_battle_reason_t reason) 
{
	// remove roll player_id from roll data
	del_player_from_roll(p->id);	
	// remove player p from the battle
	PlayerVec::iterator it = std::find(players_.begin(), players_.end(), p);

	assert(it != players_.end());

	players_.erase(it);
	
	set_player_num_limit(get_player_num_limit() ? get_player_num_limit() - 1 : 0);
	// remove player p from the player_set
	if ((is_battle_pvp() || is_battle_tmp_team_pvp()) ) {// && !is_battle_started()) {
		remove_wait_player(p);
	}

	if (is_battle_started() && !is_battle_ended() && (is_battle_pvp() || is_battle_tmp_team_pvp())) {
		// TODO: to support more than 2 players

		if (is_battle_pvp()) {
			if (get_pvp_win_team()) {
				end_reason_ = reason;
				winner_team = get_pvp_win_team();
			
				for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
					if (winner_team == (*it)->team) {
					//win team
						send_btl_over_pkg(*it);
						if (is_battle_pvp() || is_battle_tmp_team_pvp()) {
							(*it)->calc_pvp_score(1);
						}
					} else {
					//lose team
						send_btl_over_pkg(*it, false);
						if (is_battle_pvp() || is_battle_tmp_team_pvp()) {
							(*it)->calc_pvp_score(0);
						}
					}
				}
				set_battle_status(battle_ended);
		        set_all_monsters_dead();
				final_battle_logic();
			}
		}
		if (is_battle_tmp_team_pvp()) {
			if (get_pvp_win_team()) {
				end_reason_ = reason;
				winner_team = get_pvp_win_team();
			
				for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
					if (winner_team == (*it)->team) {
					//win team
						send_btl_over_pkg(*it);
						if (is_battle_pvp() || is_battle_tmp_team_pvp()) {
							(*it)->calc_pvp_score(1);
						}
					} else {
					//lose team
						send_btl_over_pkg(*it, false);
						if (is_battle_pvp() || is_battle_tmp_team_pvp()) {
							(*it)->calc_pvp_score(0);
						}
					}
				}
				set_battle_status(battle_ended);
		        set_all_monsters_dead();
				final_battle_logic();
			} else {
				if (ready_num_ == players_.size()) 
				{
					battle_count_down(p);
				}
			}
		}
	} else if (is_team_btl() && is_battle_started()) {
		if (players_.size() >= ready_num_ && players_.size()) {
			PlayerVec::iterator it = players_.begin();
			if (!test_bit_on((*it)->btl_status, player_in_battle) && test_bit_on((*it)->btl_status, player_ready_for_battle)) {
				set_battle_start((*it));
			}
		}
	}

	if (p->is_player_in_team_btl()) {
		if (is_battle_pve()) {
			leave_room(p);	
		}
		if (is_battle_pvp() || is_battle_tmp_team_pvp()) {
			notify_leave_pvp_room(p, id());
		}
	}
	TRACE_LOG("%u %u", id(), (uint32_t)players_.size());
	if (players_.size() == 0) {
		set_battle_status(battle_deletable);
	}
}

inline Player*
Battle::get_some_player()
{
	Player* p = 0;
	PlayerVec::iterator it = players_.begin();
	while (it != players_.end()) {
		if (!p || p->lv < (*it)->lv) {
			p = *it;
		}
		it++;
	}

	return p;
}

inline Player *
Battle::get_one_player()
{
	if (!players_.empty()) {
		return *(players_.begin());
	}
	return 0;
}


inline Player*
Battle::get_dead_player()
{
	PlayerVec::iterator it = players_.begin();
	while (it != players_.end()) {
		if ((*it)->is_dead()) {
			return *it;
		}
		it++;
	}

	return NULL;
}

inline Player*
Battle::get_player_by_team(uint32_t team)
{
    PlayerVec::iterator it = players_.begin();
    while (it != players_.end()) {
        if ((*it)->team == team) {
            return *it;
        }
        ++it;
    }
    return NULL;
}

inline map_t*
Battle::get_map(uint16_t mid)
{
	if ((mid < stage_max_map_num) && maps_[mid].id) {
		return &(maps_[mid]);
	}

	return 0;
}

//----------------------------------------------------------
// public static methods
//
inline bool
Battle::create_battle(Player* p, battle_mode_t btlmode, const stage_t* stg, uint32_t diff, uint8_t in_team_btl_flg)
{
	try {
		new Battle(p, btlmode, stg, diff, in_team_btl_flg);
		return true;
	} catch (...) {
		DEBUG_LOG("FAIL TO CREATE BATTLE\t[uid=%u]", p->id);
		send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		return false;
	}
}

inline void 
Battle::remove_wait_player(Player* p)
{
	PlayerList::iterator it = std::find(s_player_list_.begin(), s_player_list_.end(), p);
	if (it != s_player_list_.end()) {
		s_player_list_.erase(it);
	}

	TRACE_LOG("remove_wait_player %u %u %p", p->id, (uint32_t)s_player_list_.size(), p);
}

inline void 
Battle::remove_wait_player(PlayerList::iterator& it)
{
	s_player_list_.erase(it);

	TRACE_LOG("remove_wait_player %u", (uint32_t)s_player_list_.size());
}

inline void 
Battle::add_wait_player(Player* p)
{
	s_player_list_.push_back(p);
	TRACE_LOG("add_wait_player %u", (uint32_t)s_player_list_.size());
}

void end_pvp_on_player_dead(Battle* btl, Player* p, uint32_t win_team = 0);

//int 
//proc_lv_matching(void* owner, void* data);

void battle_statistics();

extern uint32_t pvp_players_statistics;
extern uint32_t pve_players_statistics;
extern uint32_t pve_team_players_statistics;

/** 
 * @brief  关卡额外逻辑处理
 * 
 */
void process_battle_extra_logic(Battle* btl);

#endif // KF_BATTLE_IMPL_HPP_

