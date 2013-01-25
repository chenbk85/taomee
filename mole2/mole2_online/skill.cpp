
#include "pet.h"
#include "skill.h"
#include "map.h"
#include "npc.h"

void init_all_skills()
{
	sprite_skills = g_hash_table_new(g_int_hash, g_int_equal);
	pet_skills = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_sprite_skill_by_key(gpointer key, gpointer p_skill, gpointer userdata)
{
	g_slice_free1(sizeof(skill_info_t), p_skill);
	return TRUE;
}

static gboolean free_pet_skill_by_key(gpointer key, gpointer p_skill, gpointer userdata)
{
	g_slice_free1(sizeof(pet_skill_info_t), p_skill);
	return TRUE;
}


void fini_all_skills()
{
	g_hash_table_destroy(sprite_skills);
	g_hash_table_destroy(pet_skills);
}

skill_info_t* get_skill_info(uint32_t skillid)
{
	return (skill_info_t *)g_hash_table_lookup(sprite_skills, &skillid);
}

pet_skill_info_t* get_pet_skill_info(uint32_t skillid)
{
	return (pet_skill_info_t *)g_hash_table_lookup(pet_skills, &skillid);
}

pet_skill_p_t* get_pet_skill_price(uint32_t skillid, uint32_t lv)
{
	pet_skill_info_t* pps = get_pet_skill_info(skillid);
	return (pps && pps->skill_lv_cnt >= lv) ? &pps->psp[lv - 1] : NULL;
}

uint32_t get_pet_skill_toplv(uint32_t skillid)
{
	pet_skill_info_t* pps = get_pet_skill_info(skillid);
	return pps ? (pps->skill_lv_cnt - 1) : 0;
}

int load_all_skills(xmlNodePtr cur)
{
	g_hash_table_foreach_remove(sprite_skills, free_sprite_skill_by_key, 0);
	g_hash_table_foreach_remove(pet_skills, free_pet_skill_by_key, 0);
	
	uint32_t id, level, studyable;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Skill")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			decode_xml_prop_uint32_default(&studyable, cur, "StudyPossibility", 0);
			if (studyable != 0 && studyable != 1) {
				ERROR_RETURN(("studyable value invalid\t[%u %u]", id, studyable), -1);
			}
			
			if ( (id >= BATTLE_SKILL_BASE_ID && id <= BATTLE_SKILL_MAX_ID) \
				|| (id >= LIFE_SKILL_BASE_ID && id <= LIFE_SKILL_MAX_ID)) {
				skill_info_t* psi = get_skill_info(id);
				if (psi) {
					KERROR_LOG(0, "duplicate skillid\t[%u]", id);
					return -1;
				}
				psi = (skill_info_t *)g_slice_alloc(sizeof(skill_info_t));
				memset(psi, 0, sizeof(skill_info_t));
				psi->skill_id = id;
				g_hash_table_insert(sprite_skills, &(psi->skill_id), psi);
				decode_xml_prop_uint32_default(&psi->professtion, cur, "Prof", 0);
				psi->studyable = studyable;

				if (studyable) {
					DECODE_XML_PROP_UINT32(psi->price, cur, "Price");
					if (!psi->price) {
						KERROR_LOG(0, "no price\t[%u]", id);
						return -1;
					}
				}
				
				//decode_xml_prop_uint32_default(&psi->price, cur, "Price", 1000000);
				decode_xml_prop_uint32_default(&psi->base_exp, cur, "BaseExp", 0);
				decode_xml_prop_uint32_default(&psi->rolelv, cur, "MinRoleLevel", 0);

				xmlNodePtr chl = cur->xmlChildrenNode;
				while(chl) {
					if ((!xmlStrcmp(chl->name, (const xmlChar *)"Job"))) {
						uint32_t prof, prof_id = 0xFFFFFFFF;
						DECODE_XML_PROP_INT (prof, chl, "ID");
						if (nbits_on32(prof) != 1)
							ERROR_RETURN(("Invalid prof id: %u", prof), -1);

						while (prof) {
							prof = prof >> 1;
							prof_id ++;
						}

						if (prof_id >= MAX_SKILL_PROFS || prof_id == 0xFFFFFFFF || psi->skill_prof[prof_id].prof_id) {
							ERROR_RETURN (("Invalid skill id <prof>: %u %u %d", id, prof, prof_id), -1);
						}

						psi->skill_prof[prof_id].prof_id = prof_id;
						decode_xml_prop_uint32_default(&psi->skill_prof[prof_id].add_exp, chl, "Exp", 0);
						decode_xml_prop_arr_int_default(psi->skill_prof[prof_id].phaselv, MAX_SKILL_PHASE, chl, "PhaseLevel", 0);
					}
					chl = chl->next;
				}
			} else if  (id >= PET_SKILL_BASE_ID && id < PET_SKILL_MAX_ID) {
				pet_skill_info_t* ppsi = get_pet_skill_info(id);
				if (ppsi) {
					KERROR_LOG(0, "duplicate skillid\t[%u]", id);
					return -1;
				}
				ppsi = (pet_skill_info_t *)g_slice_alloc(sizeof(pet_skill_info_t));
				memset(ppsi, 0, sizeof(pet_skill_info_t));
				ppsi->skill_id = id;
				g_hash_table_insert(pet_skills, &(ppsi->skill_id), ppsi);
				ppsi->skill_lv_cnt = 0;
				xmlNodePtr chl = cur->xmlChildrenNode;
				while(chl) {
					if ((!xmlStrcmp(chl->name, (const xmlChar *)"Lv"))) {
						DECODE_XML_PROP_UINT32(level, chl, "SkillLevel");
						if (ppsi->psp[level - 1].pet_lv == level) {
							ERROR_RETURN(("duplicate skill level\t[%u %u]", id, level), -1);
						}

						ppsi->psp[level - 1].studyable = studyable;
						ppsi->psp[level - 1].skill_lv= level;
						ppsi->psp[level - 1].skill_key = id;
						ppsi->psp[level - 1].skill_key <<= 32;
						ppsi->psp[level - 1].skill_key += level;
						if (studyable) {
							DECODE_XML_PROP_UINT32(ppsi->psp[level - 1].pet_lv, chl, "PetLevel");
							//DECODE_XML_PROP_UINT32(ppsi->psp[level - 1].price, chl, "Price");
							decode_xml_prop_uint32_default(&ppsi->psp[level - 1].price, chl, "Price", 1000000);
						}
						ppsi->skill_lv_cnt ++;
					}
					chl = chl->next;
				}
			}
		}
	
		cur = cur->next;
	}

	return 0;
}

