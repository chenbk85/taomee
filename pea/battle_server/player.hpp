#ifndef PLAYER_HPP_
#define PLAYER_HPP_


#include <list>
#include <map>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>	
#include <async_serv/dll.h>
}

#include "pea_common.hpp"
#include "fwd_decl.hpp"
#include "proto.hpp"
#include "object.hpp"
#include "battle_impl.hpp"
#include "player_state.hpp"
#include "player_skill.hpp"
#include "skill_imme_effect.hpp"
#include "battle_statistics_data.hpp"
#include "pet.hpp"

using namespace std;
using namespace taomee;

class bullet;
enum 
{
	btl_player_no_state = 0,
	btl_player_begin_load_state,
	btl_player_load_ok_state,
	btl_player_running_state,
	btl_player_end_state
};

enum 
{
	btl_player_win = 1,
	btl_player_lose,
	btl_player_draw
};

class Player : public Object, public taomee::EventableObject
{
public:
	Player(uint32_t user_id, fdsession_t* fdsession);
	~Player();
	
public:	
	int 			fd;
	fdsession_t*    fdsess;

	uint16_t    	waitcmd;
	GQueue*         pkg_queue;

	ObjectHook  	awaiting_hook;
public:	
	void clear_waitcmd()
	{
		waitcmd = 0;	
	}

public:
	//玩家属性部分
	uint32_t id;
	uint32_t server_id;
	uint32_t role_tm;
	uint8_t  nick[MAX_NICK_SIZE];
    uint32_t eye_model;
	uint32_t resource_id;

	base_exp2level_calculator*  exp2level_calc;
	
	obj_attr       *attr;
	uint32_t       team;                 //玩家阵营
	uint32_t       btl_turn_value;       //战斗出手值

	bool           init_attr();

	bool           final_attr();

	uint32_t       get_attr_value(uint16_t attr_type);

	void           set_attr_value(uint32_t attr_type, uint32_t value);

	uint32_t       get_merge_attr_value(uint16_t attr_type);

	void 		   set_merge_attr_value(uint32_t attr_type, uint32_t value);
public:
	void           init_player_attr(Battle* btl, online_proto_syn_player_info_in* p_in);

	void 		   on_hit_target(bullet* p_bullet, Player* target);
	
	void 		   on_hit(bullet* p_bullet, Player* atker);
	
	void           inc_hp(uint32_t value);

	void           dec_hp(uint32_t value);

	void           set_turn_value(int value)
	{
				   btl_turn_value = value;	
	}

	int32_t        get_turn_value()
	{
				   return btl_turn_value;	
	}
public:
	//战斗积分计算和胜负
	uint32_t       player_win_flag;
	uint32_t       total_atk_count;
	uint32_t       total_hit_count;
	uint32_t       total_damage;
	uint32_t       total_exp;
	uint32_t       total_extern_exp;

	void inc_total_atk_count()
	{
		total_atk_count ++;	
	}

	void inc_total_hit_count()
	{
		total_hit_count ++;	
	}

	void inc_total_damage(uint32_t damage)
	{
		total_damage += damage;
	}

	std::map<uint32_t, target_exp_data> *target_exp_data_maps;

	void        set_player_win_flag(uint32_t flag);

	uint32_t    get_player_win_flag();

	bool        init_btl_statistics_data();
	
	bool        final_btl_statistics_data();
	
	float       get_team_exp_factor(uint32_t battle_mode);

	float       get_win_exp_factor(uint32_t win_flag);
	
	float       get_level_exp_factor(uint32_t pet_lv, uint32_t target_lv);
public:
	//战斗
	Battle*         btl;
	uint32_t        btl_player_state;
	uint32_t        btl_round_state;

	void set_btl_round_state(uint32_t state)
	{
		btl_round_state = state;	
	}

	uint32_t get_btl_round_state()
	{
		return btl_round_state;	
	}

	void change_btl_player_state(uint32_t state)
	{
		btl_player_state = state;	
	}

	bool check_btl_player_state(uint32_t state)
	{
		return btl_player_state == state;	
	}

	bool            check_in_battle();
	void            attach_battle(Battle* btl_ptr);
	void            dettach_battle();
public:
	//skill部分			
	std::map<uint32_t, player_skill*>* skill_maps;	
	uint32_t cur_skill_lv_id;
	uint32_t default_skill_lv_id;

	bool init_player_skill(online_proto_syn_player_info_in * p_in);
	c_player_state_mrg* player_state_mrg;

public:
	void process_timer(struct timeval cur_time, uint32_t type);
public:
	//攻击倒计时计时器
	base_switch_timer  ready_attack_timer;
	//攻击调整计时器
	base_switch_timer  attack_timer;

public:
    /////////// 精灵 ///////////////////////////
    c_pet * pet;

    int init_player_pet(online_proto_syn_player_info_in * p_in);
};



extern std::map<uint32_t, Player*> all_players;








Player* get_player(uint32_t userid);

Player* add_player(uint32_t userid, fdsession_t* fdsess);

bool    is_player_exist(uint32_t userid);

void 	del_player(Player* p);

void 	clear_players(int fd);

void    all_players_routing(struct timeval cur_time);

void	player_timer(Player* p, struct timeval cur_time);

#endif
