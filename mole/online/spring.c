#include "proto.h"
#include "exclu_things.h"
#include "communicator.h"
#include "login.h"
#include "homeinfo.h"

#include "spring.h"


#define db_set_spring_msg(p_, buf) \
		send_request_to_db(SVR_PROTO_SET_SPRING_MSG, p_, SPRING_MSG_LEN, buf, p_->id)
#define db_get_spring_msg(p_, id) \
		send_request_to_db(SVR_PROTO_GET_SPRING_MSG, p_, 0, NULL, id)


int set_spring_msg_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, SPRING_MSG_LEN);
	CHECK_VALID_ID(p->id);

	DEBUG_LOG("SET SPRING MSG\t[uid=%u]", p->id);
	return db_set_spring_msg(p, body);
}

int set_spring_msg_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0);
	return 0;
}

int get_spring_msg_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	//CHECK_BODY_LEN(bodylen, 0);

	DEBUG_LOG("GET SPRING MSG\t[uid=%u %lu]", p->id, p->tiles->id);
	return db_get_spring_msg(p, p->tiles->id);
}

int get_spring_msg_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, SPRING_MSG_LEN);

	DEBUG_LOG("GET SPRING CALLBACK\t[uid=%u %s]", p->id, buf);
	response_proto_str(p, p->waitcmd, SPRING_MSG_LEN, buf, 0);
	return 0;
}


/*
 *@ get fu info
 */
int get_fu_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	uint32_t type = 3;

	DEBUG_LOG("GET FU INFO\t[%u, %lu]", p->id, p->tiles->id);
	return db_get_info_by_type(p, &type, p->tiles->id);
}

/*
 *@ send fu
 */
int send_fu_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{

	CHECK_VALID_ID(p->id);

	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	if (p->id == p->tiles->id)
		ERROR_RETURN(("%u,error can't vote yourself", p->id), -1);

	//*(uint32_t*)p->session = p->tiles->id;
	op_mm_tree_t* op_t = (op_mm_tree_t*)(p->session + 4);
	op_t->type = 3; // 3 for  vote fu
	op_t->userid = p->id;
	op_t->op_type = 0; // 0 for the first value
	memcpy(op_t->nick, p->nick, USER_NICK_LEN);
	op_t->color = p->color;
	op_t->isvip = ISVIP(p->flag);
	op_t->op_stamp = get_now_tv()->tv_sec;

	DEBUG_LOG("SEND FU\t[%u, %lu]", p->id, p->tiles->id);

	return db_set_sth_done(p, 110, 50, p->id);

}

/*
 *@ check who fertilize or water my tree
 */
int list_fu_sent_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	//bodylen == 0
	uint32_t type = 3; // 1 for maomao shu, 2 for smc, 3 for FU
	return db_list_operated(p, &type);
}

int lahm_become_shuangjiegun_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	if(p->followed && PET_IS_BECOME_SHUANGJIEGUN(p->followed)){
		PET_RECOVER_FROM_SHUANGJIEGUN(p->followed);
	} else {
		return 0;
	}

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_PET_RECOVER_FORM, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("RECOVER FROM SHUANGJIEGUN\t[%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int lahm_become_shuangjiegun_cmd (sprite_t *p, const uint8_t *body, int len)
{
	if( !p->followed )
		ERROR_RETURN(("no pet follow %u", p->id), -1);
	if (!PET_IS_SUPER_LAHM(p->followed))
		ERROR_RETURN (("pet id %u is not super lahm", p->followed->id), -1);

	PET_BECOME_SHUANGJIEGUN(p->followed);

	add_event(&(p->timer_list), lahm_become_shuangjiegun_expeired, p, 0, get_now_tv()->tv_sec + 600, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);

	DEBUG_LOG("LAHM BECOME SHUANGJIEGUN\t[pid=%u]", p->id);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

