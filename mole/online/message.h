#ifndef ONLINE_MESSAGE_H_
#define ONLINE_MESSAGE_H_

#include "benchapi.h"

#define SVR_PROTO_SEND_POSTCARD		0xE101
#define SVR_PROTO_READ_POSTCARD		0xE102
#define SVR_PROTO_GET_POSTCARDS		0xE003
#define SVR_PROTO_DEL_POSTCARD		0xE104
#define SVR_PROTO_DEL_ALL_POSTCARD	0xE106
#define SVR_PROTO_GET_UNREAD_CARD_NUM   0xE005
#define SVR_PROTO_SET_POSTCARD_GIFT_FLAG	0xE107
#define SVR_PROTO_GET_POSTCARDS_BASE_INFO	0xE008
#define SVR_PROTO_DEL_POSTCARDS		0xE109




#define db_send_postcard(p_, buf_, len_, rcver_) \
		send_request_to_db(SVR_PROTO_SEND_POSTCARD, (p_), (len_), (buf_), (rcver_))
#define db_get_postcards(p_) \
		send_request_to_db(SVR_PROTO_GET_POSTCARDS, (p_), 0, 0, (p_)->id)
#define db_read_postcard(p_, cardid_) \
		send_request_to_db(SVR_PROTO_READ_POSTCARD, (p_), 4, &(cardid_), (p_)->id)
#define db_del_postcard(p_, cardid_) \
		send_request_to_db(SVR_PROTO_DEL_POSTCARD, (p_), 4, &(cardid_), (p_)->id)
#define db_get_unread_card_num(p_) \
		send_request_to_db(SVR_PROTO_GET_UNREAD_CARD_NUM, (p_), 0, 0, (p_)->id)
#define db_del_all_postcard(p_) \
		send_request_to_db(SVR_PROTO_DEL_ALL_POSTCARD, (p_), 0, NULL, (p_)->id)


typedef struct PostCardContentSend {
	uint32_t    cardtype;
	uint32_t    sent_tm;
	uint32_t    sender_id;
	char        sender_nick[USER_NICK_LEN];
	uint32_t    map_id;
	uint32_t    msglen;
	char        msg[];
} __attribute__((packed)) postcard_content_send_t;

typedef struct MessageSendBuf {
	int total, nsent, failcnt;
	uint32_t rcvers[150];
	postcard_content_send_t card;
} __attribute__((packed)) msg_send_buf_t;

// Callbacks
int send_postcard_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_postcards_callback(sprite_t* p, uint32_t id, char* buf, int len);
int read_postcard_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_unread_card_num_callback(sprite_t* p, uint32_t id, char* buf, int len);

//
// email not found 1702
static inline int
del_postcard_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

// For proto.c
int send_postcard_cmd(sprite_t* p, uint8_t* body, int bodylen);
//
static inline int
get_postcards_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	//DEBUG_LOG("GET POSTCARD\t[uid=%u]", p->id);
	return db_get_postcards(p);
}
//
static inline int
get_unread_card_num_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	//DEBUG_LOG("GET UNREAD POSTCARD\t[uid=%u]", p->id);
	return db_get_unread_card_num(p);
}

//
static inline int
read_postcard_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t cardid;
	if ( unpkg_uint32(body, bodylen, &cardid) == -1 ) {
		return -1;
	}
	return db_read_postcard(p, cardid);
}
//
static inline int
del_postcard_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t cardid;
	if ( unpkg_uint32(body, bodylen, &cardid) == -1 ) {
		return -1;
	}
	DEBUG_LOG("DEL POSTCARD\t[uid=%u cardid=%u]", p->id, cardid);
	return db_del_postcard(p, cardid);
}

static inline int
del_all_postcard_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	DEBUG_LOG("DEL ALL POSTCARD\t[uid=%u]", p->id);
	db_del_all_postcard(p);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

//-----------------------------------------------------------
// Send a Postcard to @rcver_id
static void inline
send_postcard(const char* sender_name, userid_t sender_id, userid_t rcver_id, uint32_t cardtype, const char* msg, uint32_t map_id)
{
	static char cardbuf[1024] = { };

	postcard_content_send_t* card = (void*)cardbuf;

	card->cardtype	= cardtype;
	card->sent_tm	= get_now_tv()->tv_sec;
	card->sender_id	= sender_id;
	snprintf(card->sender_nick, USER_NICK_LEN, "%s", sender_name);
	card->map_id = map_id;
	if (msg == NULL) {
		card->msglen = 0;
	} else {
		card->msglen	= snprintf(card->msg, sizeof(cardbuf) - sizeof(postcard_content_send_t), "%s", msg);
	}
	//DEBUG_LOG("%s %d", card->msg, card->msglen);
	db_send_postcard(0, card, sizeof(postcard_content_send_t) + card->msglen, rcver_id);
}

int set_postcard_gift_flag_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_postcard_gift_flag_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_postcards_base_info_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_postcards_base_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int del_postcards_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int del_postcards_callback(sprite_t* p, uint32_t id, char* buf, int len);


#endif // ONLINE_MESSAGE_H_
