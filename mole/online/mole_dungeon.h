#ifndef MOLE_DUNGEON_H
#define MOLE_DUNGEON_H

#define DUNGEON_MAX_AWARD_KIND			16
#define DUNGEON_MAX_AWARD_ITEM_KIND		8
#define DUNGEON_MAX_REGION_KIND			8
#define DUNGEON_MAX_RANDOM_TABLE_NUM	10

#define DUNGEON_MAP_MAX_SIZE 			3
#define DUNGEON_MAP_MAX_MINE_KIND 		256
#define DUNGEON_MAP_MAX_EVENT_KIND 		256
#define DUNGEON_MAP_MAX_BOSS_KIND 		256
#define DUNGEON_MAP_MAX_TRIGGER_KIND 	256




//back from db
typedef struct explore_begin_info_s
{
	uint32_t 	map_id;
	uint32_t	screen_id;
	uint32_t 	hp;
	uint32_t	time_surplus;
	uint32_t 	exp;
	uint32_t	explore_counter;
}__attribute__((packed)) explore_begin_info_t;

//掉落物品信息
typedef struct award_item_info_s
{
	uint32_t item_id;
	uint32_t item_count;
} __attribute__((packed)) award_item_info_t;
//开采掉落信息描述
typedef struct award_info_s
{
	uint32_t item_kind;
	uint32_t rate;
	award_item_info_t item_info[DUNGEON_MAX_AWARD_ITEM_KIND];
} __attribute__((packed)) award_info_t;
//开采阶段描述
typedef struct explore_region_s
{
	uint32_t award_kind;
	uint32_t max_explore_count;
	award_info_t award_info[DUNGEON_MAX_AWARD_KIND];
} __attribute__((packed)) explore_region_t;
//开采元素枚举
enum ELEMENT_TYPE
{
	ET_GAP		= 0,
	ET_MINE,
	ET_BOSS,
	ET_EVENT,
	ET_TRIGGER,
	ET_FORCE_DWORD = 0xffffffff,
};
//矿点描述
typedef struct mine_info_s
{
	uint32_t exp_inc;
	uint32_t hp_consum;
	uint32_t region_kind;
	uint32_t max_count;
	uint32_t max_explore_times;
	explore_region_t region_info[DUNGEON_MAX_REGION_KIND];
} __attribute__((packed)) mine_info_t;
//BOSS描述
typedef struct boss_info_s
{
	uint32_t exp_inc;
	uint32_t hp_consum;
	uint32_t max_explore_times;
} __attribute__((packed)) boss_info_t;
//事件描述
typedef struct event_info_s
{
	uint32_t exp_inc;
	uint32_t hp_consum;
	uint32_t max_explore_times;
	uint32_t award_kind;
	award_info_t award_info[DUNGEON_MAX_AWARD_KIND];
} __attribute__((packed)) event_info_t;
//机关描述
typedef struct trigger_info_s
{
	uint32_t exp_inc;
	uint32_t hp_consum;
	uint32_t max_count;
	uint32_t max_explore_times;
} __attribute__((packed)) trigger_info_t;
//开采区域信息
typedef struct area_info_s
{
	uint32_t area_type;
	uint32_t element_id;
	int	 	 rand_table;
	uint32_t rate;
} __attribute__((packed)) area_info_t;
//随机挖掘点每天的数量限制
typedef struct element_limit_s
{
	uint32_t mine;
	uint32_t boss;
	uint32_t event;
	uint32_t trigger;
} __attribute__((packed)) element_limit_t;

