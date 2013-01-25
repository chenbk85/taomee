#ifndef MOLE2_MAP_H
#define MOLE2_MAP_H

#include "libxml/tree.h"
#include <math.h>
#include "sprite.h"
#include "global.h"
#include "libtaomee++/proto/proto_base.h"

#define MAP_INFO_BIRD_MAP_BIT		0x1
#define MAP_INFO_BOSS_MAP_BIT		0x2

#define IS_NORMAL_MAP(x_)			(((x_) < NORMAL_MAP_NUM) && ((x_) > 0))
#define IS_HOME_MAP(mapid) 		(high32_val(mapid) == 1 || IS_WAR_MAP(mapid)|| IS_TRADE_MAP(mapid))
#define IS_WAR_MAP(mapid)		((high32_val(mapid) & 0xFF000000) == 0xFE000000 && low32_val(mapid) > 40000)
#define IS_TRADE_MAP(mapid)		((high32_val(mapid) & 0xFF000000) == 0xFC000000 && low32_val(mapid) == 11216)

#define IS_MAZE_MAP(mapid)		((high32_val(mapid) & 0xFF000000) == 0xFF000000)
#define IS_COPY_MAP(mapid)  	((high32_val(mapid) & 0xFF000000) == 0xFD000000)
#define GET_TRADE_MAPID(r_)		(((uint64_t)(0xFC000000+(r_))<<32)|(11216))

inline bool user_is_in_war_map(sprite_t* p ){
	return p->tiles &&  IS_WAR_MAP( p->tiles->id );
}
#define MAP_HAS_BEAST(m_)		(((m_)->max_beast_cnt) || (IS_MAZE_MAP((m_)->id) && (m_)->maze_beastcnt))

#define MAP_AREA_OPEN(area_old_, area_new_) \
    ((area_new_) >= 100 && (area_new_) < 1000 && !((area_new_) % 100) \
    && ((area_old_) / (area_new_) == 1))

#define CHECK_POS_VALID(p_, x_, y_) do { \
			if((x_) > 2000 || ((y_) > 2000)) { \
				KERROR_LOG(p_->id, "invalid pos\t[%u %u]", x_, y_); \
				return -1; \
			} \
		} while(0)

#define CHECK_USER_IN_MAP(p_, mapid_) \
		do { \
			if (!(p_)->tiles || ((mapid_) && (p_)->tiles->id != (mapid_))) { \
				KERROR_LOG(p_->id, "not in map\t[%u]", (uint32_t)mapid_); \
				return send_to_self_error(p_, p_->waitcmd, cli_err_not_right_map, 1); \
			} \
		} while (0)

#define CHECK_NOT_IN_TRADE(p_) \
		do { \
			if (!(p_)->tiles || IS_TRADE_MAP((p_)->tiles->id) ) { \
				KERROR_LOG(p_->id, "in trade map\t") ; \
				return send_to_self_error(p_, p_->waitcmd, cli_err_not_right_map, 1); \
			} \
		} while (0)

#define CHECK_IN_SAME_MAP(p_, lp_) \
		do { \
			if (!(p_)->tiles || !(lp_)->tiles || ((p_)->tiles->id != ((lp_)->tiles->id))) { \
				KERROR_LOG(p_->id, "not in same map\t[%lx %lx]",(p_)->tiles->id ,(lp_)->tiles->id); \
				return send_to_self_error(p_, p_->waitcmd, cli_err_not_same_map, 1); \
			} \
		} while (0)

#define CHECK_USER_IN_MAPS(p_, mapids_,len_) \
		do { \
			if (!(p_)->tiles || (is_in((mapids_),(p_)->tiles->id ,(len_)) == -1)) { \
				KERROR_LOG(p_->id, "not in map\t[%u]", (uint32_t)(p_)->tiles->id); \
				return send_to_self_error((p_), (p_)->waitcmd, cli_err_not_right_map, 1); \
			} \
		} while (0)

enum {
	jump_type_normal,
	jump_type_bird_cost,
	jump_type_no_check,
	jump_type_bird_free,
	jump_type_with_item,
	jump_type_max,
};

void unload_maps();
int load_maps(xmlNodePtr cur);
int reload_maps(xmlNodePtr cur);
void traverse_map(int (*func)(map_t *, void*));

