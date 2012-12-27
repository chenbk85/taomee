/**
 *============================================================
 *  @file      player.hpp
 *  @brief    player related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_PLAYER_HPP_
#define KF_PLAYER_HPP_

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" {
//#include <glib.h>

#include <libtaomee/list.h>
#include <libtaomee/timer.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}
#include <map>
#include <set>
#include <list>

#include "fwd_decl.hpp"
#include "dbproxy.hpp"
#include "summon_monster.hpp"
#include "numen.hpp"
#include "secondary_profession.hpp"
#include "mail.hpp"
#include "buy_item_limit.hpp"
#include "achievement.hpp"
#include "kill_boss.hpp"
#include "use_item_history.hpp"
#include "title_system.hpp"
#include "ring_task.hpp"
#include "contest.hpp"
#include "home_impl.hpp"
#include "master_prentice.hpp"
#include "god_guard.hpp"
#include "other_active.hpp"
struct home_grp_t;
struct player_packs;
struct task_going_t;
struct trade_grp_t;
struct trade_recoder_t;

struct player_card_set_t;
struct player_card_t;
struct red_blue_info_t;
class TaskStepManager;

enum frient_type_t {
	FRIENDS = 1,
};

/*! number of players within this server */
extern uint32_t	player_num;

#pragma pack(1)

struct player_clothes_info_t {
	uint32_t clothes_id;
	uint32_t unique_id;
	uint16_t duration;
	uint16_t lv;
    uint32_t gettime;
    uint32_t timelag;
};

struct player_skill_bind_t {
	uint32_t skill_id;
	uint32_t lv;
	uint32_t key;
};
typedef std::map<uint32_t, player_skill_bind_t> PlayerSkillMap;

class PlayerTaskEventMap;

struct player_item_bind_t {
	uint8_t buf[max_itembind_len];
};

struct player_tmp_info_t {
};

struct player_range_t;


struct db_update_login_info_t {
	//uint32_t	type;
	int32_t		login_time;
	int32_t		logout_time;
};

struct db_logout_info_t {
    uint32_t map_id;
    uint32_t xpos;
    uint32_t ypos;
    int32_t  oltm;
};

struct fumo_stage_info_t {
	uint32_t stage_id;
	uint8_t grade[max_stage_difficulty_type];
	fumo_stage_info_t(uint32_t id, uint8_t* grade_in) {
		stage_id = id;
		memcpy(grade, grade_in, max_stage_difficulty_type);
	}
	fumo_stage_info_t(uint32_t id, uint32_t difficulty, uint8_t stage_grade) {
		stage_id = id;
		memset(grade, 0x0, max_stage_difficulty_type);
		grade[difficulty - 1] = stage_grade;
	}
};

bool is_player_has_stage_fumo_info(player_t *p, int stage_id, int diff);

struct warehouse_item_data
{
        warehouse_item_data()
        {
                item_id = 0;
                item_count = 0;
        }
        uint32_t  item_id;
        uint32_t  item_count;
};


struct warehouse_clothes_item_data
{
	warehouse_clothes_item_data()
	{
		id = 0;
		attireid = 0;
		get_time = 0;
		attire_rank = 0;
		duration = 0;
		attire_lv = 0;
		end_time = 0;	
	}
	uint32_t id;
	uint32_t attireid;
	uint32_t get_time;
	uint32_t attire_rank;
	uint32_t duration;
	uint32_t attire_lv;
	uint32_t end_time;
};

struct buf_skill_t {
	uint32_t buf_type;
	uint32_t buf_duration;
	uint32_t per_trim;
	uint32_t ex_id1;
	uint8_t	 db_buff_flg;
	uint32_t mutex_type;
	uint32_t start_tm;
};

enum {
	buf_type_appearance = 10,
};
typedef std::map<uint32_t, buf_skill_t> BufSkillMap;

enum {
	rank_top_type_tmp_team_score = 9,
	max_rank_top_type,
};

enum {
	swap_action_daily = 0,
	swap_action_weekly = 1,
	swap_action_monthly = 2,
	swap_action_yearly = 3,
	swap_action_for_ever = 4,

	swap_action_card_set = 54,
	

	swap_city_team_reward  = 59,

	swap_active_player_reward = 64,

	swap_achieve_need_reward = 78,

	items_upgrade_elem_max_cnt = 10,
	swap_action_elem_max_cnt = 25,
};

struct swap_action_elem_t {
	uint32_t give_type;
	uint32_t give_id;
    char name[32];
	uint32_t count;
	uint32_t role_type;
	uint32_t odds;
	uint32_t is_bcast;
};

enum need_type_t {
	need_ranker_range = 1,
	need_ranker_score = 2,
    need_other_attr   = 3, 
};

struct swap_other_ranker_t {
	uint32_t need_type;
	uint32_t active_id;
	uint32_t range[2];
	uint32_t need_cnt;
};

class swap_action_data {
	public:
		uint32_t id;
		char name[256];
		uint32_t cli_type;
		uint32_t type;
		uint32_t top_limit;
		uint32_t vip;
		uint32_t year_vip;
		uint32_t user_lv[2];
		uint32_t user_flg;
		uint32_t odds_flg;
		uint32_t week_limit[7];
		uint32_t tm_range[2];
		uint32_t start_end_time[2];
		uint32_t cd_time;
		uint32_t cost_odds;
		uint32_t cost_count;
		uint32_t time_range_id;
		uint32_t buff_id;
		//added 0620
		uint32_t ex_field_1;
		uint32_t ex_field_2;

		swap_other_ranker_t needs;

