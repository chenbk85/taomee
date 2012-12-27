/*
 * =====================================================================================
 *
 *       Filename:  year_feast.c
 *
 *    Description:  新年盛宴
 *
 *        Version:  1.0
 *        Created:  01/14/2012 11:30:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef NEW_YEAR_FEAST_H__
#define NEW_YEAR_FEAST_H__

#define	MAX_FEAST_FOOD_CNT			1000

#define MAX_FEAST_COOK_USER_CNT		3
#define MAX_FEAST_CAKE_USER_CNT		2
#define MAX_FEAST_TABLE_USER_CNT	10

//#define FEAST_TEST_VERSION

#ifndef FEAST_TEST_VERSION
#define PRE_FEAST_WASH_FOOD_RATE	50
#define PRE_FEAST_COOK_FOOD_RATE	20
#define PRE_FEAST_FOOD_TABLE_RATE	10
#define PRE_FEAST_MAKE_CAKE_RATE	100
#else
#define PRE_FEAST_WASH_FOOD_RATE	50
#define PRE_FEAST_COOK_FOOD_RATE	20
#define PRE_FEAST_FOOD_TABLE_RATE	10
#define PRE_FEAST_MAKE_CAKE_RATE	100
#endif

#define NEW_YEAR_FEAST_MAP			66
#define IS_YEAR_FEAST_MAP(mapid_)	(((uint32_t)mapid_) == NEW_YEAR_FEAST_MAP)

typedef struct year_feast_timer {
    list_head_t timer_list;
}__attribute__(( packed )) year_feast_timer_t;

enum FEAST_FIRE_STATE {
	FFS_FIRE_OFF = 0,	//还没生火的状态
	FFS_FIRE_ON = 1,	//有火的状态
};

//食材
typedef struct feast_food_s{
	uint32_t food_cnt;	//食材数量
	uint32_t food_rate;	//进度
}__attribute__((packed)) feast_food_t;

//烹饪
typedef struct feast_cook_s{
	uint32_t fire_state;							//火的状态
	uint32_t fire_userid;							//点火用户
	uint32_t cook_rate;								//进度
	uint32_t userid_list[MAX_FEAST_COOK_USER_CNT];	//烹饪区3个用户
	uint32_t cook_time[MAX_FEAST_COOK_USER_CNT];	//烹饪区3个用户烹饪的时间
}__attribute__((packed)) feast_cook_t;

//点心区
typedef struct feast_cake_s{
	uint32_t cake_rate;								//进度
	uint32_t userid_list[MAX_FEAST_CAKE_USER_CNT];	//蛋糕区2个用户
	uint32_t cake_time[MAX_FEAST_CAKE_USER_CNT];	//蛋糕区2个用户制作蛋糕的时间
}__attribute__((packed)) feast_cake_t;

//宴会桌
typedef struct feast_table_s{
	uint32_t table_rate;							//进度
	uint32_t userid_list[MAX_FEAST_TABLE_USER_CNT];	//宴会桌上的userid列表
}__attribute__((packed)) feast_table_t;

enum FEAST_START_STATE{
	FEAST_WAIT = 0,
	FEAST_START = 1,
	FEAST_END = 2,
};

typedef struct feast_info_s {
	feast_food_t	feast_food;
	feast_cook_t	feast_cook;
	feast_cake_t	feast_cake;
	feast_table_t	feast_table;
	uint32_t		start_state;
	uint32_t		start_time;						//开始时间
}__attribute__((packed)) feast_info_t;

enum USER_FEAST_POS {
	ON_NULL_POS = 0,
	ON_FIRE_POS = 1,
	ON_COOK_POS = 2,
	ON_CAKE_POS = 3,
	ON_TABLE_POS = 4,
};

enum RET_OPT_TYPE {
	ROT_GET_INFO = 0,			//协议拉取信息
	ROT_CLEAR_FOOD = 1,			//清洗食材
	ROT_ON_COOK_FIRE = 2,		//加入生火点，并生火
	ROT_OFF_COOK_FIRE = 3,		//离开生火点，并灭火
	ROT_ADD_COOK_FIRE = 4,		//生火点，加柴
	ROT_ON_COOK_POS	= 5,		//加入烹饪区
	ROT_OFF_COOK_POS = 6,		//离开烹饪区
	ROT_DO_ONCE_COOK = 7,		//完成一次烹饪
	ROT_ON_CAKE_POS = 8,		//加入蛋糕区
	ROT_OFF_CAKE_POS = 9,		//离开蛋糕区
	ROT_DO_ONCE_CAKE = 10,		//完成一次蛋糕制作
	ROT_ON_TABLE_POS = 11,		//加入宴会桌
	ROT_OFF_TABLE_POS = 12,		//离开宴会桌
	ROT_KICK_OFFLINE_USER = 13,	//踢下掉线或切地图用户
	ROT_KICK_COOK_USER = 14,	//定时时间没有操作，踢下烹饪区用户
	ROT_KICK_CAKE_USER = 15,	//定时时间没有操作，踢下蛋糕区用户
	ROT_YEAR_FEAST_OVER = 16,	//宴会结束
	ROT_YEAR_FEAST_START = 17,	//宴会开始
};

int init_year_feast_timer();
int handle_year_feast_time_out(void* onwer, void* data);
int handle_one_time_cook_time_out(void* onwer, void* data);
int handle_one_time_cake_time_out(void* onwer, void* data);

//发送所有信息
int send_all_feast_info_to_map(sprite_t* p, uint32_t opt_type, uint32_t opt_uid, int completed);

//通知前端新一轮的宴会状态 8363
int year_feast_tell_new_state();

//得到宴会的所有信息 8356
int year_feast_get_all_info_cmd(sprite_t *p, const uint8_t *buf, int len);

//清洗食物 8357
int year_feast_wash_food_cmd(sprite_t *p, const uint8_t *buf, int len);

//生火 8358
int year_feast_cook_fire_cmd(sprite_t *p, const uint8_t *buf, int len);

//烧煮完成菜肴 8359
int year_feast_cook_food_cmd(sprite_t *p, const uint8_t *buf, int len);

//加入蛋糕制作点 8360
int year_feast_set_cake_pos_cmd(sprite_t *p, const uint8_t *buf, int len);

//加入宴会桌 8361
int year_feast_set_table_pos_cmd(sprite_t *p, const uint8_t *buf, int len);

//领取奖励 8362
int year_feast_get_prize_cmd(sprite_t *p, const uint8_t *buf, int len);
int year_feast_get_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);
int year_feast_check_day_limit_callback(sprite_t* p, uint32_t id, char* buf, int len);

//用户离线时，踢出用户
int year_feast_kill_user(sprite_t *p);

//火灭时，将烹饪区的人踢下
int year_feast_clear_cook_user();

//清除蛋糕区的ren
int year_feast_clear_cake_user();

//判断userid是否在已存在
int year_feast_check_userid_in_feast(uint32_t userid);
#endif
