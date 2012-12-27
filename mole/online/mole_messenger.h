#ifndef MOLE_MESSENGER_H_
#define MOLE_MESSENGER_H_

#include "dbproxy.h"
#include "logic.h"
#include "proto.h"

//--------------------------- db-related funcs/macros begins --------------------------
// add an invitee
#define mole_msnger_add_invitee(invitee_, inviter_id_) \
		send_request_to_db(SVR_PROTO_MOLE_MSNGER_ADD_INVITEE, (invitee_), 4, &(invitee_)->id, (inviter_id_))
// check user's parent id and number of invitees
#define mole_msnger_chk_info(chker_, chkee_id_) \
		send_request_to_db(SVR_PROTO_GET_MOLE_MSNGER_INFO, (chker_), 0, 0, chkee_id_)
//--------------------------- db-related funcs/macros ends --------------------------

//---------------------------  --------------------------
void msnger_credit_inviter(sprite_t* p, uint32_t inviter_id, uint32_t invitee_num);
//---------------------------  --------------------------

//--------------------------- callbacks begin --------------------------
//
static inline int
mole_msnger_add_invitee_callback(sprite_t* p, uint32_t inviter_id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	//CHECK_WAITCMD(p, PROTO_MSNGER_INVITEE_CHK_IN);

	// to be friends
	db_add_friend(0, inviter_id, p->id);
	db_add_friend(0, p->id, inviter_id);
	//
	uint32_t invitee_num = *((uint32_t*)buf);
	msnger_credit_inviter(p, inviter_id, invitee_num);

	int i = sizeof(protocol_t);
	i += pack_user_tasks(p, msg + i);
	init_proto_head(msg, PROTO_SET_TASK, i);
	send_to_self(p, msg, i, 1);
	//please not remove the two lines code, AS often call PROTO_SET_TASK ofter PROTO_EXCHANGE_ITEM
	*(uint32_t*)p->session = 0;
	p->sess_len = 0;
	//send_exchg_itm_rsp(p, 1);
	return 0;
}
// Send Parent ID and Invitee Cnt to the Requester
static inline int
mole_msnger_chk_info_callback(sprite_t* p, uint32_t chkee_id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 16);
	CHECK_WAITCMD(p, PROTO_CHK_MSNGER_INFO);

	int i = 0;
	uint32_t inviter_id, invitee_cnt, old_invitee_cnt, kk_invitee_cnt;
	UNPKG_H_UINT32(buf, inviter_id, i);
	UNPKG_H_UINT32(buf, old_invitee_cnt, i);
	UNPKG_H_UINT32(buf, invitee_cnt, i);
	UNPKG_H_UINT32(buf, kk_invitee_cnt, i);
	DEBUG_LOG(" kk_invitee_cnt: [%u]  invitee_cnt: [%u]",  kk_invitee_cnt, invitee_cnt);
	if (p->id == chkee_id) {
		p->inviter_id  = inviter_id;
		p->invitee_cnt = invitee_cnt;
		p->old_invitee_cnt = old_invitee_cnt;
	}

	i = sizeof(protocol_t);
	PKG_UINT32(msg, chkee_id, i);    // checkee id
	PKG_UINT32(msg, inviter_id, i);  // inviter id
	PKG_UINT32(msg, old_invitee_cnt, i); //old invitee cnt of checkee
	PKG_UINT32(msg, invitee_cnt, i); // invitee cnt of checkee
	PKG_UINT32(msg, kk_invitee_cnt, i); //7k7k invitee cnt of checkee
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}


static inline int
msnger_invitee_chk_in_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	if (p->inviter_id != 0) {
		DEBUG_LOG("%u's INVITEE %u CHK IN", p->inviter_id, p->id);
		return do_exchange_item(p, 1537, 0);
	}
	ERROR_RETURN(("No Inviter Found. uid=%u", p->id), -1);
}


static inline int
chk_msnger_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t chkee_id = 0;
	unpkg_uint32(body, bodylen, &chkee_id);
	CHECK_VALID_ID(chkee_id);

	return mole_msnger_chk_info(p, chkee_id);
}
//--------------------------- funcs for proto.c ends --------------------------

#endif // MOLE_MESSENGER_H_