		swap_action_elem_t costs[swap_action_elem_max_cnt];
		uint32_t rew_count;
		swap_action_elem_t rewards[swap_action_elem_max_cnt];
	public:
		swap_action_data()
		{
			id = 0;
			memset(name, 0, sizeof(name));
			type = 0;
			top_limit = 0;
			vip = 0;
			year_vip = 0;
			user_lv[0] = 0;
			user_lv[1] = 0;
			user_flg = 0;
			odds_flg = 0;
			ex_field_1 = 0;
			ex_field_2 = 0;
            for (uint32_t i = 0; i < 7; i++)
                week_limit[i] = 0;
			tm_range[0] = 0;
			tm_range[1] = 0;
			cost_count = 0;
			rew_count = 0;
            cd_time = 0;
            cost_odds = 0;
			time_range_id = 0;
			buff_id = 0;
		}
};

class OffLineExp{
	public:

	private:
		uint32_t start_tm;
		uint32_t fumo_;
		uint32_t coin_;
};

struct player_swap_action_data {
	uint32_t id;
	uint32_t type;
	uint32_t count;
	uint32_t tm;
};

typedef std::map<uint32_t, player_swap_action_data> PlayerSwapActionDataMap;


typedef std::map<uint32_t, swap_action_data> SwapActionDataMap;

struct on_off_line_t {
    time_t on_time;
    time_t off_time;
};

typedef std::vector<on_off_line_t> OnOffLine;

struct offline_exp_data_t {
	uint32_t player_start_tm;
	uint32_t summon_start_tm;
	uint32_t off_coins;
	uint32_t off_fumo;
};

struct team_member_arr_t {
	uint32_t uid;
	uint32_t role_tm;
	uint32_t join_tm;
	uint32_t level;
    uint32_t t_coins;
    uint32_t t_exp;
};

struct fight_team_info_t {
	uint32_t login_flg;
	uint32_t team_id;
	uint32_t captain_uid;
	uint32_t captain_role_tm;
	char     captain_nick[max_nick_size];
	uint32_t team_member_cnt;
	uint32_t coin;
    uint32_t active_flag;
    uint32_t active_score;
	uint32_t score_index;
	char 	 team_name[max_nick_size];
	uint32_t team_lv;
	uint32_t team_exp;
	uint32_t last_tax_tm;
	char     team_mcast[240];
	team_member_arr_t team_member[max_team_member_cnt];
	void reset()
	{
		login_flg = 0;
		team_id = 0;
		captain_uid = 0;
		captain_role_tm = 0;
		team_member_cnt = 0;
        coin = 0;
        active_flag = 0;
        active_score = 0;
		memset(team_name, 0, sizeof(team_name));
		memset(team_member, 0, sizeof(team_member));
		last_tax_tm = 0;
	}

};

struct temporary_team_member_t {
	uint32_t userid;
	uint32_t role_tm;
	uint32_t level;
	uint32_t member_type;//0:normal 1 leader
};

struct db_offline_rsp_t {
	uint32_t player_start_tm;
	uint32_t summon_start_tm;
	uint32_t offline_coins;
	uint32_t offline_fumo;
}__attribute__((packed));

struct puzzle_info_t {
	uint32_t right_cir;
	uint32_t kick_flag;
	uint32_t book_set_flag;
};

struct role_info_t {
    uint32_t    userid;
    uint32_t    roletm;
};

/**
  * @brief Player
  */
struct player_t {
	/*! player's id */
	userid_t	id;
	/*! user's process when login or logout game*/
	uint8_t		user_progress[1];
	/*! the create time of the player's role */
	uint32_t	role_tm;
	/*! type of a player's role */
	uint32_t	role_type;
    /*! flag of power_user (0--no 1--yes)*/
	uint32_t	power_user;
	/*! 0:player is child,  1: player is adult */
	uint8_t 	adult_flg;
	/*! player's nick name */
	char		nick[max_nick_size];
    uint32_t    nick_change_tm;
	/*! vip flag */
	uint32_t	vip;
	/*! flag, currently unused */
	uint32_t	flag;
	/*! must check pay passwd after login */
	bool pay_passwd_check_flg;
	uint32_t pay_passwd_err_cnt;
	uint32_t pay_passwd_err_tm;
	
	/*! player's skill point*/
	uint32_t	skill_point;
	/*! player's money */
	uint32_t	coins;
	/*! player's amb parent userid */
	userid_t	parentid;
	/*! player's ambassador task status */
	uint8_t		amb_status;
	/*! player's amb child count */
	uint32_t	child_cnt;
	/*! count of child who achieve some condition */
	uint32_t 	achieve_cnt;
    /*! count of player strengthen attire */
    uint32_t    strengthen_cnt;

	//----------------------------------
	// player's battle atribute
	//
	/*! player's level */
	uint16_t	lv;
	/*! player's experience */
	uint32_t	exp;
	uint32_t	allocator_exp;
	/*! recode the left time of double experience*/
    uint32_t dexp_time;

    uint32_t day_flag;

    uint32_t max_times_chapter;
	/*! 0: regular experience;1: only half experience*/
	uint8_t half_exp;
	/*! strength */
	uint16_t	strength;
	/*! agility */
	uint16_t	agility;
	/*! body quality */
	uint16_t	body_quality;
	/*! stamina */
	uint16_t	stamina;
	/*! player's current hp */
	uint32_t	hp;
	/*! player's current maxhp */
	uint32_t	maxhp;
	/*! add hp per 1 sec */
    uint32_t    addhp;
	/*! add mp per 1 sec*/
    uint32_t    addmp;
	/*! player's current mp */
	uint32_t	mp;
	/*! player's max mp */
	uint32_t	maxmp;
	/*! attack power */
	uint16_t	atk;
	/*! critical attack rate */
	uint16_t	crit_rate;
	/*! defence rate */
	float		def_rate;
	/*! hit rate */
	float		hit_rate;
	/*! dodge rate */
	float		dodge_rate;