int buy_skill_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t shopid, skillid;
	int i = 0, j = 0;
	uint8_t out[16];
	UNPKG_UINT32(body, shopid, j);
	UNPKG_UINT32(body, skillid, j);
	
	CHECK_USER_IN_MAP(p, 0);
	
	map_id_t mapid = p->tiles->id;
	KDEBUG_LOG(p->id, "ADD SKILL\t[%u %u %lu]", shopid, skillid, mapid);
	if (IS_HOME_MAP(mapid) || ((IS_MAZE_MAP(mapid) && p->tiles->shop_id != shopid)) \
		|| !skill_shop_existed(shopid, skillid, mapid)) {
		KERROR_LOG(p->id, "shop not exist\t[%u %u %lu]", shopid, skillid, mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	
	skill_info_t* psi = get_skill_info(skillid);
	if (!psi || !psi->studyable) {
		return send_to_self_error((p), (p)->waitcmd, cli_err_skill_cannot_study, 1);
	}

	if ((IS_BATTLE_SKILL(skillid) && p->skill_cnt >= MAX_SKILL_BAR) \
		|| (IS_LIFE_SKILL(skillid) && p->life_skill_cnt >= MAX_SKILL_BAR)) {
		KERROR_LOG(p->id, "cli add skill max count\t[%u]", p->skill_cnt);
		return send_to_self_error(p, p->waitcmd, cli_err_skill_cnt_max, 1);
	}

	CHECK_LV_FIT(p, p, psi->rolelv, MAX_SPRITE_LEVEL);
	
	for (int k = 0; k < p->skill_cnt; k ++) {
		if (p->skills[k].skill_id == skillid) {
			KERROR_LOG(p->id, "cli add skill have got\t[%u]", skillid);
			return send_to_self_error(p, p->waitcmd, cli_err_skill_have_got, 1);
		}
	}

	CHECK_PROF_FIT(p, psi->professtion, 0);
	CHECK_XIAOMEE_ENOUGH(p, psi->price);
	
	PKG_H_UINT32(out, skillid, i);
	PKG_H_UINT8(out, 1, i);
	PKG_H_UINT32(out, - psi->price, i);
	return send_request_to_db(p, p->id, proto_db_buy_skill, out, i);
}

int del_skill_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t skill_id;
	int j = 0;
	UNPKG_UINT32(body, skill_id, j);
	for (int i = 0; i < p->skill_cnt; i ++) {
		if (p->skills[i].skill_id == skill_id) {
			return send_request_to_db(p, p->id, proto_db_del_skill, &skill_id, 4);
		}
	}
	return send_to_self_error(p, p->waitcmd, cli_err_skill_not_have, 1);
}

