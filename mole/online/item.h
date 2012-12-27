#ifndef _ITEM_H_
#define _ITEM_H_

#include <libxml/tree.h>

#include "benchapi.h"
#include "map.h"

//---------------------- DB Related ---------------------------------------
#define SVR_PROTO_SWAP_HOME_ITEM	0x111F
/*
typedef struct HomeItmToDel {
	uint32_t	itmid;
	uint32_t	cnt;
} __attribute__((packed)) home_itm_del_t;

typedef struct HomeItmToAdd {
	uint32_t	itmid;
	uint32_t	cnt;
	uint32_t	max;
} __attribute__((packed)) home_itm_add_t;

typedef struct SwapHomeItmReq {
	uint32_t	del_cnt;
	uint32_t	add_cnt;
	char		buf[];
} __attribute__((packed)) swap_home_itm_req_t;*/

#define db_swap_home_itm(p_, uid_, req_, len_) \
		send_request_to_db(SVR_PROTO_SWAP_HOME_ITEM, (p_), (len_), (req_), (uid_))

//-----------------------------------------------------------------------

#define KINDS_NUM_MAX		64
#define ITEMS_NUM_MAX		10000
//#define MAX_EXCHANGE_ITEMS	60
#define MAX_EXCHANGE_ITEMS	20
#define MAX_RAND_EXCHANGE_ITEMS	60
#define MAX_EXCHG_OUT_KIND	5
#define EXCHANGE_TABLE_SIZE 3048

#define is_wear_full(p) \
		(p->item_cnt == sizeof(p->items)/sizeof(uint32_t))

#define SUIT_ITEM_KIND			1
#define	CHG_COLOR_ITEM_KIND	2
#define	CHG_SHOW_ITEM_KIND	3
#define	THROW_ITEM_KIND		4
#define	HOME_ITEM_KIND			5
#define	PET_ITEM_KIND			6
#define	FEED_ITEM_KIND			7
#define	SPECIAL_ITEM_KIND		9
#define	POSTCARD_ITEM_KIND	8
#define	ATTR_ITEM_KIND			10
#define	PET_ITEM_CLOTH_KIND	11
#define	PET_ITEM_HONOR_KIND	12
#define	HOMELAND_ITEM_KIND	13
#define	HOMELAND_SEED_KIND	14
#define	CARD_ITEM_KIND			15
#define	BAAOHE_ITEM_KIND		16
#define	CLASS_ITEM_KIND       17
#define ANIMAL_BABY_ITEM   	18
#define PASTURE_ITEM_KIND     19
#define HAPPY_CARD_KIND       20
#define CAR_KIND               21
#define FIGHT_ITEM_KIND			22
#define PET_ITEM_CERTIF_KIND	23
#define MDOG_KIND				24
#define STREET_ITEM_KIND		25
#define RESTAURANT_ITEM_KIND	26
#define DRAGON_KIND			27
#define TEMP_ITEM_KIND			28
#define ANGEL_ITEM_KIND     	30
#define FIGHT_CARD_KIND			32
#define	FIGHT_ANGEL_KIND		33
#define	FIGHT_CLOTHES_KIND		34
#define FIGHT_COLLECT_KIND		35

#define CUTE_PIG_KIND			36
#define CUTE_PIG_CLOTHES_KIND	37
#define CUTE_PIG_ITEM_KIND		38
#define OCEAN_ANIMAL_ITEM_KIND	39
#define OCEAN_ITEM_ITEM_KIND	    40



