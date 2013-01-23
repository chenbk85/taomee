#ifndef BATTLE_IMPL_HPP_
#define BATTLE_IMPL_HPP_

#include <algorithm>
#include <vector>
#include <list>
#include <functional>
#include <boost/pool/object_pool.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include "battle_round.hpp"
#include "map_data.hpp"
#include "pea_common.hpp"
#include "stage.hpp"

using namespace std;
using namespace taomee;

#define BATTLE_ATTACK_ANIMATION_TIME (2000) //攻击前的出手动画延时
#define BATTLE_READY_ATTACK_TIME 	(12000) //攻击时间到计时            
#define BATTLE_ATTACK_TIME          (6000)  //攻击倒计时
#define PHY_MODEL_SIMULATE_TIME  	(10)    //物理容器模拟最小时间间隔
#define BATTLE_ROUND_DELAY_TIME     (2000)  //每回合延迟2秒
#define GRAVITY_ACCELERATION    	(0.98)

// 人物行动步骤
enum player_action_step
{
    // 默认空闲
    player_action_step_free = 0,
    //攻击前
    player_action_step_before_atk = 1,
    //攻击后
    player_action_step_after_atk = 2,
};



enum
{
	battle_round_no = 0,
	battle_round_start = 1,	
	battle_round_end
};

enum 
{
	battle_no_state = 0,
	battle_wait_load_state = 1,
	battle_running_state,
	battle_destroy_state,
	battle_end_state
};

enum battle_mode
{
	single_pvp_mode = 1,
	team_pvp_mode   = 2,
	single_pve_mode = 3,
	team_pve_mode   = 4
};

class Player;
class attack_obj;

class Battle: public MemPool, public EventableObject
{
public:	
	Battle(Player* creater, uint32_t battle_mode, uint32_t round_time, uint32_t stage_id, uint32_t player_count);
	virtual ~Battle();
public:
	uint32_t get_battle_id()
	{
		return btl_id;	
	}
	uint32_t get_battle_mode()
	{
		return btl_mode;	
	}
	uint32_t get_battle_stage_id()
	{
		return btl_stage_data->map_id;	
	}
	
	void set_battle_begin_wait_load_time(struct timeval cur_time)
	{
		btl_begin_wait_load_time = cur_time;	
	}

	void set_max_player_count(uint32_t player_count)
	{
		max_player_count = player_count;	
	}
public:
	//战斗玩家部分
	bool 		add_player(Player* p);
	bool 		del_player(uint32_t player_id);
	bool    	is_player_exist(uint32_t player_id);
	Player* 	get_player_by_id(uint32_t player_id);
	void    	set_creater(Player* p);
	Player* 	get_creater();
	uint32_t 	get_cur_players_count();
	bool        check_creater(Player* p);


	Player*             creater;
	std::list<Player*>  playerlist;
	uint32_t            max_player_count;           //最大战斗人数


	bool init_btl_players();

	bool final_btl_players();
	
	void player_routing(struct timeval cur_time);

public:
    virtual void on_battle_timer( struct timeval cur_time );
	virtual bool init_battle(){ return true;}
	virtual bool final_battle(){ return true;}
public:
	//战斗回合计算部分
	virtual Player* calc_player_turn();
	virtual Player* calc_player_turn_again();

	void            set_cur_turn_player(Player* p);
	Player*         get_cur_turn_player();
	
	void            set_last_turn_player(Player* p);
	Player*         get_last_turn_player();
	
	bool    		check_player_turn(Player* p);
	bool            del_player_from_queue(uint32_t uid);

	Player*             cur_turn_player;
	Player*             last_turn_player;

	std::list<Player*>  btl_turn_queue;
	uint32_t            cur_round_num;	

	base_switch_timer   round_delay_timer;//回合延时计时器

	void init_round_delay_timer( struct timeval cur_time, uint32_t delay_time);

	bool check_round_delay_not_finish( struct timeval cur_time);

	void goto_next_round( struct timeval cur_time, uint32_t delay_time);

	void inc_round_num();

	void set_all_player_round_state(uint32_t state);
	
	bool check_all_player_round_state(uint32_t state);

public:
	uint32_t 			btl_id;
	uint32_t 			btl_mode;
	uint32_t            btl_state;

	struct timeval      btl_begin_time;      		//战斗开始时间戳
	struct timeval      btl_end_time;               //战斗结束时间戳 

	uint32_t            btl_wait_load_time;  		//战斗创建后，等待玩家加载数据的时间
	struct timeval      btl_begin_wait_load_time;   //战斗开始加载时间戳

	void init_btl_begin_time();
public:
	//战斗场景描述
	stage_data*        btl_stage_data;              //战斗场景描述文件	

	stage_data*        get_stage_data();

	bool init_btl_stage_data( uint32_t battle_map_id);

	bool final_btl_stage_data();

	bool random_init_btl_stage_map();
public:
	//战斗地图表述
	map_data*           btl_map_data;               //战斗地图描述文件