	//guaji 		
	offline_exp_data_t  * offline_data;

	fight_team_info_t team_info;
	uint32_t team_top10_tv;
	/*! battle */
	battle_grp_t*	battle_grp;
	/*trade */
	trade_grp_t *  trade_grp;
	std::list<trade_recoder_t> * trade_recoders;
    uint32_t    safe_trade_room_id;

	std::map<uint32_t, player_card_set_t*> * cardset_info;
	//std::map<uint32_t, player_card_t> * cards_info;
	std::vector<uint32_t>* friends_vec;
	uint32_t last_mcast_tm;
	uint32_t last_world_tm;
	/*home*/
	home_grp_t*  home_grp;	

	/*! player's packs */
	player_packs*	my_packs;

	/*! item to keyboard bind info*/
	player_item_bind_t itembind;

	/*! sequence number */
	uint32_t	seqno;
	/*! real sequence number, for verifying Game Cheater */
	uint32_t	real_seqno;

	puzzle_info_t  * puzzle_data;


	red_blue_info_t * p_banner_info;

	/*! player show state */
	uint32_t    player_show_state;
	uint32_t    open_box_times;

	//buy item limit data
	std::map<uint32_t,  player_item_limit_data>* m_limit_data_maps;		

	//--------------ring taks---------------------//
	std::map<uint32_t, player_ring_task_data>* m_ring_task_maps;
	std::map<uint32_t, player_ring_task_history_data>* m_ring_task_history_maps;
	//-------------warehouse_bag---------
	std::map<uint32_t,  warehouse_item_data>* m_data_maps;
	std::map<uint32_t,  warehouse_clothes_item_data>* m_clothes_data_maps;

	uint32_t     vip_base_point;            	//»ù´¡ÏÀÊ¿Öµ£¬¸ÃÖµÍ¨¹ý£¬ÔùËÍ£¬¹ºÂò£¬ÈÎÎñ»ñµÃ		
	uint32_t     vip_sword_value;            	//»ù´¡ÏÀÊ¿Öµ£¬¸ÃÖµÍ¨¹ý£¬ÔùËÍ£¬ÈÎÎñ»ñµÃ		
	uint32_t     vip_total_point;           	//×ÜµÄÏÀÊ¿Öµ£¬¸ÃÖµÍ¨¹ý »ù´¡Öµ + ÔÚÏß»îÔ¾Ê±¼ä»ñµÃ
	uint32_t     vip_current_show_point;    	//µ±Ç°¼¶±ðËù¶ÔÓ¦µÄÊ£ÓàÏÀÊ¿Öµ,Õâ¸öÔÝÊ±ÎÞÓÃ£¬±¾À´ÊÇ·¢µ½¿Í»§¶ËÈ¥µÄ
	uint32_t     vip_level;                 	//ÏÀÊ¿¼¶±ð
	uint32_t     extern_warehouse_grid_count;   //µ±Ç°¶îÍâµÄÔö¼ÓµÄ²Ö¿â¸ñ×ÓÊýÁ¿
	uint32_t     extern_item_bag_grid_count;    //µ±Ç°¶îÍâµÄÔö¼ÓµÄÎïÆ·°ü¸ñ×ÓÊýÁ¿
	uint32_t     vip_begin_time;                //VIP¿ªÊ¼Ê±¼ä
	uint32_t     vip_end_time;                  //VIP½áÊøÊ±¼ä
	uint32_t     vip_months_cnt;                //VIPµ±Ç°×Ü°üÔÂÊý
	//-------------friend---------------------------
	uint32_t     forbiden_add_friend_flag;
	void	set_add_friend_flag(uint32_t flag)
	{
		forbiden_add_friend_flag = flag;
	}

	//---------------------mail--------------------------//
	std::map<uint32_t, mail_data> * m_read_mails;	
	std::map<uint32_t, mail_data> * m_unread_mails;
	
	//----------------------------------
	//---------------------¸±Ö°----------------------------
	std::map<uint32_t, secondary_profession_data> *m_pro_data_maps;
	//¸±Ö°µÄ»îÁ¦Öµ
	uint32_t  vitality_point;
	
	//---------------------Ñ«ÕÂ----------------------------//
	uint32_t m_medal_count;                                           //Ñ«ÕÂµÄÊýÁ¿	
	
	//--------------------³É¾Í-----------------------------//
//	player_achievement_data* m_achievedatas;                           //»ñµÃµÄ³É¾ÍÊý×é
	std::map<int, player_achievement_data> * m_achievedatas; 
	std::map<int, player_stat_t> * m_statdatas;
	std::map<uint32_t, kill_boss_data> *m_kill_boss_datta_maps;        //Í¨¹Ø¼ÇÂ¼
	std::map<uint32_t, use_item_data>  *m_use_item_data_maps;          //ÎïÆ·Ê¹ÓÃ¼ÇÂ¼
	uint32_t                 achieve_point;                            //³É¾ÍµãÊý
	uint32_t				 last_update_ap_tm;                        //×îºóÒ»´Î¸üÐÂ³É¾ÍµãÊýÊ±¼ä

	PlayerSwapActionDataMap *m_swap_action_data_maps;
	uint32_t get_achieve_point()
	{
		return achieve_point;
	}

	void add_achieve_point(uint32_t add_point, uint32_t time)
	{
		achieve_point += add_point;
		last_update_ap_tm = time;
	}

    CTitle* pTitle;
    uint32_t        using_achieve_title; //Íæ¼Òµ±Ç°µÄ³ÆºÅ