map_t* alloc_map(map_id_t new_map);
int enter_map(sprite_t* p, map_id_t new_entry);
void leave_map(sprite_t* p, map_id_t newmap);
map_t* get_map(map_id_t map_id);
int send_to_self(sprite_t* p, uint8_t* buffer, int len, int completed);
int send_to_self_error(sprite_t* p, int cmd, int err, int completed);
void send_to_map(sprite_t* p, uint8_t* buffer, int len, int completed,int self);
void send_to_map2(const map_t* tile, uint8_t* buffer, int len);
void send_to_map3(map_id_t mapid, uint8_t* buffer, int len);
int get_map_users_cmd(sprite_t *p, uint8_t *body, uint32_t len);
void response_proto_enter_map(sprite_t* p, int complet, int self);

int set_play_mode_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int get_map_state_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_map_state_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int chk_teamate_enter_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int register_boss_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int register_boss_map_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_boss_maps_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int check_team_boss_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
void notify_next_fight_step(sprite_t* p);
int team_chocobo_registered(sprite_t* p, map_t* pm);
int team_boss_map_registered(sprite_t* p, map_t* pm);
int cache_boss_map_registerd(sprite_t* p, map_t* pm);
int cache_bird_map_registerd(sprite_t* p, map_t* pm);
int map_state_change(void* owner, void* data);
int refresh_elite_beast(void* owner, void* data);
void rsp_elite_beast(sprite_t* p, map_t* mp);

void send_msg_to_map2(const map_t* tile, int cmd, Cmessage * c_in);
static inline double distance(int x1, int y1, int x2, int y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

static inline int near_pos(sprite_t* p, uint16_t posX, uint16_t posY)
{
	KDEBUG_LOG(p->id, "NEAR POS\t[%u %u %u %u]", posX, posY, p->posX, p->posY);
	if (posX == 0xFFFF || posY == 0xFFFF) return 1;
	return abs_uint(p->posX, posX) < 50 && abs_uint(p->posY, posY) < 50;
}

static inline map_info_t* cache_get_map_info(sprite_t* p, uint32_t mapid)
{
	return (map_info_t *)g_hash_table_lookup(p->map_infos, &mapid);
}

static inline void cache_add_bird_map(sprite_t* p, uint32_t mapid)
{
	map_info_t* pmi = cache_get_map_info(p, mapid);
	if (!pmi) {
		pmi = (map_info_t *)g_slice_alloc(sizeof(map_info_t));
		pmi->mapid = mapid;
		pmi->state = 0;
		g_hash_table_insert(p->map_infos, &pmi->mapid, pmi);
	}
	pmi->state |= MAP_INFO_BIRD_MAP_BIT;
}

static inline void cache_add_boss_map(sprite_t* p, uint32_t mapid)
{
	map_info_t* pmi = cache_get_map_info(p, mapid);
	if (!pmi) {
		pmi = (map_info_t *)g_slice_alloc(sizeof(map_info_t));
		pmi->mapid = mapid;
		pmi->state = 0;
		g_hash_table_insert(p->map_infos, &pmi->mapid, pmi);
	}
	pmi->state |= MAP_INFO_BOSS_MAP_BIT;
}

void send_msg_to_map(sprite_t* s, int cmd, Cmessage * c_in,  int completed,int self);

static inline map_copy_instance_t* get_map_copy(map_id_t portal)
{
	//TODO
	return (map_copy_instance_t*)g_hash_table_lookup(map_copys, &portal);
}

static inline map_copy_instance_t* get_map_copy_with_mapid(map_id_t mapid)
{
	return get_map_copy(mapid & 0xFFFFFFFF00000000lu);
}

static inline int get_copy_layer_idx(map_copy_instance_t* pmci, uint32_t mapid)
{
	for (int loop = 0; loop < pmci->p_copy_config->layer_cnt; loop ++) {
		if (mapid == pmci->p_copy_config->layers[loop].templatemap)
			return loop;
	}
	return -1;
}
void send_msg_to_map3(map_id_t mapid, int cmd, Cmessage * c_in );
int send_to_trade_map(void* buf, int len);
#endif
