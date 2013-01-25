#ifndef MOLE2_BUFF_H_
#define MOLE2_BUFF_H_

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"
#include "proto/mole2_db.h"

static inline void update_vip_buff(uint32_t* buf)
{
	for (uint32_t loop = 0; loop < buf[0]; loop ++) {
		buf[2 + 2 * loop] --;
		if (!buf[2 + 2 * loop]) {
			buf[0] --;
			memcpy(&buf[1 + 2 * loop], &buf[1 + 2 * buf[0]], 8);
			memset(&buf[1 + 2 * buf[0]], 0, 8);
			loop --;
		}
	}
}

static inline int update_ratio_buff(sprite_t* p)
{
	update_vip_buff(p->vip_buffs[0]);
	update_vip_buff(p->vip_buffs[1]);
	update_vip_buff(p->vip_buffs[2]);
	uint8_t out[1024];
	int i = 0;
	PKG_H_UINT32(out, 3, i);
	PKG_H_UINT32(out, 0, i);
	PKG_STR(out, p->vip_buffs[0], i, VIP_BUFF_LEN);
	PKG_H_UINT32(out, 1, i);
	PKG_STR(out, p->vip_buffs[1], i, VIP_BUFF_LEN);
	PKG_H_UINT32(out, 2, i);
	PKG_STR(out, p->vip_buffs[2], i, VIP_BUFF_LEN);
	return send_request_to_db(NULL, p->id, mole2_set_vip_buff_cmd, out, i);
}

static inline int db_set_vip_buff(sprite_t* p, uint32_t buffid)
{
	mole2_set_vip_buff_in db_in;
	stru_vip_buff tmp_buff;
	tmp_buff.petid = buffid;
	memcpy(tmp_buff.buff, p->vip_buffs[buffid], VIP_BUFF_LEN);
	db_in.petlist.push_back(tmp_buff);
	return send_msg_to_db(NULL, p->id, mole2_set_vip_buff_cmd, &db_in);
}

static inline int db_set_loopgrp(sprite_t* p, uint32_t idx)
{
	mole2_set_vip_buff_in db_in;
	stru_vip_buff tmp_buff;
	tmp_buff.petid = buffid_beast_lv10 + idx;
	memcpy(tmp_buff.buff, &p->loop_grp[idx], VIP_BUFF_LEN);
	db_in.petlist.push_back(tmp_buff);
	return send_msg_to_db(NULL, p->id, mole2_set_vip_buff_cmd, &db_in);
}


static inline uint32_t get_auto_fight_count(sprite_t* p)
{
	uint32_t* pbuff = p->vip_buffs[buffid_auto_fight];
	if (!check_same_period(pbuff[0], repeat_day, 5)) {
		pbuff[0] = get_now_tv()->tv_sec;
		pbuff[1] = MAX_AUTO_FIGHT_CNT  ;//自动战斗次数
	}
	return pbuff[1] + pbuff[2];
}

static inline void update_fight_buff(sprite_t* p, uint32_t count)
{
	uint32_t* pbuff = p->vip_buffs[buffid_auto_fight];
	if (count >= pbuff[2]) {
		pbuff[1] = count - pbuff[2];
	} else {
		pbuff[1] = 0;
		pbuff[2] = count;
	}
	pbuff[0] = get_now_tv()->tv_sec;
	db_set_vip_buff(p, buffid_auto_fight);
}

int cache_auto_hpmp_for_user(sprite_t* p);
int cache_auto_hpmp_for_pet(sprite_t* p, pet_t* pp);

int pkg_ratio_buff(uint32_t* buff, uint8_t* buf);

#endif