    OnOffLine * on_off_line[5]; //¼ÇÂ¼Íæ¼Ò×î½ü5ÌìµÄÉÏÏÂÏßÐÅÏ¢ 
	//--------------------Ð¡ÎÝ--------------------------------//
	player_home_data       *m_home_data;
	uint32_t get_home_summon_count()
	{
		return summon_mon_num;	
	}	
	uint32_t get_home_level()
	{
		return m_home_data->home_level;
	}
	
	//-----------------------------------------------------//
	// player's task
	/*! task list */
	std::set<uint32_t>* finished_tasks_set;
	/*! canceled task list */
	std::set<uint32_t>* canceled_tasks_set;
	/*! the number of minior tasks which is doing now*/
	uint32_t minior_tasks_num;
	/*! the info of tasks which is doing now*/
	std::map<uint32_t, task_going_t>* going_tasks_map;

//	TaskStepManager * player_task_steps;


	/*! home_battle_pet */
	std::vector<uint32_t>* home_btl_pets;
	void add_home_btl_pet(uint32_t pet_id)
	{
		home_btl_pets->push_back(pet_id);
	}
	void clear_home_btl_pet()
	{
		home_btl_pets->clear();
	}
    //--------------------ÉñÊÞ»¤·¨------------------------------// 
    group_t*    group_datas;
    quality_t*  add_quality;

    //--------------------µÚÒ»±ÈÎä´ó»á------------------------------// 
    pvp_game_data_t* taotai_info;
    pvp_game_data_t* advance_info;
	pvp_watch_data_t * watch_info;
	pvp_game_data_t* final_info;
    uint8_t     contest_leader_flag;
    uint32_t game_flower;

    //--------    Ê¦Í½ÐÅÏ¢     -----------
    mp_master_t *p_master; //Ê¦¸µ
    std::vector<mp_prentice_t>* p_prentice; //Í½µÜ
    uint32_t master_prentice_tm;
	//----------------------------------

	/*! map where the user is in currently */
	map_t*		cur_map;
    /*! recode the last logout map id*/
    uint32_t    last_mapid;
	/*! x pos of the map */
	uint32_t	xpos;
	/*! y pos of the map */
	uint32_t	ypos;
	/*! direction of the player */
	uint8_t		direction;
	/*! action of the player */
	uint32_t	action;	
	/*! 0: player is visible; 1: player is not visible */
	uint8_t		invisible;


	uint32_t	fumo_points_today;
	uint32_t	fumo_points_total;
	uint32_t	fumo_tower_top;
	uint32_t	fumo_tower_used_tm;
	uint32_t 	fumo_reward_flg;
	std::map<uint32_t, fumo_stage_info_t>* fumo_stage_map;

	timer_struct_t* app_t;
	uint32_t	app_mon;
	BufSkillMap* buf_skill_map;

    /*! save player list */
    std::vector<role_info_t>* reward_role_vec;

	//----------------------------------
	/*! pvp battle info */
	uint32_t	cur_continue_win;
	uint32_t	honor;
	uint32_t	exploit; //ÎäÑ«
	uint32_t	win_times;
	uint32_t	lose_times;
	uint32_t	max_conti_win_times;
	//----------------------------------

	/*! total login count */
	uint32_t	olcount;
	/*! today's login count */
	uint32_t	oltoday;
	uint32_t	oltoday_db;
	/*! last login time */
	uint32_t	ollast;
	/*! total online time */
	uint32_t	oltime;
	/*! login time */
	uint32_t	login_tm;
	/*! register time */
	uint32_t	reg_tm;
	/*! last active time of a player */
	int32_t 	last_act_tm;

	/*! save cmd id that is currently under processing */
	uint16_t	waitcmd;
	GQueue*		pkg_queue;
	/*! hook that is used to link all players whose waitcmd is not 0 together */
	list_head_t	wait_cmd_player_hook;

	//list_head_t	player_id_hook;
	/*! hold all the timers of a players */
	list_head_t timer_list;

	/*! save socket fd from parent process */
	int				fd;
	/*! fd session */
	fdsession_t*	fdsess;

	// Hooks
	/*! hook to be linked to map_t::playerlist, thus link all users within a map together */
	list_head_t	maphook;
	/*! hook to be linked to trade_list_t::player_list, link all players in trade market together*/
	list_head_t tradehook;
//	bool	tradehook_flag;
	/*! hook to be linked to uid_buckets, thus link all users in this server together */
	list_head_t	playerhook;

	/*! session length */
	int			sesslen;

	//-----------------------------------------------------------------------
	/*! unique drop item bit*/
	uint8_t		unique_item_bit[c_unique_item_bit_size];

	/*! daily restriction count array, note the 1st element is being used for captoy coin */
	uint8_t		daily_restriction_count_list[max_restr_count];

#ifdef DEV_SUMMON
	/*! number of summon monster */
	uint16_t	summon_mon_num;
	/*! all summon monster info */
	summon_mon_t	summons[max_summon_num];
	/*! summon monster for fight */
	summon_mon_t*	fight_summon;
#endif	

	uint32_t temporary_team_id;
	std::map<uint32_t, temporary_team_member_t>* m_temporary_team_map;
	std::map<uint32_t, temporary_team_member_t>* m_temporary_team_map_bak;
    
    std::vector<numen_obj_t>* m_numens;

	PlayerSkillMap* player_skill_map;

	/*! number of skill binded */
	uint32_t	skill_bind_num;
	/*! skills' info */
	player_skill_bind_t	skills_bind[max_skills_bind];
	
	/*! sign some one-off event  if hapended */
	uint8_t		once_bit[max_once_bit_size];
    uint8_t     act_record[max_limit_active_times];
	/*! number of clothes on */
	uint32_t	clothes_num;
	/*! clothes' info */
	player_clothes_info_t	clothes[max_clothes_on];

	player_range_t * p_range_info;


	uint32_t  attire_gs;

	uint32_t  team_id;//¿¿¿¿¿¿
    uint32_t  contest_win_stage;
    uint32_t  contest_final_rank;
    uint32_t  contest_final_segment; //1--lower; 2--high

    /* ! event trigger information */
    timer_struct_t* trigger_timer;
    uint16_t  trigger_event;
    uint16_t  trigger_times;
    uint16_t  trigger_block;
    uint16_t  trigger_victory;

	//contest group 
	uint64_t  contest_session;

//	uint32_t  wuseng_reward_flag;

	uint8_t	  client_buf[max_client_len];
	uint32_t  other_info[max_rank_top_type + 1];
    uint8_t   other_info_2_db_flag;
	uint8_t   other_info_2[max_rank_top_type + 1][40 + 1];

	char* tmp_session;

	/*! session for holding some info temporarily */
	char		session[];
};

/*! number of bytes allocated for each player_t object */
const int c_player_size			= sizeof(player_t) + 8192 + 4096;//8192;


#pragma pack()

/*! number of bytes available for holding temporarily info */
const int c_avail_sess_len = c_player_size - sizeof(player_t);

/**
  * @brief allocate and add a player to this server
  * @param player
  * @return pointer to the newly added player
  */
player_t* add_player(player_t* player);
/**
  * @brief delete player
  * @param p player to be deleted
  */
void del_player(player_t* p);

/**
  * @brief clear players' battle info whose online server is down
  * @param fd online fd
  */
void clear_players_battle(void* key, void* player, void* userdata);

/**
  * @brief do when battle server crashed
  * @param fd online fd
  */
void do_while_battle_svr_crashed(int btl_fd);


void do_while_home_svr_crashed(int home_fd);
/**
  * @brief get player by user id
  * @param uid user id
  * @return pointer to the player if found, 0 otherwise
  */
player_t* get_player(userid_t uid);

/**
  * @brief get player by socket fd
  * @param fd socket fd
  * @return pointer to the player if found, 0 otherwise
  */
player_t* get_player_by_fd(int fd);

/**
  * @brief a timer function to keep all players alive
  * @return 0
  */
int keep_players_alive(void* owner, void* data);

/**
  * @brief init the no time limit day
  * @param day_limit string config of no time limit day
  * @return 0
  */
int init_time_limit_day(char* day_limit, int type = 0);

/**
  * @brief init the no time double day
  * @param day_double string config of no time double day
  * @return 0
  */
int init_time_double_day(char* day_double);

/**
  * @brief set day time limit for one day
  * @param flag 0 for initilization, 1 for pre-set for the next day in advance
  */
void set_battle_time_limit(uint32_t flag);

int init_income_per_arr();
/**
  * @brief tmp , just for operating activity
  */
bool is_market_activity(player_t* p);

/**
  * @brief traverse all the players and call 'action' on each player
  * @note you should not remove any player in 'action'
  */
void traverse_players(void (*action)(void* key, void* player, void* userdata), void* data);

/**
  * @brief traverse all the players and call 'action' on each player
  * @note you can remove any player safely in 'action'
  */
void traverse_players_remove(void (*action)(player_t* player));

/**
  * @brief get player by some condition
  */
uint32_t get_random_player_list_by_lv(player_t* p, player_t** p_list);

/**
  * @brief initialize players
  */
void init_players();

/**
  * @brief finalize players
  */
void fini_players();

/**
 * @brief pack p's clothes into buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_clothes(const player_t* p, uint32_t waitcmd, void* buf);

/**
 * @brief pack the player's fumo info to btl
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_fumo_to_btl(player_t* p, void* buf);

/**
 * @brief pack the player's buf skill info to btl
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_buf_skill_to_btl(player_t* p, void* buf, uint32_t& buff_cnt);


bool is_buff_actived_on_player(player_t * p, int buff_type);

uint32_t get_player_speed(player_t * p);
/**
 * @brief pack the player's attr which need broadcast to map
 * @param buf
 * @return the idx
 */
int pack_player_lvup_noti(uint8_t* buf, uint32_t lv, uint32_t hp, uint32_t mp);
/**
 * @brief pack the player's attr which need broadcast to map
 * @param buf
 * @return the idx
 */
int pack_player_honor_up_noti(uint8_t* buf, uint32_t honor);

/**
 * @brief pack the player's bass attr 
 * @param buf
 * @return the idx
 */
int pack_player_base_attr(uint8_t* buf, player_t* p, uint32_t coins);

/**
  * @brief send level top limit to player
  * @return 0
  */
void send_lv_limit_mail(player_t* p);

/**
  * @brief send level up to player
  * @return 0
  */
void send_lv_up_mail(player_t* p, uint32_t mail_templet_id);

void add_buff_to_player(player_t*p, uint32_t buff_type, uint32_t mutex_type, uint32_t duration, uint32_t start_tm, uint32_t db_flg = 1);
inline void reset_common_action(player_t* p)
{
	p->action = 0;
	p->direction = 2;

}

/**
 * @brief ÅÐ¶ÏÊÇ·ñÊÇ VIP;
 */
inline bool is_vip_player(player_t* p)
{
	return taomee::test_bit_on(p->vip, cur_vip);
}

/**
 * @brief ÅÐ¶ÏÊÇ·ñÊÇ Äê·ÑVIP;
 */
inline bool is_vip_year_player(player_t* p)
{
    return (is_vip_player(p) && taomee::test_bit_on(p->vip, year_vip));
}

inline bool has_player_opened_vip(player_t *p)
{
	//return taomee::test_bit_on(p->vip, prev_vip);
	return p->vip != 0;
}

/**
 * @brief ÅÐ¶ÏÊÇ·ñÊÇ VIP;
 */
