#ifndef _ROOM_H_
#define _ROOM_H_

#include "pet.h"
#include "sprite.h"

#define MAP_HASH_SLOT_NUM			500
#define MAP_ITEM_CLIENT_UNKNOW_FLAG	0x01
#define MAP_ITEM_ONLY_ONCE_FLAG		0x02

#define BASE_HOMELAND	2000000000

#define TOP_MAP_MAX			20
#define HOME_TILES_LEN		17
#define MAP_WIDTH			960
#define MAP_HEIGHT			560
#define NORMAL_MAP_NUM		2000
#define IS_NORMAL_MAP(x_)			(((x_) < NORMAL_MAP_NUM) && ((x_) > 0))
#define IS_HOUSE_MAP(mapid_)		((mapid_) > 10000 && (mapid_) < BASE_HOMELAND)
#define IS_HOUSE1_MAP(mapid_)		(((uint64_t)mapid_)>>32 == 11)
#define IS_HOUSE2_MAP(mapid_)		(((uint64_t)mapid_)>>32 == 12)
#define JY_MAP(id)					((map_id_t)(BASE_HOMELAND + (id)))
#define CLASS_MAP(id)					(((((map_id_t)1)<<32) + (id)))
#define PASTURE_MAP(id)					(((((map_id_t)2)<<32) + (id)))
#define HOUSE1_MAP(id)					(((((map_id_t)11)<<32) + (id)))
#define HOUSE2_MAP(id)					(((((map_id_t)12)<<32) + (id)))
#define IS_JY_MAP(mapid_)			(((mapid_) > BASE_HOMELAND) && (!(((uint64_t)mapid_)>>32)))
#define IS_SPACE(mapid_)            (((uint64_t)mapid_)>>32)
#define IS_CLASS_MAP(mapid_)            (((uint64_t)mapid_)>>32 == 1)
#define IS_PASTURE_MAP(mapid_)            (((uint64_t)mapid_)>>32 == 2)

#define SHOP_MAP(uid)				(((((map_id_t)31)<<32) + (uid)))
#define IS_SHOP_MAP(mapid)			(((uint64_t)mapid)>>32 == 31)
#define GET_UID_SHOP_MAP(mapid)	((uint32_t)(mapid))
#define GET_UID_CLASSROOM_MAP(mapid)	((uint32_t)(mapid))


#define GET_UID_IN_JY(mapid_)		((((uint32_t)mapid_) - BASE_HOMELAND))
#define GET_UID_IN_HOUSE(mapid_)		((uint32_t)(mapid_))
#define GET_UID_IN_CLASSID(mapid_)		((uint32_t)(mapid_))
#define GET_UID_IN_PASTURE(mapid_)		((uint32_t)(mapid_))
#define GET_UID_IN_PARADISE(mapid_)		((uint32_t)(mapid_))
#define GET_UID_IN_EXHIBIT(mapid_)		((uint32_t)(mapid_))
#define GET_UID_IN_CUTEPIG(mapid_)		((uint32_t)(mapid_))

#define GET_MAP_TYPE(mapid_) 			(uint32_t)(((uint64_t)mapid_) >> 32)

#define IS_SPRITE_VISIBLE(p_)	(!(p_)->group || !GAME_STARTED((p_)->group) || MAP_VISIBLE_GAME((p_)->group))

#define IS_CLASSROOM_MAP(mapid)			(((uint64_t)mapid)>>32 == 32)

//天使乐园
#define IS_PARADISE_MAP(mapid)            	(((uint64_t)mapid)>>32 == 300)
#define PARADISE_MAP(id)					((((map_id_t)300)<<32) + (id))

//珍宝库
#define IS_EXHIBIT_MAP(mapid)				(((uint64_t)mapid)>>32 == 33)
#define GET_EXHIBIT_MAP(id) 				((((map_id_t)33)<<32) + (id))

//养猪场
#define IS_CUTEPIG_MAP(mapid)				(((uint64_t)mapid)>>32 == 34)
#define GET_CUTEPIG_MAP(id) 				((((map_id_t)34)<<32) + (id))

//养猪场 美美屋
#define IS_CUTEPIG_SHOW_MAP(mapid)			(((uint64_t)mapid)>>32 == 35)
#define GET_CUTEPIG_SHOW_MAP(id) 			((((map_id_t)35)<<32) + (id))

//机械工厂  
#define IS_CUTEPIG_MACHINE_MAP(mapid)           (((uint64_t)mapid)>>32 == 36)
#define GET_CUTEPIG_MACHINE_MAP(id)             ((((map_id_t)36)<<32) + (id))

struct entry_pos {
	int			map;
	int			x;
	int			y;
};

typedef struct {
	uint32_t roomid;
	uint32_t last_level;
	uint32_t exp;
	int		 money;
	uint32_t evaluate;
	uint32_t flush_time;
	uint32_t flush_evaluate;
	uint32_t inner_style;
	uint32_t shop_style;
	uint32_t dish_total;
	uint32_t employed_count;
	uint8_t	 honors[8];
	uint32_t dish_bad;
	uint16_t stack_cmd;
	uint16_t stack_len;
	uint32_t stack[32];
} shop_map_info;

typedef struct {
	uint32_t roomid;
	uint32_t energy;
	uint32_t lovely;
	uint32_t course_id;
	uint32_t graduate_all;
	uint32_t class_all;
	uint8_t  honors[4];
} lahm_class_info;


typedef union {
	shop_map_info	shop;
	lahm_class_info	lahm_class;
} map_udat;

