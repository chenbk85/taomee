#ifndef MOLE_ANGEL_H
#define MOLE_ANGEL_H

#define ROUND_COUNT 60
#define ANGEL_CLEAN_TIME  30

typedef struct angel {
	uint32_t auto_increase_id;
	uint32_t angel_id;
	uint32_t pos_id;
	uint32_t growth;
	uint32_t state;
	uint32_t last_caltime;
	uint32_t variate_id;
	uint32_t rate;
	uint32_t diff_mature;//距离成实撵时间
} __attribute__((packed)) angel_t;

typedef struct attire {
	uint32_t attire_id;
} __attribute__((packed)) attire_t;

typedef struct angel_info{
	uint32_t	uid;
	uint32_t	level;
	uint32_t	angel_count;
} __attribute__((packed))angel_info_t;

typedef struct visitor_list{
	uint32_t	uid;
	uint32_t    time;
}__attribute__((packed))visitor_list_t;

typedef struct visitor_angel_info{
	uint32_t	uid;
	uint32_t    time;
	uint32_t	level;
	uint32_t	angel_count;
} __attribute__((packed))visitor_angel_info_t;

typedef struct hot_info{
	uint32_t	id;
	uint32_t    count;
}__attribute__((packed))hot_info_t;

typedef struct friend_count{
	uint32_t	count;
}__attribute__((packed))friend_count_t;

typedef struct paradise_user_id{
	uint32_t	userid;
}__attribute__((packed))paradise_user_id_t;

typedef struct paradise_prop_id{
	uint32_t	prop_id;
}__attribute__((packed))paradise_prop_id_t;

typedef struct monster_info{
	uint32_t	id;
	uint32_t    time;
	uint32_t    count;
	uint32_t    stime;
}__attribute__((packed))monster_info_t;

typedef struct round_monster_info{
    uint32_t    round_id;
	uint32_t    count;
	uint32_t    total_count;//到该轮为止的怪物总数
	uint32_t    exp_limit;
	uint32_t    time_limit;
	monster_info_t    monsters[10];
}__attribute__((packed))round_monster_info_t;

typedef struct td_task_info{
	uint32_t		mapid;
	uint32_t		mode;
	uint32_t		start_time;
	float		    exp_scale;
	float		    rate_scale;
	uint32_t		round_count;
	round_monster_info_t  	round_monster[ROUND_COUNT];
}__attribute__(( packed )) td_task_info_t;

typedef struct role_info{
	uint32_t	id;
	uint32_t    exp;
	uint32_t    rate;
	uint32_t    count;
}__attribute__((packed))role_info_t;

typedef struct td_role_info{
	uint32_t		role_count;
	role_info_t  	roles[501];
}__attribute__(( packed )) td_role_info_t;

typedef struct td_game_end{
	uint32_t barrier_id;
	uint32_t experience;
	uint32_t mode;
	uint32_t is_pass;
	uint32_t level;
	uint32_t honor_type;
    uint32_t honor_id;
}__attribute__((packed))td_game_end_t;

typedef struct angel_timer {
	list_head_t timer_list;
}__attribute__(( packed )) angel_clean_timer_t;


typedef struct honor_item{
	uint32_t		honor;
	uint32_t		item_count;
	item_unit_t  	item[20];
}__attribute__(( packed ))honor_item_t;

typedef struct angel_honor_bonus{
	uint32_t		angel_type;
	uint32_t		honor_count;
	honor_item_t	item_honor[32];
}__attribute__(( packed )) angel_honor_bonus_t;

typedef struct angel_compose{
	uint32_t 		goal_angel_id;
	uint32_t		goal_angel_cnt;
	uint32_t 		fail_angel_id;
	uint32_t		fail_angel_cnt;
	uint32_t 		rate;
	uint32_t 		level;
	uint32_t 		exp;
	uint32_t 		angel_cnt;
	item_unit_t		angel_iut[4];
	uint32_t 		material_cnt;
	item_unit_t 	material_iut[4];
}__attribute__(( packed )) angel_compose_t;