inline bool is_power_user_player(player_t* p)
{
	return taomee::test_bit_on(p->power_user, 1);
}

inline bool is_passed_stage(player_t* p, uint32_t stage_id)
{
	return (p->fumo_stage_map->count(stage_id) > 0);
}
/**
 * @brief ÊÇ·ñÒÑ¾­×ö¹ýÃ¿ÈÕÏÞÖÆ»î¶¯
 */
inline bool is_done_restriction_active(player_t* p, uint32_t id)
{
    return p->daily_restriction_count_list[id - 1];
}

inline bool is_client_need_check_pay_passwd(player_t* p)
{
	return (p->client_buf[3]);
}

//---------------------------------------------------------------
// For platform of statstics
//---------------------------------------------------------------

inline void add_player_timer(player_t* p)
{
	ADD_TIMER_EVENT(p, keep_players_alive, 0, get_now_tv()->tv_sec + 50);
}

inline uint32_t find_player_stage_grade(player_t* p, uint32_t stageid, uint32_t diff)
{
	if (diff > max_stage_difficulty_type) {
		return 0;
	}
	std::map<uint32_t, fumo_stage_info_t>::iterator it = 
			p->fumo_stage_map->find(stageid);
	if (it != p->fumo_stage_map->end()) {
		return it->second.grade[diff - 1];
	}
	return 0;
}

/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_universal_interface_1(uint32_t cmd, uint32_t id, uint32_t cnt);

/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_universal_interface(uint32_t cmd, uint32_t id);

/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_universal_interface_2(uint32_t cmd, uint32_t id, uint32_t cnt1, uint32_t cnt2);

enum {
    monster_drop = 1,
    swap_action  = 2,
    trade_action = 3,
    task_reward  = 4,
    store_trade  = 5,
    system_reward= 6,
    vip_receive  = 7,
    item_cost_enum = 8, // The last == COST
};


/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_item_universal_interface(uint32_t id, int channel_id, uint32_t cnt);

/**
 * @brief add here for platform of statstics
 */
void do_stat_log_child_lv_num(player_t* p);

/**
 * @brief add here for platform of statstics
 */
void do_stat_log_achieve_child_num(player_t* p);

void cache_a_pkg(player_t *p, char* buf, uint32_t buflen);

/**
 * @brief calc and init a player's battle attr
 * @param p
 */
void calc_player_attr(player_t* p);

bool is_achieve_amb_lv(player_t* p);

void set_once_bit_on(player_t* p, uint32_t pos);

void set_player_event_trigger(player_t* p, uint32_t times, uint32_t block, uint32_t victory);

void reduce_exploit(player_t* p, uint32_t reduce_exploit);



//------------------------------------------------------------------
// Cmds
//------------------------------------------------------------------

struct query_forbiden_friend_flag_rsp_t
{
	uint32_t err_code;
	uint32_t userid;
	uint32_t flag;
};

int forbiden_add_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int db_forbiden_add_friend(player_t* p, uint32_t flag);

int db_query_forbiden_friend_flag(player_t* p, uint32_t mimi_id);

int db_query_forbiden_friend_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);



/**
 * @brief player get simple information
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_usr_simple_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief player get detail information
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_usr_detail_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief player changes nickname
  * @param p the player who tends to change nickname
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int change_usr_nickname_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get max times chapter
  * @param p the player who scaned the times 
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_max_times_chapter_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player changes nickname
  * @param p the player who scaned the times
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_max_times_chapter_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player set it's client buf
  * @param p the player who tends to change nickname
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_client_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player get it's client buf
  * @param p the player who tends to change nickname
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_client_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player get player other info
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0
  */
int player_get_other_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//------------------------------------------------------------------
// request to dbproxy
//------------------------------------------------------------------
/**
  * @brief get player's info from db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_get_player(player_t* p, userid_t uid);
/**
  * @brief set role info:exp,lv,coins...to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_set_role_base_info(player_t* p);

/**
  * @brief add amb chieve num to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see add_amb_achieve_num_callback
  */
int db_add_amb_achieve_num(uint32_t parentid);

/**
  * @brief get fumo info from db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_get_fumo_info(player_t* p);

/**
  * @brief get the data of double experience from database
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_get_double_exp_data(player_t* p);

int db_add_buff(player_t* p, uint32_t buf_type, uint32_t duration, uint32_t mutex_type, bool ret_flg = true);

void del_buff_on_player(player_t * p, uint32_t buf_type);

void db_del_buff(player_t * p, uint32_t buf_type);

//bool is_buff_actived_on_player(player_t * p, uint32_t buf_type);

/**
  * @brief set the data of double experience from database
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_set_double_exp_data(player_t* p, uint32_t dexp_time);

/**
  * @brief set stage info to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_set_stage_info(player_t* p, 	uint16_t stageid, uint32_t bossid, uint32_t difficulty, 
	uint8_t stage_grade, uint32_t damage);

//------------------------------------------------------------------
// callback for handling package return from dbproxy
//------------------------------------------------------------------
/**
  * @brief callback for handling player's info returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_get_player_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for handling getting simple info returned from dbproxy
 * @param p the requester
 * @param uid id of the requestee
 * @param body body of the returning package
 * @param length of body
 * @param ret errno returned from dbproxy
 * @return 0 on success, -1 on error
 */
int db_get_usr_simple_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for handling getting detail info returned from dbproxy
 * @param p the requester
 * @param uid id of the requestee
 * @param body body of the returning package
 * @param length of body
 * @param ret errno returned from dbproxy
 * @return 0 on success, -1 on error
 */
int db_get_usr_detail_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling nickname change action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_change_usr_nickname_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

 /**
  * @brief callback for get fumo info
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_fumo_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
 /**
  * @brief callback for get double exp time info
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_double_exp_data_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
  * @brief callback for set double exp time info
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_set_double_exp_data_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_add_buff_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for get client buf
 */