typedef struct map {
	map_id_t	id;
	uint8_t		flag;
	char		name[64];
	int			party_period;

	int			sprite_num;
	list_head_t	sprite_list_head;
	list_head_t	hash_list;
	int			entry_num;
	union {
		struct entry_pos 	entries[MAX_ENTRY_NUM_PER_MAP];
	}u;

	uint8_t			pet_cnt		: 3;
	uint8_t			pet_loaded	: 1;
	map_udat		mmdu[1];//notice
/*	the struct pets below is used only normal maps,
	so this can used the pets's space,somewhat it is like the union struct;
	if you want change this,only change the mmdu[0] to mmdu[1];
*/	struct pet		pets[MAX_PETS_PER_SPRITE];

	//for item - To Be Removed
	int				item_count;
	list_head_t		item_list_head;
	list_head_t		timer_list;
	list_head_t		ritem_list;
	list_head_t		hero_list_head;
	time_t			next_flight;
	uint32_t 		open_hour;
	uint32_t		close_hour;
	uint32_t		door_map;

	// for Scene Game
	game_group_t*	gamegrp;
	uint32_t		blood;
	uint8_t			rick_attack_tag[3];
} map_t;

/*enum {
	normal_map     = 1,
	map_with_game  = 2
};*/
#define MAP_WITH_GAME(m)	!!((m)->flag & 2)
#define SCENE_GAME_MAP(m)	!!((m)->flag & 4)

typedef struct map_item_group {
	uint8_t			type   : 5;
	uint8_t			action : 3;
	uint8_t			flag;

	//for game
	game_t*			game;
	game_group_t*	ggp;

	//for items search
	uint8_t		itm_count;
	int			itm_pos;
	int			leavegame_pos_x;
	int			leavegame_pos_y;
	//all groups
	list_head_t		list;
} map_item_group_t;

typedef struct map_item {
	uint8_t		id;

	uint8_t		occupied  : 4;
	uint8_t		direction : 4;
	map_item_group_t*	mig;

	//all items in one map
	list_head_t	list;
} map_item_t;

typedef struct pet_map {
	map_id_t		id;

	//all pets in one map
	list_head_t	list;
} pet_map_t;

typedef struct out_pet {
	pet_t* pet;
	//all pets in one map
	list_head_t	list;
} out_pet_t;

#define ITEM_OCCUPIED(item)			!!((item)->occupied & 1)
#define SET_ITEM_OCCUPIED(item)		(item)->occupied |= 1
#define ITEM_GAME_STARTED(item)		!!((item)->occupied & 2)
#define SET_ITEM_GAME_START(item)	((item)->occupied |= 2)
#define SET_ITEM_GAME_END(item)		((item)->occupied &= ~2)

extern list_head_t map_hash_slots[MAP_HASH_SLOT_NUM];
extern map_t normal_map[NORMAL_MAP_NUM];

static inline map_t* get_map(map_id_t map_id)
{
	map_t *p ;
	if (IS_NORMAL_MAP(map_id)) {
		return &normal_map[map_id];
	}
	list_for_each_entry(p, &map_hash_slots[map_id % MAP_HASH_SLOT_NUM], hash_list) {
		if (p->id == map_id)
		{
			return p;
		}
	}
	return NULL;
}

void unload_maps();
int  load_maps(const char* file);
void traverse_map(int (*func)(map_t *, void*));

map_t* alloc_map(map_id_t new_map);
int enter_home(sprite_t* p, uint32_t sprite_home);
int enter_map(sprite_t *p, map_id_t new_map, map_id_t old,uint32_t gdst);
void leave_map(sprite_t* p, int);
//map_t* get_map(map_id_t map_id);
struct entry_pos* get_entry(struct map* p, int id);
map_item_t* get_map_item(struct map* p, int id);
map_item_t* get_map_items(const map_item_group_t* p);

void release_map_items(game_group_t* ggp, map_item_group_t* p);

int send_to_self(sprite_t* p, uint8_t* buffer, int len, int completed);
int send_to_self_error(sprite_t* p, int cmd, int err, int completed);
void xform_users_at_map(map_id_t mapid, uint32_t itmid, time_t expiretm);

void send_to_map(sprite_t* s, uint8_t* buffer, int len, int completed);
void send_to_map2(const map_t* tile, uint8_t* buffer, int len);
void send_to_map3(map_id_t mapid, uint8_t* buffer, int len);
void send_to_map_except_self(sprite_t* s, uint8_t* buffer, int len, int completed);
void send_to_all_player_except_self(sprite_t* s, uint8_t* buffer, int len, int completed);
void send_to_all_players(sprite_t* s, uint8_t* buffer, int len, int completed);


void send_to_group(game_group_t* ggp, void* buffer, int len);
void send_to_group_except_self(game_group_t* ggp, uint32_t id, void* buffer, int len);
void init_outside_petmap();
int add_pet_to_map(pet_t* pet, map_id_t map_id);
int get_outside_pets_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int rm_pet_from_map(sprite_t* p, uint32_t pet_id, map_id_t map_id);

void rm_usrpets_when_logout(sprite_t* p);
int respond_pet_to_map(pet_t* pet, uint32_t action);
int get_recent_sprite_in_map_cmd (sprite_t *p, const uint8_t *body, int len);
int fight_boss_cmd(sprite_t *p, const uint8_t *body, int len);
int get_boss_info_cmd (sprite_t *p, const uint8_t *body, int len);
int blood_grow(void* owner, void* data);
int get_all_house_sprite_num(userid_t uid);

int get_map_user_cnt_cmd(sprite_t* p, const uint8_t *body, int len);
int get_map_user_cnt_callback(sprite_t* p,  uint32_t count);


#endif
