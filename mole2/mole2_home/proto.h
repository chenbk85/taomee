#ifndef MOLE2_PROTO_H
#define	MOLE2_PROTO_H

#include <time.h>
#include <stdint.h>
#include <glib.h>
extern "C"{
#include <libtaomee/project/types.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/list.h>
#include <async_serv/net_if.h>
}


//#include "util.h"

#include "global.h"
#include "onlinehome.h"

enum {
	proto_max_len	= 8 * 1024
};

typedef struct home_proto {
	uint32_t	len;
	uint32_t	onlineid;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	mapid_t		homeid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) home_proto_t;

typedef struct client_proto {
	uint32_t	len;
	uint16_t	cmd;
	userid_t	id;
	uint32_t	seq;
	uint32_t	ret;
	uint8_t		body[];
} __attribute__((packed)) client_proto_t;

void tranfer_to_home(home_t* p_home, void* buf, int len);

void tranfer_to_home_cover_zero(home_t* p_home, void* buf, int len);

int send_to_online(void* buf, int len); 

void send_all_users_to_self(userid_t userid, uint32_t onlineid, home_t* p_home);

static inline int pkg_proto_head(void* buf,
								uint32_t len,
								uint32_t onlineid,
								uint16_t cmd,
								userid_t userid,								
								mapid_t	 homeid,
								uint32_t opid)
{
#ifdef __cplusplus
	home_proto_t* p_proto = reinterpret_cast<home_proto_t*>(buf);
#else
	home_proto_t* p_proto = buf;
#endif
	p_proto->len		= len;
	p_proto->onlineid	= onlineid;
	p_proto->cmd		= cmd;
	p_proto->ret		= 0;
	p_proto->id			= userid;	
	p_proto->homeid		= homeid;
	p_proto->opid		= opid;
	return sizeof(home_proto_t);
}

inline int init_pkg_proto(home_proto_t *p_proto,
								uint32_t onlineid,
								uint16_t cmd,
								userid_t userid,								
								mapid_t	 homeid,
								uint32_t opid)
{
	p_proto->len		= sizeof(*p_proto) ;
	p_proto->onlineid	= onlineid;
	p_proto->cmd		= cmd;
	p_proto->ret		= 0;
	p_proto->id			= userid;	
	p_proto->homeid		= homeid;
	p_proto->opid		= opid;
	return sizeof(home_proto_t);
}

static inline void pkg_client_head(void* buf, int cmd, int len)
{
#ifdef __cplusplus
	client_proto_t* p = reinterpret_cast<client_proto_t*>(buf);
#else
	client_proto_t* p = buf;
#endif
	p->len = htonl(len);
	p->cmd = htons(cmd);
	p->ret = 0;
}


static inline void init_two_head(void* buf, int len, uint32_t onlineid, uint16_t cmd, userid_t uid, mapid_t homeid, uint32_t opid)
{
	pkg_client_head((char*)buf + sizeof(home_proto_t), opid, len - sizeof(home_proto_t));
	pkg_proto_head(buf, len, onlineid, cmd, uid, homeid, opid);
}


#endif
