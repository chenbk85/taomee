
#ifndef HS_PROTO_H
#define HS_PROTO_H

#include <stdint.h>

#define proto_test_alive	3000

enum {
	pkg_size	= 8192
};

enum BroadcasterCmd {
	MCAST_mole_grp_msg		= 61001,
	MCAST_home_msg			= 61002,
	MCAST_login_msg			= 61003,
	MCAST_mole_class_msg	= 61004,
	MCAST_mole_sprite_info	= 61005
};

//
enum SpriteOpConst {
	HS_enter_map 			= 0,
	HS_walk_pet				= 1,
	HS_send_home_users		= 2,
	HS_get_map_users_cnt	= 3,
	
	HS_MODIFY_NICK			= 208,
	HS_MODIFY_COLOR			= 209,
	HS_WALK					= 303,
	HS_ACTION				= 305,
	HS_LEAVE_MAP			= 402,
	HS_SHOW_ITEM_USE		= 504,
	HS_SHOW_ITEM_EXPIRE		= 510,
	HS_CANCEL_CHANGE_FORM	= 516,
	HS_LAHM_DO_ACTION		= 1212,
	HS_LAHM_ACTION_EXPIRE	= 1214,
	HS_PROTO_CALL_DRAGON	= 1228,
	HS_max
};

typedef uint64_t map_id_t;
typedef uint32_t userid_t;

// For Broadcaster
typedef struct CommunicatorHeader {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	map_id_t    mapid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) communicator_header_t;


/* Define the communication protocol between the switch and online 
   server(or adminer server, login server) */
typedef struct svr_proto {
        /* package head */
        uint32_t	len; // package length, no more than 4k bytes
        uint32_t	seq; // must be 0
        uint16_t	cmd; // cmd id, >OxEA60
        uint32_t	ret; // Result, must be 0
        userid_t	id; // Userid
        /* package body, variable length */
        uint8_t		body[];
} __attribute__((packed))svr_proto_t;

typedef struct protocol {
	uint32_t	len;
	uint8_t		ver;
	uint32_t	cmd;
	userid_t	id;
	uint32_t	ret;
	uint8_t		body[];
} __attribute__((packed)) protocol_t;

static inline void
init_proto_head(void* buf, userid_t uid, uint32_t len, uint32_t seq, uint16_t cmd, uint32_t ret)
{
	svr_proto_t* hdr = buf;
	hdr->len = len;
	hdr->seq = seq;
	hdr->cmd = cmd;
	hdr->ret = ret;
	hdr->id  = uid;
}

static inline void
init_proto_head2(void* buf, int cmd, int len)
{
#ifdef __cplusplus
	protocol_t* p = reinterpret_cast<protocol_t*>(buf);
#else
	protocol_t* p = buf;
#endif
	p->len = htonl(len);
	p->cmd = htonl(cmd);
	p->ver = 0x27;
	p->ret = 0;
}

#endif