#define OLD_SUIT_ITEM_KIND			1
#define	OLD_CHG_COLOR_ITEM_KIND	(1 << 1)
#define	OLD_CHG_SHOW_ITEM_KIND	(1 << 2)
#define	OLD_THROW_ITEM_KIND		(1 << 3)
#define	OLD_HOME_ITEM_KIND			(1 << 4)
#define	OLD_PET_ITEM_KIND			(1 << 5)
#define	OLD_FEED_ITEM_KIND			(1 << 6)
#define	OLD_SPECIAL_ITEM_KIND		(1 << 7)
#define	OLD_POSTCARD_ITEM_KIND	(1 << 8)
#define	OLD_ATTR_ITEM_KIND			(1 << 9)
#define	OLD_PET_ITEM_CLOTH_KIND	(1 << 10)
#define	OLD_PET_ITEM_HONOR_KIND	(1 << 11)
#define	OLD_HOMELAND_ITEM_KIND	(1 << 12)
#define	OLD_HOMELAND_SEED_KIND	(1 << 13)
#define	OLD_CARD_ITEM_KIND			(1 << 14)
#define	OLD_BAAOHE_ITEM_KIND		(1 << 15)
#define	OLD_CLASS_ITEM_KIND       (1 << 16)
#define OLD_ANIMAL_BABY_ITEM   	(1 << 17)
#define OLD_PASTURE_ITEM_KIND     (1 << 18)
#define OLD_HAPPY_CARD_KIND       (1 << 19)
#define OLD_CAR_KIND               (1 << 20)
#define OLD_PET_ITEM_CERTIF_KIND	(1 << 22)
#define OLD_MDOG_KIND				(1 << 23)
#define OLD_STREET_ITEM_KIND		(1 << 24)
#define OLD_RESTAURANT_ITEM_KIND	(1 << 25)
#define OLD_DRAGON_KIND				(1 << 26)
#define OLD_TEMP_ITEM_KIND			(1 << 27)
#define OLD_ANGEL_ITEM_KIND     	(1 << 29)



#define RELOAD_ITEM				62001
#define RELOAD_GAME				62002
#define RELOAD_QUESTION         62003
#define RELOAD_GIFTS         	62004
#define RELOAD_GAM_DBP_IP		62005
#define RELOAD_PIC_SRV			62006
#define RELOAD_TUYA_SRV			62007
#define SET_DB_TIMEOUT          62008
#define RELOAD_HOLIDAYS         62009
#define SET_HOMESVR_SWITCH   	62010
#define RELOAD_DIRTY_FILE		62011
#define RELOAD_RAND_TYPE 		62012
#define RELOAD_PRO_WORK			62013
#define RELOAD_CNADY 			62014
#define RELOAD_CARDS			62015
#define RELOAD_EXCHANGE			62016
#define RELOAD_EXCLU_THING		62017
#define RELOAD_TASKS			62018
#define RELOAD_NPC_TASKS		62019
#define RELOAD_SWAP				62020
#define RELOAD_RAND				62021
#define RELOAD_GAME_BONUS		62022
#define RELOAD_RACE_BONUS		62023
#define RELOAD_FRIEND_BOX		62024
#define RELOAD_LAHM_STUDENTS	62025
#define RELOAD_LAHM_LESSONS		62026
#define RELOAD_LAHM_CLASSROOM_BONUS		62027
#define RELOAD_LAHM_LESSON_QUESTIONS	62028
#define RELOAD_DONATE_ITEM		62029
#define RELOAD_MAPS				62030
#define RELOAD_TD_TASKS			62031
#define RELOAD_ANGEL_COMPOSE 	62032
#define RELOAD_DUNGEON_MAP 		62033
#define RELOAD_TV_QUESTION		62034
#define RELOAD_AF_SKILL			62035
#define RELOAD_AF_PRIZE			62036
#define RELOAD_AF_PRIZE_TYPE	62037
#define RELOAD_AF_BARRIER		62038
#define RELOAD_AF_TASK			62039
#define RELOAD_TD_TASK			62040

enum {
	ITEM_WITHOUT_BODY_FLAG = 0,
	ITEM_ON_BODY_FLAG,
	ITEM_FLAG_MAX
};

typedef struct home_item {
	uint32_t	itemid;
	uint16_t	x;
	uint16_t	y;
	uint8_t		way;
	uint8_t		visible;
	uint8_t		layer;
	uint8_t		type;
	uint8_t		rotation;
	uint8_t		pad[3];
} __attribute__((packed)) home_item_t;

typedef struct pet_attr {
	int8_t		hungry;
	int8_t		thirsty;
	int8_t		sanitary;
	int8_t		spirit;
	int8_t		curability;
	int8_t		rebirth;
	int8_t		type;
} pet_attr_t;

