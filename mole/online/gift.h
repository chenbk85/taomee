

#ifndef ONLINE_GIFT_H
#define ONLINE_GIFT_H

#include "benchapi.h"
#include "dbproxy.h"

#define GIFTS_TABLE_SIZE	1024
#define MSG_LEN 			151

typedef struct {
	uint32_t		send_gift_id;
	int				in_cnt;
	item_unit_t		in_item[MAX_EXCHANGE_ITEMS];
	int				out_cnt;
	item_unit_t		out_item[MAX_EXCHG_OUT_KIND];
}__attribute__(( packed ))send_gift_t;

typedef struct {
	uint32_t		itm_type;
	uint32_t		itmid;
	uint32_t		count;
}__attribute__(( packed ))dec_itm_t;

typedef struct {
	uint32_t		itm_type;
	uint32_t		itmid;
	uint32_t		count;
	uint32_t		max;
}__attribute__(( packed ))add_itm_t;

typedef struct {
	uint32_t 		gid;
	uint32_t		count;
	uint8_t			mesg[MSG_LEN];
	uint32_t		frd_id;
	uint32_t		send_len;
	uint8_t			send_buf[200];
	uint32_t		dec_len;
	uint8_t			dec_buf[200];
}__attribute__((packed))send_one_t;

extern send_gift_t gifts[GIFTS_TABLE_SIZE];

static inline send_gift_t* get_gift_entry(int idx)
{
	if (idx >= GIFTS_TABLE_SIZE || gifts[idx].out_cnt == 0)
		return NULL;

	return &gifts[idx];
}

static inline int modify_send_gift_times(sprite_t* p, int count, userid_t uid)
{
	return send_request_to_db(SVR_PROTO_MODIFY_SEND_GIFT_CNT, p, 4, &count, uid);
}

#define db_get_send_gift_times(p_) \
	    send_request_to_db(SVR_PROTO_GET_SEND_GIFT_CNT, p_, 0, NULL, p_->id)

static inline int send_gift_fail_callback(sprite_t* p, send_one_t* aso)
{
	send_gift_t* lsg = get_gift_entry(aso->gid);

	uint8_t tmp_buf[256];
	// restore his items
	*(uint32_t*)tmp_buf = 0;				//in_cnt = 0
	*(uint32_t*)(tmp_buf + 4) = lsg->in_cnt;  //out_cnt
	*(uint32_t*)(tmp_buf + 8) = 0;
	*(uint32_t*)(tmp_buf + 12) = 0;
	int loop, j = 16;
	for (loop = 0; loop < lsg->in_cnt; loop++) {
		item_t *pitm = lsg->in_item[loop].itm;
		PKG_H_UINT32(tmp_buf, 0, j);
		PKG_H_UINT32(tmp_buf, pitm->id, j);
		PKG_H_UINT32(tmp_buf, lsg->in_item[loop].count * aso->count, j);
		PKG_H_UINT32(tmp_buf, pitm->max, j);
	}

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, 0, j, tmp_buf, p->id);
}

int exchg_gift_callback(sprite_t* p, uint32_t id, char* buf, int len);
int send_one_gift_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int load_gifts(const char *file);
int get_additional_gift_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int modify_send_gift_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_gift_times_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_gift_times_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif


