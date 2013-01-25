#ifndef MOLE2_HONOR_H_
#define MOLE2_HONOR_H_

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"
#include "items.h"

#define BEGIN_HONOR_ID		1

#define IS_TEMP_HONOR(honor_) ((honor_) > 2000 && (honor_) <= 3000)
#define IS_MASTER_HONOR(honor_) ((honor_) >= 3021 && (honor_) <= 3030)

int load_honor_titles(xmlNodePtr cur);

int use_honor_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int check_honor_exist(sprite_t* p, uint32_t honor_id);

honor_attr_t* get_honor_attr(uint32_t honor_id);

static inline void notify_user_honor_up(sprite_t* p, int uptype, int completed, int self)
{
	uint8_t buff[64] = {0};
	int j = sizeof(protocol_t);
	PKG_UINT32(buff, p->id, j);
	PKG_UINT32(buff, p->honor, j);
	PKG_UINT32(buff, uptype, j);
	PKG_UINT32(buff, p->hp, j);
	PKG_UINT32(buff, p->hp_max, j);
	PKG_UINT32(buff, p->mp, j);
	PKG_UINT32(buff, p->mp_max, j);
	PKG_UINT32(buff, p->prof_lv, j);
	init_proto_head(buff, cli_use_honor_cmd, j);
	send_to_map(p, buff, j, completed, self);
	KDEBUG_LOG(p->id, "HONOR UP\t[%u %u %u]", p->honor, self, p->prof_lv);
}

static inline int db_set_user_honor(sprite_t* p, uint32_t id, uint32_t honor_id)
{
	return send_request_to_db(p, id, proto_db_update_user_honor, &honor_id, 4);
}

static inline int check_week_clear_honor(uint32_t hid)
{
	honor_attr_t *pha=get_honor_attr(hid);
	if(pha && pha->clear_type)
		return 1;
	return 0;
}

static inline void db_add_user_honor(sprite_t* p, uint32_t uid, uint32_t joblv, uint32_t pre_honor, uint32_t new_honor)
{
	uint8_t out[32];
	int j = 0;
	PKG_H_UINT32(out, pre_honor, j);
	PKG_H_UINT32(out, new_honor, j);
	uint32_t time=check_week_clear_honor(new_honor)? (get_next_sunday()):0;
	PKG_H_UINT32(out, time, j);
	PKG_H_UINT32(out, joblv, j);
	send_request_to_db(p, uid, proto_db_add_user_honor, out, j);
}

static inline int get_pre_honor(sprite_t* p, honor_attr_t* pha)
{
	for (uint32_t loop = 0; loop < pha->pre_honor_cnt; loop ++) {
		if (check_honor_exist(p, pha->pre_honor_id[loop])) {
			return pha->pre_honor_id[loop];
		}
	}
	
	return 0;
}

static inline void update_one_honor(sprite_t* p, honor_attr_t* pha) 
{
	for (uint32_t loop = 0; loop < pha->pre_honor_cnt; loop ++) {
		g_hash_table_remove(p->user_honors, &pha->pre_honor_id[loop]);
	}
	g_hash_table_insert(p->user_honors, &pha->honor_id, pha);
}

static inline int update_user_honor(sprite_t* p, uint32_t* p_honors, uint32_t honor_cnt)
{
	for (uint32_t loop = 0; loop < honor_cnt; loop ++) {
		//DEBUG_LOG("HONOR:%u", p_honors[loop]);
		honor_attr_t* pha = get_honor_attr(p_honors[loop]);
		if (pha) {
			DEBUG_LOG("add HONOR:%u", p_honors[loop]);
			g_hash_table_insert(p->user_honors, &pha->honor_id, pha);
		}
	}

	return 4 * honor_cnt;
}

static inline void set_user_honor(sprite_t* p, uint32_t honor_id)
{
	if (p->honor == honor_id)
		return;

	honor_attr_t* p_ha = get_honor_attr(p->honor);
	if (p_ha) {
		reduce_second_attr(p, &p_ha->attr,1);
	}

	p_ha = get_honor_attr(honor_id);
	if (p_ha) {
		add_second_attr(p, &p_ha->attr,1);
		p->honor = honor_id;
	} else {
		p->honor = 0;
	}
}

#endif