int del_skill_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t skill_id;
	int j = 0;
	KNOTI_LOG(p->id, "DEL SKILL\t[%u]", skill_id);
	UNPKG_H_UINT32(buf, skill_id, j);
	cache_del_skill(p, skill_id);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int buy_skill_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	int32_t xiaomee, cost;
	skill_t* ps = (skill_t*)buf;
	int j = sizeof(skill_t);
	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, cost, j);
	p->xiaomee = xiaomee;
	monitor_sprite(p, "BUY SKILL");
	cache_add_skill(p, ps);

	return response_user_add_skill(p, ps, -cost,1);
}

int buy_pet_skill_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t shopid, petid, skillid, skill_lv;
	int j = 0;
	UNPKG_UINT32(body, shopid, j);
	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, skillid, j);
	UNPKG_UINT32(body, skill_lv, j);
	KDEBUG_LOG(p->id, "ADD PET SKILL\t[%u %u %u %u]",shopid, petid, skillid, skill_lv);

	CHECK_USER_IN_MAP(p, 0);
	
	map_id_t mapid = p->tiles->id;
	if (IS_HOME_MAP(mapid) || (IS_MAZE_MAP(mapid) && p->tiles->shop_id != shopid) \
		|| !pet_skill_shop_existed(shopid, skillid, skill_lv, mapid)) {
		KERROR_LOG(p->id, "item shop not exsited\t[%u %u %lu]", shopid, skillid, mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);
	
	pet_skill_p_t* p_psp = get_pet_skill_price(skillid, skill_lv);
	beast_t* p_ba = get_beast(p_p->pettype);
	if (!p_psp || !p_psp->studyable || !p_ba) {
		KERROR_LOG(p->id, "cannot buy skill\t[%u %u %u]", p_p->pettype, skillid, skill_lv);
		return send_to_self_error(p, p->waitcmd, cli_err_skill_cannot_study, 1);
	}

	CHECK_XIAOMEE_ENOUGH(p, p_psp->price);
	CHECK_PET_LV_FIT(p, p_p, p_psp->pet_lv, MAX_SPRITE_LEVEL);

	int loop = 0;
	for (loop = 0; loop < p_p->skill_cnt; loop ++) {
		if (p_p->skills[loop].skill_id == skillid) {
			if (p_p->skills[loop].skill_level >= skill_lv || p_p->skills[loop].skill_exp) {
				return send_to_self_error(p, p->waitcmd, cli_err_skill_have_got, 1); 
			} else {
				break;
			}
		}
	}
	
	if (loop == p_p->skill_cnt && p_p->skill_cnt >= p_ba->skillbar) {
		return send_to_self_error(p, p->waitcmd, cli_err_skill_cnt_max, 1);
	}

	return db_buy_pet_skill(p, p->id, p_p, skillid, p_psp->skill_lv, p_psp->price);
}