static inline void pkg_angel_info(void* buf, const angel_t* agt, int* indx)
{
	PKG_UINT32(buf, agt->auto_increase_id, *indx);
	PKG_UINT32(buf, agt->angel_id, *indx);
	PKG_UINT32(buf, agt->pos_id, *indx);
	PKG_UINT32(buf, agt->growth, *indx);
	PKG_UINT32(buf, agt->state, *indx);
	//PKG_UINT32(buf, agt->last_caltime, *indx);
	PKG_UINT32(buf, agt->variate_id, *indx);
	PKG_UINT32(buf, agt->rate, *indx);
	PKG_UINT32(buf, agt->diff_mature, *indx);
}
static inline int db_add_paradise_visitors(sprite_t *p, userid_t id, uint32_t layer)
{
	uint32_t dbbuf[] = {p->id, layer};

	return send_request_to_db(SVR_PROTO_ADD_PARADISE_VISTOR, 0, 8, dbbuf, id);
}

enum {
	angel_hot = 1,
	home_hot  = 2,
	farm_hot  = 3,
	house_hot = 4,
	restaurant_hot = 5,
	classroom_hot  = 6,
	knightcard_hot = 7,//森林骑士卡牌
	fightcard_hot  = 8,//骑士对战卡牌
};


enum TD_AWARD_TYPE
{
	//挑战赛
	TAT_PL1		= 0,
	TAT_PL2,
	TAT_SL1,
	TAT_SL2,
	TAT_SL3,
	TAT_MAX
};

typedef struct td_game_award_s
{
	uint32_t item_id;
	uint32_t item_cnt;
	uint32_t rate[TAT_MAX];
}td_game_award_t;

#define TD_CHALLENGE_GAME_MAX_ITEM_KIND			10					//奖励物品最大类型数

typedef struct td_game_task_info_s
{
	uint32_t award_cnt;
	td_game_award_t award_info[TD_CHALLENGE_GAME_MAX_ITEM_KIND];
	uint32_t total_rate[TAT_MAX];
}td_game_task_info_t;

static inline int db_add_hot_intem(sprite_t *p, userid_t id, uint32_t item_id)
{
	uint32_t dbbuf[] = {item_id};
	//DEBUG_LOG("----------db_add_hot_intem id:%u", item_id);
	return send_request_to_db(SVR_PROTP_HOT_STATISTICS, 0, 4, dbbuf, id);
}


int get_paradise_cmd(sprite_t* p, const uint8_t* body, int len);
int get_paradise_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_paradise_store_items_cmd(sprite_t* p, uint8_t* body, int len);
int get_paradise_store_items_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_nimsbus_cmd(sprite_t* p, uint8_t* body, int len);
int add_nimsbus_callback(sprite_t* p, uint32_t id, char* buf, int len);

int feed_angle_seed_cmd(sprite_t* p, const uint8_t* body, int len);
int feed_angle_seed_callback(sprite_t* p, uint32_t id, char* buf, int len);

int build_angle_contract_cmd(sprite_t* p, const uint8_t* body, int len);
int build_angle_contract_callback(sprite_t* p, uint32_t id, char* buf, int len);

int free_angle_in_paradise_cmd(sprite_t* p, const uint8_t* body, int len);
int free_angle_in_paradise_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_contract_angles_cmd(sprite_t* p, uint8_t* body, int len);
int get_contract_angles_callback(sprite_t* p, uint32_t id, char* buf, int len);

int gain_game_angles_cmd(sprite_t* p, uint8_t* body, int len);
int gain_game_angles(sprite_t* p);

int free_captured_angle_in_paradise_cmd(sprite_t* p, const uint8_t* body, int len);
int free_captured_angle_in_paradise_callback(sprite_t* p, uint32_t id, char* buf, int len);

int make_angle_fallowed_cmd(sprite_t* p, const uint8_t* body, int len);
int make_angle_fallowed_callback(sprite_t* p, uint32_t id, char* buf, int len);

int change_to_angle_cmd(sprite_t* p, const uint8_t* body, int len);
int change_to_angle_callback(sprite_t* p, uint32_t id, char* buf, int len);

int change_background_cmd(sprite_t* p, const uint8_t* body, int len);

int get_friend_paradise_info_cmd(sprite_t* p, const uint8_t* body, int len);
int get_friend_paradise_info_callback(sprite_t* p, uint32_t id, char* buf, int len);


int get_visitor_paradise_info_cmd(sprite_t* p, const uint8_t* body, int len);
int get_visitor_paradise_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_visitor_paradise_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_hot_item_info_cmd(sprite_t* p, const uint8_t* body, int len);
int get_hot_item_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int gain_game_angles_exchange_item(sprite_t* p);

int use_prop_info_cmd(sprite_t* p, const uint8_t* body, int len);
int use_prop_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_angel_info_for_td_cmd(sprite_t* p, const uint8_t* body, int len);
int get_angel_info_for_td_callback(sprite_t* p, uint32_t id, char* buf, int len);

