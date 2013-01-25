
#ifndef MOLE2_CLI_LOGIN_H
#define MOLE2_CLI_LOGIN_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "util.h"
#include "benchapi.h"
#include "map.h"

#define MOLE2_GAME_FLAG		7

/**
  * @brief session for loggin in
  */
typedef struct login_session {
	uint32_t	ip;
	uint32_t	tm1;
	uint32_t	uid;	
	uint32_t	tm2;
}__attribute__((packed))login_session_t;


typedef struct get_sprite_rsp {
	char				nick[USER_NICK_LEN];
	char				signature[32];
	uint32_t			flag;
	uint32_t			color;
	uint32_t			register_time;
	uint8_t				race;
	uint8_t				professtion;
	int32_t				prof_lv;
	uint32_t			honor;
	uint32_t			xiaomee;
	uint32_t			level;
	uint32_t			experience;
	uint16_t			physique;
	uint16_t			strength;
	uint16_t 			endurance;
	uint16_t			quick;
	uint16_t			intelligence;
	uint16_t			attr_addition;
	uint32_t			hp;
	uint32_t			mp;
	uint32_t			injury_lv;
	uint8_t				in_front;
	uint32_t			max_cloth_cnt;
	uint32_t			max_m_cnt;
	uint32_t			max_c_cnt;
}__attribute__((packed))get_sprite_rsp_t;

int login_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen);
int add_session_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen);

int db_get_sprite_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int check_session_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int add_session_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int proc_final_login_step(sprite_t* p);
void rsp_proto_login(sprite_t* p);
int auto_add_energy(void*owner, void *data);

static inline int db_get_sprite(sprite_t* p, userid_t uid)
{
	return send_request_to_db(p, uid, proto_db_get_sprite, NULL, 0);
}


#endif

