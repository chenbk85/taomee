#include "central_online.h"
#include "dbproxy.h"
#include "proto.h"
#include "util.h"

#include "message.h"

//-------------------------------------------------------------------------

typedef struct PostCardContentRcv {
	uint32_t    cardid;
	uint32_t    cardtype;
	uint32_t    sent_tm;
	uint32_t    sender_id;
	char        sender_nick[USER_NICK_LEN];
	uint32_t    mapid;
	uint32_t    msglen;
	char        msg[];
} __attribute__((packed)) postcard_content_rcv_t;

//-------------------------------------------------------------------------

//------------------- CallBacks ---------------------------------------------
int send_postcard_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	msg_send_buf_t* sendbuf = (void*)(p->session);
	if ( *((uint32_t*)buf) ) { // ok, a given receiver got the postcard
		send_attr_update_noti(p->id, 0, sendbuf->rcvers[sendbuf->nsent], 3);
	} else { // failed to send a postcard
		sendbuf->rcvers[(sendbuf->failcnt)++] = htonl(sendbuf->rcvers[sendbuf->nsent]);
	}
	//
	if ( ++(sendbuf->nsent) != sendbuf->total ) { // more rcvers to send to
		//DEBUG_LOG("send=%d total=%d", sendbuf->nsent, sendbuf->total);
		return db_send_postcard(p, &(sendbuf->card), sizeof(postcard_content_send_t) + sendbuf->card.msglen, sendbuf->rcvers[sendbuf->nsent]);
	} else { // postcard sent completely
		if (sendbuf->failcnt) {
			item_t* pitm = get_item_prop(sendbuf->card.cardtype);
			DEBUG_LOG("POSTCARD MOLE BEAN ROLLBACK\t[uid=%u cnt=%d]", p->id, sendbuf->failcnt);
			db_attr_op(p, sendbuf->failcnt * pitm->price, 0, 0, 0, 0, ATTR_CHG_roll_back, 0);
		}
		p->sess_len = 0;
		int i = sizeof(protocol_t);
		PKG_UINT8(msg, sendbuf->failcnt, i);
		PKG_STR(msg, sendbuf->rcvers, i, sendbuf->failcnt * 4);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}
}

int get_postcards_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	int i = 0;
	uint32_t unread_cnt, read_cnt, total;
	UNPKG_H_UINT32(buf, unread_cnt, i);
	UNPKG_H_UINT32(buf, read_cnt, i);
	total = unread_cnt + read_cnt;
	CHECK_BODY_LEN(len, total * 4 + 8);
	//
	int j = sizeof(protocol_t);
	PKG_UINT16(msg, total, j);
	PKG_UINT16(msg, unread_cnt, j);
	int k = 0;
	uint32_t cardid;
	for ( ; k != unread_cnt; ++k ) {
		UNPKG_H_UINT32(buf, cardid, i);
//		DEBUG_LOG("UNREAD CARD\t[uid=%u cardid=%d]", p->id, cardid);
		PKG_UINT32(msg, cardid, j);
	}
	PKG_UINT16(msg, read_cnt, j);
	for ( k = 0; k != read_cnt; ++k ) {
		UNPKG_H_UINT32(buf, cardid, i);
//		DEBUG_LOG("READ CARD\t[uid=%u cardid=%d]", p->id, cardid);
		PKG_UINT32(msg, cardid, j);
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int read_postcard_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(postcard_content_rcv_t));

	const postcard_content_rcv_t* card = (void*)buf;
	CHECK_BODY_LEN(len, sizeof(postcard_content_rcv_t) + card->msglen);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, card->cardid, i);
	PKG_UINT32(msg, card->sender_id, i);
	PKG_STR(msg, card->sender_nick, i, sizeof card->sender_nick);
	PKG_UINT32(msg, card->sent_tm, i);
	PKG_UINT32(msg, card->cardtype, i);
	PKG_UINT32(msg, card->mapid, i);
	PKG_UINT32(msg, card->msglen, i);
	PKG_STR(msg, card->msg, i, card->msglen);
	init_proto_head(msg, p->waitcmd, i);

	return send_to_self(p, msg, i, 1);
}
//-------------------------------------------------------------------------

int get_unread_card_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	int i = 0;
	uint32_t unread_cnt;
	UNPKG_H_UINT32(buf, unread_cnt, i);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, unread_cnt, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

static int unpack_postcard_sending_pkg(uint8_t* body, int bodylen, msg_send_buf_t* sendbuf);

int send_postcard_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	msg_send_buf_t* sendbuf = (void*)(p->session);

	if (unpack_postcard_sending_pkg(body, bodylen, sendbuf) == -1) {
		return -1;
	}

	send_chatlog(SVR_PROTO_CHATLOG, p, 1, sendbuf->card.msg, sendbuf->card.msglen);
	CHECK_DIRTYWORD(p, sendbuf->card.msg);

	sendbuf->nsent   = 0;
	sendbuf->failcnt = 0;
	p->sess_len      = sizeof(msg_send_buf_t);
	// package to db
	sendbuf->card.sent_tm   = get_now_tv()->tv_sec;
	sendbuf->card.sender_id = p->id;
	memcpy(sendbuf->card.sender_nick, p->nick, sizeof sendbuf->card.sender_nick);

	if (sendbuf->card.cardtype == 1000177)
	{
	    uint32_t info_m[2] = {0};
        info_m[0] = p->id;
        info_m[1] = 1;
	    msglog(statistic_logfile, 0x20D100A, now.tv_sec, info_m, sizeof(info_m));
	}
	else if (sendbuf->card.cardtype == 1000265)
	{
	    uint32_t info_m[2] = {0};
        info_m[0] = p->id;
        info_m[1] = 1;
	    msglog(statistic_logfile, 0x0409B47F, now.tv_sec, info_m, sizeof(info_m));
	}

	item_t* pitm = get_item_prop(sendbuf->card.cardtype);
	DEBUG_LOG("SEND POSTCARD\t[uid=%u rcvcnt=%d]", p->id, sendbuf->total);
	return db_attr_op(p, -(pitm->price * sendbuf->total), 0, 0, 0, 0, ATTR_CHG_send_postcard, 0);
}