int td_game_start_cmd(sprite_t* p, const uint8_t* body, int len);
//int td_game_start_callback(sprite_t* p, uint32_t id, char* buf, int len);
int td_game_kill_monster_cmd(sprite_t* p, const uint8_t* body, int len);
int td_game_end_cmd(sprite_t* p, const uint8_t* body, int len);
int td_game_end_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_fight_level_cmd(sprite_t* p, const uint8_t* body, int len);
int get_fight_level_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_fight_score_cmd(sprite_t* p, const uint8_t* body, int len);
int get_fight_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int load_tdtask_info(const char *file, td_task_info_t* p_td_task_info);
int load_td_role_info(const char *file);
int load_tdtask_all( );
int count_time_exp_limit(td_task_info_t* p_td_task_info, int round);

int clean_black_angel_start_cmd(sprite_t* p, const uint8_t* body, int len);
int clean_black_angel_sucess_cmd(sprite_t* p, const uint8_t* body, int len);
int clean_black_angel_sucess_callback(sprite_t* p, uint32_t id, char* buf, int len);
int clean_black_angel_end_cmd(sprite_t* p, const uint8_t* body, int len);

int get_angel_in_hospital_cmd(sprite_t* p, const uint8_t* body, int len);
int get_angel_in_hospital_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_angel_cmd(sprite_t* p, const uint8_t* body, int len);
int add_angel_callback(sprite_t* p, uint32_t id, char* buf, int len);

int use_tear_of_jack_cmd(sprite_t* p, const uint8_t* body, int len);
int use_tear_of_jack_callback(sprite_t* p, uint32_t id, char* buf, int len);

int angel_leave_hospotal_cmd(sprite_t* p, const uint8_t* body, int len);
int angel_leave_hospotal_callback(sprite_t* p, uint32_t id, char* buf, int len);

int use_prop_for_td_cmd(sprite_t* p, const uint8_t* body, int len);
int get_prop_for_td_cmd(sprite_t* p, const uint8_t* body, int len);

int angel_show_cmd(sprite_t* p, const uint8_t* body, int len);
int angel_show_callback(sprite_t* p, uint32_t id, char* buf, int len);

int angel_regain_cmd(sprite_t* p, const uint8_t* body, int len);
//the callback use "angel_show_callback"

int angel_get_favorite_info_cmd(sprite_t* p, const uint8_t* body, int len);
int angel_get_favorite_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_angel_paradise_background_cmd(sprite_t * p, const uint8_t* body, int len);
int get_angel_paradise_background_callback(sprite_t* p, uint32_t id, char* buf, int len);

int exchange_davis_bean_cmd(sprite_t * p, const uint8_t* body, int len);
int exchange_davis_bean_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_angel_paradise_honors_cmd(sprite_t * p, const uint8_t* body, int len);
int get_angel_paradise_honors_callback(sprite_t* p, uint32_t id, char* buf, int len);
int angel_get_honor_bonus_cmd(sprite_t * p, const uint8_t* body, int len);
int angel_get_honor_bonus_callback(sprite_t* p, uint32_t id, char* buf, int len);
int load_angel_honor_bonus(const char *file);
int db_add_singel_angel(uint32_t userid, uint32_t angel_id, uint32_t angel_count, uint32_t angel_type);
int db_add_singel_angel_ex(uint32_t userid, item_unit_t * iut);
int save_monster_game_cmd(sprite_t* p, uint8_t* body, int len);
int save_monster_game(sprite_t* p);

int load_angel_compose_material_conf(const char * file);
int get_angel_compose_material_cmd(sprite_t* p, const uint8_t* body, int len);
int get_angel_compose_material_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_compose_new_angel_cmd(sprite_t* p, const uint8_t* body, int len);
int user_compose_new_angel_callback(sprite_t* p, uint32_t id, char* buf, int len);

//20111223天使挑战赛 获取奖励
int angel_get_game_award_cmd(sprite_t* p, const uint8_t* body, int len);
int angel_get_game_award_callback(sprite_t* p, uint32_t id, char* buf, int len);

//加载奖励信息
int angel_load_game_award_info( char* file );

int get_vip_angel_info_cmd(sprite_t* p, const uint8_t* body, int len);
int get_vip_angel_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int exchange_vip_angel_cmd(sprite_t* p, const uint8_t* body, int len);
int exchange_vip_angel_callback(sprite_t* p, uint32_t id, char* buff, int len);

void reload_angel_td_conf();
#endif