typedef struct {
	uint32_t	type;
	uint32_t	channel;
	uint32_t	condition;
	uint32_t	stoves;
	uint32_t	tables;
	uint32_t	foodtables;
	uint32_t	postion[2];
} shop_item_T;

typedef struct {
	uint32_t	type;
	uint32_t	count;
	uint32_t	channel;
	uint32_t	condition;
	uint32_t	timer;
	uint32_t	bad_timer;
	uint32_t	addexp;
	uint32_t	food[6];
	uint32_t	star[5];
	uint32_t	ex_cnt;
	uint32_t	ex_exp;
} food_item_T;

typedef struct {
	uint32_t	levelNum[3];
} animal_item_T;

typedef struct {
	uint32_t	abilityNum[3];
} angel_item_T;

typedef struct {
	uint32_t type;
	uint32_t flag;
	uint32_t add_val;
	uint32_t round;
	uint32_t swap_id;	//swap cmd id
	uint32_t limit_lvl;
}__attribute__((packed)) af_item_t;

typedef struct {
	uint32_t card_type;
	uint32_t map_item_id;
	uint32_t add_type;
	uint32_t add_val;
	uint32_t level;
	uint32_t point;
	uint32_t max_collect;
	uint32_t donate_val;		//捐献价格
	uint32_t exchange_val;		//兑换价格
}__attribute__((packed)) af_card_t;

typedef struct {
	uint32_t power;
	uint32_t iq;
	uint32_t smart;
	uint32_t strong;
}__attribute__((packed)) af_clothes_t;

typedef struct {
	uint32_t wishing_cnt;
	uint32_t limit_lvl;
}__attribute__((packed)) af_collect_t;

typedef struct {
	uint32_t breed;
	uint32_t sex;
	uint32_t weight;
	uint32_t glamour;
	uint32_t strength;
	uint32_t growth;
	uint32_t lifetime;
	uint32_t oppsite_id;	//异性ID
	uint32_t mate_cost;		//交配价格
	uint32_t buy_level;		//购买等级
	uint32_t proc_exp;		//加工经验
	uint32_t var_rat;		//变异概率
	uint32_t var_id;		//变异体ID
	uint32_t price;			//卖出单位价格
	uint32_t weight_up;
	uint32_t grow_up;
	uint32_t price_up;
	uint32_t str_up;
	uint32_t glamour_up;
}__attribute__((packed)) cp_base_info_t;

typedef struct
{
	uint32_t type;
	uint32_t time_last;
}__attribute__((packed)) cp_item_t;

typedef struct
{
	uint32_t type;
	uint32_t star;
	uint32_t abilityNum[3];
}__attribute__((packed)) ocean_animal_t;

typedef struct
{
	uint32_t type;
	uint32_t value;
}__attribute__((packed)) ocean_item_t;


typedef struct item {
	uint32_t	id;
	// bit 1 for buyability, 2 for sellability,
	// 3 for vip buyability, 4 for vip sellability, -- Not Yet Used
	// 5 for vip only, 6 for vip can buy
	char		name[64];
	uint16_t	tradability;
	int			price;
	int			beans;
	int 		layer;
	int 		sell_price;
	float		discount;
	uint32_t	max;
	uint32_t	wishing;
	uint32_t	ggj;
	uint32_t	owner;
	uint32_t 	vip_gift_level;
	uint32_t	zhongzi_youzai_level;
	uint32_t	item_type;			//0：普通物品， 1：农场植物产出，2：牧场动物产出
	uint32_t    shellprice;
	union {
		pet_attr_t			feed_attr;
		shop_item_T			shop_tag;
		food_item_T			food_tag;
		animal_item_T		animal_tag;
		angel_item_T		angel_tag;
		af_item_t			af_item_tag;
		af_card_t			af_card_tag;
		af_clothes_t		af_cloth_tag;
		af_collect_t		af_collect_tag;
		cp_base_info_t		cp_base_info_tag;
		cp_item_t			cp_item_tag;
		ocean_animal_t		ocean_animal_tag;
		ocean_item_t		ocean_item_tag;
	} u;
} item_t;

