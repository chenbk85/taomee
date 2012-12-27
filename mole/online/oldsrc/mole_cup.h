/*#ifndef ONLINE_MOLE_CUP_H_
#define ONLINE_MOLE_CUP_H_

#include <ant/bitmanip/bitmanip.h>

#include "benchapi.h"
#include "dbproxy.h"
#include "logic.h"
#include "util.h"

#define SVR_PROTO_ADD_MEDAL			0xD101
#define SVR_PROTO_CHK_CUP_INFO		0xD002
#define SVR_PROTO_CHK_RECENT_MEDALS	0xD003
#define SVR_PROTO_CHK_CUP_HISTORY	0xD005

int db_add_medal(sprite_t* p, int medal, int gameid);

//static inline int
//db_chk_medal_top10(sprite_t* p)
//{
//	return send_request_to_db(SVR_PROTO_CHK_MEDAL_TOP_TEN, p, 0, 0, p->id);
//}

#define db_chk_recent_medals(p_) \
		send_request_to_db(SVR_PROTO_CHK_RECENT_MEDALS, (p_), 0, 0, (p_)->id);
#define db_chk_cup_history(p_) \
		send_request_to_db(SVR_PROTO_CHK_CUP_HISTORY, (p_), 0, 0, (p_)->id);
#define db_chk_cup_info(p_) \
		send_request_to_db(SVR_PROTO_CHK_CUP_INFO, (p_), 0, 0, (p_)->id);

//--------------- CallBacks ------------------------------------
int add_medal_callback(sprite_t* p, uint32_t id, char* buf, int len);

//static inline int
//chk_medal_top10_callback(sprite_t* p, uint32_t id, char* buf, int len)
//{
//	return 0;
//}

int chk_recent_medals_callback(sprite_t* p, uint32_t id, char* buf, int len);
int chk_cup_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int chk_cup_history_callback(sprite_t* p, uint32_t id, char* buf, int len);

//--------------- For proto.c ------------------------------------
static inline int
set_torch_on_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	p->tmpinfo.flag = set_bit_on32(p->tmpinfo.flag, 1);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT8(msg, 1, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

static inline int
chk_cup_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return db_chk_cup_info(p);
}

static inline int
chk_cup_history_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return db_chk_cup_history(p);
}

static inline int
chk_recent_medal_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return db_chk_recent_medals(p);
}

static inline int
get_cup_prise_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	if (p->flag & 0xF8000000) {
		*((uint32_t*)(p->session)) = (test_bit_on32(p->flag, 32) ? 160062 : 160079);
		p->sess_len = 4;
		return do_buy_item(p, *((uint32_t*)(p->session)), 1, 1);
	}
	response_proto_uint32(p, p->waitcmd, 0, 0);
	return 0;
}

//-------------------------------------------------
//static inline int
//get_medal(sprite_t* p)
//{
//	int medal_type = 0;
//	if (p->flag & 0xF8000000) {
//		int rnd = rand() % 100;
//		if (rnd < 10) {
//			medal_type = 101;
//		} else if (rnd < 35) {
//			medal_type = 102;
//		} else if (rnd < 80) {
//			medal_type = 103;
//		}
//	}
//	return medal_type;
//}

#endif // ONLINE_MOLE_CUP_H_ */
