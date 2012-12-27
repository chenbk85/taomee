

#include "proto.h"
#include "exclu_things.h"
//#include "communicator.h"

#include "party.h"


typedef struct party_info{
	uint32_t 	party_start_time;
	uint32_t    mapid;
	uint32_t 	party_type;
	userid_t 	id;
	uint32_t 	isvip;
	uint32_t 	color;
	char		nick[USER_NICK_LEN];
}__attribute__(( packed ))party_info_t;

typedef struct party_list{
	uint32_t 		count;
	party_info_t	party[];
}__attribute__(( packed ))party_list_t;

typedef struct own_party{
	uint32_t 	online_id;
	uint32_t 	party_start_time;
	uint32_t    mapid;
	uint32_t 	party_type;
}__attribute__(( packed ))own_party_t;

typedef struct own_party_list{
	uint32_t 		count;
	own_party_t		own_party[];
}__attribute__(( packed ))own_party_list_t;

#define db_get_party_count(p_, buf)	\
		send_request_to_db(SVR_PROTO_GET_PARTY_COUNT, (p_), 4, buf, 0)
#define db_get_mole_party(p_, buf)	\
		send_request_to_db(SVR_PROTO_GET_MOLE_PARTY, (p_), 12, buf, 0)
#define db_get_own_mole_party(p_, buf)	\
		send_request_to_db(SVR_PROTO_GET_OWN_MOLE_PARTY, (p_), 4, buf, 0)

int get_party_count_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	//bodylen == 0
	CHECK_VALID_ID(p->id);

	return db_get_party_count(p, &(config_cache.bc_elem->online_id));
}

int get_party_count_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)buf, j);
	DEBUG_LOG("PARTY COUNT\t[%u %u]", p->id, *(uint32_t*)buf);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_mole_party_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	CHECK_VALID_ID(p->id);
	uint32_t buffer[3];
	int j = 0;
	buffer[0] = config_cache.bc_elem->online_id;  // online id
	UNPKG_UINT32(body, buffer[1], j);  // party index
	UNPKG_UINT32(body, buffer[2], j);  // the number of party

	return db_get_mole_party(p, buffer);
}

int get_mole_party_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	party_list_t* pl = (party_list_t*)buf;

	CHECK_BODY_LEN(len, pl->count * sizeof(party_info_t) + 4);

	int loop, j = sizeof(protocol_t);
	PKG_UINT32(msg, pl->count, j);
	for (loop = 0; loop < pl->count; loop++) {
		PKG_UINT32(msg, pl->party[loop].party_start_time, j);
		PKG_UINT32(msg, pl->party[loop].mapid, j);
		PKG_UINT32(msg, pl->party[loop].party_type, j);
		PKG_UINT32(msg, pl->party[loop].id, j);
		PKG_UINT32(msg, pl->party[loop].isvip, j);
		PKG_UINT32(msg, pl->party[loop].color, j);
		PKG_STR(msg, pl->party[loop].nick, j, USER_NICK_LEN);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int set_mole_party_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	CHECK_VALID_ID(p->id);

	uint32_t* buffer = (uint32_t*)p->session;
	int j = 0;
	buffer[0] = config_cache.bc_elem->online_id;  	// online id
	UNPKG_UINT32(body, buffer[1], j);  				// party start time
	UNPKG_UINT32(body, buffer[2], j);  				// which map
	UNPKG_UINT32(body, buffer[3], j);  				// party type

	// 1209600 mean 14 days second
	if (buffer[1] < get_now_tv()->tv_sec || buffer[1] > get_now_tv()->tv_sec + 1209600){
		ERROR_RETURN(("bad time\t[%u %u]", p->id, buffer[1]), -1);
	}
	if (( get_map(buffer[2]) && ( IS_NORMAL_MAP(buffer[2]) ))
		|| ( buffer[2] == p->id )){
		buffer[4] = p->id;
		buffer[5] = ISVIP(p->flag);
		buffer[6] = p->color;
		memcpy(&buffer[7], p->nick, USER_NICK_LEN);
		p->sess_len = 44;

		return db_set_sth_done(p, 109, 1, p->id);
	} else {
		ERROR_RETURN(("bad map id\t[%u %u]", p->id, buffer[2]), -1);
	}
}

int set_mole_party_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	// len == 0
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}


int get_own_mole_party_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	// bodylen == 0
	CHECK_VALID_ID(p->id);

	return db_get_own_mole_party(p, &(p->id));
}

int get_own_mole_party_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	own_party_list_t* po = (own_party_list_t*)buf;

	CHECK_BODY_LEN(len, po->count * sizeof(own_party_t) + 4);

	int loop, j = sizeof(protocol_t);
	PKG_UINT32(msg, po->count, j);
	for (loop = 0; loop < po->count; loop++) {
		PKG_UINT32(msg, po->own_party[loop].online_id, j);
		PKG_UINT32(msg, po->own_party[loop].party_start_time, j);
		PKG_UINT32(msg, po->own_party[loop].mapid, j);
		PKG_UINT32(msg, po->own_party[loop].party_type, j);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