#define itm_buyable(p_, itm_) \
		((!(itm_)->beans) && (!((itm_)->tradability & 1) || (itm_vip_buyable(itm_) && ISVIP((p_)->flag))))
#define itm_buyable_beans(p_, itm_) \
		(((itm_)->beans) && (!itm_vip_buyable(itm_) || (itm_vip_buyable(itm_) && ISVIP((p_)->flag))))
#define itm_sellable(itm_) \
		!((itm_)->tradability & 2)
#define itm_vip_only(itm_) \
		!!((itm_)->tradability & 16)
#define itm_set_vip_only(itm_) \
		((itm_)->tradability |= 16)
#define itm_vip_buyable(itm_) \
		!!((itm_)->tradability & 32)
#define itm_set_vip_buyable(itm_) \
		((itm_)->tradability |= 32)
#define itm_wishable(p_, itm_) \
		(((itm_)->wishing & 1))

#define IS_SUPER_LAHM_ITM(itm_) \
		((itm_)->owner == 2)

#define IS_NOMAL_LAHM_ITM(itm_) \
		((itm_)->owner == 1)

typedef struct item_kind {
	uint32_t	id;
	uint32_t	start;
	uint32_t	end;
	uint32_t	kind;
	uint32_t	count;
	int			max;
	item_t*		items;
} item_kind_t;

typedef struct {
	item_t*	itm;
	int	count;
} item_unit_t;

typedef struct {
	int			id;
	int			pre_id;
	int			not_recv;
	int			quit_task;
	int			not_limit;
	int			mole_level_limit;
	int			incnt;
	item_unit_t	in[MAX_EXCHANGE_ITEMS];
	int			outcnt[MAX_EXCHG_OUT_KIND];
	item_unit_t	out[MAX_EXCHG_OUT_KIND][MAX_EXCHANGE_ITEMS];
	size_t		outkind_cnt;
	int			flag_to_set[MAX_EXCHG_OUT_KIND];
} exchange_item_t;

typedef struct {
	uint32_t id, itmpos;
	uint32_t flag;
	int maxpoints, startpoint, max, item_num_per_point, reborntime, cnt;
	int start_time, end_time;
	int multiplicator, multiplied;

	int disappear_time;
	timer_struct_t* disappear_tmr;

	uint32_t ngenerated, ncollected;

	struct map* map;

	list_head_t entry;
	list_head_t timer_list;
} rand_item_t;

enum {
	ritm_fully_gen			= 1,
	ritm_gen_on_demand		= 2,
	ritm_appear_any_time	= 4
};

#define RITM_IS_GEN_ON_DEMAND(ritm_) \
		!!((ritm_)->flag & ritm_gen_on_demand)
#define RITM_SET_GEN_ON_DEMAND(ritm_) \
		((ritm_)->flag |= ritm_gen_on_demand)
#define RITM_IS_FULLY_GEN(ritm_) \
		!!((ritm_)->flag & ritm_fully_gen)
#define RITM_SET_FULLY_GEN(ritm_) \
		((ritm_)->flag |= ritm_fully_gen)
#define RITM_IS_APPEAR_ANY_TIME(ritm_) \
		!!((ritm_)->flag & ritm_appear_any_time)
#define RITM_SET_APPEAR_ANY_TIME(ritm_) \
		((ritm_)->flag |= ritm_appear_any_time)

const exchange_item_t* get_exchg_entry(int idx);
int  load_items(const char* file);
void unload_items();
int  load_exchange_items(const char* file);

item_t* get_item_prop(uint32_t id);
item_kind_t* find_kind_of_item(uint32_t id);
item_kind_t* get_item_kind(uint32_t kind_id);
void new_get_item_kind_range(uint32_t type, uint32_t* start, uint32_t* end);
void get_item_kind_range (uint32_t type, uint32_t *start, uint32_t *end);

int is_wear_item(sprite_t* p, uint32_t id);
int wear_item(sprite_t* p, uint32_t id);
int unwear_item(sprite_t* p, uint32_t id);

// Callbacks
int item_trading_callback(sprite_t* p, uint32_t id, char* buf, int len);