int db_get_client_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for get max times chapter
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_max_times_chapter_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int set_player_show_state_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int db_set_player_show_state(player_t* p, uint32_t player_state);


int db_set_player_open_box_times(player_t* p, uint32_t times);


int get_donate_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int db_get_donate_count(player_t* p);
int db_get_donate_count_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret);



int db_get_friend_list(player_t *p);
int db_get_friend_list_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret);

int db_get_user_public_info(player_t *p);

int db_get_user_public_info_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret);

int get_player_hero_top_cmd(player_t *p, uint8_t *body, uint32_t bodylen);

int db_get_player_hero_top(player_t *p);

int db_get_player_hero_top_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret);

int db_set_player_hero_top_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret);

int db_set_player_hero_top_info(player_t *p);


int db_add_donate(player_t* p, int add_value, bool callback = false);
int send_online_tm_notification(void *owner, void *data);
int limit_players_online_time(void * owner, void *data);

int get_offline_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int exchange_coins_exploit_cmd(player_t *p, uint8_t * body, uint32_t bodylen);
int db_exchange_coins_exploit_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


int insert_offline_fumo_coins_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int start_offline_mode_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int end_player_offline_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int end_summon_offline_cmd(player_t *p, uint8_t * body, uint32_t bodylen);


int db_get_offline_info_callback(player_t * p, uint32_t uid, void * body, uint32_t bodylen, uint32_t ret); 

//int list_wuseng_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);
//
//int db_list_wuseng_info_callback(player_t * p, uint32_t uid, void * body, uint32_t bodylen, uint32_t ret); 
//
//int db_set_wuseng_info(player_t *p, uint32_t pass_time);

int db_get_other_info(player_t *p);

int db_get_other_info_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret);

int get_player_community_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int db_get_player_community_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret);

int db_set_role_info(player_t* p, const char* column_str, uint32_t value);

/**
  * @brief 
  * @param 
  * @return ·µ»Ø¸ÃÀàÐÍ¸üÐÂ¹ýºóµÄÖµ
  */
uint32_t set_player_other_info(player_t *p, uint32_t type, uint32_t value);

void add_player_other_info(uint32_t uid, uint32_t role_tm, uint32_t type, uint32_t add_val);

int list_other_active_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

/**
  * @brief 
  * @param ·µ»Ø¸ÃÀàÐÍµÄÖµ
  * @return 
  */
uint32_t get_player_other_info_value(player_t *p, uint32_t type);


bool is_player_have_friend(player_t* p, uint32_t friend_id);
void erase_player_friend(player_t* p, uint32_t friend_id);
void add_player_friend(player_t* p, uint32_t friend_id);


int handle_player_normal_logic(void * p, void * data);

void send_player_synthesis_attribute_to_db(player_t *p);

uint32_t calc_and_save_player_attribute(player_t *p, uint32_t guard = 1);
//------------------------------------------------------------------
// struct
//------------------------------------------------------------------
#pragma pack(1)


struct get_player_rsp_t {
	uint32_t	role_tm;
	uint32_t	role_type;
	uint32_t	power_user;
	uint32_t    flag;
	uint32_t    regtime;
	char		nick[max_nick_size];
    uint32_t    nick_change_tm;
	uint32_t    vip;
	uint32_t    vip_month_cnt;
	uint32_t    vip_begin_tm;
	uint32_t    vip_end_tm;
	uint32_t    vip_point;
	uint32_t    sword_value;
	//uint32_t    vip_extern_item_bag_grid_count;
	uint32_t    vip_extern_warehouse_grid_count;
	uint32_t	skill_point;
    uint32_t    map_id;
    uint32_t    xpos;
    uint32_t    ypos;
	//uint32_t	color;
	uint32_t	lv;
	uint32_t	exp;
	uint32_t	allocator_exp;
	uint32_t	hp;
	uint32_t	mp;
	uint32_t    coins;
	
	uint32_t    honor;
	uint32_t    exploit;
	uint32_t    win_times;
	uint32_t    lose_times;
	uint32_t    max_conti_win_times;
	uint32_t	fumo_points_today;
	uint32_t	fumo_points_total;
	uint32_t	fumo_tower_top;
	uint32_t 	fumo_tower_used_tm;
	uint32_t    ol_count;
	uint32_t    ol_today;
	uint32_t    ol_last;
	uint32_t    ol_time;
	uint8_t		unique_item_bit[c_unique_item_bit_size];
	uint8_t		itembind[max_itembind_len];
	
	uint8_t		amb_status;
	userid_t	parentid;
	uint32_t	child_cnt;
	uint32_t	achieve_cnt;
	uint8_t		once_bit[max_once_bit_size];
	uint8_t		act_record[max_limit_active_times];
	uint32_t	double_exp_time;
	uint32_t    day_flag;
    uint32_t    max_times_chapter;
	uint32_t    show_state;
	uint32_t    open_box_times;
	uint32_t    strengthen_cnt;
	uint32_t    achieve_point;
	uint32_t    team_id;
    uint8_t     god_guard[80];
	uint32_t    last_update_ap_tm;
	uint32_t    achieve_title;
	uint32_t    forbiden_add_friend_flag;
	uint32_t 	home_active_point;
	uint32_t 	home_last_tm;
	uint32_t    vitality_point;
	uint32_t	offline_msg_size;
	uint32_t    clothes_cnt;
	uint32_t    skill_cnt;
	uint32_t	done_tasks_num;
	uint32_t	going_tasks_num;
	uint32_t	cancelled_daily_tasks_num;
	uint32_t	packs_clothes_cnt;
	uint32_t	packs_item_cnt;
	uint32_t	fumo_stage_cnt;
	uint32_t	db_buff_cnt;

