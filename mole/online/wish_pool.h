#ifndef ONLINE_WISHING_POOL_
#define ONLINE_WISHING_POOL_

#include "benchapi.h"
#include "central_online.h"
#include "dbproxy.h"
#include "logic.h"
#include "proto.h"

// pkg to db
typedef struct DelWishSend {
	userid_t	sender_id;
	userid_t	rcvid;
	uint32_t	date;
} __attribute__((packed)) del_wish_send_t;

#define SVR_PROTO_ADD_WISH			0x1501
#define SVR_PROTO_CHK_IF_WISH_MADE	0x1402
#define SVR_PROTO_DEL_WISH			0x1503
#define SVR_PROTO_GET_WISH_LIST		0x1404

//----------------------------------------------------------
#define db_add_wish(p_, buf_) \
		send_request_to_db(SVR_PROTO_ADD_WISH, (p_), (p_)->sess_len, (buf_), (p_)->id)
#define db_chk_if_wish_made(p_) \
		send_request_to_db(SVR_PROTO_CHK_IF_WISH_MADE, (p_), 4, &((p_)->id), (p_)->id)
#define db_del_wish(p_, buf_, len_) \
		send_request_to_db(SVR_PROTO_DEL_WISH, (p_), (len_), (buf_), (p_)->id)
#define db_get_wish_list(p_) \
		send_request_to_db(SVR_PROTO_GET_WISH_LIST, (p_), 4, &((p_)->id), (p_)->id)

//----------------------------------------------------------
static inline int
add_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(userid_t) + 30);

	userid_t rcvid = *((userid_t*)(buf));
	if (rcvid != p->id) {
		const char* itmname = buf + sizeof(userid_t);
		const struct tm* t  = get_now_tm();

		char txt[256];
#ifdef TW_VER
		len = snprintf(txt, sizeof txt,
						"%.16s在%d月%d日給你許了個願望（送給你%.30s）^_^記得明天去愛心教堂，點擊神龜許願池看看願望是否被實現哦！！！如果一周內願望未開啟的話，將被自動放棄！",
						p->nick, t->tm_mon + 1, t->tm_mday, itmname);
#else
		len = snprintf(txt, sizeof txt,
						"%.16s在%d月%d日给你许了个愿望（送给你%.30s）^_^记得明天去爱心教堂，点击神龟许愿池看看愿望是否被实现哦！！！如果一周内愿望未开启的话，将被自动放弃！",
						p->nick, t->tm_mon + 1, t->tm_mday, itmname);
#endif
		len = text_notify_pkg(msg + 4, 0, 0, 0, 0, admin, 0, len, txt);

		sprite_t* rcver = get_sprite(rcvid);
		if (rcver) {
			send_to_self(rcver, msg + 4, len, 0);
		} else {
			*(uint32_t*)msg = 0;
			chat_across_svr(rcvid, msg, len + 4);
		}
	}

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

static inline int
chk_if_wish_made_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_uint32(p, p->waitcmd, 0, 0);
	return 0;
}

int del_wish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_wish_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

//----------------------------------------------------------


//----------------------------------------------------------
int make_wish_cmd(sprite_t* p, const uint8_t* body, int bodylen);

static inline int
chk_if_wish_made_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	return db_chk_if_wish_made(p);
}

static inline int
fetch_realized_wish_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	del_wish_send_t delwish;
	if (unpkg_uint32_uint32(body, bodylen, &(delwish.sender_id), &(delwish.date)) == 0) {
		delwish.rcvid = p->id;
		DEBUG_LOG("FETCH WISH\t[uid=%u sndid=%u date=%u]",	p->id, delwish.sender_id, delwish.date);
		return db_del_wish(p, &delwish, sizeof delwish);
	}

	return -1;
}

static inline int
get_realized_wishes_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 0);

	return db_get_wish_list(p);
}

#endif // ONLINE_WISHING_POOL_
