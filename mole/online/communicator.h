#ifndef ONLINE_COMMUNICATOR_H_
#define ONLINE_COMMUNICATOR_H_

#include "benchapi.h"

#include "map.h"
#include "pet.h"
#include "protoheaders.h"
#include "mcast.h"

#define MAX_HOMESERV_NUM	8

enum BroadcasterCmd {
	MCAST_mole_grp_msg		= 61001,
	MCAST_home_msg			= 61002,
	MCAST_login_msg			= 61003,
	MCAST_mole_class_msg	= 61004,
	MCAST_mole_sprite_info	= 61005,
	MCAST_sport_team_score	= 61006,
	MCAST_reload_conf		= 62000
};

enum SpriteOpConst {
	SOP_enter_map,
	SOP_walk_pet,
	SOP_get_users_from_HS,
	SOP_get_map_users_cnt,
	SOP_max
};

typedef struct CommunicatorBody {
	map_id_t    mapid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) communicator_body_t;

typedef struct sprite_to_online{
	userid_t		id;
	char			nick[USER_NICK_LEN];
	userid_t		inviter_id;
	uint32_t		invitee_cnt;
	uint32_t		old_invitee_cnt;

	uint32_t		flag; // VIP, SMC, Lock Home, Mole Messenger, Deny Req of Adding Friends, used to be VIP
	uint32_t		color;
	uint8_t			login		: 1;
	uint8_t			direction	: 3;

	int8_t			item_cnt;
	uint32_t		items[MAX_ITEMS_WITH_BODY];

	//for map
	uint16_t		posX;
	uint16_t		posY;
	void*			tiles;
	uint32_t		action;
	uint32_t		action2;
	uint32_t		sub_grid;

    uint32_t        car_status;//0, no car; 1, driving;
    car_t           car_driving;

    uint32_t        student_count;
    lahm_student_t  students[STUDENT_COUNT_MAX];
    uint32_t        teach_exp;
    uint32_t        exam_num;

    uint32_t        dietitian_exp;

    uint32_t        animal_nbr;
    animal_attr_t   animal;

	uint32_t 		dragon_id;
	uint8_t			dragon_name[16];
	uint32_t		dragon_growth;
	uint32_t		dungeon_explore_exp;
	uint32_t		team_id;
	//cmd process system
	void*			group;
	pet_t*			followed;
	sprite_tmp_info_t   tmpinfo;
	uint32_t			lamu_action;
}__attribute__ ((packed)) sprite_to_online_t;

extern int school_bar_fd;
extern int use_homeserver;
extern int home_serv_fd[MAX_HOMESERV_NUM];

int send_to_broadcaster(int cmd, int body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid);
int send_to_homeserver(int cmd, int body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid);
int handle_homeserv_pkg(communicator_header_t* compkg, int len);
int send_to_school_bar(int cmd, int body_len, const void* body_buf, userid_t id);

static inline void
broadcast_sprite_info(sprite_t* p)
{
	map_id_t mapid = p->tiles->id;
	if (!IS_NORMAL_MAP(mapid)){
		int addlen = (p->followed ? sizeof(pet_t) : 0);
		if (addlen) {
			memcpy(p->session, p->followed, addlen);
		}
		if (use_homeserver) {
		    DEBUG_LOG("userid:%d sprite_t len :%lu addlen:%d",p->id, sizeof(sprite_t), addlen);
			send_to_homeserver(MCAST_home_msg, sizeof(sprite_t) + addlen, p, p->id, mapid, SOP_enter_map);
		} else {
			send_to_broadcaster(MCAST_home_msg, sizeof(sprite_t) + addlen, p, p->id, mapid, SOP_enter_map);
		}
	}
}

static inline void
broadcast_pet_info(sprite_t* p, int towalk)
{
	map_id_t mapid = p->tiles->id;
	if (!IS_NORMAL_MAP(mapid)){
		if (use_homeserver) {
			send_to_homeserver(MCAST_home_msg, (towalk ? sizeof(pet_t) : 0), p->followed, p->id, mapid, SOP_walk_pet);
		} else {
			send_to_broadcaster(MCAST_home_msg, (towalk ? sizeof(pet_t) : 0), p->followed, p->id, mapid, SOP_walk_pet);
		}
	}
}

void traverse_across_svr_sprites(map_id_t mapid, void (*action)(void* key, void* sinfo, void* data), void* data);
sprite_t* get_across_svr_sprite(userid_t uid, map_id_t mapid);
uint32_t  get_across_svr_sprite_cnt(map_id_t mapid);

int  sprite_info_chg_op(uint32_t uid, void* buf, int len);
int  init_communicator();
void fini_communicator();
void remove_all_across_svr_sprites();
int home_server_fd_idx(int fd);

void handle_mcast_pkg(communicator_header_t* compkg, int len);

#endif // ONLINE_COMMUNICATOR_H_
