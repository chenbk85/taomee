/*
 * =====================================================================================
 *
 *       Filename:  mole_dragon.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/05/2010 10:18:10 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef  MOLE_DRAGON_INC
#define  MOLE_DRAGON_INC

#define DRAGON_NICK_LEN 16

typedef struct mole_dragon_info {
	uint32_t id;
	uint8_t name[DRAGON_NICK_LEN];
	uint32_t growth;
	uint32_t state;
}__attribute__((packed)) mole_dragon_info_t;

struct call_dragon_in {
	uint32_t id;
	uint32_t set_state;
}__attribute__((packed));

struct feed_dragon_in {
	uint32_t dragon_id;
	uint32_t growth;
	uint32_t item_id;
	uint32_t growth_limit;
}__attribute__((packed));

struct dragon_id_name {
	uint32_t dragon_id;
	uint8_t name[DRAGON_NICK_LEN];
}__attribute__((packed));

enum {
	NOT_CALL_STATE = 0,
	CALL_STATE = 1,
};

int get_dragon_bag_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_dragon_bag_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int call_dragon_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int call_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int release_dragon_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int release_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int set_dragon_name_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int set_dragon_name_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int feed_dragon_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int feed_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int brood_dragon_egg_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int brood_dragon_egg_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int accelerate_dragon_brood_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int accelerate_dragon_brood_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_baby_dragon_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_baby_dargon_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_brood_dragon_egg_time_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_brood_dragon_egg_time_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_call_dragon_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_call_dragon_info_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int is_dragon_adult(sprite_t *p);

int expand_dragon_bag_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int train_kaku_nian_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_train_kaku_nian_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_train_kaku_nian_info_callback(sprite_t *p, uint32_t id, const char *buf, int len);
int change_ice_dragon_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int change_ice_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len);



#endif   /* ----- #ifndef MOLE_DRAGON_INC  ----- */

