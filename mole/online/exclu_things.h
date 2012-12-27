#ifndef ONLINE_EXCLUSIVE_THINGS_H_
#define ONLINE_EXCLUSIVE_THINGS_H_

#include "dbproxy.h"
#include "logic.h"
#include "proto.h"
#include "util.h"
#include "message.h"

typedef struct ExclusiveThings {
	int			id;
	int			exclu_id;
	int			exchg_id;
	uint32_t	outputkind;
	uint32_t	flag;
	uint32_t	vip_level;
} exclu_things_t;

enum {
	vip_only_exclu_thing	= 1
};


static inline int
check_vip_level(sprite_t *p, uint32_t need_vip_level)
{
	if ((need_vip_level == 0) || (p->vip_level == need_vip_level)) {
		return 1;
	}
	return 0;
}

#define VIP_ONLY_EXCLU_THING(exclu_) \
		!!((exclu_)->flag & vip_only_exclu_thing)

// parse events.xml
int  load_exclusive_things(const char* file);
void destroy_exclusive_things();
int get_month_tasks_callback(sprite_t* p, uint32_t id, char* buf, int len);

// Utils
const exclu_things_t* get_exclu_thing(int idx);

//------------- Memory DB Operations -------------
#define db_chk_if_sth_done(p_, pkg_) \
		send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, (p_), 4, (pkg_), (p_)->id)

#define db_get_month_tasks(p_, uid) \
		send_request_to_db(SVR_PROTO_GET_MONTH_TASKS, (p_), 0, NULL, uid)

static inline int
db_set_sth_done(sprite_t* p, uint32_t type, uint32_t maxvalue, uint32_t uid)
{
	uint32_t buf[3]={type, maxvalue,1};
	return send_request_to_db(SVR_PROTO_SET_STH_DONE, p, 12, &buf, uid);
}

static inline int
db_set_sth_done_cnt(sprite_t* p, uint32_t type, uint32_t limit, uint32_t add, uint32_t uid)
{
	uint32_t buf[3]={type, limit,add};
	return send_request_to_db(SVR_PROTO_SET_STH_DONE, p, 12, &buf, uid);
}

static inline int
db_set_month_task_done(sprite_t* p, uint32_t type, uint32_t maxvalue, uint32_t uid)
{
	uint32_t buf[3]={type, maxvalue, 1};
	return send_request_to_db(SVR_PROTO_SET_MONTH_TASK, p, 12, &buf, uid);
}

static inline int
db_clean_sth_done(sprite_t* p, uint32_t type, uint32_t uid)
{
 	return send_request_to_db(SVR_PROTO_CLEAN_STH_DONE, p, 4, &type, uid);
}

int chk_if_sth_done_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_sth_done_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_month_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int exchange_thing_to_another(sprite_t* p, const uint8_t* body, int bodylen);
int exchange_thing_to_another_callback(sprite_t* p, uint32_t id, char* buf, int len);
int lamu_get_skill_item_libao(sprite_t* p, const uint8_t* body, int bodylen);
int lamu_give_skill_item_libao(sprite_t* p);
int lamu_help_ciwei_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int get_yymmdd(time_t t);
static inline int
chk_if_sth_done_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t type;
	UNPKG_UINT32(body, type, i);

    DEBUG_LOG("1 CHK IF STH DONE\t[uid=%u type=%u]", p->id, type);

	const exclu_things_t* exclu = get_exclu_thing(type);
	if (exclu) {
		p->sess_len = 0;
		PKG_STR(p->session, body, p->sess_len, 4);
		return db_chk_if_sth_done(p, &(exclu->exclu_id));
	}
	p->sess_len = 0;
	PKG_STR(p->session, body, p->sess_len, 4);
	return db_chk_if_sth_done(p, &type);
}

int set_sth_done_cmd(sprite_t* p, const uint8_t* body, int bodylen);


#endif // ONLINE_EXCLUSIVE_THINGS_H_
