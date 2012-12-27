
#ifndef SVR_PROTO_H
#define SVR_PROTO_H

#include <stdint.h>

#include "util.h"
#include "global.h"
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include "Csprite.h"


enum {
	bt_battle_info_msg		= 61006,
	bt_notify_info_cli		= 61007,
};

enum {
	bt_cross_info_msg		= 65001,
};


/**
  * @brief code protocol type
  */
typedef struct code_proto {
	uint32_t	len;
	uint32_t	ver;
	uint16_t	cmd;
	uint32_t	ret;
	userid_t	id;
	uint8_t		body[];
}__attribute__((packed))code_proto_t;


// For switch
typedef struct switch_proto {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	sender_id;
	uint8_t		body[];
} __attribute__((packed)) switch_proto_t;


int send_msg_to_db_ex(Csprite* p, userid_t id, uint16_t cmd,  Cmessage * c_in=NULL );

int send_msg_to_db(Csprite* p, uint16_t cmd, Cmessage * c_in=NULL  );
int send_msg_to_switch(Csprite* p, uint16_t cmd, userid_t sender_id ,Cmessage * c_in);
void connect_to_switch();
void send_sw_report_user_onoff( Csprite* p, bool is_on_online );
void send_sw_user_island_complete(Csprite* p, uint32_t islandid);
void main_login_add_session(Csprite * p, uint32_t which_game, uint32_t ip);
void main_login_add_game_flag(Csprite * p, uint16_t channelid, uint32_t  which_game);

int send_udp_post_msg_to_db(userid_t id, uint16_t cmd, Cmessage* c_in = NULL);
#endif
