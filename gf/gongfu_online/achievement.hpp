#ifndef _ACHIEVEMNET_H_
#define _ACHIEVEMNET_H_

#include"fwd_decl.hpp"
#include <vector>
#include <map>
using namespace std;


#define MAX_ACHIEVEMENT_COUNT 1000

struct player_t;

struct player_stat_t {
	uint32_t stat_id;
	uint32_t stat_cnt;
}__attribute__((packed));

struct db_player_stat_rsp_t {
	uint32_t total_cnt;
	player_stat_t stat_info[];
}__attribute__((packed));

struct stat_data_t {
	uint32_t stat_id;
	uint32_t achieve_id;
	uint32_t need_cnt;
};

//相关的硬统计需求， ID=1 表示玩家杀怪数量, ID=2表示PVE组队通关次数, ID=3表示使用复活草的数目 
//ID=4 徒弟进阶成功的统计 ID=5 购买收费道具的个数, ID=6竞技场获胜统计， ID=7 2V2PK获胜统计, ID=8 邀请码成功人数
void add_player_stat_cnt(player_t * p, uint32_t stat_id, uint32_t add_cnt);

void db_get_player_stat_info(player_t * p);

int db_get_player_stat_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int get_player_stat_info(player_t * p, int stat_id);


typedef struct achievement_data
{
	achievement_data()
	{
		id_ = 0;
		add_point_ = 0;
		brodcast_ = 0;
		title_ = 0;
	}
	uint32_t id_;
	uint32_t add_point_;
	uint32_t brodcast_;
	uint32_t title_;
//	char name_[1024];
//	char xiaoba_url_[1024];
}achievement_data;

class achievement_data_mgr
{
public:
	achievement_data_mgr(){}
	~achievement_data_mgr(){}

public:
	bool init(const char* xml);
	bool init_ex(const char* xml);
	bool final();
	bool is_data_exist(uint32_t id);
	achievement_data* get_achievement_data_by_id(uint32_t id);
	uint32_t  get_add_achieve_point_by_id(uint32_t id);
	stat_data_t * get_stat_data_by_id(uint32_t stat_id);

private:
	std::map<uint32_t, achievement_data*> data_maps;	
	std::map<uint32_t, stat_data_t*> stat_maps;
};


typedef struct player_achievement_data
{
	player_achievement_data(int id, uint32_t time)
		: id_(id),
		  get_time_(time)
	{ }

	int id_;
	uint32_t get_time_;
}player_achievement_data;


bool init_player_achievement_data(player_t* p);
bool final_player_achievement_data(player_t* p);
void set_player_achievement_data(player_t* p, int id, uint32_t get_time);
bool is_player_achievement_data_exist(player_t* p, int id);
//uint32_t get_player_achievement_data_count(player_t* p);


//typedef struct achievement_callback_handle
//{
//	achievement_callback_handle()
//	{
//		id = 0;
//		p_check_func = NULL;
//		p_do_func = NULL;
//	}
//
//	uint32_t id;
//	check_func   p_check_func;
//	do_func      p_do_func;
//}achievement_callback_handle;
//
//bool init_achievement_callback_handles();
//bool final_achievement_callback_handles();
//bool register_achievement_callback_handle(uint32_t id,  check_func func1,  do_func func2);
//bool unregister_achievement_callback_handle(uint32_t id);

typedef struct achievement_data_element
{
	achievement_data_element()
	{
		achieve_type = 0;
		get_time = 0;
	}
	uint32_t achieve_type;
	uint32_t get_time;
}achievement_data_element;

typedef struct get_achievement_data_list_rsp_t
{
	uint32_t count;
	achievement_data_element datas[];
}get_achievement_data_list_rsp_t;


//uint32_t calc_player_medal_count(player_t* p);
//bool     check_stage_passed(player_t* p, uint32_t stage_id);
//bool     check_all_attire_quality_level(player_t* p, uint32_t level);

int db_get_achievement_data_list(player_t* p);
int db_replace_achievement_data(player_t* p, uint32_t achieve_type, uint32_t get_time, uint32_t add_achieve_point);
int db_get_achievement_data_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


int get_achievement_data_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int send_player_achievement_data_list(player_t* p);
int send_add_achievement_data(player_t* p, uint32_t achieve_type, uint32_t get_time, uint32_t achieve_point);
int achievement_stat_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen); 

int broadcast_add_achievement(player_t* p, uint32_t achieve_type);
void proc_achieve_mcast(const void * body, uint32_t bodylen);

void player_gain_achieve(player_t * p, int achieve_id);

enum achieve_event_type_t {
	event_pass_stage = 1,
	event_task_finish = 2,
	event_ranker_range = 3,
	event_kill_monster = 4,
	event_pvp_win  = 5,
	event_use_item = 6,
	event_gain_item = 7,
	event_gain_clothes = 8,
	event_summon_info = 9,
	event_team_info  = 10,
	event_clothes_info = 11,
};

struct achieve_event {
   achieve_event(int id, int flag1 = 0, 
		   int flag2 = 0, int flag3 = 0, int flag4 = 0) 
	   : event_id(id), 
	     flag1_(flag1), 
		 flag2_(flag2),
		 flag3_(flag3),
		 flag4_(flag4)
	{}
   int event_id; 
   int flag1_; 
   int flag2_;
   int flag3_;
   int flag4_;
};

void process_pve_win_achieve_logic(player_t *p, int stage, int btl_type, int stage_grade, int diff = 1);
void proc_kill_monster_achieve_logic(player_t * p, int monster_id, int diff, int battle_killed_cnt);
void proc_task_achieve_logic(player_t * p, int task_type, int task_id);
enum ranker_type {
	fumo_ranker_type = 1, //伏魔塔排行榜
	pre_normal_ranker_type = 2, //ranker_top里面的前通用排行, 
	pre_team_ranker_tyoe = 3, 
	normal_ranker_type = 4, //redis-serve cache svr 类个人排行榜 
	normal_team_ranker_type = 5,//reid-svr chec svr 类团队排行榜
	ranker_contest_final  = 6,//天下第一比武大会， id=1 为高级争霸排名第一
};

void proc_ranker_range_achieve_logic(player_t *p, int ranker_type, int range_id, int self_range);


enum {
	btl_use_item = 1,
	swap_del_item = 2,
	task_use_item = 3,
	summon_scroll_skill = 4,
	normal_use_item = 5,
};

void proc_use_item_achieve_logic(player_t *p, int item_id, int cnt, int channel, int swap_id = 0);

enum {
	gain_item_vip_shop = 1,
	gain_item_by_swap = 2,
};	

void proc_gain_item_achieve_logic(player_t *p, int item_id, int cnt, int channle, int swap_id = 0);

enum {
	gain_clothes_vip_shop = 1,
	gain_clothes_compose = 2,
	strengthen_update_attire = 3,
};

void proc_gain_clothes_achieve_logic(player_t *p, int clothes_id, int lv, int channel, int swap_id =0);
void proc_pvp_win_achieve_logic(player_t *p, int pvp_lv, int win_status);

void proc_summon_achieve_logic(player_t *p);

void proc_player_team_achieve_logic(player_t *p);

void proc_player_clothes_logic(player_t * p);

//void process_pvp_win_achieve_logic(player_t * p, uint32_t pvp_lv, uint32_t win_status);
#endif