int buy_pet_skill_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	uint32_t petid, skill_id, skill_lv, xiaomee;
	int32_t cost;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, petid, j);
	UNPKG_H_UINT32(buf, skill_id, j);
	UNPKG_H_UINT8(buf, skill_lv, j);
	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, cost, j);
	KDEBUG_LOG(p->id, "ADD PET SKILL CALLBACK\t[%u %u %u %u %d]", petid, skill_id, skill_lv, xiaomee, cost);
	p->xiaomee = xiaomee;
	monitor_sprite(p, "BUY PET SKILL");

	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	cache_add_pet_skill(p, p_p, skill_id, skill_lv);

	PKG_UINT32(msg, petid, i);
	PKG_UINT32(msg, skill_id, i);
	PKG_UINT8(msg, skill_lv, i);
	PKG_UINT32(msg, xiaomee, i);
	PKG_UINT32(msg, - cost, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int del_pet_skill_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t petid, skill_id, skill_lv;
	int j = 0;
	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, skill_id, j);
	UNPKG_UINT32(body, skill_lv, j);

	KDEBUG_LOG(p->id, "DEL PET SKILL\t[%u %u %u]", petid, skill_id, skill_lv);

	return db_del_pet_skill(p, p->id, petid, skill_id);
}

int del_pet_skill_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int j = 0;
	uint32_t petid, skill_id, skill_lv;
	UNPKG_H_UINT32(buf, petid, j);
	UNPKG_H_UINT32(buf, skill_id, j);
	UNPKG_H_UINT8(buf, skill_lv, j);
	
	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);
	
	cache_del_pet_skill(p_p, skill_id);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int unlock_pet_talent_skill_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t petid;
	int i = 0;
	UNPKG_UINT32(body, petid, i);
	pet_t* pp = get_pet_inbag(p, petid);
	uint32_t xiaomee = 500;
	CHECK_XIAOMEE_ENOUGH(p, xiaomee);
	CHECK_PET_INBAG(p, pp, petid);
	CHECK_PET_LV_FIT(p, pp, 60, MAX_SPRITE_LEVEL);

	int loop;
	for (loop = 0; loop < pp->skill_cnt; loop ++) {
		if (pp->skills[loop].skill_exp) {
			pp->skills[loop].skill_exp = 0;
			db_buy_pet_skill(NULL, p->id, pp, pp->skills[loop].skill_id, pp->skills[loop].skill_level, xiaomee);
			xiaomee = 0;
		}
	}
	p->xiaomee -= 500;
	response_proto_uint32_uint32(p, p->waitcmd, petid, p->xiaomee, 1, 0);
	return 0;
}

