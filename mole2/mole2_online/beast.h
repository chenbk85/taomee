
#ifndef MOLE2_BEAST_H
#define MOLE2_BEAST_H

#include "benchapi.h"
#include "dbproxy.h"
#include "util.h"
#include "battle.h"

#define SPEC_BEAST_BASE			100000

enum {
	beast_group_lvtype_max	= 0,
	beast_group_lvtype_min	= 1,
	beast_group_lvtype_ave	= 2,
	beast_group_lvtype_m3	= 3,
	beast_group_lvtype_m3_ex= 4,
};

enum {
	group_type_normal		= 0,
	group_type_maze			= 1,
	group_type_refresh		= 2,
	group_type_clean		= 3,
	group_type_firework		= 4,
	group_type_rand			= 5,
};

enum {
	refresh_time_speed		= 0x01,
	refresh_cnt_speed		= 0x02,
	refresh_immediately		= 0x04,
	refresh_wait_all		= 0x08,
	refresh_manual			= 0x10,
	refresh_personal		= 0x20,
};

void try_start_group(refresh_group_t* prg);
void try_end_group(refresh_group_t* prg);
void noti_map_grp_refreshed(refresh_group_t* prg);

beast_group_t* get_beast_grp(uint32_t grpid);
void init_beast_grp();
void fini_beast_grp();
int load_beast_grp(xmlNodePtr cur);
int load_rare_beasts(xmlNodePtr cur);

refresh_group_t* get_refresh_grp(uint32_t refreshid);

int refresh_beast_grp(void* owner, void* data);
void refresh_all_map_grp();
int reduce_map_grp(sprite_t *p,uint32_t refreshid, map_t* tile, uint32_t grpid);
int refresh_one_group(refresh_group_t* prg);

int get_map_grp_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_monster_handbook_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_monster_handbook_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int get_monster_handbook_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_monster_handbook_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

beast_kill_t* cache_get_beast_kill_info(sprite_t* p, uint32_t beastid);

#endif