	uint8_t     buf[];
	//char		tasklist[c_tasklist_size];
};
struct get_player_wear_clothes_t {
	uint32_t clothes_id;
	uint32_t unique_id;
	uint32_t duration;
	uint32_t attirelv;
	uint32_t gettime;
	uint32_t timelag;
};

struct get_player_skill_bind_t {
	uint32_t skill_id;
	uint32_t lv;
	uint32_t key;
};

struct get_player_done_task_t {
	uint32_t task_id;
};

struct get_player_going_task_t {
	uint32_t task_id;
	uint8_t  ser_buf[20];
	uint8_t	 cli_buf[40];
};

struct get_player_cancel_daily_task_t {
	uint32_t task_id;
};

struct get_player_pack_clothes_t {
	uint32_t clothes_id;
	uint32_t unique_id;
	uint32_t duration;
	uint32_t attirelv;
	uint32_t gettime;
	uint32_t timelag;
};
struct get_player_pack_item_t {
	uint32_t item_id;
	uint32_t cnt;
};
struct get_player_fumo_stage_t {
	uint32_t stage_id;
	uint8_t  grade[6];
};

struct get_player_db_buff_t {
	uint32_t buff_type;
	uint32_t duration;
	uint32_t mutex_type;
	uint32_t start_tm;
};

struct clothes_info{
    uint32_t    clothesid;
    uint32_t    gettime;
    uint32_t    endtime;
    uint32_t    attirelv;
};

struct get_user_simple_info_rsp_t {
	uint32_t	id;
	uint32_t	role_tm;
	uint32_t	role_type;
    //uint32_t	hp;
	//uint32_t	mp;

	uint32_t	power_user;
	uint32_t    lv;
    uint8_t		nick[max_nick_size];
	//uint32_t	color;
	//added by cws 20120509
    uint8_t		datas[ACTIVE_BUF_LEN];
    
    uint32_t	vip;
    uint32_t	m_base_value;
	uint32_t	vip_begin_tm;
	uint32_t	vip_end_tm;
	uint32_t	clothes_cnt;
    clothes_info    clothes[];
};

struct get_user_detail_info_rsp_t {
	uint32_t	id;
	uint32_t	lv;
	uint32_t	exp;
	//uint32_t	hp;
	//uint32_t	mp;
	uint32_t	honor;
	uint32_t	exploit;
	uint32_t	win_times;
	uint32_t	lose_times;
	uint32_t	max_conti_win_times;
    uint32_t    using_achieve_title;
    uint8_t     datas[ACTIVE_BUF_LEN];
};

struct fumo_stage_t {
	uint32_t	stage_id;
	uint8_t		grade[max_stage_difficulty_type];
};

struct get_fumo_info_rsp_t {
	uint32_t	count;
	struct fumo_stage_t	fumo_stage[];
};

struct get_double_exp_data {
    uint32_t day_flag;
    uint32_t double_exp_time;
};

struct add_buf_rsp_t {
	uint32_t	type;
	uint32_t    duration;
	uint32_t 	mutex_type;
	uint32_t 	start_tm;
};

struct exchange_coins_exploit_t {
	uint32_t coins;
	uint32_t exploit;
};

struct friend_t {
	uint32_t userid;
	uint32_t type;
};

struct get_friend_rsp_t {
	uint32_t friend_cnt;
	friend_t friends[];
};

struct get_user_public_info_rsp_t {
	uint8_t sex;//	uint8	1	¿¿¿¿:0 ¿¿¿2 ¿¿¿4 ¿¿¿¿
	uint32_t birthday;//	uint32	4	¿¿¿¿¿¿¿¿¿¿8¿¿¿0-9¿¿¿¿:1900~¿¿¿¿¿¿¿
	char telephone[16];//	char	16	¿¿¿¿¿¿¿¿¿¿
	char mobile[15];//	char	15	¿¿¿¿¿¿¿¿¿¿:11¿¿¿¿¿¿¿¿3¿¿¿¿¿¿¿¿¿****
	char post_code[6];//	char	6	¿¿¿¿¿6¿¿¿0-9
	uint16_t addr_province;//	uint16	2	¿¿¿¿¿¿¿¿¿¿
	uint16_t addr_city;//	uint16	2	¿¿¿¿¿¿¿¿¿¿
	char addr[192];//	char	192	¿¿¿¿¿¿¿¿
	char interest[192];//	char	192	¿¿¿¿¿¿¿¿¿¿
	char real_name[15];//	char	15	¿¿¿¿:¿¿¿¿, ¿¿¿****
	char personal_id[18];//	char	18	¿¿¿¿¿:¿¿¿15¿¿18¿0-9 a-z A-Z¿¿¿¿¿¿¿3¿¿¿4¿¿¿¿¿****
	char nick_name[90];//	char	90	¿¿¿¿¿¿¿¿
	char signature[380];//	char	384	¿¿¿¿¿¿¿¿¿¿
};

struct hero_top_rsp_t {
	uint32_t sort;
};

struct donate_rsp_t{
	uint32_t red;
	uint32_t black;
};

struct get_other_info_rsp_item_t {
	uint32_t type;
	uint32_t value;
};

struct get_other_info_rsp_t {
	uint32_t cnt;
	get_other_info_rsp_item_t item[]; 
};

struct get_player_community_info_rsp_t {
	uint32_t team_id;
	uint32_t friend_cnt;
	uint32_t summon_cnt;
	uint32_t card_cnt;
	uint32_t achieve_cnt;
	uint32_t fumo_points;
};

#pragma pack()

#endif // KF_PLAYER_HPP_

