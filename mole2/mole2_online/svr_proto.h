
#ifndef SVR_PROTO_H
#define SVR_PROTO_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include <arpa/inet.h>
#include <async_serv/async_serv.h>

#include "benchapi.h"
#include "map.h"
#include "sprite.h"
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>


enum {
	bt_battle_info_msg		= 61006,
	bt_notify_info_cli		= 61007,
};

enum {
	bt_cross_info_msg		= 65001,
};


/**
  * @brief db protocol type
  */
typedef struct db_proto {
	/*! package length */
	uint32_t	len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	userid_t	id;
	uint8_t		body[];
}__attribute__((packed))db_proto_t;

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

// For batrserv
typedef struct batrserv_proto {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	battle_id_t	btid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) batrserv_proto_t;

// For homeserv
typedef struct homeserv_proto {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	map_id_t    mapid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) homeserv_proto_t;


int send_sw_keepalive_pkg(void* owner, void* data);

void connect_to_switch();
int send_to_switch(sprite_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, uint32_t sender_id);
int send_msg_to_switch(sprite_t* p, userid_t id, uint16_t cmd,  Cmessage * c_in );
int send_request_to_db(sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);
int send_to_db_no_sprite(userid_t id, uint16_t cmd, uint32_t seq, const void* dbpkgbuf, uint32_t body_len);
int send_to_batrserv(sprite_t* p, int cmd, uint32_t body_len, const void* body_buf, userid_t id, battle_id_t btid, uint32_t opid);
int batrserv_fd_idx(int fd);
int send_to_homeserv(sprite_t* p, int cmd, uint32_t body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid);


int homeserv_fd_idx(int fd);
int send_msg_to_db(sprite_t* p, userid_t id, uint16_t cmd,  Cmessage * c_in );

int send_msg_to_homeserv( userid_t id, map_id_t mapid, uint32_t opid, Cmessage * c_in=NULL );

int send_msg_to_homeserv_ex(sprite_t* p, int cmd, char * head_buf, Cmessage* c_in, userid_t id, map_id_t mapid, uint32_t opid);
int send_request_to_vipserv(sprite_t* p, uint32_t cmd, const void* codepkgbuf, uint32_t body_len);
#endif
