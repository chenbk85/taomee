#ifndef ONLINE_HOMEMAP_H_
#define ONLINE_HOMEMAP_H_

#include "benchapi.h"
#include "battle.h"
#include "map.h"
#include "pet.h"

enum BroadcasterCmd {
	MCAST_mole_grp_msg		= 61001,
	MCAST_home_msg			= 61002,
	MCAST_login_msg			= 61003,
	MCAST_mole_class_msg	= 61004,
	MCAST_mole_sprite_info	= 61005,
	MCAST_sport_team_score	= 61006,
	MCAST_reload_conf		= 62000
};

//
enum SpriteOpConst {
	SOP_enter_map	=  1,
	SOP_get_home_user,
	SOP_home_msg,
	SOP_walk_pet,
	SOP_user_show,
	SOP_user_walk,
	SOP_set_user_nick,
	SOP_get_users_from_HS,
	SOP_max			=  100,
};

// holds all the @sprites that are in @mapid
typedef struct SpriteWhere {
	map_id_t 	mapid;
	GHashTable* sprites;
} sprite_where_t;


/*! sprite_ol */
typedef struct attire_simple {
	uint32_t	attireid;
	uint32_t	type;
	uint16_t	level;
} __attribute__((packed)) attire_simple_t;

/*! sprite simple info */
typedef struct sprite_base_info{
	userid_t			userid;
	char				nick[USER_NICK_LEN];
	uint32_t			flag;
	uint32_t			hero_team;
	uint32_t 			color;
	uint8_t				race;
	uint8_t				prof;
	uint32_t			prof_lv;
	uint32_t			honor;
	uint32_t			pk_point;
	uint32_t			level;
	uint32_t			hp;
	uint32_t			mp;
	uint32_t			hp_max;
	uint32_t			mp_max;
	uint32_t			injury_lv;		// 0: healthy 1:white 2:yellow 3:red
	uint32_t 			changetype;//shapeshifting变身精灵类型
	map_id_t			homeid;
	uint32_t			pos_x;
	uint32_t			pos_y;
	uint32_t			base_action;
	uint32_t			advance_action;
	uint8_t				direction;
	uint32_t			teamid;
	uint32_t			team_idx;
	uint8_t				team_registerd;
	char				team_nick[TEAM_NICK_LEN];
	uint8_t				team_switch;
	uint8_t				pk_switch;
	uint8_t				busy_state;
	//uint32_t			xiaomee;
	//char				shop_nick[TEAM_NICK_LEN];	
	/*! attire on body */		
	uint8_t				attire_cnt;
	attire_simple_t		sz_attire[MAX_ITEMS_WITH_BODY*2];
	/*! whether pet followed */
	uint8_t				pet_followed;
}__attribute__((packed))sprite_base_info_t;

typedef struct pet_simple_info{
	/*! pet followed */
	uint32_t			petid;
	uint32_t			pet_type;
	uint8_t 			pet_race;
	char				pet_nick[16];
	uint32_t			pet_level;
}__attribute__((packed))pet_simple_info_t;


typedef struct sprite_ol {
	sprite_base_info_t	sprite_info;
	/*! pet followed */
	pet_simple_info_t	pet_sinfo;
}__attribute__((packed)) sprite_ol_t;

//
int handle_homeserv_pkg(homeserv_proto_t* compkg, int len);
void traverse_across_svr_sprites(map_id_t mapid, void (*action)(void* key, void* sinfo, void* data), void* data);
void  init_home_maps();

void clean_home_by_idx(int idx);

void fini_home_maps();
void remove_all_across_svr_sprites();

void try_del_home_by_id(map_id_t homeid);


static inline void
pkg_home_sprite_info(sprite_t* p, uint8_t* buff)
{
	sprite_base_info_t* psbi = (sprite_base_info_t*)buff;
	psbi->userid 		= p->id;
	memcpy(psbi->nick, p->nick, sizeof(psbi->nick));
	psbi->flag = 		p->flag;
	psbi->hero_team =	p->hero_team;
	psbi->color = 		p->color;
	psbi->race = 		p->race;
	psbi->prof=		 	p->prof;
	psbi->prof_lv	=	p->prof_lv;
	psbi->honor = 		p->honor;
	psbi->pk_point	=	p->pkpoint_total;
	psbi->level = 		p->level;
	psbi->hp = 			p->hp;
	psbi->mp = 			p->mp;
	psbi->hp_max = 		p->hp_max;
	psbi->mp_max = 		p->mp_max;
	psbi->injury_lv = 	p->injury_lv;
	psbi->changetype = 	p->shapeshifting.pettype;
	psbi->homeid = 		p->tiles->id;
	psbi->pos_x = 		p->posX;
	psbi->pos_y = 		p->posY;
	psbi->base_action = p->base_action;
	psbi->advance_action = p->advance_action;
	psbi->direction= 	p->direction;
	psbi->teamid = 		p->btr_team ? p->btr_team->teamid: 0;
	psbi->team_idx = 	p->btr_team ? get_user_in_bt(p->btr_team, p->id) : 0;
	psbi->team_switch = team_switch_state(p);
	psbi->pk_switch = 	p->pk_switch;
	psbi->busy_state = 	p->busy_state;
	//psbi->xiaomee    =  p->xiaomee;
	//memcpy(psbi->shop_nick, p->shopname, sizeof(psbi->shop_nick));
	psbi->attire_cnt = 	0;
	memset(psbi->sz_attire, 0, sizeof(attire_simple_t) * MAX_ITEMS_WITH_BODY*2);
	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t*)pcloths->data;
		psbi->sz_attire[psbi->attire_cnt].attireid = pc->clothid;
		psbi->sz_attire[psbi->attire_cnt].type = pc->clothtype;
		psbi->sz_attire[psbi->attire_cnt].level = pc->clothlv;
		psbi->attire_cnt ++;
		pcloths = pcloths->next;
	}
	g_list_free(head);

	psbi->pet_followed = p->p_pet_follow ? 1 : 0;
	if (p->p_pet_follow) {
		pet_simple_info_t* ppsi = (pet_simple_info_t*)(buff + sizeof(sprite_base_info_t));
		ppsi->petid = p->p_pet_follow->petid;
		ppsi->pet_type = p->p_pet_follow->pettype;
		ppsi->pet_race = p->p_pet_follow->race;
		memcpy(ppsi->pet_nick, p->p_pet_follow->nick, sizeof(ppsi->pet_nick));
		ppsi->pet_level = p->p_pet_follow->level;
	}
}