// RandItem Funcs
int  load_ritem_config(xmlNodePtr cur, map_t* m);
void destroy_ritems(list_head_t* head);

int  do_collect_item(sprite_t* p, uint32_t pos, uint32_t itemid);
void do_notify_ritm_info(void* p, int tomap, const list_head_t* ritmlist, int cmd);
int  do_gen_ritm_on_demand(map_t* m, uint32_t itmid, int force_gen);
int  notify_rand_item_info(void* owner, void* data);
int do_sell_item(sprite_t* p, uint32_t itemid, uint32_t nitems, uint32_t price);

int do_ask_for_items(sprite_t* p, uint32_t incnt, const uint8_t* inbuf, uint32_t outcnt, const uint8_t* outbuf);

int respond_items_count(sprite_t * p, char * buf, int len);

//------------------ For Use In proto.c Begin ------------------
int ask_for_items_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int chk_item_amount_cmd(sprite_t* p, const uint8_t* body, int bodylen);

//--------------------------------------
//	Buy Items Related Funcs Begin
//
int buy_item_cmd(sprite_t* p, const uint8_t* body, int len);
//	Buy Items Related Funcs End
//--------------------------------------

//--------------------------------------
//	Sell Items Related Funcs Begin
//
int bargaining_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int sell_items_cmd(sprite_t* p, const uint8_t* body, int bodylen);
//	Sell Items Related Funcs End
//--------------------------------------

//--------------------------------------
//	Compose Item Related Funcs Begin
//
int compound_item_cmd(sprite_t* p, const uint8_t* body, int bodylen);
//	Compose Item Related Funcs End
//--------------------------------------

int ritm_gen_on_demand_cmd(sprite_t* p, const uint8_t* body, int bodylen);

//----------------------------------------------
//
int exchg_item2_cmd(sprite_t* p, const uint8_t* body, int bodylen);

//------------------ For Use In proto.c End ------------------

int reload_conf_op(uint32_t uid, void* buf, int len);

//
static inline item_t*
get_item(const item_kind_t* itmkind, uint32_t itmid)
{
	uint32_t idx = itmid - itmkind->start;

	return ((idx < itmkind->count) ? &(itmkind->items[idx]) : 0);
}

// add exchg attr
static inline void
add_exchg_attr(sprite_t* p, uint8_t* sess)
{
	if (*sess == 1) {
		p->yxb    += ((uint32_t*)(sess + 1))[0];
		p->exp    += ((uint32_t*)(sess + 1))[1];
		p->strong += ((uint32_t*)(sess + 1))[2];
		p->iq     += ((uint32_t*)(sess + 1))[3];
		p->lovely += ((uint32_t*)(sess + 1))[4];
	}
}
// pack attr for adding on later
static inline void
pack_exchg_attr(uint8_t* sess, uint32_t itmid, int cnt)
{
	if (itmid < 5) {
		*sess = 1;
		((uint32_t*)(sess + 1))[itmid] = cnt;
	}
}

 int db_buy_itm_freely(sprite_t* p, userid_t uid, uint32_t itmid, uint32_t cnt);

extern exchange_item_t exits[EXCHANGE_TABLE_SIZE];

int get_closet_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_closet_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int unset_closet_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_closet_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_closet_callback(sprite_t* p, uint32_t id, char* buf, int len);
int unset_closet_callback(sprite_t* p, uint32_t id, char* buf, int len);
int ggj_exchg_itm_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int ggj_exchg_item(sprite_t* p);
int get_items_count_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_items_price_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int newyear_exchange_items_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int do_newyear_exchange_items(sprite_t *p);
int give_npc_some_items_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int do_give_npc_items(sprite_t *p);

int buy_item_by_beans_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int buy_item_by_beans_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_temp_item_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int add_temp_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_items_count_new_cmd(sprite_t* p, const uint8_t* body, int bodylen);
void reload_td_tasks_cmd(uint32_t opid, const void* body, int len);
void reload_angel_compose_cmd(uint32_t opid, const void* body, int len);

void reload_dungeon_map_cmd(uint32_t opid, const void* body, int len);

int get_item_array_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_item_array_callback(sprite_t* p, uint32_t id, char* buf, int len);
#endif
