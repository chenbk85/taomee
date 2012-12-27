#ifndef ONLINE_MARKET_H_
#define ONLINE_MARKET_H_

#include "dbproxy.h"
#include "logic.h"
#include "exclu_things.h"

#define MAGIC_CODE_BEGIN    1540
#define MAGIC_CODE_END		1600
#define SVR_PROTO_CHK_MAGIC_CODE_USED	0x6005
#define SVR_PROTO_SET_MAGIC_CODE_USED	0x6104
//
#define SVR_PROTO_BUY_REAL_ITEM		0xA501

#define db_chk_if_magic_code_used(p_, buf_) \
	send_request_to_db(SVR_PROTO_CHK_MAGIC_CODE_USED, (p_), 12, (buf_), (p_)->id)
#define db_set_magic_code_used(p_, buf_) \
	send_request_to_db(SVR_PROTO_SET_MAGIC_CODE_USED, 0, 12, (buf_), (p_)->id)
//
#define db_buy_real_item(p_, buf_) \
	send_request_to_db(SVR_PROTO_BUY_REAL_ITEM, (p_), 418, (buf_), (p_)->id)

//------------------ CallBacks ------------------------
static inline int
chk_magic_code_used_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t exchg_id = *((int*)buf);
	if ( (exchg_id >= MAGIC_CODE_BEGIN) && (exchg_id <= MAGIC_CODE_END) ) {
		return do_exchange_item(p, exchg_id, 0);
	}
	ERROR_RETURN( ("invalid exchg id %d", exchg_id), -1 );
}

static inline int
buy_real_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	//DEBUG_LOG("buy_real_item_callback %u", p->id);
	return 0;
}

//------------------ For proto.c ------------------------
static inline int
buy_real_item_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 418);

	uint32_t* cnt   = (uint32_t*)(body + 410);
	uint32_t* itmid = (uint32_t*)(body + 414);
	*cnt   = ntohl(*cnt);
	*itmid = ntohl(*itmid);

	DEBUG_LOG("BUY REAL ITEM\t[uid=%u itm=%s itmid=%u cnt=%u]", p->id, body + 360, *itmid, *cnt);
	return db_buy_real_item(p, body);
}

static inline int
submit_magic_code_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);

	memcpy(p->session + 1024, body, bodylen);
	DEBUG_LOG("SBM MAGIC CODE\t[uid=%u code=%.12s]", p->id, body);
	return db_set_sth_done(p, 121, 50, p->id);
	//return db_chk_if_magic_code_used(p, body);
}

#endif // ONLINE_MARKET_H_