int get_skill_list_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int i = sizeof(protocol_t);
	KDEBUG_LOG(p->id,"get_skill_list_cmd %u",p->skill_expbox);
	PKG_UINT32(msg, p->skill_expbox, i);
	PKG_UINT32(msg, p->skill_cnt + p->life_skill_cnt, i);
	int loop;
	for (loop = 0; loop < p->skill_cnt; loop ++) {
		PKG_UINT32(msg, p->skills[loop].skill_id, i);
		PKG_UINT8(msg, p->skills[loop].skill_level, i);
		PKG_UINT32(msg, p->skills[loop].skill_exp, i);
	}

	for (loop = 0; loop < p->life_skill_cnt; loop ++) {
		PKG_UINT32(msg, p->life_skills[loop].skill_id, i);
		PKG_UINT8(msg, p->life_skills[loop].skill_level, i);
		PKG_UINT32(msg, p->life_skills[loop].skill_exp, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

uint32_t get_max_skill_exp(sprite_t* p, skill_t* ps, skill_info_t* psl)
{
	int skill_exp_ratio[MAX_SKILL_LEVEL] = {0, 1, 4, 11, 25, 46, 74, 102, 137, 179};
	uint32_t job_maxlv = psl->skill_prof[p->prof].phaselv[p->prof_lv];
	return psl->base_exp * skill_exp_ratio[job_maxlv - 1];
}

void cache_update_skill_level(sprite_t* p, skill_t* ps, skill_info_t* psl, uint32_t new_exp)
{
	int skill_exp_ratio[MAX_SKILL_LEVEL] = {0, 1, 4, 11, 25, 46, 74, 102, 137, 179};
	uint32_t job_maxlv, max_exp;
	KDEBUG_LOG(p->id, "UPDATE SKILL EXP LV\t[%u %u]", ps->skill_id, new_exp);
	if (!p->prof) // no profess skill
		job_maxlv = psl->skill_prof[0].phaselv[0];
	else
		job_maxlv = psl->skill_prof[p->prof].phaselv[p->prof_lv];
	KDEBUG_LOG(p->id,"xxx %u %u ", ps->skill_level, job_maxlv );
	if (job_maxlv <= ps->skill_level) {
		return;
	}
	max_exp = psl->base_exp * skill_exp_ratio[job_maxlv - 1];
	if (new_exp >= max_exp)
		ps->skill_exp = max_exp;
	else
		ps->skill_exp = new_exp;

	int lv = 0;
	for (lv = 0; lv < MAX_SKILL_LEVEL; lv ++) {
		if (ps->skill_exp < psl->base_exp * skill_exp_ratio[lv])
			break;			
	}
	if (lv != ps->skill_level)
		KNOTI_LOG(p->id, "SKILL LV UP\t[%u %u %u]", ps->skill_id, ps->skill_level, lv);
	ps->skill_level = lv;
}


int cache_update_skills(sprite_t* p, skill_t* apsi, uint32_t skill_cnt)
{
	p->skill_cnt = 0;
	p->life_skill_cnt = 0;
	for (uint32_t loop = 0; loop < skill_cnt; loop ++) {
		skill_t* ps = apsi + loop;
		if (IS_LIFE_SKILL(ps->skill_id) && p->life_skill_cnt < MAX_SKILL_BAR) {
			memcpy(&p->life_skills[p->life_skill_cnt], ps, sizeof(skill_t));
			p->life_skill_cnt ++;
		} else if (IS_BATTLE_SKILL(ps->skill_id) && p->skill_cnt < MAX_SKILL_BAR) {
			memcpy(&p->skills[p->skill_cnt], ps, sizeof(skill_t));
			p->skill_cnt ++;
		} else {
			KERROR_LOG(p->id, "Invalid skill id or too much\t[%u %u %u]", ps->skill_id, p->skill_cnt, p->life_skill_cnt);
		}
	}

	return skill_cnt * sizeof(skill_t);
}

int cache_add_skill(sprite_t* p, skill_t* ps)
{
	KDEBUG_LOG(p->id, "ADD CACHE SKILL\t[%u]", ps->skill_id);
	if (IS_BATTLE_SKILL(ps->skill_id) && p->skill_cnt < MAX_SKILL_BAR) {
		memcpy(&p->skills[p->skill_cnt], ps, sizeof(skill_t));
		p->skill_cnt ++;
		return 0;
	} else if (IS_LIFE_SKILL(ps->skill_id) && p->life_skill_cnt < MAX_SKILL_BAR) {
		memcpy(&p->life_skills[p->life_skill_cnt], ps, sizeof(skill_t));
		p->life_skill_cnt ++;
		return 0;
	}
	log_sprite_skill(p, 0);
	log_sprite_skill(p, 1);
	ERROR_RETURN(("add skill fail\t[%u %u]", p->id, ps->skill_id), -1);
}

int cache_del_skill(sprite_t* p, uint32_t skill_id)
{
	KDEBUG_LOG(p->id, "DEL CACHE SKILL\t[%u]", skill_id);
	if (IS_BATTLE_SKILL(skill_id)) {
		for (int loop = 0; loop < p->skill_cnt; loop ++) {
			if (p->skills[loop].skill_id == skill_id) {
				p->skill_cnt --;
				memcpy(&p->skills[loop], &p->skills[p->skill_cnt], sizeof(skill_t));
				memset(&p->skills[p->skill_cnt], 0, sizeof(skill_t));
				return 0;
			}
		}
	} else if (IS_LIFE_SKILL(skill_id)) {
		for (int loop = 0; loop < p->life_skill_cnt; loop ++) {
			if (p->life_skills[loop].skill_id == skill_id) {
				p->life_skill_cnt --;
				memcpy(&p->life_skills[loop], &p->life_skills[p->life_skill_cnt], sizeof(skill_t));
				memset(&p->life_skills[p->life_skill_cnt], 0, sizeof(skill_t));
				return 0;
			}
		}
	}
	ERROR_RETURN(("del skill not exist\t[%u %u]", p->id, skill_id), -1);
}

skill_t* cache_get_skill(sprite_t* p, uint32_t skill_id)
{
	int loop = 0;
	if (IS_BATTLE_SKILL(skill_id)) {
		for (loop = 0; loop < p->skill_cnt; loop ++) {
			if (skill_id == p->skills[loop].skill_id)
				return &p->skills[loop];
		}
		return NULL;
	} else if (IS_LIFE_SKILL(skill_id)) {
		for (loop = 0; loop < p->life_skill_cnt; loop ++) {
			if (skill_id == p->life_skills[loop].skill_id)
				return &p->life_skills[loop];
		}
		return NULL;
	}
	KERROR_LOG(p->id, "skill type invalid\t[%u]", skill_id);
	return NULL;
}

int db_buy_pet_skill(sprite_t* p, uint32_t uid, pet_t* p_p, uint32_t skillid, uint32_t skilllv, int cost)
{
	int i = 0;
	uint8_t out[64];
	PKG_H_UINT32(out, p_p->petid, i);
	PKG_H_UINT32(out, skillid, i);
	PKG_H_UINT8(out, skilllv, i);
	PKG_H_UINT32(out, - cost, i);
	return send_request_to_db(p, uid, proto_db_pet_buy_skill, out, i);
}

int db_del_pet_skill(sprite_t* p, uint32_t uid, uint32_t petid, uint32_t skillid)
{
	int i = 0;
	uint8_t out[64];
	PKG_H_UINT32(out, petid, i);
	PKG_H_UINT32(out, skillid, i);
	PKG_H_UINT8(out, 0, i);

	return send_request_to_db(p, uid, proto_db_pet_del_skill, out, i);

}

int set_default_skill_level(sprite_t* p, Cmessage* c_in)
{
	set_default_skill_level_in* p_in = P_IN;
	skill_t* ps;
	if (p_in->petid) {
		pet_t* pp = get_pet_inbag(p, p_in->petid);
		CHECK_PET_INBAG(p, pp, p_in->petid);
		ps = cache_get_pet_skill(pp, p_in->skillid);
		if (!ps || p_in->defaultlv > ps->skill_level) {
			KERROR_LOG(p->id, "pet skill no exist\t[%u %u %u]", p_in->petid, p_in->skillid, p_in->defaultlv);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
	} else {
		ps = cache_get_skill(p, p_in->skillid);
		if (!ps || p_in->defaultlv > ps->skill_level) {
			KERROR_LOG(p->id, "skill no exist\t[%u %u %u]", p_in->petid, p_in->skillid, p_in->defaultlv);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
	}
	*(skill_t **) p->session = ps;
	*(uint32_t *)(p->session + sizeof(skill_t *)) = p_in->defaultlv;
	return db_set_default_skill_level(p, p->id, p_in->petid, p_in->skillid, p_in->defaultlv);
}

int mole2_set_skill_def_level(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	skill_t* ps = *(skill_t **)p->session;
	ps->default_lv = *(uint32_t *) (p->session + sizeof(skill_t *));
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int allocate_skill_exp(sprite_t* p, Cmessage* c_in)
{
	allocate_skill_exp_in* p_in = P_IN;
	skill_t* ps = cache_get_skill(p, p_in->skillid);
	skill_info_t* psi = get_skill_info(p_in->skillid);
	if (!ps || !psi || IS_LIFE_SKILL(p_in->skillid)) {
		KERROR_LOG(p->id, "invalid skillid\t[%u %p %p]", p_in->skillid, ps, psi);
		return send_to_self_error(p, p->waitcmd, cli_err_skill_not_have, 1);
	}

	if (p_in->exp > p->skill_expbox) {
		KERROR_LOG(p->id, "skill exp not enough\t[%u %u]", p_in->exp, p->skill_expbox);
		return send_to_self_error(p, p->waitcmd, cli_err_exp_not_enough, 1);
	}

	if (ps->skill_exp + p_in->exp > get_max_skill_exp(p, ps, psi)) {
		KERROR_LOG(p->id, "too much\t[%u %u %u]", ps->skill_exp, p_in->exp, get_max_skill_exp(p, ps, psi));
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	*(uint32_t *)p->session = p_in->exp;
	*(skill_t **)(p->session + 4) = ps;
	*(skill_info_t **)(p->session + 4 + sizeof(skill_t *)) = psi;

	mole2_add_skill_exp_in db_in;
	db_in.add_exp = -p_in->exp;
	return send_msg_to_db(p, p->id, mole2_add_skill_exp_cmd, &db_in);

}

int mole2_add_skill_exp(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_add_skill_exp_out* p_out = P_OUT;
	p->skill_expbox = p_out->cur_exp;

	uint32_t add_exp = *(uint32_t *)p->session;
	skill_t* ps = *(skill_t **)(p->session + 4);
	skill_info_t* psi = *(skill_info_t **)(p->session + 4 + sizeof(skill_t *));

	KDEBUG_LOG(p->id,"mole2_add_skill_exp %u %u",p->skill_expbox,add_exp);
	cache_update_skill_level(p, ps, psi, ps->skill_exp + add_exp);
	db_update_skill_info(NULL, p->id, ps);
	db_set_default_skill_level(NULL, p->id, 0, ps->skill_id, ps->default_lv);

	allocate_skill_exp_out cli_out;
	cli_out.skillid = ps->skill_id;
	cli_out.skill_level = ps->skill_level;
	cli_out.skill_exp = ps->skill_exp;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}
//自动领悟职业技能
#define     NEW_PROF_SKILL_CNT  7
void add_skill_for_new_prof(sprite_t *p,uint32_t level)
{
	cli_noti_add_skill_out cli_out;
	if(IS_NEW_PROF(p->prof)){
		uint32_t arr[4][NEW_PROF_SKILL_CNT]={{4001,4002,4003,4004,4005,4006},
							{4011,4012,4013,4014,4015,4016},
							{4021,4022,4023,4024,4025,4026},
							{4031,4032,4033,4034,4035,4036}};
		uint32_t idx=p->prof-prof_gedoushi;
		uint32_t cnt=(level-30)/10+3;
		DEBUG_LOG("idx=%u cnt=%u",idx,cnt);
		for( uint32_t loop=0 ; loop < cnt ; loop++){
			skill_info_t* psi = get_skill_info(arr[idx][loop]);
			if( psi && !cache_get_skill(p,arr[idx][loop])){
				//KDEBUG_LOG(p->id,"new prof add skill:%u",psi->skill_id);
				uint8_t out[64];
				int i=0;
				PKG_H_UINT32(out, psi->skill_id, i);
				PKG_H_UINT8(out, 1, i);
				PKG_H_UINT32(out, 0, i);
				send_request_to_db(NULL, p->id, proto_db_buy_skill, out, i);
				skill_t tmp={psi->skill_id,1,1,0};
				cache_add_skill(p,&tmp);
				memset(out,0,sizeof(out));
				i = sizeof(protocol_t);
				PKG_UINT32(out, tmp.skill_id, i);
				PKG_UINT8(out, tmp.skill_level, i);
				PKG_UINT32(out, tmp.skill_exp, i);
				PKG_UINT32(out, p->xiaomee, i);
				PKG_UINT32(out, 0, i);
				init_proto_head(out, proto_cli_buy_skill, i);
				send_to_self(p, out, i, 0);
			}
		}
	}
}
