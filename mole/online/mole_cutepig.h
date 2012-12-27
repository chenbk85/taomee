/*
 * =====================================================================================
 *
 *       Filename:  mole_cutepig.h
 *
 *    Description:  养猪场
 *
 *        Version:  1.0
 *        Created:  09/06/2011 10:03:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee, ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef MOLE_CUTE_PIG_H
#define MOLE_CUTE_PIG_H
#define CUTE_PIG_MAX_NAME_LEN			16
#define CUTE_PIG_MAX_MATE_CNT			10
#define CUTE_PIG_TASK_AWARD_ITEM_CNT	5
#define CUTE_PIG_MAX_PROCESS_ITEM_KIND	5
#define CUTE_PIG_MAX_PROCESS_TYPE		100

#define	CP_GOLD_ITEM_ID					0
#define	CP_EXP_ITEM_ID					14

#define CP_SHOW_MAX_ITEM_CNT			8
#define CP_SHOW_MAX_PIG_CNT				3
#define CUTEPIG_MAP_ITEMS_MAX			10


enum PIGLET_SATE{
	PS_INFANCY	= 0x01, //幼年状态
	PS_YOUTH	= 0x02, //青年状态
	PS_MATURE	= 0x03, //成年状态
	PS_DYING	= 0x04, //濒死状态
	PS_EXPIRE	= 0x08, //死亡状态
	PS_PREGNANT	= 0x10, //怀孕状态
	PS_DOUBLE	= 0x20,	//双胞胎
	PS_VAR		= 0x40,	//必然变异
	PS_Train1	= 0x80,	//训练1
	PS_Train2	= 0x100,//训练2
	PS_MATEABLE	= 0x200,//是否可以怀孕
};

enum CUTE_PIG_SEX
{
	CPS_MALE = 0,
	CPS_FEMALE,
	CPS_VAR,		//变种猪
};

enum CP_MATE_RET
{
	CMR_OK					= 0,
	CMR_NOT_ENOUGH_MONEY,
	CMR_INVALID_PIG_ID,
	CMR_TAR_CAN_NOT_MATE,
	CMR_CAN_NOT_MATE,
	CMR_INVALID_USER,
	CMR_DB_ERR,
	CMR_NOT_ENOGH_SPACE,
};

enum CP_GAME_INFO_STATE
{
	CGIS_NEW_PLAYER = 1,
};


typedef struct cp_game_info_s
{
	uint32_t	state;
	uint32_t	level;
	uint32_t	exp;
	uint32_t	proc_fac_lv;
	uint32_t	show_stage_lv;
	uint32_t	machine_lvl;
	uint32_t	honor;
	uint32_t	feed_cnt;
	uint32_t	process_cnt;
	//	uint32_t	gold;
	uint32_t	bath_time;
	uint32_t	formation;
	uint32_t	background_id;
	uint8_t		formation_msg[56];
	uint32_t	pig_count;
} __attribute__((packed)) cp_game_info_t;

typedef struct cp_pig_info_simple_s
{
	uint32_t	pig_id;
	uint32_t	item_id;
	uint32_t	breed;
	uint32_t	sex;
	uint32_t	hungry_degree;
	uint32_t	state;
	uint32_t	tickle_cnt;					//逗乐次数
	uint32_t	transform_id;
	uint32_t	dress_1;
	uint32_t	dress_2;
}__attribute__((packed)) cp_pig_info_simple_t;

typedef struct cp_pig_raise_back_s
{
	uint32_t	pig_id;
	uint32_t	item_id;
	uint32_t	breed;
	uint32_t	sex;
	uint32_t	hungry_degree;
	uint32_t	state;
	uint32_t	transform_id;
	uint32_t	dress_1;
	uint32_t	dress_2;
	uint8_t		name[CUTE_PIG_MAX_NAME_LEN];
}__attribute__((packed)) cp_pig_raise_back_t;

typedef struct cp_new_pig_info_s
{
	uint32_t	item_id;
	uint8_t		mother_name[CUTE_PIG_MAX_NAME_LEN];
	uint8_t		father_name[CUTE_PIG_MAX_NAME_LEN];
	uint8_t		name[CUTE_PIG_MAX_NAME_LEN];
	uint32_t	breed;
	uint32_t	generation;
	uint32_t	sex;
	uint32_t	growth;
	uint32_t	weight;
	uint32_t	glamour;
	uint32_t	strength;
	uint32_t	lifetime;
	uint32_t	pirce;
}__attribute__((packed)) cp_new_pig_info_t;

typedef struct cp_pig_info_s
{
	uint32_t	pig_id;						//猪仔编号
	uint32_t	item_id;					//物品表里的ID
	uint8_t		mother_name[CUTE_PIG_MAX_NAME_LEN];
	uint8_t		father_name[CUTE_PIG_MAX_NAME_LEN];
	uint8_t		name[CUTE_PIG_MAX_NAME_LEN];
	uint32_t	state;						//生长期，濒死， 死亡， 怀孕
	uint32_t	age;						//年龄
	uint32_t	lifetime;					//寿命
	uint32_t	hungry_degree;				//饥饿度
	uint32_t	breed;						//品种
	uint32_t	generation;					//世代
	uint32_t	sex;						//性别
	uint32_t	growth;						//生长速度
	uint32_t	weight;						//体重
	uint32_t	glamour;					//魅力
	uint32_t	strength;					//强壮
	uint32_t	transform_id;				//变形ID
	uint32_t	transform_time;				//变形时间，db返回变形开始时间，发送给前端为变形状态剩余时间
	uint32_t	mate_cnt;					//交配次数
	uint32_t	tickle_cnt;					//逗乐次数
	uint32_t	train_point;				//训练点数
	uint32_t	parturition_time;			//分娩时间
	uint32_t	dress_1;					//装饰物
	uint32_t	dress_2;
	uint32_t	space_left;					//剩余空间，可以再养多少个小猪
    uint32_t    able_insect;
}__attribute__((packed)) cp_pig_info_t;

typedef struct cp_feed_info_s
{
	uint32_t tar_id;
	uint32_t fodder_id;
	uint32_t is_friend;
}__attribute__((packed)) cp_feed_info_t;

typedef struct
{
	uint32_t item_id;
	uint32_t item_cnt;
	uint32_t cost;
}__attribute__((packed)) cp_buy_cmd_t;

typedef struct cp_mate_info_s
{
	uint32_t my_pig_id;
	uint32_t tar_user_id;
	uint32_t tar_pig_id;
	uint32_t gold_cnt;
	uint32_t mate_cost;
	cp_pig_info_t my_pig_info;
	cp_pig_info_t tar_pig_info;
}cp_mate_info_t;

typedef struct{
	uint32_t item_id;
	uint32_t item_cnt;
}cp_item_info_t;

typedef struct cp_task_info_s
{
	uint32_t task_type;
	uint32_t count;
	uint32_t award_gold;
	uint32_t award_exp;
	uint32_t min_level;
	uint32_t max_level;
	uint32_t item_kind;
	cp_item_info_t item_info[CUTE_PIG_TASK_AWARD_ITEM_CNT];
}cp_task_info_t;

typedef struct cp_process_ele_info_s
{
	uint32_t pig_breed;
	uint32_t award;
	uint32_t proc_level;
	uint32_t item_kind_in;
	uint32_t item_id_in[CUTE_PIG_MAX_PROCESS_ITEM_KIND];
	uint32_t item_cnt_in[CUTE_PIG_MAX_PROCESS_ITEM_KIND];
}cp_process_ele_info_t;

typedef struct cp_process_info_s
{
	uint32_t				process_count;
	cp_process_ele_info_t	process_info[CUTE_PIG_MAX_PROCESS_TYPE];
}cp_process_info_t;

enum FEED_RET
{
	FR_OK = 0,
	FR_NOT_ENOUGH_FODDER,
	FR_NOT_HUNGRY,
	FR_LIMIT,
	FR_NOT_FRIEND,
};

enum TASK_STATE
{
	TS_GET_TASK = 1,
	TS_CAN_SUBMIT,
	TS_HAS_SUBMIT,
};

enum PROCESS_RET
{
	PR_OK = 0,
	PR_NOT_ENOUGH_STUFF,
	PR_LIMIT,
};

enum CP_STH_DONE_EVENT
{
	CSDE_FEED,			//喂食
	CSDE_BATHE,			//洗澡
	CSDE_PICK_AWAED,	//开宝箱
};

typedef struct cp_use_card_info_s
{
	uint32_t card_id;
	uint32_t tar_id;
}__attribute__((packed)) cp_use_card_info_t;

enum CP_USE_CARD_RESULT
{
	CUCR_OK		= 0,
	CUCR_NOT_ENOUGH_CARD,
	CUCR_INVALID_CARD,
	CUCR_CAN_NOT_USE_TO_SELF,
	CUCR_NO_EFFECT,
};

enum CP_CARD
{
	CC_ANGEL	= 1613128,
	CC_DEMON	= 1613129,
	CC_CUPID	= 1613130,
	CC_RECOVER	= 1613131,
};
enum CP_ITEM_KIND
{
	CIK_FODDER		= 1,	//饲料
	CIK_SP_ITEM		= 2,	//功能道具，工厂升级
	CIK_STUFF		= 3,	//食材
	CIK_SYS			= 4,	//系统道具
	CIK_PACKAGE		= 5,	//礼包
	CIK_CARD		= 6,	//卡片
	CIK_BACKGROUND	= 7,	//背景
	CIK_SHOW_BUF	= 8,	//表演用道具
	CIK_SHOW_DEBUF	= 9,	//表演用道具
	CIK_SHOW_ITEM	= 10,	//伸展台装饰物
};

enum CP_SHOW_RET
{
	CSR_OK				= 0,
	CSR_LIMIT,
	CSR_INVALID_DATA,
};

typedef struct cp_show_result_info_s
{
	uint32_t result;
	uint32_t result_left;
	int32_t my_score;
	int32_t tar_score;
	uint32_t pig_cnt;
	uint32_t tar_id;
	uint32_t tar_lv;
	uint32_t tar_exp;
	uint32_t my_pig[CP_SHOW_MAX_PIG_CNT];
	uint32_t item_cnt[CP_SHOW_MAX_ITEM_CNT];
} cp_show_result_info_t;

typedef struct cp_add_exp_gold_info_s
{
	sprite_t*	p;
	uint32_t	tar_id;
	uint32_t	cur_lv;
	uint32_t	cur_exp;
	uint32_t*	add_exp;
	uint32_t	add_gold;
	uint32_t	is_from_db;
} cp_add_exp_gold_info_t;

enum CP_SHOW_ITEM
{
	SI_ADD_20	= 0,
	SI_ADD_100,
	SI_ADD_150,
	SI_ADD_50,
	SI_MIN_20,
	SI_MIN_100,
	SI_MIN_150,
	SI_MIN_50,
};

enum CP_SHOW_ACHIEVE
{
	CSA_INVALID			= -1,
	CSA_COMBO_WIN_15,
	CSA_COMBO_WIN_10,
	CSA_COMBO_LOSE_5,
	CSA_WIN,
	CSA_LOSE,
	CSA_MAX,
};

enum CP_SHOW_GET_ACHIEVE_RET
{
	CSGAR_OK = 0,		//成功
	CSGAR_HAD_GET,		//已经获取
	CSGAR_INCONFORMITY,	//条件不满足
};

enum CP_HONOR_TYPE
{
	CHT_20TIMES_COLLECT = 25,		//分馆收藏20只				db
	CHT_3TIMES_LOGIN	= 26,		//登陆3次					*
	CHT_20TIMES_PK		= 27,		//美美猪pk20次				*
	CHT_TASK_FINISH		= 28,		//完成臭臭任务				db
	CHT_1TIMES_MAKE		= 29,		//加工一只猪				*
	CHT_10TIMES_PLAY	= 30,		//给10个好友猪逗乐			*
	CHT_30TIMES_WIN		= 31,		//美美猪pk连胜30次			*
	CHT_20TIMES_FEED	= 32,		//给20个好友的猪喂食		*
	CHT_10TIMES_BATHE	= 33,		//给10个好友的猪洗澡		*
	CHT_5TIMES_EXTEND	= 34,		//给5只猪延长寿命			*
	CHT_10TIMES_MATE	= 35,		//给自己的猪进行10次培育	*
	CHT_30TIMES_BOX		= 36,		//开启宝箱30次				db
	CHT_1TIMES_BUEAT	= 37,		//装扮自己美美屋			db
	CHT_50TIMES_PIG		= 38,		//培育成年猪猪50只
	CHT_10TIMES_DISH	= 39,		//加工10道美味				*
	CHT_3TIMES_CATCH	= 40,		//参与肥肥抓捕3次			*
};

typedef struct cp_show_with_npc_info_s
{
	uint32_t npc_id;
	uint32_t is_npcs_pig;
	uint32_t pig_cnt;
	uint32_t my_pig[CP_SHOW_MAX_PIG_CNT];
	uint32_t item_cnt[CP_SHOW_MAX_ITEM_CNT];
}cp_show_with_npc_info_t;

typedef struct cp_pig_map_explor_mine_s
{
    uint32_t	id;
	uint32_t	lvl_limit;
	uint32_t	nead_time;
	uint32_t	gemid;
	uint32_t	special_pig[2];
	uint32_t	rate;
	uint32_t	special_rate;
	uint32_t	energy;
}__attribute__((packed)) cp_pig_map_explor_mine_t;

typedef struct map_item_rand_s{
	item_t*	itm;
	int	count;
	float rand_start;
	float rand_end;
} map_item_rand_t;

typedef struct map_mine_rand_s{
	uint32_t		mapid;
	int				item_cnt;
	map_item_rand_t map_items[CUTEPIG_MAP_ITEMS_MAX];
}__attribute__((packed)) map_mine_rand_t;



int	cutepig_load_config_file();
int	cutepig_load_level_info( char* file );
int	cutepig_load_task_info( char* file );
int	cutepig_load_process_info( char* file );
int cutepig_load_bs_npc_pk_award_info( char* file );
int cutepig_load_ie_info( char* file );

//增加经验和金币
int cutepig_add_exp_gold( cp_add_exp_gold_info_t* info );
//获取当前等级
int	cutepig_get_cur_level( uint32_t exp );
//通知db增加经验
int cutepig_db_add_exp( uint32_t user_id, int32_t exp );

//获取养猪场信息
int cutepig_get_game_info_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_get_game_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取猪仔的具体信息
int cutepig_get_pig_info_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_get_pig_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取背包信息
int cutepig_get_knapsack_info_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_get_knapsack_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//购买物品
int cutepig_buy_item_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_buy_exchange_ok(sprite_t*p);

//养猪
int cutepig_raise_pig_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_raise_pig_callback(sprite_t* p, uint32_t id, char* buf, int len);

//喂食
int cutepig_feed_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_feed_get_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int	cutepig_feed_callback(sprite_t* p, uint32_t id, char* buf, int len);

//洗澡
int cutepig_bathe_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_bathe_callback(sprite_t* p, uint32_t id, char* buf, int len);

//逗逗
int cutepig_tickle_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_tickle_callback(sprite_t* p, uint32_t id, char* buf, int len);

//训练
int cutepig_training_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_training_callback(sprite_t* p, uint32_t id, char* buf, int len);

//改名
int cutepig_set_name_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_set_name_callback(sprite_t* p, uint32_t id, char* buf, int len);

//设定队形
int cutepig_set_formation_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_set_formation_callback(sprite_t* p, uint32_t id, char* buf, int len);

//出售
int cutepig_sale_cmd(sprite_t* p, const uint8_t* body, int len);
int	cutepig_sale_callback(sprite_t* p, uint32_t id, char* buf, int len);
int	cutepig_sale_exchange_ok(sprite_t*p);

//交配
int cutepig_mate_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_mate_check_callback(sprite_t* p, uint32_t id, char* buf, int len);
int	cutepig_mate_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_mate_result( sprite_t* p, uint32_t isOK, uint32_t cost );

//获取指定玩家的可以交配的公猪的信息
int cutepig_get_players_male_pig_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_players_male_pig_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取好友列表
int cutepig_get_friend_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_friend_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取任务列表
int cutepig_get_task_list_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_task_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

//接任务
int cutepig_accept_task_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_accept_task_callback(sprite_t* p, uint32_t id, char* buf, int len);

//交任务
int cutepig_submit_task_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_submit_task_callback(sprite_t* p, uint32_t id, char* buf, int len);

//工厂加工
int cutepig_process_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_process_callback(sprite_t* p, uint32_t id, char* buf, int len);

//公告列表
int cutepig_get_notice_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_notice_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//随机好运
int cutepig_get_rand_lucky_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_rand_lucky_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取图鉴
int cutepig_get_illustrate_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_illustrate_callback(sprite_t* p, uint32_t id, char* buf, int len);

//建筑升级
int cutepig_upgrade_building_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_upgrade_building_callback(sprite_t* p, uint32_t id, char* buf, int len);

//使用道具
int cutepig_use_item_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_use_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
//使用美美屋装饰道具
int cutepig_use_show_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
//设置当前衣服
int cutepig_set_clothes_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取彩虹猪票
int cutepig_get_ticket_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_ticket_callback(sprite_t* p, uint32_t id, char* buf, int len);

//通知db某件事完成了，用于猪倌的（喂食等）可以对他人操作的任务统计
int cutepig_notice_db_sth_done(sprite_t* p, uint32_t event_id);

//使用卡片
int cutepig_use_card_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_card_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_use_card_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_use_card_send_result( sprite_t* p, uint32_t result );

//查询卡片使用状态
int cutepig_get_card_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_card_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//领取某种猪
int cutepig_exchange_pig_with_items_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_exchange_pig_with_items_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_exchange_pig_sth_done_back( sprite_t* p );

//设置背景
int cutepig_set_background_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_set_background_callback(sprite_t* p, uint32_t id, char* buf, int len);

//分馆交换
int cutepig_swap_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_swap_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取分管信息
int cutepig_get_2nd_game_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_secondary_game_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取美美屋伸展台信息
int cutepig_get_show_stage_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_show_stage_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美屋伸展台，个人表演
int cutepig_beauty_show_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_beauty_show_callback(sprite_t* p, uint32_t id, char* buf, int len);

//小猪跟随
int cutepig_set_pig_follow_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_set_pig_follow_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美斗秀赛获得奖励
int cutepig_get_beauty_game_prize_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_check_beauty_game_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_get_beauty_game_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美斗秀赛查询昨天、今天及总的斗秀次数
int cutepig_get_beauty_game_record_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_beauty_game_record_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美斗秀赛 主动通知 定时特定次数礼包
int cutepig_get_beauty_game_limit_gift(uint32_t gift_type);

//美美斗秀赛 领取奖励
int cutepig_get_beauty_game_gift_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_check_beauty_game_time_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_get_beauty_game_gift_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美斗秀赛 与npc对战
int cutepig_beauty_show_with_npc_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_beauty_show_with_npc_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cp_beauty_show_check_use_npc_pig_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美斗秀赛 成就状态拉取
int cutepig_get_bs_npc_pk_achieve_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_bs_npc_pk_achieve_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//美美斗秀赛 成就领取
int cutepig_get_bs_npc_pk_achieve_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_bs_npc_pk_achieve_callback(sprite_t* p, uint32_t id, char* buf, int len);

//肥肥称重赛
int cutepig_npc_weight_pk_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_npc_weight_pk_callback(sprite_t* p, uint32_t pig_weight );
int cutepig_npc_weight_pk_check_callback(sprite_t* p, uint32_t id, char* buf, int len);
//获取沉重比赛状态
int cutepig_get_npc_weight_pk_info_cmd(sprite_t* p, const uint8_t* body, int len);

//图鉴收集兑换美美猪
int cutepig_get_pig_by_illustrate_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_pig_by_illustrate_callback(sprite_t* p, uint32_t id, char* buf, int len);

//肥肥馆荣誉 拉取所有荣誉
int cutepig_get_all_honor_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_all_honor_callback(sprite_t* p, uint32_t id, char* buf, int len);

int send_honor_event_to_db(sprite_t* p, uint32_t honor_id, uint32_t expend, uint32_t id);

int exchange_user_piglet_house_cmd(sprite_t* p, uint8_t* body, int bodylen);
int exchange_user_piglet_house_callback(sprite_t* p, uint32_t id, char* buf, int len);

//piglet machine factory
int cutepiglet_guider_get_work_cmd(sprite_t* p, uint8_t* body, int bodylen);
int cute_piglet_guider_get_work_callback(sprite_t*p, uint32_t id, char* buf, int len);
int cutepiglet_guider_set_work_cmd(sprite_t* p, uint8_t* body, int bodylen);

int load_cutepig_explor_mine_conf(const char* file);
int load_map_mine_rand_conf(const char *file);
int cutepig_get_piglet_machine_work_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_piglet_machine_work_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_user_pig_explor_mine_cmd(sprite_t* p, uint8_t* body, int bodylen);
int cutepig_user_pig_explor_mine_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_get_explor_mine_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cutepig_get_explor_mine_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_user_piglet_melt_ore_cmd(sprite_t * p,const uint8_t * body,int len);
int cutepig_user_piglet_melt_ore_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_user_produce_machine_part_cmd(sprite_t * p,const uint8_t * body,int len);
int cutepig_user_produce_machine_part_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_user_pig_finish_work_cmd(sprite_t* p, uint8_t* body, int bodylen);
int cutepig_user_pig_finish_work_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_get_user_piglet_work_machine_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int cutepig_get_user_piglet_work_machine_callback(sprite_t* p, uint32_t id, char* buf, int len);

int cutepiglet_use_accelerate_machine_tool_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int cutepiglet_use_accelerate_machine_tool_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cute_piglet_get_use_special_acc_tool_cnt_callback(sprite_t *p, uint32_t id, char* buf, int len);


int cutepig_user_produce_special_machine_cmd(sprite_t* p, uint8_t* body, int bodylen);
int cutepig_user_produce_special_machine_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cutepig_user_give_random_machine_something_cmd(sprite_t* p, uint8_t *body, int bodylen);
int cutepig_use_give_random_machine_something_callback(sprite_t *p, uint32_t id, char* buf, int len);

int cutepiglet_guide_machine_produce_cmd(sprite_t*p ,uint8_t* body, int len);
int cutepiglet_guid_machine_produce_callback(sprite_t*p, uint32_t id, char* buf, int len);

int cutepiglet_take_back_mining_pig_cmd(sprite_t* p, uint8_t* body, int len);

#endif
