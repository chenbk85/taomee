#ifndef RACE_BONUS_H
#define RACE_BONUS_H
#include "item.h"

typedef struct {
	uint32_t		teamid;
	uint32_t		item_count;
	item_unit_t  	item[10];
}__attribute__(( packed ))team_item_t;

typedef struct {
	uint32_t		raceid;
	uint32_t		team_count;
	team_item_t		teams[10];
}__attribute__(( packed )) race_bonus_t;


int gain_race_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int send_race_bonus_to_db(sprite_t *p, uint32_t raceid, uint32_t teamid);
int send_race_bonus_to_client(sprite_t *p, uint32_t raceid, uint32_t teamid);
int get_race_teamid_flag_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int load_race_bonus(const char *file);


#endif