static inline int pkg_home_attire_simple(int count ,const attire_simple_t* p_attire, uint8_t* buf)
{
	int j = 0;
	PKG_UINT8 (buf, count, j);
	for (int i = 0; i < count; i++) {
		PKG_UINT32 (buf, p_attire[i].attireid, j);
		PKG_UINT32 (buf, p_attire[i].type, j);
		PKG_UINT16 (buf, p_attire[i].level, j);
	}
	return j;
}

static inline int pkg_home_pet_follow(const sprite_ol_t* p, uint8_t* buf)
{
	int i = 0;
	if (!p->sprite_info.pet_followed) {
		PKG_UINT32(buf, 0, i);
		PKG_UINT32(buf, 0, i);
		PKG_UINT8(buf, 0, i);
		PKG_STR(buf, "", i, USER_NICK_LEN);
		PKG_UINT32(buf, 0, i);
		return i;
	}
	
	PKG_UINT32(buf, p->pet_sinfo.petid, i);
	PKG_UINT32(buf, p->pet_sinfo.pet_type, i);
	PKG_UINT8(buf, p->pet_sinfo.pet_race, i);
	PKG_STR(buf, p->pet_sinfo.pet_nick, i, USER_NICK_LEN);
	PKG_UINT32(buf, p->pet_sinfo.pet_level, i);
	return i;
}

static inline int pkg_sprite_ol_simply(const sprite_ol_t *p, uint8_t *buf)
{
	int i = 0;
	KDEBUG_LOG(p->sprite_info.userid, "PKG SPRITE OL");
	PKG_UINT32(buf, p->sprite_info.userid, i);
	PKG_STR(buf, p->sprite_info.nick, i, USER_NICK_LEN);
	PKG_UINT32(buf, p->sprite_info.flag, i);
	i += 16;
	PKG_UINT32(buf, p->sprite_info.hero_team, i);
	PKG_UINT32(buf, p->sprite_info.color, i);
	PKG_UINT8(buf, p->sprite_info.race, i);
	PKG_UINT8(buf, p->sprite_info.prof, i);
	PKG_UINT32(buf, p->sprite_info.prof_lv, i);
	PKG_UINT32(buf, p->sprite_info.honor, i);
	PKG_UINT32(buf, p->sprite_info.pk_point, i);
	PKG_UINT32(buf, p->sprite_info.level, i);
	PKG_UINT32(buf, p->sprite_info.hp, i);
	PKG_UINT32(buf, p->sprite_info.mp, i);
	PKG_UINT32(buf, p->sprite_info.hp_max, i);
	PKG_UINT32(buf, p->sprite_info.mp_max, i);
	PKG_UINT32(buf, p->sprite_info.injury_lv, i);
	PKG_UINT32(buf, p->sprite_info.changetype, i);

	PKG_MAP_ID(buf, p->sprite_info.homeid, i);
	PKG_UINT32(buf, p->sprite_info.pos_x, i);
	PKG_UINT32(buf, p->sprite_info.pos_y, i);
	PKG_UINT32(buf, p->sprite_info.base_action, i);
	PKG_UINT32(buf, p->sprite_info.advance_action, i);
	PKG_UINT8(buf, p->sprite_info.direction, i);
	PKG_UINT32(buf, p->sprite_info.teamid, i);
	PKG_UINT32(buf, p->sprite_info.team_idx, i);
	PKG_UINT32(buf, p->sprite_info.team_registerd, i);
	PKG_STR(buf, p->sprite_info.team_nick, i, TEAM_NICK_LEN);
	PKG_UINT8(buf, p->sprite_info.team_switch, i);
	PKG_UINT8(buf, p->sprite_info.pk_switch, i);
	PKG_UINT8(buf, p->sprite_info.busy_state, i);
	i += 32;
	i += pkg_home_attire_simple(p->sprite_info.attire_cnt, p->sprite_info.sz_attire, buf + i);
	i += pkg_home_pet_follow(p, buf + i);
	return i;
}

static inline void
broadcast_sprite_info(sprite_t* p)
{
	map_id_t mapid = p->tiles->id;
	if (IS_HOME_MAP(mapid)){
		uint8_t buff[4096];
		pkg_home_sprite_info(p, buff);
		int len = sizeof(sprite_base_info_t) + (p->p_pet_follow ? sizeof(pet_simple_info_t) : 0);
		send_to_homeserv(p, MCAST_home_msg, len, buff, p->id, mapid, SOP_enter_map); 
	}
}

int init_home_handle_funs();
int noti_close_shop(void*owner, void *data);
void home_del_a_user(sprite_where_t *spwhere, uint32_t leave_uid);
#endif