//分屏信息
typedef struct dungeon_screen_info_s
{
	uint32_t map_id;											//实际进入的地图ID
	uint32_t area_count;
	area_info_t area_info[DUNGEON_MAX_MAP_AREA_NUM];
	uint32_t rand_table_count;
	uint32_t rand_table[DUNGEON_MAX_RANDOM_TABLE_NUM];			//随机位置组的成员数目
} __attribute__((packed)) dungeon_screen_info_t;
//地图信息
typedef struct dungeon_map_info_s
{
	uint32_t screen_count;
	dungeon_screen_info_t screen_info[DUNGEON_MAX_SCREEN_NUM];
	element_limit_t random_element_limit;
} __attribute__((packed)) dungeon_map_info_t;
//地图数据
typedef struct dungeon_all_map_info_s
{
	uint32_t map_count;
	dungeon_map_info_t map_info[DUNGEON_MAP_MAX_SIZE];
} __attribute__((packed)) dungeon_all_map_info_t;
//矿点数据
typedef struct dungeon_all_mine_info_s
{
	uint32_t kind;
	mine_info_t	info[DUNGEON_MAP_MAX_MINE_KIND];
} __attribute__((packed)) dungeon_all_mine_info_t;
//BOSS数据
typedef struct dungeon_all_boss_info_s
{
	uint32_t kind;
	boss_info_t	info[DUNGEON_MAP_MAX_BOSS_KIND];
} __attribute__((packed)) dungeon_all_boss_info_t;
//事件数据
typedef struct dungeon_all_event_info_s
{
	uint32_t kind;
	event_info_t info[DUNGEON_MAP_MAX_EVENT_KIND];
} __attribute__((packed)) dungeon_all_event_info_t;
//机关数据
typedef struct dungeon_all_trigger_info_s
{
	uint32_t kind;
	trigger_info_t info[DUNGEON_MAP_MAX_TRIGGER_KIND];
} __attribute__((packed)) dungeon_all_trigger_info_t;

//根据挖宝经验计算等级
uint32_t dungeon_get_level_by_exp(uint32_t exp);


//开始探险
int	dungeon_begin_explore_cmd(sprite_t* p, const uint8_t* body, int len);
int	dungeon_begin_explore_callback(sprite_t* p, uint32_t id, char* buf, int len);
int dungeon_begin_explore_send_result(sprite_t* p);
//获取探险背包信息
int dungeon_get_bag_info_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_get_bag_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
//探索制定区域
int dungeon_explore_area_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_explore_mine(sprite_t* p, uint32_t element_id, uint32_t area_id);
int dungeon_explore_boss(sprite_t* p, uint32_t element_id, uint32_t area_id);
int dungeon_explore_event(sprite_t* p, uint32_t element_id, uint32_t area_id);
int dungeon_explore_trigger(sprite_t* p, uint32_t element_id, uint32_t area_id);
//向前端返回开采后的数据
int dungeon_explore_area_send_result(sprite_t* p, award_info_t* award, uint32_t area_id, uint32_t exp_inc, uint32_t hp_consum);
//this command dosn't need communicate with db

//同步血量
int dungeon_syn_hp_cmd(sprite_t* p, const uint8_t* body, int len);

// 拾取道具
int dungeon_pick_item_cmd(sprite_t* p, const uint8_t* body, int len);
//needn't communicate with db
//使用道具
int dungeon_use_item_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_use_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
//结束探险
int	dungeon_end_explore_cmd(sprite_t* p, const uint8_t* body, int len);
// needn't communicate with db
//解析配置文件
int dungeon_load_all_map_info();
int dungeon_load_map_info(char* file, dungeon_map_info_t* map_info);
int dungeon_load_map_element(char* file);
int dungeon_parse_mine_info(xmlNodePtr ptr, dungeon_all_mine_info_t* mine_info);
int dungeon_parse_boss_info(xmlNodePtr ptr, dungeon_all_boss_info_t* boss_info);
int dungeon_parse_event_info(xmlNodePtr ptr, dungeon_all_event_info_t* event_info);
int dungeon_parse_trigger_info(xmlNodePtr ptr, dungeon_all_trigger_info_t* trigger_info);

//珍宝库部分
//获取展台信息
int dungeon_get_exhibit_info_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_get_exhibit_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
//获取仓库信息
int dungeon_get_storage_info_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_get_storage_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
//获取好友信息
int dungeon_get_friend_info_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_get_friend_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
//获取访客信息
int dungeon_get_visitor_info_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_get_visitor_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//访客记录
inline int dungeon_add_exhibit_visitors(sprite_t *p, userid_t id);
inline int dungeon_commit_level_to_db(sprite_t* p, uint32_t level);

//从全局db返回访客等级信息
int dungeon_get_visitor_full_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//更换背景
int dungeon_set_background_map_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_set_background_map_callback(sprite_t* p, uint32_t id, char* buf, int len);
//设定展示物品
int dungeon_set_exhibit_item_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_set_exhibit_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
//交换展台上物品
int dungeon_swap_exhibit_item_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_swap_exhibit_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
//变换展示品展示方位
int dungeon_set_exhibit_state_cmd(sprite_t* p, const uint8_t* body, int len);
int dungeon_set_exhibit_state_callback(sprite_t* p, uint32_t id, char* buf, int len);


extern dungeon_all_map_info_t dungeon_map_info;
#endif

