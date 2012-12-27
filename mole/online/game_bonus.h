#ifndef GAME_BONUS_H
#define GAME_BONUS_H
#include "item.h"

typedef struct {
	uint32_t	start;
	uint32_t	end;
	uint32_t 	day_type;
	uint32_t 	day_count;
	uint32_t	medal;
	uint32_t	item_count;
	item_unit_t	item[10];
}__attribute__(( packed ))score_item_t;

typedef struct {
	uint32_t		game_id;
	uint32_t		cmd;
	uint32_t		score_count;
	score_item_t	score[20];
}__attribute__(( packed )) game_bonus_t;


int add_game_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int game_bonus_day_limit_callback(sprite_t *p);
int send_game_bonus_to_db(sprite_t *p, uint32_t gameid, uint32_t i);
int send_game_bonus_to_client(sprite_t *p, uint32_t gameid, uint32_t i);
int load_game_bonus(const char *file);
int sendto_db_game_items(sprite_t *p,uint8_t *buff);
int pack_client_itmid_accord_id_score(sprite_t *p, uint8_t buff[], int *index, int game_bonus_id, int score, int itmid);
int cmd_callback(sprite_t* p, uint32_t id, char* buf, int len);
#endif
