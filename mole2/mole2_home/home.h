#ifndef	HOME_H
#define	HOME_H

#include	"proto.h"
#include 	"onlinehome.h"

enum state_type {
	team_switch	= 0,
	sns_state		= 1,
	pk_switch		= 2,
};

int proto_enter_home_op(home_proto_t* p_proto);
int handle_online_op(home_t* p_home, sprite_ol_t* p, home_proto_t* data);

home_t* add_home(mapid_t homeid);
home_t* free_home(home_t* p_home);
sprite_ol_t* add_sprite_ol(home_t* p_home, home_proto_t* p_proto);
void free_sprite_ol(home_t* p_home, sprite_ol_t* p);

void add_online_user(home_t* p_home, uint32_t onlineid);
void del_online_user(home_t* p_home, uint32_t onlineid);

void clean_all_users_from_online(int fd);

void init_home_maps();

void fini_home_maps();



#endif
