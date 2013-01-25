#ifndef HOME_BENCHAPI_H
#define HOME_BENCHAPI_H

extern "C" {

#include <libtaomee/project/types.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/list.h>
#include <async_serv/net_if.h>
}
#include <string.h>
#include <glib.h>

#define MAX_ATTIRE_ON_BODY	8

#define NICK_LEN	16
#define SIGN_LEN	32
#define TEAM_NICK_LEN 16

typedef uint64_t	mapid_t;

typedef uint64_t	map_id_t;

/*!  home */
typedef struct home {
	mapid_t		homeid;
	GHashTable*	sprites;
	GHashTable*	onlines;
} home_t;

typedef struct online_each {
	uint32_t	onlineid;
	uint32_t	user_cnt;
}online_each_t;

/*! online home */
/*! sprite attire */
typedef struct attire_simple {
	uint32_t	attireid;
	uint32_t	type;
	uint16_t	level;
} __attribute__((packed))attire_simple_t;

/*! sprite simple info */
typedef struct sprite_base_info{
	userid_t			userid;
	char				nick[NICK_LEN];
	uint32_t			flag;
	uint32_t			hero_team;
	uint32_t 			color;
	uint8_t				race;
	uint8_t				profession;
	uint32_t			prof_lv;
	uint32_t			honor;
	uint32_t			pk_point;
	uint32_t			level;
	uint32_t			hp;
	uint32_t			mp;
	uint32_t			hp_max;
	uint32_t			mp_max;
	uint32_t			injury_lv;		// 0: healthy 1:white 2:yellow 3:red
	uint32_t			changetype;		// 0: healthy 1:white 2:yellow 3:red
	mapid_t				homeid;
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
	//char				shop_nick[TEAM_NICK_LEN];
	/*! attire on body */		
	uint8_t				attire_cnt;
	attire_simple_t		sz_attire[MAX_ATTIRE_ON_BODY*2];
	uint8_t				pet_followed;
}__attribute__((packed))sprite_base_info_t;

typedef struct {
	uint32_t	petid;
	uint32_t	pet_type;
	uint8_t 	pet_race;
	char		pet_nick[NICK_LEN];
	uint32_t	pet_level;
}__attribute__((packed))pet_simple_info_t;

typedef struct sprite_ol {
	uint32_t		onlineid;
	uint16_t		cmd;
	userid_t		id;
	mapid_t			homeid;
	uint32_t		opid;
	sprite_base_info_t	sprite_info;
	pet_simple_info_t	pet_sinfo;
} __attribute__((packed))sprite_ol_t;

#define ADD_TIMER(owner, nbr_, data_, last_time_)  \
	ADD_TIMER_EVENT_EX((owner), (nbr_), (data_), get_now_tv()->tv_sec + (last_time_))

#endif