	map_data*           get_btl_map_data();         

	uint32_t            get_btl_map_id();

	bool init_btl_map_data(uint32_t battle_map_id);

	bool final_btl_map_data();                     

	bool player_fall_down(Player* p);

	void all_fall_down(uint32_t bullet_syn_number, uint32_t bullet_times);

	bool check_move_able(Player* p, int32_t new_x, int32_t new_y);
public:
	//物理模型模拟容器,接口与BOX2D相似，方便后期移植
	phy_container       container;       	
	
	bool init_phy_container();                      
	
	bool final_phy_container();
	
	bool add_phy_model_obj(phy_model* p_model);

	bool del_phy_model_obj(phy_model* p_model);

	void phy_container_routing(struct timeval cur_time);
public:
	//攻击过程对象流程，一次攻击过程就是一个attack_obj的对象
	std::list<attack_obj*> attack_obj_list;		

	bool add_attack_obj(attack_obj* obj);

	bool del_attack_obj(attack_obj* obj);

	bool del_attack_obj_by_owner_id(uint32_t owner_id);

	bool init_attack_obj_list();

	bool final_attack_obj_list();

	void attack_obj_routing( struct timeval cur_time);

	bool is_attack_round_finish()
	{
		return attack_obj_list.size() == 0;	
	}
public:
	//战斗数据统计，包括胜负积分啥的,经验计算,伤害统计
	uint32_t win_team;

	void set_win_team(uint32_t team)
	{
		win_team = team;	
	}

	uint32_t get_win_team()
	{
		return win_team;	
	}

	virtual bool calc_battle_statistics_data();

	virtual bool statistics_player_dead(Player* atker, Player* dead);

	virtual bool statistics_player_damage(Player* atker, Player* dead, uint32_t damage);

	bool init_battle_statistics_data();

	bool final_battle_statistics_data();

public:
	void change_btl_state(uint32_t state)
	{
		btl_state = state;
	}
	bool check_btl_state(uint32_t state)
	{
		return btl_state == state;	
	}

	uint32_t get_btl_state()
	{
		return btl_state;	
	}
public:
	bool check_syn_players();

	bool check_load_resource_players();

	void change_all_btl_player_state(uint32_t state);

	virtual bool check_battle_end();
public:

	void broadcast_to_all(void* pkg, uint32_t len, Player* clear_waitcmd = NULL);

public:
	void notify_all_load_resouce();

	void notify_all_battle_start();

	void notify_all_player_move(Player* p);

	void notify_all_player_turn(Player* p);
	
	void notify_all_player_turn_again(Player* p);

	void notify_all_player_attack(Player* p, uint32_t skill_id, uint32_t skill_lv, uint32_t gun_angle, uint32_t muzzle_velocity);

	void notify_all_player_ready_attack(Player* p);

	void notify_all_player_dead_delay(Player* p, uint32_t syn_number, uint32_t bullet_times);

	void notify_all_player_dead(Player* p);

	void notify_all_battle_end();

	void notify_player_battle_end(Player* p);

	void notify_all_player_attack_end(Player* p);

	void notify_all_obj_attr_chg_delay(Player* p, uint64_t type, uint32_t syn_number, uint32_t bullet_times, uint32_t common_flag = 0);

	void notify_all_fall_down(Player* p);

	void notify_all_leave_battle(uint32_t uid);
	
	void notify_all_bullet_hit_delay(uint32_t bullet_id, uint32_t bullet_syn_number, uint32_t bullet_times, uint32_t x, uint32_t y);
	
	void notify_all_progress_bar(uint32_t userid, uint32_t progress);

	void notify_all_fall_down_delay(Player* p, uint32_t syn_number, uint32_t bullet_times);

	void notify_all_select_skill(Player* p, uint32_t skill_id, uint32_t skill_lv);
};

/////////////////////////////////////////////////////////////////////

class single_pve_battle: public Battle
{
public:
	single_pve_battle(Player* creater, uint32_t battle_mode, uint32_t map_id, uint32_t player_count)
	:Battle(creater, battle_mode, 0, map_id, player_count)
	{
		
	}
	void on_battle_timer(){}
	bool init_battle(){return true;}
	bool final_battle(){return true;}
};

////////////////////////////////////////////////////////////////////

class team_pve_battle: public Battle
{
public:
	team_pve_battle(Player* creater, uint32_t battle_mode, uint32_t stage_id, uint32_t player_count)
	:Battle(creater, battle_mode, 0, stage_id, player_count)
	{
		
	}
	void on_battle_timer(){}
	bool init_battle(){return true;}
	bool final_battle(){return true;}
};

class team_pvp_battle: public Battle
{
public:
	team_pvp_battle(Player* creater, uint32_t battle_mode, uint32_t stage_id, uint32_t player_count)
	:Battle(creater, battle_mode, 0, stage_id, player_count)
	{
		
	}
	void on_battle_timer(){}
	bool init_battle(){return true;}
	bool final_battle(){return true;}
};








#endif
