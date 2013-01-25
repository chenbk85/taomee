#include "buff.h"

int mole2_get_vip_buff (sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_vip_buff_out* p_out = P_OUT;
	for (uint32_t loop = 0; loop < p_out->petlist.size(); loop ++) {
		if (p_out->petlist[loop].petid < buffid_max_vip) {
			KTRACE_LOG(p->id, "%u", p_out->petlist[loop].petid);
			memcpy(p->vip_buffs[p_out->petlist[loop].petid], p_out->petlist[loop].buff, VIP_BUFF_LEN);
		} else if (p_out->petlist[loop].petid >= buffid_beast_lv10 && p_out->petlist[loop].petid < buffid_max_beast) {
			int idx = p_out->petlist[loop].petid - buffid_beast_lv10;
			memcpy(&p->loop_grp[idx], p_out->petlist[loop].buff, VIP_BUFF_LEN);
		}
	}

	return send_request_to_db(p, p->id, mole2_get_hero_team_cmd, NULL, 0);
}

int cache_auto_hpmp_for_user(sprite_t* p)
{
	int ret = 0;
	uint32_t* pbuff = p->vip_buffs[buffid_hpmp];
	if (p->hp < p->hp_max && pbuff[0]) {
		uint32_t addhp = p->hp_max - p->hp;
		if (addhp > pbuff[0])
			addhp = pbuff[0];
		p->hp += addhp;
		pbuff[0] -= addhp;
		ret = 1;
	}

	if (p->mp < p->mp_max && pbuff[1]) {
		uint32_t addmp = p->mp_max - p->mp;
		if (addmp > pbuff[1])
			addmp = pbuff[1];
		p->mp += addmp;
		pbuff[1] -= addmp;
		ret = 1;
	}
	return ret;
}

int cache_auto_hpmp_for_pet(sprite_t* p, pet_t* pp)
{
	int ret = 0;
	uint32_t* pbuff = p->vip_buffs[buffid_hpmp];
	if (pp->hp < pp->hp_max && pbuff[0]) {
		uint32_t addhp = pp->hp_max - pp->hp;
		if (addhp > pbuff[0])
			addhp = pbuff[0];
		pp->hp += addhp;
		pbuff[0] -= addhp;
		ret = 1;
	}

	if (pp->mp < pp->mp_max && pbuff[1]) {
		uint32_t addmp = pp->mp_max - pp->mp;
		if (addmp > pbuff[1])
			addmp = pbuff[1];
		pp->mp += addmp;
		pbuff[1] -= addmp;
		ret = 1;
	}
	return ret;
}

int pkg_ratio_buff(uint32_t* buff, uint8_t* buf)
{
	uint32_t factor = 100, count = 0xFFFFFFFF;
	for (uint32_t loop = 0; loop < buff[0]; loop ++) {
		factor = factor * buff[1 + 2 * loop] / 100;
		count = buff[2 + 2 * loop] < count ? buff[2 + 2 * loop] : count;
	}

	if (factor >= 300)
		factor = 300;

	int i = 0;
	PKG_UINT32(buf, factor, i);
	PKG_UINT32(buf, count, i);
	return i;
}


