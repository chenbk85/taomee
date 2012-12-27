#ifndef PRIZE_H
#define PRIZE_H

#define MAX_PRIZE_TYPE			20	//20种奖励类型
#define MAX_PRIZE_COUNT			400	//奖励物品数为400
#define MAX_PRIZE_OUT_COUNT		5	//5种奖励
#define MAX_PRIZE_TYPE_COUNT	20	//奖励类型配表的最大上限
#define MAX_BARRIER_PRIZE_COUNT	70	//关卡掉落物品的种类
#define MAX_BARRIER_COUNT		80	//最大关卡数量
#define MAX_FIGHT_MAP_COUNT		10	//战斗地图最大数量

#define IS_PVP_MAP(mapid) (mapid == 0)
#define IS_BOSS_MAP(mapid)	(mapid >= 100)
#define IS_MONSTER_MAP(mapid) (!(IS_PVP_MAP(mapid) || IS_BOSS_MAP(mapid)) )
#define IS_BOSS_BARRIER(barrier_id)

typedef struct{
	uint32_t ID;
	uint32_t prize_id;	//奖励ID
	uint32_t prize_cnt;	//奖励数量
	uint32_t level;		//等级限制
	uint32_t vip;		//vip限制
	uint32_t win;		//胜负限制
	uint32_t rate;		//获取几率
}__attribute__((packed)) prize_t;

typedef struct{
	uint32_t start_idx;	//vector中的开始idx
	uint32_t end_idx;	//结束
	uint32_t start_id;	//开始ID
	uint32_t end_id;	//结束ID
	uint32_t prize_type;
	uint32_t count;
}__attribute__((packed)) prize_range_t;

enum all_prize_type {
	PT_ANGEL = 1,		//天使
	PT_SKILL,			//被动技能
	PT_ENERGY_ITEM,		//增益道具
	PT_EXP_ITEM_1,		//主动经验道具
	PT_ATTR_ITEM,		//增益类道具
	PT_OTHER_ITEM,		//其他道具
	PT_MAP_COLLECT,		//地图收集品
	PT_MONSTER_COLLECT,	//怪物收集品
	PT_MONSTER_CARD,	//怪物卡
	PT_MONEY,			//摩尔豆
	PT_VITALITY_ITEM,	//活力道具
	PT_EXP_ITEM_2,		//被动经验道具
	PT_CLOTHES,			//装扮
};

typedef struct {
	uint32_t count;
	uint32_t prize_list[5];
}__attribute__((packed)) prize_type_array_t;

typedef struct {
	uint32_t ID;
	prize_type_array_t prize_array[MAX_PRIZE_OUT_COUNT];
}__attribute__((packed)) rand_prize_type_t;

typedef struct {
	uint32_t ID;
	uint32_t exp_win;
	uint32_t exp_lose;
	uint32_t prize_type;
	uint32_t lose_prize_type;
	uint32_t finish;
	uint32_t prize_list[MAX_BARRIER_PRIZE_COUNT];
	uint32_t prize_cnt;
	uint32_t prize_idx[MAX_PRIZE_TYPE + 1];
}__attribute__((packed)) fight_barrier_t;

typedef struct {
	uint32_t map_id;
	uint32_t barrier_cnt;
	uint32_t barrier_id[8];
}__attribute__((packed)) map_barrier_info_t;

int load_prize_conf(const char* file);
int load_prize_type_conf(const char* file);
int load_barrier_conf(const char* file);

prize_range_t*	get_prize_range_from_kind(uint32_t type);
prize_t* get_prize_from_prize_idx(uint32_t idx);

int rand_one_prize_type(prize_type_array_t* type_array);
int pack_prize_buff(uint32_t* buff, prize_t* itm, int* len);
int rand_one_prize_in_array(uint32_t* parray, uint32_t size);
int rand_one_type_barrier_prize(uint32_t* prize_cnt, uint32_t levelID, uint32_t prizetype, uint32_t cnt);
int rand_some_prize_in_array(uint32_t* prize, const uint32_t* plist, uint32_t size, uint32_t count);
int rand_one_prize_in_prize_conf(uint32_t* prize, uint32_t prize_type, uint32_t count);
uint32_t* get_one_type_prize_in_barrier(uint32_t barrier_id, uint32_t kind, uint32_t* count);

int get_prize_list(void *buff, uint32_t levelID, uint32_t win, uint32_t level);
map_barrier_info_t* get_map_barrier_from_id(uint32_t map_id);
#endif
