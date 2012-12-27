
#include "proto.h"
#include "dbproxy.h"
#include "christmas.h"


#define db_set_christmas_wish(p_, buf) \
		send_request_to_db(SVR_PROTO_SET_CHRISTMAS_WISH, p_, MAX_CHRISTMAS_WISH_LEN, buf, (p_)->id)
#define db_isset_christmas_wish(p_) \
		send_request_to_db(SVR_PROTO_IS_SET_CHRISTMAS_WISH, p_, 0, NULL, (p_)->id)
#define db_get_christmas_wish(p_, buf) \
		send_request_to_db(SVR_PROTO_GET_CHRISTMAS_WISH, p_, 0, NULL, (p_)->id)
#define db_isget_christmas_wish(p_) \
		send_request_to_db(SVR_PROTO_IS_GET_CHRISTMAS_WISH, p_, 0, NULL, (p_)->id)


int set_christmas_wish_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, MAX_CHRISTMAS_WISH_LEN);
	CHECK_VALID_ID(p->id);

	p->sess_len = 0;
	UNPKG_STR(body, p->session, p->sess_len, MAX_CHRISTMAS_WISH_LEN);
	*(p->session + p->sess_len - 1) = '\0';

	CHECK_DIRTYWORD(p , p->session);

	DEBUG_LOG("SET CHRIS\t[uid=%u]", p->id);
	return db_isset_christmas_wish(p);
}

int set_christmas_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len == 0
	DEBUG_LOG("SET CHRIS CALLBACK\t[uid=%u]", p->id);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int is_set_christmas_wish_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	
	return db_isset_christmas_wish(p);
}

int is_set_christmas_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t flag = *(uint32_t*)buf;
	if (flag != 0 && flag != 1)
		ERROR_RETURN(("bad flag\t[uid=%u flag=%u]", p->id, flag), -1);

	switch	(p->waitcmd) {
	case	PROTO_IS_SET_CHRISTMAS_WISH:
			response_proto_uint32(p, p->waitcmd, flag, 0);
			return 0;
	case	PROTO_SET_CHRISTMAS_WISH:
			if (flag == 0){
				return db_set_christmas_wish(p, p->session);
			} else{
				return send_to_self_error(p,p->waitcmd, -ERR_already_set_christmas, 1);
			}
	default:
			ERROR_RETURN(("bad cmd %u", p->waitcmd), -1);
	}
}

int get_christmas_wish_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_VALID_ID(p->id);
	
	return db_isget_christmas_wish(p);
}

int get_christmas_wish_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	CHECK_BODY_LEN(len, 259);
typedef struct gift{
	uint32_t itemid;
	uint32_t count;
}gift_t;
	gift_t rand_gifts[4] = {{180015, 2},{180016, 2},{180025, 3},{180029, 1}};
	
	uint32_t sendid = *(uint32_t*)buf;
	
	int index = rand()%4;
	db_single_item_op(0, p->id, 12321, 1, 1);
	db_single_item_op(0, p->id, rand_gifts[index].itemid, rand_gifts[index].count, 1);
	
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, sendid, j);
	PKG_UINT32(msg, rand_gifts[index].itemid, j);
	PKG_UINT32(msg, rand_gifts[index].count, j);
	PKG_STR(msg, buf + 4, j, MAX_CHRISTMAS_WISH_LEN);
	
	DEBUG_LOG("SEND WISH\t[uid=%u]", p->id);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int is_get_christmas_wish_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_VALID_ID(p->id);
	
	return db_isget_christmas_wish(p);
}

int is_get_christmas_wish_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t flag = *(uint32_t*)buf;
	if (flag != 0 && flag != 1)
		ERROR_RETURN(("bad flag\t[uid=%u flag=%u]", p->id, flag), -1);

	switch	(p->waitcmd) {
	case	PROTO_IS_GET_CHRISTMAS_WISH:
			response_proto_uint32(p, p->waitcmd, flag, 0);
			return 0;
	case	PROTO_GET_CHRISTMAS_WISH:
			if (flag == 0){
				return db_get_christmas_wish(p, p->session);
			} else{
				ERROR_LOG("already got wish\t[uid=%u]", p->id);
				return send_to_self_error(p,p->waitcmd, -ERR_already_get_christmas, 1);
			}
	default:
			ERROR_RETURN(("bad cmd %u", p->waitcmd), -1);
	}
}

int lahm_dress_hat_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;
	
	if(p->followed && PET_IS_DRESS_HAT(p->followed)){
		PET_UNDRESS_HAT(p->followed);
	} else {
		return 0;
	}
	
	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_UNDRESS_CHRISTMAS_HAT, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("UNDRESS CHRIS HAT\t[%u %d]", p->id, p->action);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int lahm_dress_hat_cmd (sprite_t *p, const uint8_t *body, int len)
{
	if( !p->followed )
		ERROR_RETURN(("no pet follow %u", p->id), -1);
	
	PET_DRESS_HAT(p->followed);
	
	add_event(&(p->timer_list), lahm_dress_hat_expeired, p, 0, get_now_tv()->tv_sec + 3600, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	
	DEBUG_LOG("LAHM DRESS CHRIS HAT\t[pid=%u]", p->id);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	
	return 0;
}


