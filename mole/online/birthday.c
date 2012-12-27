
#include "proto.h"
#include "dbproxy.h"
#include "birthday.h"


#define db_set_birthday(p_, buf) \
		send_request_to_db(SVR_PROTO_SET_BIRTHDAY, p_, 4, buf, (p_)->id);

int set_birthday_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	uint32_t birthday;
	int j = 0;
	UNPKG_UINT32(body, birthday, j);
	*(uint32_t*)p->session = birthday;

	DEBUG_LOG("SET BIRTHDAY\t[uid=%u birth=%u]", p->id, birthday);
	return db_set_birthday(p, &birthday);
}


int set_birthday_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len == 0
	p->birthday = *(uint32_t*)p->session;

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int dress_birthday_clothes_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	p->action = 0;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_BIRTHDAY_CLOTHES_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("BIRTHDAY CLOTHES EXPIRED\t[uid=%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int dress_birthday_clothes_cmd (sprite_t *p, const uint8_t *body, int len)
{
	//CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	const struct tm* tm_tmp1;
	struct tm tm_tmp2;
	tm_tmp1 = get_now_tm();
	localtime_r((time_t*)&(p->birthday), &tm_tmp2);

	//DEBUG_LOG("DRESS BIRTH CLOTHES\t[%u %u %u %u %u]", p->id, tm_tmp1->tm_mday, tm_tmp1->tm_mon, tm_tmp2.tm_mday, tm_tmp2.tm_mon);
	if (tm_tmp1->tm_mday != tm_tmp2.tm_mday || tm_tmp1->tm_mon != tm_tmp2.tm_mon)
		ERROR_RETURN(("not your birthday\t[%u %d %d %d %d]", p->id, tm_tmp1->tm_mday, tm_tmp1->tm_mon + 1, tm_tmp2.tm_mday, tm_tmp2.tm_mon + 1), -1);

	uint32_t itmid = 19000; // only for birthday
	p->action = itmid;
	DEBUG_LOG("DRESS BIRTHDAY CLOTHES\t[uid=%u]", p->id);

	add_event(&(p->timer_list), dress_birthday_clothes_expeired, p, 0, get_now_tv()->tv_sec + 600, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