//-------- Utils For Unpacking ------------------------------------------------
static int unpack_postcard_sending_pkg(uint8_t* body, int bodylen, msg_send_buf_t* sendbuf)
{
	CHECK_BODY_LEN_GE(bodylen, 6);

	int i = 0;
	UNPKG_UINT32(body, sendbuf->card.cardtype, i);
	if (!get_item_prop(sendbuf->card.cardtype)) {
		ERROR_RETURN(("Invalid CardType %u", sendbuf->card.cardtype), -1);
	}
	UNPKG_UINT8(body, sendbuf->card.msglen, i);
	CHECK_INT_GE(sendbuf->card.msglen, 1);
	CHECK_INT_LE(sendbuf->card.msglen, 151);
	CHECK_BODY_LEN_GE(bodylen, 10 + sendbuf->card.msglen);
	UNPKG_STR(body, sendbuf->card.msg, i, sendbuf->card.msglen);
	sendbuf->card.msg[sendbuf->card.msglen - 1] = '\0';
	UNPKG_UINT8(body, sendbuf->total, i);
	CHECK_INT_LE(sendbuf->total, 150);
	CHECK_BODY_LEN(bodylen, 6 + sendbuf->card.msglen + 4 * sendbuf->total);
	// unpack rcver id
	int j = 0;
	for ( ; j != sendbuf->total; ++j ) {
		UNPKG_UINT32(body, sendbuf->rcvers[j], i);
	}

	return 0;
}

int set_postcard_gift_flag_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t cardid;
	if ( unpkg_uint32(body, bodylen, &cardid) == -1) {
		return -1;
	}
	return send_request_to_db(SVR_PROTO_SET_POSTCARD_GIFT_FLAG, p, 4, &cardid, p->id);
}

int set_postcard_gift_flag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_postcards_base_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_POSTCARDS_BASE_INFO, p, 0, 0, p->id);
}

int get_postcards_base_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	int i = 0;
	uint32_t unread_cnt, read_cnt;
	UNPKG_H_UINT32(buf, unread_cnt, i);
	UNPKG_H_UINT32(buf, read_cnt, i);
	CHECK_BODY_LEN(len, unread_cnt*(4*6+16) + read_cnt*(4*6+16) + 8 );
	if ((unread_cnt + read_cnt) > 900)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, unread_cnt, j);
	int k = 0;
	uint32_t cardid, emailid, senderid, sendtime, flag, is_award;
	char  sender_nick[USER_NICK_LEN];
	memset(sender_nick, 0, USER_NICK_LEN);

	for ( ; k != unread_cnt; ++k ) {
		UNPKG_H_UINT32(buf, cardid, i);
		UNPKG_H_UINT32(buf, emailid, i);
		UNPKG_H_UINT32(buf, senderid, i);
		UNPKG_H_UINT32(buf, sendtime, i);
		UNPKG_H_UINT32(buf, flag, i);
		UNPKG_H_UINT32(buf, is_award, i);
		UNPKG_STR(buf, sender_nick, i, USER_NICK_LEN);

		PKG_UINT32(msg, cardid, j);
		PKG_UINT32(msg, emailid, j);
		PKG_UINT32(msg, senderid, j);
		PKG_UINT32(msg, sendtime, j);
		PKG_UINT32(msg, flag, j);
		PKG_UINT32(msg, is_award, j);
		PKG_STR(msg, sender_nick, j, USER_NICK_LEN);
	}
	PKG_UINT32(msg, read_cnt, j);
	for ( k = 0; k != read_cnt; ++k ) {
		UNPKG_H_UINT32(buf, cardid, i);
		UNPKG_H_UINT32(buf, emailid, i);
		UNPKG_H_UINT32(buf, senderid, i);
		UNPKG_H_UINT32(buf, sendtime, i);
		UNPKG_H_UINT32(buf, flag, i);
		UNPKG_H_UINT32(buf, is_award, i);
		UNPKG_STR(buf, sender_nick, i, USER_NICK_LEN);

		PKG_UINT32(msg, cardid, j);
		PKG_UINT32(msg, emailid, j);
		PKG_UINT32(msg, senderid, j);
		PKG_UINT32(msg, sendtime, j);
		PKG_UINT32(msg, flag, j);
		PKG_UINT32(msg, is_award, j);
		PKG_STR(msg, sender_nick, j, USER_NICK_LEN);

	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int del_postcards_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t cardid = 0, count = 0;
	int i = 0;
	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_UINT32(body, count, i);
	CHECK_BODY_LEN(bodylen, count*4+4);

	if (count > 20)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    uint8_t dbbuff[1024] = {0};
    int j = 0;
    PKG_H_UINT32(dbbuff, count, j);
	int k = 0;
	for (k = 0; k < count ; k++)
	{
	    UNPKG_UINT32(body, cardid, i);
	    PKG_H_UINT32(dbbuff, cardid, j);
	}

	DEBUG_LOG("DEL POSTCARDS\t[uid=%u count=%u]", p->id, count);
	return send_request_to_db(SVR_PROTO_DEL_POSTCARDS, p, j, dbbuff, p->id);
}

int del_postcards_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}



//---------------------------------------------------------------------------
