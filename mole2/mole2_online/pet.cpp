
#include "battle.h"
#include "pet.h"
#include "skill.h"
#include "util.h"
#include "items.h"
#include "task_new.h"
#include "home.h"
#include "beast.h"
#include "activity.h"

beast_t* get_beast_no_check(uint32_t id)
{
	id = id % SPEC_BEAST_BASE;
	return id && id < MAX_BEAST_TYPE ? &beasts_attr[id - 1] : NULL;
}

beast_t* get_beast(uint32_t id)
{
	beast_t* p = get_beast_no_check(id);
	return p && p->id ? p : NULL;
}

beast_lvinfo_t* get_beast_lvinfo_no_check(beast_t* pba, uint32_t lv)
{
	if (lv > MAX_SPRITE_LEVEL || !lv)
		return NULL;
	std::map<uint32_t ,beast_lvinfo_t>::iterator it=pba->levels.find(lv);
	if(it == pba->levels.end()){
		beast_lvinfo_t tmp;
		memset(&tmp, 0, sizeof(beast_lvinfo_t ));
		pba->levels[lv]=tmp;
		it=pba->levels.find(lv);
	}
	return &(it->second);
	//return 	&pba->levels[lv-1];
}

beast_lvinfo_t* get_beast_lvinfo(uint32_t beastid, uint32_t lv)
{
	beast_t* pba = get_beast(beastid);
	if (!pba) return NULL;
	beast_lvinfo_t* pbl = get_beast_lvinfo_no_check(pba, lv);
	return pbl && pbl->id ? pbl : NULL;
}


int load_beast_drop_item(xmlNodePtr cur, beast_lvinfo_t* pbl)
{
	int ids[MAX_DROP_ITEM_TYPE], rate[MAX_DROP_ITEM_TYPE];
	pbl->item_cnt = decode_xml_prop_arr_int_default(ids, MAX_DROP_ITEM_TYPE, cur, "ItemID", 0);
	if (pbl->item_cnt != decode_xml_prop_arr_int_default(rate, MAX_DROP_ITEM_TYPE, cur, "Rate", 0))
		ERROR_RETURN(("drop item rate count not match\t[%u %u]", pbl->id, pbl->item_cnt), -1);
	int loop;
	for (loop = 0; loop < pbl->item_cnt; loop ++) {
		if (!get_item(ids[loop]) && !get_cloth(ids[loop])) {
			ERROR_RETURN(("drop item id invalid\t[%u %u]", pbl->id, ids[loop]), -1);
		}
		pbl->items[loop].itemid = ids[loop];
		pbl->items[loop].rate = rate[loop];
	}
	return 0;
}

int load_beast_talentskill(xmlNodePtr cur, beast_t* pba)
{
	pba->talent_skill_cnt = decode_xml_prop_arr_int_default((int *)pba->talent_skill_ids, MAX_SKILL_BAR, cur, "ID", 0);
	if (pba->talent_skill_cnt != decode_xml_prop_arr_int_default((int *)pba->talent_skill_percent, MAX_SKILL_BAR, cur, "Percent", 0))
		ERROR_RETURN(("rate count not match\t[%u %u]", pba->id, pba->talent_skill_cnt), -1);

	int loop, percent = 0;
	for (loop = 1; loop < pba->talent_skill_cnt; loop ++) {
		percent += pba->talent_skill_percent[loop];
	}

	if (pba->talent_skill_percent[0] != RAND_COMMON_RAND_BASE || percent != RAND_COMMON_RAND_BASE)
		ERROR_RETURN(("percent invalid\t[%u %u]", pba->talent_skill_percent[0], percent), -1);
	KTRACE_LOG(pba->id, "TALENT SKILL\t[%u %u %u %u]", pba->talent_skill_ids[0], pba->talent_skill_ids[1], pba->talent_skill_ids[2], pba->talent_skill_ids[3]);
	return 0;
}

int load_beast_baseskill(xmlNodePtr cur, beast_t* pba)
{
	int ids[MAX_SKILL_BAR], percent[MAX_SKILL_BAR];
	pba->skill_cnt = decode_xml_prop_arr_int_default(ids, MAX_SKILL_BAR, cur, "ID", 0);
	if (pba->skill_cnt != decode_xml_prop_arr_int_default(percent, MAX_SKILL_BAR, cur, "Percent", 0))
		ERROR_RETURN(("drop item rate count not match\t[%u %u]", pba->id, pba->skill_cnt), -1);

	if (pba->skill_cnt + pba->skill_cnt_all > MAX_SKILL_BAR) {
		ERROR_RETURN(("skill cnt max\t[%u %u %u]", pba->id, pba->skill_cnt, pba->skill_cnt_all), -1);
	}

	if (pba->skill_cnt_all && pba->skill_cnt) {
		memcpy(&pba->skills[pba->skill_cnt], &pba->skills[0], pba->skill_cnt_all * sizeof(beast_skill_t));
	}
	pba->skill_cnt_all += pba->skill_cnt;
	int loop;
	for (loop = 0; loop < pba->skill_cnt; loop ++) {
		pba->skills[loop].skillid = ids[loop];
		pba->skills[loop].percent = percent[loop];
	}
	return 0;
}

int load_beast_specskill(xmlNodePtr cur, beast_t* pba)
{
	uint32_t skillcnt;
	int ids[MAX_SKILL_BAR], percent[MAX_SKILL_BAR];
	skillcnt = decode_xml_prop_arr_int_default(ids, MAX_SKILL_BAR, cur, "ID", 0);
	if (skillcnt != decode_xml_prop_arr_int_default(percent, MAX_SKILL_BAR, cur, "Percent", 0))
		ERROR_RETURN(("drop item rate count not match\t[%u %u]", pba->id, skillcnt), -1);

	pba->skill_cnt_all += skillcnt;

	if (pba->skill_cnt_all > MAX_SKILL_BAR) {
		ERROR_RETURN(("skill cnt max\t[%u %u %u]", pba->id, pba->skill_cnt, pba->skill_cnt_all), -1);
	}

	uint32_t loop;
	for (loop = 0; loop < skillcnt; loop ++) {
		pba->skills[loop + pba->skill_cnt].skillid = ids[loop];
		pba->skills[loop + pba->skill_cnt].percent = percent[loop];
	}
	return 0;
}

int load_beast_lvinfo(xmlNodePtr cur, beast_t* pba)
{
	uint32_t lv;
	DECODE_XML_PROP_UINT32(lv, cur, "Lv");
	beast_lvinfo_t* pbl = get_beast_lvinfo_no_check(pba, lv);
	if (!pbl) {
		ERROR_RETURN(("beast lv info invalid\t[%u %u]", pba->id, lv), -1);
	}

	if (pbl->id) {
		ERROR_RETURN(("beast lv info duplicate\t[%u %u]", pba->id, lv), -1);
	}
	pbl->id = pba->id;
	pbl->level = lv;
	DECODE_XML_PROP_UINT32(pbl->catchable, cur, "Catch");
	return load_beast_drop_item(cur, pbl);
}

void log_beast_attr(beast_t* pba)
{
	KTRACE_LOG(0, "BEASTS\t[%u %u %s %u %u %u %u]\t[%u %u %u %u %u]\t[%u %u %u %u]\t[%u %u]", \
		pba->id, pba->race, pba->nick, pba->price, pba->sell_price, pba->tradability, pba->vip_tradability, \
		pba->physi_factor, pba->stren_factor, pba->endur_factor, pba->quick_factor, pba->intel_factor, \
		pba->earth, pba->water, pba->fire, pba->wind, pba->handbooklv, pba->skillbar);
	int loop;
	for (loop = 0; loop < pba->skill_cnt_all; loop ++) {
		KTRACE_LOG(0, "SKILLS\t[%u]\t[%u %u]", pba->skill_cnt_all, pba->skills[loop].skillid, pba->skills[loop].percent);
	}
	//for (loop = 0; loop < MAX_SPRITE_LEVEL; loop ++) {
		//if (pba->levels[loop].id) {
			//KTRACE_LOG(0, "LVINFO\t[%u %u %u %u %u]", pba->levels[loop].id, pba->levels[loop].catchable, 
				//pba->levels[loop].item_cnt, pba->levels[loop].items[0].itemid, pba->levels[loop].items[0].rate);
		//}
	//}
}

int load_beasts(xmlNodePtr cur)
{
	//memset(beasts_attr, 0, sizeof(beasts_attr));
	for (uint32_t i=0;i< sizeof( beasts_attr)/sizeof(beasts_attr[0] );i++  ){
		beasts_attr[i].id=0;
		beasts_attr[i].skill_cnt=0;
	}

	memset(valid_beast_cnt, 0, sizeof(valid_beast_cnt));
	memset(valid_beasts, 0, sizeof(valid_beasts));
	int id = 0;

	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Type")) { 
			DECODE_XML_PROP_INT (id, cur, "ID");
			beast_t* p = get_beast_no_check(id);
			if (!p || p->id) ERROR_RETURN(("invalid or duplicate beast id %u", id), -1);
						
			p->id = id;
			DECODE_XML_PROP_INT (p->race, cur, "Race");
			DECODE_XML_PROP_STR (p->nick, cur, "Name");
			
			DECODE_XML_PROP_INT (p->physi_factor, cur, "PhysiFactor");
			DECODE_XML_PROP_INT (p->stren_factor, cur, "StrenFactor");
			DECODE_XML_PROP_INT (p->endur_factor, cur, "EndurFactor");
			DECODE_XML_PROP_INT (p->quick_factor, cur, "QuickFactor");
			DECODE_XML_PROP_INT (p->intel_factor, cur, "IntelFactor");
			DECODE_XML_PROP_INT (p->style, cur, "Style");
			
			DECODE_XML_PROP_INT (p->earth, cur, "Earth");
			DECODE_XML_PROP_INT (p->water, cur, "Water");
			DECODE_XML_PROP_INT (p->fire, cur, "Fire");
			DECODE_XML_PROP_INT (p->wind, cur, "Wind");	

			DECODE_XML_PROP_INT (p->price, cur, "Price");
			DECODE_XML_PROP_INT (p->sell_price, cur, "SellPrice");
			DECODE_XML_PROP_INT (p->tradability, cur, "Tradability");
			DECODE_XML_PROP_INT (p->vip_tradability, cur, "VipTradability");
			DECODE_XML_PROP_INT (p->handbooklv, cur, "HandbookLV");	
			DECODE_XML_PROP_INT (p->skillbar, cur, "SkillsBar");

			uint32_t factor = p->physi_factor + p->stren_factor + p->endur_factor + p->quick_factor + p->intel_factor;
			if (factor <= 105 && id < 2000) {
				valid_beasts[0][valid_beast_cnt[0] ++] = id;
			} else if (factor >= 106 && factor <= 120 && id < 2000) {
				valid_beasts[1][valid_beast_cnt[1] ++] = id;
			} else if (factor >= 121 && id < 3000) {
				valid_beasts[2][valid_beast_cnt[2] ++] = id;
			}
			
			p->spec_ccnt = decode_xml_prop_arr_int_default(p->spec_catch, MAX_SPEC_CATCH_TYPE, cur, "SpecCatch", 0);
			if (p->skillbar > MAX_SKILL_BAR)
				ERROR_RETURN(("skillbar number over max %u %u", id, p->skillbar), -1);

			xmlNodePtr chl = cur->xmlChildrenNode; 
			while(chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"TalentSkill") && load_beast_talentskill(chl, p)) {
					return -1;
				}
				
				if (!xmlStrcmp(chl->name, (const xmlChar *)"BaseSkill") && load_beast_baseskill(chl, p)) {
					return -1;
				}

				if (!xmlStrcmp(chl->name, (const xmlChar *)"SpecSkill") && load_beast_specskill(chl, p)) {
					return -1;
				}

				if (!xmlStrcmp(chl->name, (const xmlChar *)"Beast") && load_beast_lvinfo(chl, p)) {
					return -1;
				}
				chl = chl->next;
			}

			log_beast_attr(p);

			int percent = 0;
			for (int loop = 0; loop < p->skill_cnt_all; loop ++) {
				percent += p->skills[loop].percent;
			}
			if (percent != 1000) {
				ERROR_RETURN(("skill percent not 1000\t[%u %u]", p->id, percent), -1);
			}
		}
		cur = cur->next;
	}

	return 0;
}

pet_exchange_t* get_pet_exchange_info_no_check(uint32_t id)
{
	return id && id <= MAX_PET_EXCHANGE_CNT ? &pet_exchanges[id - 1] : NULL;
}

pet_exchange_egg_t* get_pet_exchange_egg_no_check(uint32_t id)
{
	return id && id <= MAX_PET_EX_EGG_CNT ? &exchange_egg[id - 1] : NULL;
}

int load_rare_beasts(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Node")) { 
			uint32_t id, can_mix;
			DECODE_XML_PROP_UINT32 (id, cur, "BeastType");
			beast_t* pba = get_beast(id);
			if (!pba) {
				ERROR_RETURN(("invalid beasttype\t[%u]", id), -1);
			}

			DECODE_XML_PROP_UINT32(can_mix, cur, "IsMixture");
			KTRACE_LOG(0, "RARE BEASTS\t[%u %u]", id, can_mix);

			pet_exchange_t* ppe = get_pet_exchange_info_no_check(id + 10000);
			if (!ppe || ppe->id) {
				ERROR_RETURN(("invalid or duplicate pet exchange\t[%u]", ppe ? ppe->id : 0), -1);
			}
			
			ppe->id = id + 10000;
			ppe->can_mix = can_mix;

			DECODE_XML_PROP_INT_DEFAULT(ppe->raretype, cur, "PetType", 0);
			
			if (can_mix) {
				ppe->getcnt = decode_xml_prop_arr_int_default((int *)ppe->gettypes, MAX_PETS_PER_SPRITE, cur, "MixturePetID", 0);
				uint32_t loop;
				for (loop = 0; loop < ppe->getcnt; loop ++) {
					ppe->getcnts[loop] = 1;
				}

				ppe->typecnt = decode_xml_prop_arr_int_default((int *)ppe->types, MAX_PETS_PER_SPRITE, cur, "MixturePets", 0);
				if (ppe->typecnt != decode_xml_prop_arr_int_default((int *)ppe->cnts, MAX_PETS_PER_SPRITE, cur, "MixturePetNums", 0))
					ERROR_RETURN(("cnt not match\t[%u %u]", id, ppe->typecnt), -1);

				int itemids[MAX_PET_EXCHANGE_ITEM], cnts[MAX_PET_EXCHANGE_ITEM];
				ppe->itemcnt = decode_xml_prop_arr_int_default(itemids, MAX_PET_EXCHANGE_ITEM, cur, "MixtureNeeds", 0);
				if (ppe->itemcnt != decode_xml_prop_arr_int_default(cnts, MAX_PETS_PER_SPRITE, cur, "MixtureNeedNums", 0))
					ERROR_RETURN(("cnt not match\t[%u %u]", id, ppe->itemcnt), -1);
				for (loop = 0; loop < ppe->itemcnt; loop ++) {
					if (!get_item(itemids[loop])) {
						ERROR_RETURN(("invalid itemid\t[%u %u]", id, itemids[loop]), -1);
					}
					ppe->items[loop].itemid = itemids[loop];
					ppe->items[loop].count = cnts[loop];
				}
			}
		}
		cur = cur->next;
	}

	return 0;
}


pet_exchange_t* get_pet_exchange_info(uint32_t id)
{
	pet_exchange_t* ppe = get_pet_exchange_info_no_check(id);
	return ppe && ppe->id ? ppe : NULL;
}

int load_pet_exchange(xmlNodePtr cur)
{
	memset(pet_exchanges, 0, sizeof(pet_exchanges));

	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"PetExchange")) {
			uint32_t id;
			DECODE_XML_PROP_INT (id, cur, "ID");
			pet_exchange_t* ppe = get_pet_exchange_info_no_check(id);
			if (!ppe || ppe->id) {
				KERROR_LOG(0, "invalid or duplicate pet exchange id\t[%u]", id);
				return -1;
			}
			
			ppe->id = id;
			ppe->can_mix = 1;
			ppe->typecnt = decode_xml_prop_arr_int_default((int *)ppe->types, MAX_PETS_PER_SPRITE, cur, "TypeList", 0);
			if (ppe->typecnt != decode_xml_prop_arr_int_default((int *)ppe->cnts, MAX_PETS_PER_SPRITE, cur, "CntList", 0)) {
				KERROR_LOG(0, "cnt not match\t[%u %u]", id, ppe->typecnt);
				return -1;
			}

			ppe->getcnt = decode_xml_prop_arr_int_default((int *)ppe->gettypes, MAX_PETS_PER_SPRITE, cur, "GetTypeList", 0);
			if (ppe->getcnt != decode_xml_prop_arr_int_default((int *)ppe->getcnts, MAX_PETS_PER_SPRITE, cur, "GetCntList", 0)) {
				KERROR_LOG(0, "cnt not match\t[%u %u]", id, ppe->getcnt);
				return -1;
			}

			uint32_t loop, allcnt = 0, getcnt = 0;
			for (loop = 0; loop < ppe->typecnt; loop ++) {
				if (!get_beast(ppe->types[loop])) {
					KERROR_LOG(0, "invalid beast id\t[%u %u]", id, ppe->types[loop]);
					return -1;
				}

				ppe->lvs[loop] = 1;
				allcnt += ppe->cnts[loop];
			}

			for (loop = 0; loop < ppe->getcnt; loop ++) {
				if (!get_beast(ppe->gettypes[loop])) {
					KERROR_LOG(0, "invalid beast id\t[%u %u]", id, ppe->gettypes[loop]);
					return -1;
				}
				getcnt += ppe->getcnts[loop];
			}

			if (allcnt <= getcnt || getcnt > MAX_PETS_PER_SPRITE) {
				KERROR_LOG(0, "cnt invalid\t[%u %u]", id, ppe->gettypes[loop]);
				return -1;
			}
		}
		cur = cur->next;
	}

	return 0;
}

int load_pet_exchange_egg(xmlNodePtr cur)
{
	memset(exchange_egg, 0, sizeof(exchange_egg));

	cur = cur->xmlChildrenNode; 
	uint32_t rate=0;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Kind")) {
			uint32_t id;
			DECODE_XML_PROP_INT (id, cur, "ID");
			//DEBUG_LOG("xxx1111111 %u");
			pet_exchange_egg_t* ppe = get_pet_exchange_egg_no_check(id);
			if (!ppe || ppe->id) {
				KERROR_LOG(0, "invalid or duplicate pet exchange id\t[%u]", id);
				return -1;
			}
			ppe->id = id;
			DECODE_XML_PROP_INT (ppe->rate, cur, "Rate");
			rate+=ppe->rate;
			xmlNodePtr  chl=cur->xmlChildrenNode;
			uint32_t cnt=0,share_rate=0;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar*)"Item")) {
					exchange_egg_info_t *peei=&ppe->egg_info[cnt];
					DECODE_XML_PROP_UINT32(peei->itemid, chl, "ID");
					DECODE_XML_PROP_UINT32(ppe->egg_rate[cnt], chl, "Rate");
					DECODE_XML_PROP_UINT32(peei->race, chl, "Race");
					share_rate += ppe->egg_rate[cnt];
					//DEBUG_LOG("xxx id=%u rate=%u cnt=%u",peei->itemid,ppe->egg_rate[cnt],cnt);
				}
				cnt++;
				if( cnt > MAX_CONTAIN_EGG_CNT ){
					ERROR_RETURN(("egg cnt max"), -1);
				}
				chl = chl->next;
			}
			ppe->egg_cnt=cnt;
			if(share_rate != 1000){
				ERROR_RETURN(("egg rate error"), -1);
			}

		}
		cur = cur->next;
	}
	if( rate != 1000){
		ERROR_RETURN(("pet echange egg rate error"), -1);
	}
	return 0;
}
/*
   gift_pet_t* get_g_book_gift_pet_nocheck(uint32_t id)
   {
   if(!id || id > sizeof(g_gook_gift_pets) / sizeof(g_gook_gift_pets[0]))
   return NULL;
   return &g_gook_gift_pets[id-1];
   }

   gift_pet_t* get_g_book_gift_pet(uint32_t id)
   {
	gift_pet_t* p = get_g_book_gift_pet_nocheck(id);
	return (p && p->id == id) ? p : NULL;
}

int load_g_book_gift_pet(xmlNodePtr cur)
{
	memset(g_gook_gift_pets, 0, sizeof(g_gook_gift_pets));

	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"GiftPet")) {
			uint32_t id;
			DECODE_XML_PROP_INT (id, cur, "ID");
			gift_pet_t* gp = get_g_book_gift_pet_nocheck(id);
			if (!gp || gp->id) {
				KERROR_LOG(0, "invalid or duplicate pet gift_pet id\t[%u]", id);
				return -1;
			}
			
			gp->id = id;
			DECODE_XML_PROP_INT(gp->pettype, cur, "PetType");
			if (!get_beast(gp->pettype)) {
				KERROR_LOG(0, "invalid beast id\t[%u %u]", id, gp->pettype);
				return -1;
			}
			gp->checkcnt = decode_xml_prop_arr_int_default((int *)gp->checktypes, MAX_PETS_PER_SPRITE, cur, "ChkList", 0);
			if (!gp->checkcnt || gp->checkcnt > MAX_PETS_PER_SPRITE) {
				KERROR_LOG(0, "cnt invalid\t[%u]", id);
				return -1;
			}
			uint32_t loop;
			for (loop = 0; loop < gp->checkcnt; loop++) {
				if (!get_beast(gp->checktypes[loop])) {
					KERROR_LOG(0, "invalid beast id\t[%u %u]", id, gp->checktypes[loop]);
					return -1;
				}
			}
		}
		cur = cur->next;
	}

	return 0;
}
*/
void calc_pet_second_level_attr(pet_t* p)
{
	int effect_ratio = 10;//get_injury_effect(p->injury_lv);
	p->hp_max 	= ROUND_1000((8 * p->physi100 + 2 * p->stren100 + 3 * p->endur100 + 3 * p->quick100 + p->intel100) * effect_ratio) + 20;
	p->mp_max 	= ROUND_1000((p->physi100 + 2 * p->stren100 + 2 * p->endur100 + 2 * p->quick100 + 10* p->intel100) * effect_ratio) + 20;
	p->attack	= ROUND_10000((2 * p->physi100 + 27 * p->stren100 + 3 * p->endur100 + 3 * p->quick100 + 2 * p->intel100) * effect_ratio) + 20;
	p->defense 	= ROUND_10000((2 * p->physi100 + 3 * p->stren100 + 30 * p->endur100 + 3 * p->quick100 + 2 * p->intel100) * effect_ratio) + 20;
	p->speed 	= ROUND_10000((p->physi100 + 2 * p->stren100 + 2 * p->endur100 + 20 * p->quick100 + p->intel100) * effect_ratio) + 20;

	p->spirit 	= ROUND_1000((int32_t)(-3 * p->physi100 - p->stren100 + 2 * p->endur100 - p->quick100 + 8 * p->intel100)) + 100;
	p->resume	= ROUND_1000((int32_t)(8 * p->physi100 - p->stren100 - p->endur100 + 2 * p->quick100 - 3 * p->intel100)) + 100;

	p->spirit = (p->spirit > 0) ? p->spirit : 1;
	p->resume = (p->resume > 0) ? p->resume : 1;

	p->hit_rate = 0;
	p->avoid_rate = 0;

//生命、攻击、防御、速度显示值:属性点换算*5 
	p->hp_max *= 5;
	p->attack *= 5;
	p->defense *= 5;
	p->speed *= 5;
}

void init_pet_skills(pet_db_t* ppd, beast_t* pba)
{
	ppd->skill_cnt = 0;
	if (pba->talent_skill_cnt > 1) {
		ppd->skills[ppd->skill_cnt].skill_id = pba->talent_skill_ids[0];
		ppd->skills[ppd->skill_cnt].skill_level = 0xFF;
		ppd->skills[ppd->skill_cnt].skill_exp = 0;
		ppd->skill_cnt ++;

		int idx = rand_type_idx(pba->talent_skill_cnt - 1, pba->talent_skill_percent + 1, RAND_COMMON_RAND_BASE);
		ppd->skills[ppd->skill_cnt].skill_id = pba->talent_skill_ids[idx + 1];
		ppd->skills[ppd->skill_cnt].skill_level = 0xFF;
		ppd->skills[ppd->skill_cnt].skill_exp = 0;
		ppd->skill_cnt ++;
	} else {
		for (int loop = 0; loop < pba->skill_cnt; loop ++) {
			if (pba->skills[loop].skillid >= PET_SPEC_SKILL_BASE_ID)
				continue;
			ppd->skills[ppd->skill_cnt].skill_id = pba->skills[loop].skillid;
			ppd->skills[ppd->skill_cnt].skill_level = 1;
			ppd->skills[ppd->skill_cnt].skill_exp = 0;
			ppd->skill_cnt ++;
		}
	}
}

int user_add_pet(sprite_t* p, uint32_t pettype, uint32_t level, int must_inbag, int growth, uint32_t rand_diff)
{
	uint32_t state = 0;
		
	beast_t* p_ba = get_beast(pettype);
	if (!p_ba) {
		KERROR_LOG(p->id, "battle add pet beast info not in xml\t[%u]", pettype);
		return -1;
	}

	if (p->pet_cnt_actual >= MAX_PETS_PER_SPRITE) {
		if (must_inbag) {
			KERROR_LOG(p->id, "pet bag full\t[%u %u]", pettype, p->pet_cnt_actual);
			return -1;
		}
		state = db_warehouse;
	} else {
		p->pet_cnt_actual ++;
		state = just_in_bag;
		if(p->pet_cnt_assist < 3) {
			state |= for_assist;
			p->pet_cnt_assist++;
		}
	}

	uint8_t out[256];
	pet_db_t* p_pd = (pet_db_t *)out;
	gen_pet_attr(p_pd, p_ba, level, growth, IS_ADMIN(p) ? 0 : rand_diff);
	p_pd->action_state = state;

	if (state == db_warehouse)
		db_set_home_flag(NULL, p->id, home_flag_pet_house, 1);

	init_pet_skills(p_pd, p_ba);

	int i = sizeof(pet_db_t) + p_pd->skill_cnt * sizeof(skill_t);

	KDEBUG_LOG(p->id, "DB ADD PET\t[id=%u %u cnt=1]", pettype, level);

	msg_log_pet_level(p_pd->level, 0);
	msg_log_pet_race(p_pd->race, 1);
	msg_log_pet_catch(pettype, level);

	p->all_petcnt ++;

	return send_request_to_db(0, p->id, proto_db_add_pet_all, out, i);
}

int cli_set_all_pet_state(sprite_t *p,Cmessage *c_in)
{
	cli_set_all_pet_state_in *p_in=P_IN;
	CHECK_SELF_BATTLE_INFO(p, p->id);
	CHECK_USER_APPLY_PK(p, p);
	uint32_t fight_cnt=0,stand_cnt=0,assist_cnt=0;
	if(p_in->pets_list.size() !=  p->pet_cnt){
		KERROR_LOG(p->id,"pet count err %u %u",p_in->pets_list.size(),p->pet_cnt);
		return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
	}
	for(uint8_t loop=0 ; loop<p_in->pets_list.size() ; loop++ ){
		if( p_in->pets_list[loop].state<1 || p_in->pets_list[loop].state>4
			   	|| !get_pet_inbag(p, p_in->pets_list[loop].petid)){
			KERROR_LOG(p->id,"pet state err %u ",p_in->pets_list[loop].state);
			return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
		}
		switch ( p_in->pets_list[loop].state){
			case rdy_fight :
				fight_cnt++;
				break;
			case on_standby:
				stand_cnt++;
				break;
			case for_assist:
				assist_cnt++;
				break;
			default :
				break;
		}
	}
	if(fight_cnt>1 || stand_cnt>2 || assist_cnt >2 ){
		KERROR_LOG(p->id,"count err %u %u %u",fight_cnt, stand_cnt,assist_cnt);
		return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
	}
	for(uint8_t loop=0 ; loop<p_in->pets_list.size() ; loop++ ){
		uint32_t state=p_in->pets_list[loop].state;
		uint32_t petid=p_in->pets_list[loop].petid;
		pet_t *p_p = get_pet_inbag(p, petid);
		if(p_p->action_state != state){
			if((state==rdy_fight || state== for_assist) && p_p->level > p->level+5){
				db_set_pet_state(p, p->id, petid,just_in_bag);
			}else{
				KDEBUG_LOG(p->id,"set all state %u %u %u",p_p->petid,p_p->action_state,state);
				p_p->action_state = state;
				db_set_pet_state(NULL, p->id, petid, state);
			}
		}
	}
	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}

int set_pet_state_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t petid, state;
	int j = 0;
	
	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, state, j);
	CHECK_SELF_BATTLE_INFO(p, petid);
	CHECK_USER_APPLY_PK(p, p);
	pet_t *p_p = get_pet_inbag(p, petid);

	if(state >= pet_state_max){
		KERROR_LOG(p->id, "invalid state: %u", state);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	KDEBUG_LOG(p->id, "SET PET ACTION START\t[%u %u]", petid, state);
	*(uint32_t *)p->session = 0;

	/* ->0 put in ware house, no check, db will do */
	/* ->1 if (0->1) need check max(6) other let db do */
	/* ->2 pet must in bag and max(3) with rdy fight*/
	/* ->3 pet must in bag and no rdy fight and max(3) with pet standby*/
	if (state == just_in_bag) {
		if (!p_p && p->pet_cnt == MAX_PETS_PER_SPRITE)
			return send_to_self_error(p, p->waitcmd, cli_err_pet_bag_full, 1);
        else if(p_p && p_p->action_state == just_in_bag)
            return send_to_self_error(p, p->waitcmd, cli_err_pet_alreay_inbag, 1);
	} else if (state == for_assist) {
		CHECK_PET_INBAG(p, p_p, petid);
		CHECK_PET_LV_FIT(p, p_p, 1, p->level + 5);
		if (p_p->action_state == for_assist || p->pet_cnt_assist >= 2) {
			KERROR_LOG(p->id, "assist cnt max\t[%u %u]", p->pet_cnt_assist, petid);
			return -1;
		}
	} else if (state == on_standby) {
		CHECK_PET_INBAG(p, p_p, petid);
		CHECK_PET_LV_FIT(p, p_p, 1, p->level + 5);

		if (p_p->action_state != rdy_fight) {	
			if (p_p->action_state == state) {
				KERROR_LOG(p->id, "pet state same\t[%u %u]", petid, state);
				return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
			}
			
			if (p->pet_cnt_standby >= MAX_PET_STANDBY)
				return send_to_self_error(p, p->waitcmd, cli_err_pet_standby_too_much, 1);
		}
	} else if (state == rdy_fight) {
		CHECK_PET_INBAG(p, p_p, petid);
		CHECK_PET_LV_FIT(p, p_p, 1, p->level + 5);
		
		if (!IS_PET_STANDBY(p_p->action_state) && p->pet_cnt_standby >= MAX_PET_STANDBY)
			return send_to_self_error(p, p->waitcmd, cli_err_pet_standby_too_much, 1);
		if (p_p->action_state == rdy_fight) {
			response_proto_uint32_uint32(p, p->waitcmd, petid, 0, 1, 0);
			return 0;
		}
		pet_t* pet_rdy_fight = get_pet_rdy_fight(p);
		if (pet_rdy_fight) {
			pet_rdy_fight->action_state = on_standby;
			db_set_pet_state(p, p->id, pet_rdy_fight->petid, on_standby);
			*(uint32_t *)p->session = pet_rdy_fight->petid;
		}
	}
	
	KDEBUG_LOG(p->id, "SET PET ACTION FIGHT\t[%u %u]", petid, state);
	
	return db_set_pet_state(p, p->id, petid, state ? state : db_warehouse);
}

int set_pet_state_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	KDEBUG_LOG(p->id,"set_pet_state_callback");	
	CHECK_DBERR(p, ret);
	pet_db_t* p_pdb = (pet_db_t *)buf;
	CHECK_BODY_LEN(len, sizeof(pet_db_t) + p_pdb->skill_cnt * sizeof(skill_t));
	update_inbag_pet(p, p_pdb);
	switch (p->waitcmd) {
		case proto_cli_set_pet_state:
			response_proto_uint32_uint32(p, p->waitcmd, p_pdb->petid, *(uint32_t *)p->session, 1, 0);
			break;
		case proto_cli_del_pet:
			response_proto_uint32_uint32(p, p->waitcmd, p_pdb->petid, 0, 1, 0);
			break;
		case cli_exchange_pet_for_item_cmd:
			if (p_pdb->pettype == *(uint32_t *)p->session) {
				uint32_t itemid = *(uint32_t *)(p->session + 4), count = 0;
				switch (itemid) {
				case item_id_xiaomee:
					count = 1000;
					db_add_xiaomee(NULL, p->id, count);
					p->xiaomee += count;
					msg_log_pet_for_xiaomee(p->id);
					break;
				case item_id_exp:
					count = 2000;
					add_exp_to_sprite(p, count);
					msg_log_pet_for_exp(p->id);
					break;
				case item_id_skill_expbox:
					count = 500;
					p->skill_expbox += count;
					db_add_skill_expbox(NULL, p->id, count);
					msg_log_pet_for_skill_exp(p->id);
					break;
				case item_id_pet_expbox:
					count = 20000;
					p->expbox += count;
					db_set_expbox(NULL, p->id, p->expbox);
					msg_log_pet_for_pet_exp(p->id);
					break;
				case item_id_vacancy:
					count = ranged_random(5, 10);
					p->vacancy_cnt += count;
					db_day_add_ssid_cnt(NULL, p->id, item_id_vacancy, count, -1);
					msg_log_vacancy(p->id, p->vacancy_cnt);
					msg_log_pet_for_vacancy(p->id);
					break;
				}

				cli_exchange_pet_for_item_out cli_out;
				cli_out.petid = p_pdb->petid;
				cli_out.itemid = itemid;
				cli_out.count = count;
				send_msg_to_self(p, p->waitcmd, &cli_out, 1);
			} else {
				KERROR_LOG(p->id, "pet type not match\t[%u %u]", p_pdb->pettype, *(uint32_t *)p->session);
				p->waitcmd = 0;
			}
			return 0;
		default:
			return -1;
	}
	return 0;
}

int del_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_USER_APPLY_PK(p, p);
    uint32_t petid;
    int j = 0;
    UNPKG_UINT32(body, petid, j);
    KDEBUG_LOG(p->id, "DEL PET\t[%u]", petid);
	KF_LOG("del_pet",p->id, "DEL PET\t[%u]", petid);
    return db_set_pet_state(p, p->id, petid, get_now_tv()->tv_sec);
}

int cli_exchange_pet_for_item(sprite_t* p, Cmessage* c_in)
{
	cli_exchange_pet_for_item_in* p_in = P_IN;
	pet_exchange_t* ppe = get_pet_exchange_info(p_in->pettype + 10000);
	if (!ppe || ppe->raretype != 1) {
		KERROR_LOG(p->id, "invalid pettype\t[%u %u %p]", p_in->petid, p_in->pettype, ppe);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_type_invalid, 1);
	}
	*(uint32_t *)p->session = p_in->pettype;
	*(uint32_t *)(p->session + 4) = p_in->itemid;
	item_t* pday = cache_get_day_limits(p, ssid_pet_for_item);
	if (pday->count >= 5) {
		KERROR_LOG(p->id, "day max\t[%u]", pday->count);
		return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
	}
	pday->count ++;
	db_day_add_ssid_cnt(NULL, p->id, ssid_pet_for_item, 1, 10);
	if (p_in->itemid == item_id_exp && p->experience > MAX_TEMP_EXP_EX - 1)
		return send_to_self_error(p, p->waitcmd, cli_err_exp_too_much, 1);
	return db_set_pet_state(p, p->id, p_in->petid, get_now_tv()->tv_sec);
}

int get_pettype_idx(pet_exchange_t* ppe, uint32_t pettype)
{
	for (uint32_t loop = 0; loop < ppe->typecnt; loop ++) {
		if (ppe->types[loop] == pettype)
			return loop;
	}
	return -1;
}

int pkg_add_pet_info(beast_t* pba, uint8_t* out, uint32_t location, uint32_t rand_diff)
{
	pet_db_t* p_pd = (pet_db_t *)out;
	gen_pet_attr(p_pd, pba, 1, 0, rand_diff);
	p_pd->action_state = location;
	//int i = sizeof(pet_db_t);

	p_pd->skill_cnt = 0;
	if (pba->talent_skill_cnt > 1) {
		p_pd->skills[p_pd->skill_cnt].skill_id = pba->talent_skill_ids[0];
		p_pd->skills[p_pd->skill_cnt].skill_level = 0xFF;
		p_pd->skills[p_pd->skill_cnt].skill_exp = 0;
		p_pd->skill_cnt ++;

		int idx = rand_type_idx(pba->talent_skill_cnt - 1, pba->talent_skill_percent + 1, RAND_COMMON_RAND_BASE);
		p_pd->skills[p_pd->skill_cnt].skill_id = pba->talent_skill_ids[idx + 1];
		p_pd->skills[p_pd->skill_cnt].skill_level = 0xFF;
		p_pd->skills[p_pd->skill_cnt].skill_exp = 0;
		p_pd->skill_cnt ++;
	} else {
		for (int loop = 0; loop < pba->skill_cnt; loop ++) {
			if (pba->skills[loop].skillid >= PET_SPEC_SKILL_BASE_ID)
				continue;
			p_pd->skills[p_pd->skill_cnt].skill_id = pba->skills[loop].skillid;
			p_pd->skills[p_pd->skill_cnt].skill_level = 1;
			p_pd->skills[p_pd->skill_cnt].skill_exp = 0;
			p_pd->skill_cnt ++;
		}
	}

	/*  
	for (int loop = 0; loop < pba->skill_cnt; loop ++) {
		if (pba->skills[loop].skillid >= PET_SPEC_SKILL_BASE_ID)
			continue;
		PKG_H_UINT32(out, pba->skills[loop].skillid, i);
		PKG_H_UINT8(out, 1, i);
		PKG_H_UINT8(out, 0xFF, i);
		PKG_H_UINT32(out, 0, i);
		p_pd->skill_cnt ++;
	}

	for (int loop = p_pd->skill_cnt; loop < MAX_SKILL_BAR; loop ++) {
		PKG_H_UINT32(out, 0, i);
		PKG_H_UINT8(out, 0, i);
		PKG_H_UINT8(out, 0, i);
		PKG_H_UINT32(out, 0, i);
	}
	*/

	msg_log_pet_level(p_pd->level, 0);
	msg_log_pet_race(p_pd->race, 1);
	msg_log_pet_catch(p_pd->pettype, p_pd->level);
	return sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t);
}

int exchange_pet_cmd(sprite_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_USER_APPLY_PK(p, p);
	uint32_t eid, count, petid, pettype;
	int j = 0;
	UNPKG_UINT32(body, eid, j);
	UNPKG_UINT32(body, count, j);
	KDEBUG_LOG(p->id, "EXCHANGE PET\t[%u %u]", eid, count);
	CHECK_BODY_LEN(bodylen, 8 + 8 * count);
	pet_exchange_t* ppe = get_pet_exchange_info(eid);
	if (!ppe || !ppe->can_mix) {
		KERROR_LOG(p->id, "exchange id invalid\t[%u %u %u]", eid, count, ppe ? ppe->typecnt : 0);
		return send_to_self_error(p, p->waitcmd, cli_err_exchange_id_invalid, 1);
	}

	int i = 0;
	uint8_t chkval[MAX_PETS_PER_SPRITE] = {0};
	uint8_t out[1024] = {0};
	PKG_H_UINT32(out, count, i);
	PKG_H_UINT32(out, ppe->getcnt, i);
	uint32_t loop;
	uint32_t petcnt = MAX_PETS_PER_SPRITE - p->pet_cnt_actual;
	for (loop = 0; loop < count; loop ++) {
		UNPKG_UINT32(body, petid, j);
		UNPKG_UINT32(body, pettype, j);
		pet_t* pp = get_pet_inbag(p, petid);
		//CHECK_PET_INBAG(p, pp, petid);
		int idx = get_pettype_idx(ppe, pettype);
		if (idx == -1) {
			return send_to_self_error(p, p->waitcmd, cli_err_pet_type_invalid, 1);
		}
		//CHECK_PET_LV_FIT(p, pp, ppe->lvs[idx], MAX_SPRITE_LEVEL);
		chkval[idx] += 1;
		PKG_H_UINT32(out, petid, i);
		PKG_H_UINT32(out, pettype, i);
		PKG_H_UINT32(out, pp ? pp->action_state : 0, i);

		if (pp)
			petcnt ++;
	}

	for (loop = 0; loop < ppe->typecnt; loop ++) {
		if (chkval[loop] != ppe->cnts[loop])
			return send_to_self_error(p, p->waitcmd, cli_err_pet_type_invalid, 1);
	}

	for (loop = 0; loop < ppe->itemcnt; loop ++) {
		CHECK_ITEM_EXIST(p, ppe->items[loop].itemid, ppe->items[loop].count);
	}

	for (loop = 0; loop < ppe->getcnt; loop ++) {
		beast_t* pba = get_beast(ppe->gettypes[loop]);
		if (!pba) {
			KERROR_LOG(p->id, "beast id invalid\t[%u]", ppe->gettypes[loop]);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		i += pkg_add_pet_info(pba, out + i, loop < petcnt ? just_in_bag : db_warehouse, IS_ADMIN(p) ? 0 : rand_diff_default);
	}

	memcpy(p->session, ppe, sizeof(pet_exchange_t));
	return send_request_to_db(p, p->id, proto_db_exchange_pet, out, i);
}

void del_improve_need_item(sprite_t* p)
{
	pet_exchange_t* ppe = (pet_exchange_t *)p->session;
	uint32_t loop;
	for (loop = 0; loop < ppe->itemcnt; loop ++) {
		cache_reduce_kind_item(p, ppe->items[loop].itemid, ppe->items[loop].count);
		db_add_item(NULL, p->id, ppe->items[loop].itemid, -ppe->items[loop].count);
	}
}

int exchange_pet_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t typecnt, getcnt;
	int j = 0;
	UNPKG_H_UINT32(buf, typecnt, j);
	UNPKG_H_UINT32(buf, getcnt, j);
	uint32_t needlen = 8 + typecnt * 12 + getcnt * (sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t));
	CHECK_BODY_LEN(len, needlen);
	p->all_petcnt += getcnt;
	p->all_petcnt -= typecnt;

	del_improve_need_item(p);

	uint32_t loop;
	for (loop = 0; loop < typecnt; loop ++) {
		uint32_t petid, location;
		UNPKG_H_UINT32(buf, petid, j);
		j += 4;
		UNPKG_H_UINT32(buf, location, j);
		if (location)
			cache_del_pet_inbag(p, petid);
	}
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, getcnt, i);
	for (loop = 0; loop < getcnt; loop ++) {
		pet_db_t* ppd = (pet_db_t *)(buf + j);
		update_inbag_pet(p, ppd);
		msg_log_pet_catch(ppd->pettype, ppd->level);
		j += sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t);
		PKG_UINT32(msg, ppd->petid, i);
		PKG_UINT32(msg, ppd->pettype, i);
		PKG_STR(msg, ppd->nick, i, USER_NICK_LEN);
		PKG_UINT32(msg, ppd->level, i);
		PKG_UINT32(msg, ppd->action_state, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}
int set_pet_nick_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint8_t buf[4 + USER_NICK_LEN];
	uint32_t petid;
	char nick[USER_NICK_LEN];
	int j = 0;
	UNPKG_UINT32(body, petid, j);
	UNPKG_STR(body, nick, j, USER_NICK_LEN);
	nick[USER_NICK_LEN - 1] = '\0';

	if (strlen(nick) == 0) {
		ERROR_RETURN(("set pet nick null %u", p->id), -1);
	}

	*(uint32_t*)buf = petid;
	memcpy(buf + 4, nick, USER_NICK_LEN);

	CHECK_DIRTYWORD(p, nick);
	KDEBUG_LOG(p->id, "SET PET NICK\t[%u %s]", petid, nick);

	send_request_to_db(p, p->id, proto_db_set_pet_nick, buf, 4 + USER_NICK_LEN);
	return 0;
}

int set_pet_nick_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t petid;
	int j = 0;
	CHECK_DBERR(p, ret);

	KDEBUG_LOG(p->id, "SET PET NICK CALLBACK\t[%u %u]", id, petid);
	
	UNPKG_H_UINT32(buf, petid, j);
	pet_t* p_p = get_pet_inbag(p, petid);
	if (p_p) {
		memcpy(p_p->nick, buf + 4, USER_NICK_LEN);
		p_p->nick[USER_NICK_LEN - 1] = '\0';
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, p_p ? p_p->petid : 0, i);
	PKG_STR(msg, p_p ? p_p->nick : p->nick, i, USER_NICK_LEN);
	init_proto_head(msg, p->waitcmd, i);
	if (p_p == p->p_pet_follow) {
		send_to_map(p, msg, i, 1,1);
	} else {
		send_to_self(p, msg, i, 1);
	}
	return 0;
}

int set_pet_follow_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t petid;
	int	j = 0;
	
	UNPKG_UINT32(body, petid, j);	
	if (petid != 0 && get_pet_inbag(p, petid) == NULL) {
		KERROR_LOG(p->id, "cli petid exception;petid\t[%u]", petid);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_not_inbag, 1);
	}
	
	*(uint32_t*)p->session = petid;

	KDEBUG_LOG(p->id, "cli set pet follow\t[%u %u]", p->waitcmd, petid);
	
	return db_set_pet_follow(p, p->id, petid);
}

int set_pet_follow_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	uint32_t petid;
	int j = 0;
	UNPKG_H_UINT32(buf, petid, j);
	
	pet_flag_unset(p->p_pet_follow, pet_follow_flag);
	p->p_pet_follow = get_pet_inbag(p, petid);
	pet_flag_set(p->p_pet_follow, pet_follow_flag);
	

	KDEBUG_LOG(p->id, "DB SET PET FOLLOW\t[%u, %u]", p->waitcmd, petid);

	notify_pet_follow(p, 1);
	
	return 0;
}

int get_pet_info_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t petid;
	int i = sizeof(protocol_t), j = 0;
	UNPKG_UINT32(body, petid, j);

	pet_t* p_p = get_pet_inbag(p, petid);
	if (p_p) {
		i += pkg_pet_info (p_p, msg + i);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}
	return send_request_to_db(p, p->id, proto_db_pet_get_info, &petid, 4);
}

int pkg_other_pet_info(pet_t* p_p, uint8_t* buf)
{
	int j = 0;
	PKG_UINT32(buf, p_p->petid, j);
	PKG_UINT32(buf, p_p->pettype, j);
	PKG_UINT8(buf, p_p->race, j);
	PKG_UINT32(buf, p_p->flag, j);
	
	PKG_STR(buf, p_p->nick, j, USER_NICK_LEN);
	PKG_UINT32(buf, p_p->level, j);
	PKG_UINT32(buf, p_p->experience, j);
	PKG_UINT32(buf, p_p->hp, j);
	PKG_UINT32(buf, p_p->mp, j);
	PKG_UINT8(buf, p_p->earth, j);
	PKG_UINT8(buf, p_p->water, j);
	PKG_UINT8(buf, p_p->fire, j);
	PKG_UINT8(buf, p_p->wind, j);

	PKG_UINT32(buf, p_p->injury_lv, j);
	PKG_UINT8(buf, p_p->action_state, j);
	
	PKG_UINT32(buf, p_p->hp_max, j);
	PKG_UINT32(buf, p_p->mp_max, j);

	uint16_t para_all = (p_p->base_attr_db.physi_para10 + p_p->base_attr_db.stren_para10 \
		+ p_p->base_attr_db.endur_para10 + p_p->base_attr_db.quick_para10 + p_p->base_attr_db.intel_para10) / 10;
	PKG_UINT16(buf, para_all, j);
	PKG_UINT32(buf, p_p->skill_cnt, j);

	return j;
}

int get_pet_info_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	pet_db_t* p_pdb = (pet_db_t*)buf;
	pet_t pet;

	uint32_t needlen = sizeof(pet_db_t) + p_pdb->skill_cnt * sizeof(skill_t);
	CHECK_BODY_LEN(len, needlen);
	update_pet_from_db(p, &pet, p_pdb, false);
	int i = sizeof(protocol_t);
	switch (p->waitcmd) {
	case proto_cli_get_pet_info:
		i += pkg_pet_info(&pet, msg + i);
		break;
	case proto_cli_get_other_pet_info:
		PKG_UINT32(msg, id, i);
		i += pkg_other_pet_info(&pet, msg + i);
		break;
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int get_other_pet_info_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t uid, petid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, petid, j);
	sprite_t* s = get_sprite(uid);
	if (s) {
		pet_t* pp = get_pet_inbag(s, petid);
		if (pp) {
			int i = sizeof(protocol_t);
			PKG_UINT32(msg, uid, i);
			i += pkg_other_pet_info(pp, msg + i);
			init_proto_head(msg, p->waitcmd, i);
			KDEBUG_LOG(p->id, "RSP OTHER PET INFO\t[%u %u]", uid, petid);
			return send_to_self(p, msg, i, 1);
		}
	}
	return send_request_to_db(p, uid, proto_db_pet_get_info, &petid, 4);
}

int get_pet_list_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	return send_request_to_db(p, p->id, proto_db_get_pet_list, 0, 0);
}

//int update_pets_inhouse(sprite_t* p, uint8_t* buf, uint32_t buflen, uint32_t pet_cnt)
//{
	//uint32_t len=0;
	//p->pets_inhouse->erase(p->pets_inhouse->begin(), p->pets_inhouse->end());
	//for (uint32_t loop = 0; loop < pet_cnt; loop++) {
		//CHECK_BODY_LEN_GE(buflen, uint32_t(len + sizeof(pet_db_t)));
		//pet_db_t* ppd = (pet_db_t *)(buf + len);
		//len += sizeof(pet_db_t);
		//CHECK_BODY_LEN_GE(buflen, uint32_t(len + ppd->skill_cnt * sizeof(skill_t)));
		//len += sizeof(skill_t) * ppd->skill_cnt;

		//pet_simple_t ps;
		//ps.race=ppd->race;
		//p->pets_inhouse->insert(std::make_pair(ppd->petid,ps));
	//}
	//return 0;
//}

int get_pet_list_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t pet_cnt;
	int i = 0, j = 0, k = sizeof(protocol_t);

	CHECK_DBERR(p, ret);

	UNPKG_H_UINT32(buf, pet_cnt, j);
	KDEBUG_LOG(p->id, "GET PETALL LIST CALLBACK[%u]", pet_cnt);

	update_user_pets(p, buf + 4, len - 4, pet_cnt);
	//update_pets_inhouse(p, buf + 4, len - 4, pet_cnt);

	PKG_UINT32(msg, p->pet_cnt,k);

	for (i = 0; i < p->pet_cnt; i ++) {
		k += pkg_pet_info(&p->pets_inbag[i], msg + k);
	}

	init_proto_head(msg, p->waitcmd, k);

	KDEBUG_LOG(p->id, "11111 GET PETALL LIST CALLBACK[%u][%u]", pet_cnt,p->waitcmd );
	return send_to_self(p, msg, k, 1);
}

int get_user_pet_list_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t uid = 0;
	UNPKG_UINT32(body, uid, j);
	sprite_t* lp =  get_sprite(uid);
	if (!lp) {
		KERROR_LOG(p->id, "user offline %u", uid);
		return send_to_self_error(p, p->waitcmd, cli_err_user_offline, 1);
	}
	
	if (!p->btr_team || !lp->btr_team || p->btr_team->teamid != lp->btr_team->teamid) {
		return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_team, 1);
	}

	int i = sizeof(protocol_t), loop;
	PKG_UINT32(msg, lp->id, i);
	PKG_UINT32(msg, lp->pet_cnt, i);
	for (loop = 0; loop < lp->pet_cnt; loop ++) {
		i += pkg_pet_info(&lp->pets_inbag[loop], msg + i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int get_pet_inhouse_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t range[2] = {0,910};

	UNPKG_UINT32(body, range[0], j);
	UNPKG_UINT32(body, range[1], j);

	if(range[1] == 0 || range[1] > 910) range[1] = 910;

	return send_request_to_db(p, p->id, proto_db_get_pet_inhouse, range, 8);
}

int get_all_petcnt_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	response_proto_uint32(p, p->waitcmd, p->all_petcnt - p->pet_cnt_actual, 1, 0);
	return 0;
}

typedef struct house_pet {
	uint32_t	petid;
	uint32_t	pettype;
	uint8_t		race;
	char		nick[USER_NICK_LEN];
	uint32_t	level;
	uint32_t	location;
}__attribute__((packed))house_pet_t;

int get_pet_inhouse_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int j = 0;
	uint32_t start;
	uint32_t pet_cnt;
	uint32_t total;
	UNPKG_H_UINT32(buf, start, j);
	UNPKG_H_UINT32(buf, pet_cnt, j);
	UNPKG_H_UINT32(buf, total, j);
	KDEBUG_LOG(p->id, "PET INHOUSE\t[%u] range(%u,%u)", total,start,pet_cnt);
	CHECK_BODY_LEN(len, pet_cnt * sizeof(house_pet_t) + j);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, total, i);
	PKG_UINT32(msg, start, i);
	PKG_UINT32(msg, pet_cnt, i);
	db_set_home_flag(NULL, p->id, home_flag_pet_house, 0);
	uint32_t loop;

	//p->pets_inhouse->erase(p->pets_inhouse->begin(), p->pets_inhouse->end());
	for (loop = 0; loop < pet_cnt; loop ++) {
		house_pet_t* php = (house_pet_t*)(buf + j + loop * sizeof(house_pet_t));
		PKG_UINT32(msg, php->petid, i);
		PKG_UINT32(msg, php->pettype, i);
		PKG_UINT8(msg, php->race, i);
		PKG_STR(msg, php->nick, i, USER_NICK_LEN);
		PKG_UINT32(msg, php->level, i);
		PKG_UINT32(msg, php->location, i);
		std::map<uint32_t,pet_simple_t >::iterator it=p->pets_inhouse->find(php->petid);
		if( it == p->pets_inhouse->end()){
			//if not exist then insert
			pet_simple_t ps;
			ps.race=php->race;
			ps.level=php->level;
			p->pets_inhouse->insert(std::make_pair(php->petid,ps));
		}else{//if exist then update
			it->second.race=php->race;
			it->second.level=php->level;
		}
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int update_pet_location_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_USER_APPLY_PK(p, p);
	uint32_t petid1, petid2, local1, local2;
	uint8_t buf[32];
	int i = 0, j = 0;
	UNPKG_UINT32(body, petid1, j);
	UNPKG_UINT32(body, local1, j);
	UNPKG_UINT32(body, petid2, j);
	UNPKG_UINT32(body, local2, j);

	if (get_pet_inbag(p, petid1) || get_pet_inbag(p, petid2))
		return send_to_self_error(p, p->waitcmd, cli_err_pet_alreay_inbag, 1);

	local1 = !local1 ? db_warehouse : local1;
	local2 = !local2 ? db_warehouse : local2;

	if (local1 < db_warehouse || local2 < db_warehouse) {
		KERROR_LOG(p->id, "invalid pet location\t[%u %u]", local1, local2);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	KDEBUG_LOG(p->id, "UPDATE PET LOCATION\t[%u %u %u %u]", petid1, local1, petid2, local2);
	PKG_H_UINT32(buf, petid1, i);
	PKG_H_UINT32(buf, local1, i);
	PKG_H_UINT32(buf, petid2, i);
	PKG_H_UINT32(buf, local2, i);
	return send_request_to_db(p, p->id, proto_db_update_pet_location, buf, i);
}

int cure_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t petid;
	int i = 0, j = 0;
	UNPKG_UINT32(body, petid, j);

	/*
	if (!ISVIP(p->flag) && p->level > 15 && MAP_HAS_BEAST(p->tiles)) {
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	*/

	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	uint8_t out[32];

	uint32_t xiaomee = (100 - p_p->level) / 4 + p_p->level - 20;
	CHECK_XIAOMEE_ENOUGH(p, xiaomee);

	if (p_p->hp >= p_p->hp_max && p_p->mp >= p_p->mp_max)
		return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);

	xiaomee = 0 - xiaomee;

	PKG_H_UINT32(out, petid, i);
	PKG_H_UINT32(out, p_p->hp_max, i);
	PKG_H_UINT32(out, p_p->mp_max, i);
	PKG_H_UINT32(out, xiaomee, i);

	return send_request_to_db(p, p->id, proto_db_pet_cure,out, i);
}

int cure_pet_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t petid, xiaomee, hp, mp;
	int32_t add_value;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, petid, j);
	UNPKG_H_UINT32(buf, hp, j);
	UNPKG_H_UINT32(buf, mp, j);
	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, add_value, j);

	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	p_p->hp = hp;
	p_p->mp = mp;

	KDEBUG_LOG(p->id, "CURE PET CALLBACK\t[%u %u %u %u %d]", petid, hp, mp, xiaomee, add_value);

	p->xiaomee = xiaomee;
	monitor_sprite(p, "CURE PET");

	PKG_UINT32(msg, petid, i);
	PKG_UINT32(msg, hp, i);
	PKG_UINT32(msg, mp, i);
	PKG_UINT32(msg, xiaomee, i);
	PKG_UINT32(msg, -add_value, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int cure_pet_injury_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t petid;
	int i = 0, j = 0;
	UNPKG_UINT32(body, petid, j);

	/*
	if (!ISVIP(p->flag) && p->level > 15 && MAP_HAS_BEAST(p->tiles)) {
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	*/

	KDEBUG_LOG(p->id, "CURE PET INJURY\t[%u]", petid);
	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	uint8_t out[32];

	uint32_t xiaomee = ((100 - p_p->level) + (p_p->level - 20 ) * 4) * (p_p->injury_lv + 1) / 4;
	if (!p_p->injury_lv)
		return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
	CHECK_XIAOMEE_ENOUGH(p, xiaomee);

	xiaomee = 0 - xiaomee;

	PKG_H_UINT32(out, petid, i);
	PKG_H_UINT32(out, xiaomee, i);

	return send_request_to_db(p, p->id, proto_db_pet_cure_injury, out, i);
}

int cure_pet_injury_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t petid, xiaomee;
	int32_t add_value;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, petid, j);
	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, add_value, j);

	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	p_p->injury_lv = 0;
	calc_pet_second_level_attr(p_p);

	KDEBUG_LOG(p->id, "CURE PET INJURY CALLBACK[%u %u %d]", petid, xiaomee, add_value);

	p->xiaomee = xiaomee;
	monitor_sprite(p, "CURE PET INJURY");

	PKG_UINT32(msg, petid, i);
	PKG_UINT32(msg, p_p->injury_lv, i);
	PKG_UINT32(msg, p_p->hp_max, i);
	PKG_UINT32(msg, p_p->mp_max, i);
	PKG_UINT32(msg, xiaomee, i);
	PKG_UINT32(msg, -add_value, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int add_pet_base_attr_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t petid;
	uint16_t attr[5], attr_add_all = 0, attr_all;
	int j = 0, i;
	uint8_t out[64];
	UNPKG_UINT32(body, petid, j);
	for (i = 0; i < 5; i ++) {
		UNPKG_UINT16(body, attr[i], j);
		attr_add_all += attr[i];
	}
	KDEBUG_LOG(p->id, "ADD PET BASE ATTR\t[%u %u %u %u %u %u]", petid, attr[0], attr[1], attr[2], attr[3], attr[4]);

	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	if (attr_add_all > p_p->attr_addition) {
		KERROR_LOG(p->id, "attr add not enough\t[%u %u %u]", petid, attr_add_all, p_p->attr_addition);
		return send_to_self_error(p, p->waitcmd, cli_err_attr_invalid, 1);
	}

	attr_all = attr_add_all + ROUND_100(p_p->physi100) + ROUND_100(p_p->endur100) \
		+ ROUND_100(p_p->stren100) + ROUND_100(p_p->quick100) + ROUND_100(p_p->intel100);
	if ((ROUND_100(p_p->physi100) + attr[0]) * 2 > attr_all \
		|| (ROUND_100(p_p->stren100) + attr[1]) * 2 > attr_all \
		|| (ROUND_100(p_p->endur100) + attr[2]) * 2 > attr_all \
		|| (ROUND_100(p_p->quick100) + attr[3]) * 2 > attr_all \
		|| (ROUND_100(p_p->intel100) + attr[4]) * 2 > attr_all) {
		return send_to_self_error(p, p->waitcmd, cli_err_attr_invalid, 1);
	}

	j = 0;
	PKG_H_UINT32(out, petid, j);
	PKG_H_UINT16(out, p_p->base_attr_db.physi_add + attr[0], j);
	PKG_H_UINT16(out, p_p->base_attr_db.stren_add + attr[1], j);
	PKG_H_UINT16(out, p_p->base_attr_db.endur_add + attr[2], j);
	PKG_H_UINT16(out, p_p->base_attr_db.quick_add + attr[3], j);
	PKG_H_UINT16(out, p_p->base_attr_db.intel_add + attr[4], j);
	PKG_H_UINT16(out, p_p->attr_addition - attr_add_all, j);

	return send_request_to_db(p, p->id, proto_db_pet_add_base_attr, out, j);
}

int recommend_pet_add_attr(sprite_t* p, Cmessage* c_in)
{
	typedef struct pet_add_attr {
		uint32_t		petid;
		int16_t			physi_add;
		int16_t		stren_add;
		int16_t		endur_add;
		int16_t		quick_add;
		int16_t		intel_add;
		uint16_t		attr_addition;
	}__attribute__((packed)) pet_add_attr_t;
	recommend_pet_add_attr_in* p_in = P_IN;
	pet_t* pp = get_pet_inbag(p, p_in->petid);
	CHECK_PET_INBAG(p, pp, p_in->petid);
	if (!pp->attr_addition) {
		KERROR_LOG(p->id, "attr add 0");
		//return -1;
		return send_to_self_error(p, p->waitcmd, cli_err_attr_invalid, 1);
	}

	beast_t* pb = get_beast(pp->pettype);
	if (!pb) {
		KERROR_LOG(p->id, "invalid beast type\t[%u]", pp->pettype);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	pet_add_attr_t attr;
	attr.petid = p_in->petid;
	attr.quick_add = pp->base_attr_db.quick_add;
	attr.attr_addition = 0;
	uint32_t single_max;

	switch (pb->style) {
		case 1://物理系
			attr.endur_add = pp->base_attr_db.endur_add;
			attr.intel_add = pp->base_attr_db.intel_add;
			single_max = ROUND_100(pp->physi100) + ROUND_100(pp->endur100) + ROUND_100(pp->quick100) + ROUND_100(pp->intel100);
			if (ROUND_100(pp->stren100) + pp->attr_addition <= single_max) {
				attr.stren_add = pp->base_attr_db.stren_add + pp->attr_addition;
				attr.physi_add = pp->base_attr_db.physi_add;
			} else if (ROUND_100(pp->stren100) < single_max){
				uint32_t stren_add = single_max - ROUND_100(pp->stren100);
				stren_add = stren_add + (pp->attr_addition - stren_add) / 2;
				attr.stren_add = pp->base_attr_db.stren_add + stren_add;
				attr.physi_add = pp->base_attr_db.physi_add + pp->attr_addition - stren_add;
			} else {
				attr.stren_add = pp->base_attr_db.stren_add;
				attr.physi_add = pp->base_attr_db.physi_add + pp->attr_addition;
			}
			break;
		case 2://魔法系
			attr.stren_add = pp->base_attr_db.stren_add;
			attr.physi_add = pp->base_attr_db.physi_add;
			single_max = ROUND_100(pp->physi100) + ROUND_100(pp->endur100) + ROUND_100(pp->quick100) + ROUND_100(pp->stren100);
			if (ROUND_100(pp->stren100) + pp->attr_addition <= single_max) {
				attr.intel_add = pp->base_attr_db.intel_add + pp->attr_addition;
				attr.endur_add = pp->base_attr_db.endur_add;
			} else if (ROUND_100(pp->stren100) < single_max){
				uint32_t intel_add = single_max - ROUND_100(pp->stren100);
				intel_add = intel_add + (pp->attr_addition - intel_add) / 2;
				attr.intel_add = pp->base_attr_db.intel_add + intel_add;
				attr.endur_add = pp->base_attr_db.endur_add + pp->attr_addition - intel_add;
			} else {
				attr.intel_add = pp->base_attr_db.intel_add;
				attr.endur_add = pp->base_attr_db.endur_add + pp->attr_addition;
			}
			break;
		default:
			KERROR_LOG(p->id, "invalid pet style\t[%u]", pb->style);
			return -1;
	}
	return send_request_to_db(p, p->id, proto_db_pet_add_base_attr, &attr, sizeof(attr));
}


void init_cli_pet_info(pet_t* pp, stru_cli_pet_info* ppi)
{
	ppi->petid			= pp->petid;
	ppi->pettype		= pp->pettype;
	ppi->race			= pp->race;
	ppi->flag			= pp->flag;
	memcpy(ppi->nick, pp->nick, USER_NICK_LEN);
	ppi->level			= pp->level;
	ppi->experience		= pp->experience;
	ppi->physique		= ROUND_100(pp->physi100);
	ppi->strength		= ROUND_100(pp->stren100);
	ppi->endurance		= ROUND_100(pp->endur100);
	ppi->quick			= ROUND_100(pp->quick100);
	ppi->intelligence	= ROUND_100(pp->intel100);
	ppi->attr_addition	= pp->attr_addition;
	ppi->attr_add		= pp->base_attr_db.physi_add + pp->base_attr_db.endur_add
		+ pp->base_attr_db.stren_add + pp->base_attr_db.quick_add + pp->base_attr_db.intel_add;
	ppi->hp				= pp->hp;
	ppi->mp				= pp->mp;
	ppi->earth			= pp->earth;
	ppi->water			= pp->water;
	ppi->fire			= pp->fire;
	ppi->wind			= pp->wind;
	ppi->injurylv		= pp->injury_lv;
	ppi->state			= pp->action_state;
	ppi->hpmax			= pp->hp_max;
	ppi->mpmax			= pp->mp_max;
	ppi->attack			= pp->attack;
	ppi->defense		= pp->defense;
	ppi->speed			= pp->speed;
	ppi->spirit			= pp->spirit;
	ppi->resume			= pp->resume;
	ppi->hit			= pp->hit_rate;
	ppi->avoid			= pp->avoid_rate;
	ppi->bisha			= pp->bisha;
	ppi->fightback		= pp->fight_back;
	ppi->para_all		= (pp->base_attr_db.physi_para10 + pp->base_attr_db.stren_para10
		+ pp->base_attr_db.endur_para10 + pp->base_attr_db.quick_para10 + pp->base_attr_db.intel_para10) / 10;

	for (int loop = 0; loop < pp->skill_cnt; loop ++) {
		stru_user_skill petskill;
		petskill.skillid = pp->skills[loop].skill_id;
		petskill.skilllv = pp->skills[loop].skill_level;
		petskill.skillexp = pp->skills[loop].skill_exp;
		ppi->skills.push_back(petskill);
	}

}

int add_pet_base_attr_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int j = 0;
	uint32_t petid;
	UNPKG_H_UINT32(buf, petid, j);
	pet_t* p_p = get_pet_inbag(p, petid);
	CHECK_PET_INBAG(p, p_p, petid);

	UNPKG_H_UINT16(buf, p_p->base_attr_db.physi_add, j);
	UNPKG_H_UINT16(buf, p_p->base_attr_db.stren_add, j);
	UNPKG_H_UINT16(buf, p_p->base_attr_db.endur_add, j);
	UNPKG_H_UINT16(buf, p_p->base_attr_db.quick_add, j);
	UNPKG_H_UINT16(buf, p_p->base_attr_db.intel_add, j);
	UNPKG_H_UINT16(buf, p_p->attr_addition, j);

	calc_pet_5attr(p_p);
	calc_pet_second_level_attr(p_p);

	reset_pet_add_attr_out out;	
	init_cli_pet_info(p_p, &out.pet);
	KDEBUG_LOG(p->id, "PET ATTR BACK\t[%u %u]", p_p->petid, p_p->attr_addition);
	
	return send_msg_to_self(p, p->waitcmd, &out, 1);

}

uint16_t modify_attr_factor(uint32_t rand_diff, uint32_t& low_cnt, uint32_t& over_val, uint32_t factor, uint16_t baseval, uint16_t addval, uint32_t& left)
{
	uint32_t max = (factor + 10 - rand_diff) * 10;
	if (addval + baseval > max) {
		over_val += addval + baseval - max;
		baseval = max;
		low_cnt --;
		left = 0;
	} else {
		baseval	+= addval;
		left = 1;
	}
	return baseval;
}

uint16_t add_over_val(uint16_t baseval, uint32_t avg, uint32_t& mod, uint32_t left)
{
	if (left) {
		baseval += avg + mod;
		mod = 0;
	}
	return baseval;
}

void rand_pet_with_item(mole2_set_pet_attributes_in* p_pd, beast_t* pb, uint32_t rand_diff, int func)
{
	uint32_t physi_para10 = 	calc_pet_attr_para10(pb->physi_factor, rand_diff);
	uint32_t stren_para10 = 	calc_pet_attr_para10(pb->stren_factor, rand_diff);
	uint32_t endur_para10 = 	calc_pet_attr_para10(pb->endur_factor, rand_diff);
	uint32_t quick_para10 = 	calc_pet_attr_para10(pb->quick_factor, rand_diff);
	uint32_t intel_para10 = 	calc_pet_attr_para10(pb->intel_factor, rand_diff);

	KDEBUG_LOG(0, "XXXXX\t[%u %u %u %u %u %u]", rand_diff, physi_para10, stren_para10, endur_para10, quick_para10, intel_para10);

	// total_para must not greater than total_star
	uint16_t total_para10 = physi_para10 + stren_para10 + endur_para10 + quick_para10 + intel_para10;
	uint16_t total_factor = pb->physi_factor + pb->stren_factor + pb->endur_factor + pb->quick_factor + pb->intel_factor;

	if (total_para10 > 10 * total_factor) {
		uint16_t a = (total_para10 - 10 * total_factor) / 5;
		physi_para10 = physi_para10 > a ? (physi_para10 - a) : 0;
		stren_para10 = stren_para10 > a ? (stren_para10 - a) : 0;
		endur_para10 = endur_para10 > a ? (endur_para10 - a) : 0;
		quick_para10 = quick_para10 > a ? (quick_para10 - a) : 0;
		intel_para10 = intel_para10 > a ? (intel_para10 - a) : 0;
	}

	p_pd->physiqueinit	= 	physi_para10 * 2;
	p_pd->strengthinit	= 	stren_para10 * 2;
	p_pd->enduranceinit = 	endur_para10 * 2;
	p_pd->quickinit		= 	quick_para10 * 2;
	p_pd->iqinit 		= 	intel_para10 * 2;

	switch(func) {
		case item_for_pet_param:
			p_pd->physiqueparam	= 	physi_para10;
			p_pd->strengthparam	= 	stren_para10;
			p_pd->enduranceparam= 	endur_para10;
			p_pd->quickparam	= 	quick_para10;
			p_pd->iqparam		= 	intel_para10;
			break;
		case item_for_pet_init:
			total_para10 = p_pd->physiqueparam + p_pd->strengthparam + p_pd->enduranceparam + p_pd->quickparam + p_pd->iqparam;
			total_factor = physi_para10 + stren_para10 + endur_para10 + quick_para10 + intel_para10;
			
			if (total_para10 > total_factor) {
				uint16_t a = (total_para10 - total_factor) / 5;
				uint32_t low_cnt = 5, over_val = 0, physique, strength, endurance, quick, iq;
				p_pd->physiqueparam = modify_attr_factor(rand_diff, low_cnt, over_val, pb->physi_factor, physi_para10, a, physique);
				p_pd->strengthparam = modify_attr_factor(rand_diff, low_cnt, over_val, pb->stren_factor, stren_para10, a, strength);
				p_pd->enduranceparam= modify_attr_factor(rand_diff, low_cnt, over_val, pb->endur_factor, endur_para10, a, endurance);
				p_pd->quickparam	= modify_attr_factor(rand_diff, low_cnt, over_val, pb->quick_factor, quick_para10, a, quick);
				p_pd->iqparam	  	= modify_attr_factor(rand_diff, low_cnt, over_val, pb->intel_factor, intel_para10, a, iq);

				if (over_val && low_cnt) {
					uint32_t avg_val = over_val / low_cnt, mod_val = over_val % low_cnt;
					p_pd->physiqueparam = add_over_val(p_pd->physiqueparam, avg_val, mod_val, physique);
					p_pd->strengthparam = add_over_val(p_pd->strengthparam, avg_val, mod_val, strength);
					p_pd->enduranceparam = add_over_val(p_pd->enduranceparam, avg_val, mod_val, endurance);
					p_pd->quickparam = add_over_val(p_pd->quickparam, avg_val, mod_val, quick);
					p_pd->iqparam = add_over_val(p_pd->iqparam, avg_val, mod_val, iq);
				}
			} else if (total_para10 <= total_factor) {
				uint16_t a = (total_factor - total_para10) / 5;
				p_pd->physiqueparam = physi_para10 > a ? (physi_para10 - a) : 0;
				p_pd->strengthparam = stren_para10 > a ? (stren_para10 - a) : 0;
				p_pd->enduranceparam = endur_para10 > a ? (endur_para10 - a) : 0;
				p_pd->quickparam = quick_para10 > a ? (quick_para10 - a) : 0;
				p_pd->iqparam = intel_para10 > a ? (intel_para10 - a) : 0;
				KDEBUG_LOG(0, "YYYYY");
			}

			p_pd->physiqueinit	= 	p_pd->physiqueparam * 2;
			p_pd->strengthinit	= 	p_pd->strengthparam * 2;
			p_pd->enduranceinit	= 	p_pd->enduranceparam * 2;
			p_pd->quickinit		= 	p_pd->quickparam * 2;
			p_pd->iqinit		= 	p_pd->iqparam * 2;
			break;
	}
}


int cli_reset_pet(sprite_t* p, Cmessage* c_in)
{
	cli_reset_pet_in* p_in = P_IN;
	pet_t* p_p = get_pet_inbag(p, p_in->petid);
	KDEBUG_LOG(p->id, "RESET PET\t[%u %u]", p_in->petid, p_in->itemid);
	CHECK_PET_INBAG(p, p_p, p_in->petid);
	normal_item_t* pni = get_item(p_in->itemid);
	CHECK_ITEM_VALID(p, pni, p_in->itemid);
	CHECK_ITEM_EXIST(p, p_in->itemid, 1);

	if (!p_p->flag & pet_1_lv_flag) {
		KERROR_LOG(p->id, "pet can not reset\t[%u]", p_p->petid);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_cannt_reset, 1);
	}

	beast_t* pb = get_beast(p_p->pettype);
	if (!pb) {
		KERROR_LOG(p->id, "no such pet in xml\t[%u]", p_p->pettype);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	*(uint32_t *)p->session = p_in->itemid;
	*(pet_t* *)(p->session + 4) = p_p;

	mole2_set_pet_attributes_in db_in;
	db_in.petid = p_p->petid;
	db_in.level = p_p->level;
	db_in.exp = p_p->experience;
	db_in.physiqueadd = p_p->base_attr_db.physi_add;
	db_in.strengthadd = p_p->base_attr_db.stren_add;
	db_in.enduranceadd = p_p->base_attr_db.endur_add;
	db_in.quickadd = p_p->base_attr_db.quick_add;
	db_in.iqadd = p_p->base_attr_db.intel_add;
	db_in.attradd = p_p->attr_addition;
	db_in.hp = p_p->hp;
	db_in.mp = p_p->mp;
	switch (pni->function) {
		case item_for_pet_init:
			db_in.physiqueparam = p_p->base_attr_db.physi_para10;
			db_in.strengthparam = p_p->base_attr_db.stren_para10;
			db_in.enduranceparam = p_p->base_attr_db.endur_para10;
			db_in.quickparam = p_p->base_attr_db.quick_para10;
			db_in.iqparam = p_p->base_attr_db.intel_para10;
			break;
		case item_for_pet_param:
			break;
		default:
			return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}
	
	rand_pet_with_item(&db_in, pb, rand_diff_default, pni->function);
	return send_msg_to_db(p, p->id, mole2_set_pet_attributes_cmd, &db_in);
}

int mole2_set_pet_attributes(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_set_pet_attributes_out* p_out = P_OUT;


	pet_t* p_p = *(pet_t* *)(p->session + 4);

	p_p->base_attr_db.physi_para10	= p_out->physiqueparam;
	p_p->base_attr_db.stren_para10	= p_out->strengthparam;
	p_p->base_attr_db.endur_para10	= p_out->enduranceparam;
	p_p->base_attr_db.quick_para10	= p_out->quickparam;
	p_p->base_attr_db.intel_para10	= p_out->iqparam;
	p_p->base_attr_db.physi_init100	= p_out->physiqueinit;
	p_p->base_attr_db.stren_init100	= p_out->strengthinit;
	p_p->base_attr_db.endur_init100	= p_out->enduranceinit;
	p_p->base_attr_db.quick_init100	= p_out->quickinit;
	p_p->base_attr_db.intel_init100	= p_out->iqinit;
	p_p->base_attr_db.physi_add		= p_out->physiqueadd;
	p_p->base_attr_db.stren_add		= p_out->strengthadd;
	p_p->base_attr_db.endur_add		= p_out->enduranceadd;
	p_p->base_attr_db.quick_add		= p_out->quickadd;
	p_p->base_attr_db.intel_add		= p_out->iqadd;

	p_p->attr_addition = p_out->attradd;
	p_p->level = p_out->level;
	p_p->experience = p_out->exp;

	calc_pet_5attr(p_p);
	calc_pet_second_level_attr(p_p);
	KDEBUG_LOG(p->id , "PET HP MP\t[%u %u]", p_p->hp, p_p->hp_max);
	p_p->hp = p_p->hp > p_p->hp_max ? p_p->hp_max : p_p->hp;
	p_p->mp = p_p->mp > p_p->mp_max ? p_p->mp_max : p_p->mp;

	uint32_t itemid = *(uint32_t *)p->session;
	if(itemid){
		cache_reduce_kind_item(p, itemid, 1);
		db_add_item(NULL, p->id, itemid, -1);
	}

	cli_reset_pet_out cli_out;
	init_cli_pet_info(p_p, &cli_out.pet);

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int reset_pet_add_attr(sprite_t *p,  Cmessage * c_in)
{
	reset_pet_add_attr_in* p_in = P_IN;	
	pet_t* pp = get_pet_inbag(p, p_in->petid);
	CHECK_PET_INBAG(p, pp, p_in->petid);
	normal_item_t* pni = get_item(p_in->itemid);
	CHECK_ITEM_VALID(p, pni, p_in->itemid);
	CHECK_ITEM_FUNC(p, pni, item_for_reset_attr);
	if (pni->target != item_target_pet_self) {
		KERROR_LOG(p->id, "item target err\t[%u]", pni->target);
		return -1;
	}
	CHECK_ITEM_EXIST(p, p_in->itemid, 1);
	CHECK_PET_LV_FIT(p, pp, pni->minlv, pni->maxlv);
	uint8_t out[64];
	int j = 0;
	PKG_H_UINT32(out, pp->petid, j);
	PKG_H_UINT16(out, 0, j);
	PKG_H_UINT16(out, 0, j);
	PKG_H_UINT16(out, 0, j);
	PKG_H_UINT16(out, 0, j);
	PKG_H_UINT16(out, 0, j);
	uint16_t attr_addition = pp->base_attr_db.physi_add
							 + pp->base_attr_db.endur_add
							 + pp->base_attr_db.stren_add
							 + pp->base_attr_db.quick_add
							 + pp->base_attr_db.intel_add
							 + pp->attr_addition;

	if (attr_addition == pp->attr_addition) 
		return send_to_self_error(p, p->waitcmd, cli_err_not_need_reset, 1);
	PKG_H_UINT16(out, attr_addition, j);

	cache_reduce_kind_item(p, p_in->itemid, 1);
	db_add_item(NULL, p->id, p_in->itemid, -1);

	return send_request_to_db(p, p->id, proto_db_pet_add_base_attr, out, j);
}

int db_add_pet_exp_level(sprite_t* p, pet_t* pp)
{
	uint8_t out[64];
	int i = 0;
	PKG_H_UINT32(out, pp->petid, i);
	PKG_H_UINT32(out, pp->experience, i);
	PKG_H_UINT32(out, pp->level, i);
	PKG_H_UINT32(out, pp->attr_addition, i);
	PKG_H_UINT32(out, pp->hp, i);
	PKG_H_UINT32(out, pp->mp, i);

	return send_request_to_db(0, p->id, proto_db_pet_levelup, out, i);
}

void calc_pet_5attr(pet_t* pet) 
{
	pet->physi100 = pet->base_attr_db.physi_para10 * (pet->level - 1) * 10 / MAGIC_GROW_RATE \
		+ pet->base_attr_db.physi_init100 + pet->base_attr_db.physi_add * 100;
	pet->stren100 = pet->base_attr_db.stren_para10 * (pet->level - 1) * 10 / MAGIC_GROW_RATE \
		+ pet->base_attr_db.stren_init100 + pet->base_attr_db.stren_add * 100;
	pet->endur100 = pet->base_attr_db.endur_para10 * (pet->level - 1) * 10 / MAGIC_GROW_RATE \
		+ pet->base_attr_db.endur_init100 + pet->base_attr_db.endur_add * 100;
	pet->quick100 = pet->base_attr_db.quick_para10 * (pet->level - 1) * 10 / MAGIC_GROW_RATE \
		+ pet->base_attr_db.quick_init100 + pet->base_attr_db.quick_add * 100;
	pet->intel100 = pet->base_attr_db.intel_para10 * (pet->level - 1) * 10 / MAGIC_GROW_RATE \
		+ pet->base_attr_db.intel_init100 + pet->base_attr_db.intel_add * 100;
}

int pkg_pet_info(pet_t* p_p, uint8_t* buf)
{
	int j = 0;
	PKG_UINT32(buf, p_p->petid, j);
	PKG_UINT32(buf, p_p->pettype, j);
	PKG_UINT8(buf, p_p->race, j);
	PKG_UINT32(buf, p_p->flag, j);
	
	PKG_STR(buf, p_p->nick, j, USER_NICK_LEN);
	PKG_UINT32(buf, p_p->level, j);
	PKG_UINT32(buf, p_p->experience, j);
	PKG_UINT16(buf, ROUND_100(p_p->physi100), j);
	PKG_UINT16(buf, ROUND_100(p_p->stren100), j);
	PKG_UINT16(buf, ROUND_100(p_p->endur100), j);
	PKG_UINT16(buf, ROUND_100(p_p->quick100), j);
	PKG_UINT16(buf, ROUND_100(p_p->intel100), j);
	PKG_UINT16(buf, p_p->attr_addition, j);
	uint16_t attr_add = p_p->base_attr_db.physi_add
		+ p_p->base_attr_db.endur_add
		+ p_p->base_attr_db.stren_add
		+ p_p->base_attr_db.quick_add
		+ p_p->base_attr_db.intel_add;
	PKG_UINT16(buf, attr_add, j);

	PKG_UINT32(buf, p_p->hp, j);
	PKG_UINT32(buf, p_p->mp, j);
	PKG_UINT8(buf, p_p->earth, j);
	PKG_UINT8(buf, p_p->water, j);
	PKG_UINT8(buf, p_p->fire, j);
	PKG_UINT8(buf, p_p->wind, j);

	PKG_UINT32(buf, p_p->injury_lv, j);
	PKG_UINT8(buf, p_p->action_state, j);
	
	PKG_UINT32(buf, p_p->hp_max, j);
	PKG_UINT32(buf, p_p->mp_max, j);
	PKG_UINT16(buf, p_p->attack, j);
	PKG_UINT16(buf, p_p->defense, j);
	PKG_UINT16(buf, p_p->speed, j);
	PKG_UINT16(buf, p_p->spirit, j);
	PKG_UINT16(buf, p_p->resume, j);
	PKG_UINT16(buf, p_p->hit_rate, j);
	PKG_UINT16(buf, p_p->avoid_rate, j);
	PKG_UINT16(buf, p_p->bisha, j);
	PKG_UINT16(buf, p_p->fight_back, j);

	uint16_t para_all = (p_p->base_attr_db.physi_para10 + p_p->base_attr_db.stren_para10 \
		+ p_p->base_attr_db.endur_para10 + p_p->base_attr_db.quick_para10 + p_p->base_attr_db.intel_para10) / 10;
	PKG_UINT16(buf, para_all, j);

	PKG_UINT32(buf, p_p->skill_cnt, j);
	for (int loop = 0; loop < p_p->skill_cnt; loop ++) {
		PKG_UINT32(buf, p_p->skills[loop].skill_id, j);
		PKG_UINT8(buf, p_p->skills[loop].skill_level, j);
		PKG_UINT32(buf, p_p->skills[loop].skill_exp, j);
	}

	return j;
}

void gen_pet_attr(pet_db_t* p_pd, beast_t* pb, uint32_t level, int growth, uint32_t rand_diff)
{
	memset(p_pd, 0, sizeof(pet_db_t));
	p_pd->pettype = pb->id;
	p_pd->race = pb->race;
	
	memcpy(p_pd->nick, pb->nick, USER_NICK_LEN);
	p_pd->level = level;
	p_pd->flag = (p_pd->level == 1) ? (p_pd->flag | pet_1_lv_flag) : p_pd->flag;
	p_pd->experience = get_lv_exp(p_pd->level);
	p_pd->earth = pb->earth;
	p_pd->water = pb->water;
	p_pd->fire = pb->fire;
	p_pd->wind = pb->wind;
	
	p_pd->base_attr_db.physi_para10 = 	calc_pet_attr_para10(pb->physi_factor, rand_diff);
	p_pd->base_attr_db.stren_para10 = 	calc_pet_attr_para10(pb->stren_factor, rand_diff);
	p_pd->base_attr_db.endur_para10 = 	calc_pet_attr_para10(pb->endur_factor, rand_diff);
	p_pd->base_attr_db.quick_para10 = 	calc_pet_attr_para10(pb->quick_factor, rand_diff);
	p_pd->base_attr_db.intel_para10 = 	calc_pet_attr_para10(pb->intel_factor, rand_diff);

	// total_para must not greater than total_star
	uint16_t total_para10 = p_pd->base_attr_db.physi_para10 + p_pd->base_attr_db.stren_para10 + p_pd->base_attr_db.endur_para10 + p_pd->base_attr_db.quick_para10 + p_pd->base_attr_db.intel_para10;
	int total_factor = pb->physi_factor + pb->stren_factor + pb->endur_factor + pb->quick_factor + pb->intel_factor;
	total_factor -= rand() % (0 - growth + 1);

	if (total_para10 > 10 * total_factor) {
		uint16_t a = (total_para10 - 10 * total_factor) / 5;
		p_pd->base_attr_db.physi_para10 = p_pd->base_attr_db.physi_para10 > a ? (p_pd->base_attr_db.physi_para10 - a) : 0;
		p_pd->base_attr_db.stren_para10 = p_pd->base_attr_db.stren_para10 > a ? (p_pd->base_attr_db.stren_para10 - a) : 0;
		p_pd->base_attr_db.endur_para10 = p_pd->base_attr_db.endur_para10 > a ? (p_pd->base_attr_db.endur_para10 - a) : 0;
		p_pd->base_attr_db.quick_para10 = p_pd->base_attr_db.quick_para10 > a ? (p_pd->base_attr_db.quick_para10 - a) : 0;
		p_pd->base_attr_db.intel_para10 = p_pd->base_attr_db.intel_para10 > a ? (p_pd->base_attr_db.intel_para10 - a) : 0;
	}

	p_pd->base_attr_db.physi_init100 = 	p_pd->base_attr_db.physi_para10 * 2;
	p_pd->base_attr_db.stren_init100 = 	p_pd->base_attr_db.stren_para10 * 2;
	p_pd->base_attr_db.endur_init100 = 	p_pd->base_attr_db.endur_para10 * 2;
	p_pd->base_attr_db.quick_init100 = 	p_pd->base_attr_db.quick_para10 * 2;
	p_pd->base_attr_db.intel_init100 = 	p_pd->base_attr_db.intel_para10 * 2;

	int physi100 = p_pd->base_attr_db.physi_para10 * (p_pd->level - 1) * 10 / MAGIC_GROW_RATE \
		+ p_pd->base_attr_db.physi_init100;
	int stren100 = p_pd->base_attr_db.stren_para10 * (p_pd->level - 1) * 10 / MAGIC_GROW_RATE \
		+ p_pd->base_attr_db.stren_init100;
	int endur100 = p_pd->base_attr_db.endur_para10 * (p_pd->level - 1) * 10 / MAGIC_GROW_RATE \
		+ p_pd->base_attr_db.endur_init100;
	int quick100 = p_pd->base_attr_db.quick_para10 * (p_pd->level - 1) * 10 / MAGIC_GROW_RATE \
		+ p_pd->base_attr_db.quick_init100;
	int intel100 = p_pd->base_attr_db.intel_para10 * (p_pd->level - 1) * 10 / MAGIC_GROW_RATE \
		+ p_pd->base_attr_db.intel_init100;

	p_pd->hp = ROUND_100(8 * physi100 + 2 * stren100 + 3 * endur100 + 3 * quick100 + intel100) + 20;
	p_pd->mp = ROUND_100(physi100 + 2 * stren100 + 2 * endur100 + 2 * quick100 + 10* intel100) + 20;
	p_pd->hp *=8;
	p_pd->action_state = just_in_bag;
	p_pd->skill_cnt = 0;
}


void notify_pet_follow(sprite_t* p, int complet)
{
	uint8_t out[128];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->id, i);
	i += pkg_pet_follow(p->p_pet_follow, out + i);
	init_proto_head(out, proto_cli_pet_follow, i);
	send_to_map(p, out, i, complet,1);
}

static inline void try_give_talent_skill(sprite_t* p, pet_t* p_p, beast_t* pba, uint32_t skillid)
{
	if (p_p->skill_cnt < pba->skillbar) {
		p_p->skills[p_p->skill_cnt].skill_id = skillid;
		p_p->skills[p_p->skill_cnt].skill_level = p_p->level / 10 + 1;
		p_p->skills[p_p->skill_cnt].skill_exp = 1;
		p_p->skill_cnt ++;
		db_buy_pet_skill(NULL, p->id, p_p, skillid, 0xFF, 0);
		return;
	}

	skill_t* ps = &p_p->skills[0];
	for (int loop = 1; loop < p_p->skill_cnt; loop ++) {
		if (!p_p->skills[loop].skill_exp && p_p->skills[loop].skill_level < ps->skill_level) {
			ps = &p_p->skills[loop];
		}
	}

	KNOTI_LOG(p->id, "GIVE TALENT SKILL DEL\t[%u %u %u %u]", p_p->petid, ps->skill_id, ps->skill_level, skillid);
	db_del_pet_skill(NULL, p->id, p_p->petid, ps->skill_id);
	ps->skill_id = skillid;
	ps->skill_level = p_p->level / 10 + 1;
	ps->skill_exp = 1;
	db_buy_pet_skill(NULL, p->id, p_p, skillid, 0xFF, 0);
}

static inline void compensate_pet_talent_skill(sprite_t* p, pet_t* p_p)
{
	beast_t* pba = get_beast(p_p->pettype);
	if (!pba || pba->talent_skill_cnt < 2) {
		return;
	}

	skill_t* pst1 = cache_get_pet_skill(p_p, pba->talent_skill_ids[0]);
	skill_t* pst2 = NULL;
	for (int loop = 1; loop < pba->talent_skill_cnt; loop ++) {
		pst2 = cache_get_pet_skill(p_p, pba->talent_skill_ids[loop]);
		if (pst2) {
			KDEBUG_LOG(p->id, "HAVE RAND SKILL\t[%u %u]", p_p->petid, pst2->skill_id);
			break;
		}
	}

	if (pst1 && pst2) {
		pst1->skill_level = p_p->level / 10 + 1;
		pst1->skill_exp = 1;
		db_buy_pet_skill(NULL, p->id, p_p, pst1->skill_id, 0xFF, 0);

		pst2->skill_level = p_p->level /10 + 1;
		pst2->skill_exp = 1;
		db_buy_pet_skill(NULL, p->id, p_p, pst2->skill_id, 0xFF, 0);
	} else if (pst1) {
		pst1->skill_level = p_p->level / 10 + 1;
		pst1->skill_exp = 1;
		db_buy_pet_skill(NULL, p->id, p_p, pst1->skill_id, 0xFF, 0);

		uint32_t skillid = pba->talent_skill_ids[rand() % (pba->talent_skill_cnt - 1) + 1];
		try_give_talent_skill(p, p_p, pba, skillid);
	} else if (pst2) {
		pst2->skill_level = p_p->level /10 + 1;
		pst2->skill_exp = 1;
		db_buy_pet_skill(NULL, p->id, p_p, pst2->skill_id, 0xFF, 0);
		try_give_talent_skill(p, p_p, pba, pba->talent_skill_ids[0]);
	} else {
		try_give_talent_skill(p, p_p, pba, pba->talent_skill_ids[0]);
		uint32_t skillid = pba->talent_skill_ids[rand() % (pba->talent_skill_cnt - 1) + 1];
		try_give_talent_skill(p, p_p, pba, skillid);
	}
}

void reset_pet_talent_skill(sprite_t* p, pet_t* p_p)
{
	uint32_t talent_skill_cnt = 0;
	for (uint32_t loop = 0; loop < p_p->skill_cnt; loop ++) {
		if (p_p->skills[loop].skill_exp == 1) {
			talent_skill_cnt ++;
			if (talent_skill_cnt > 2) {
				p_p->skills[loop].skill_exp = 0;
				db_buy_pet_skill(NULL, p->id, p_p, p_p->skills[loop].skill_id, p_p->skills[loop].skill_level, 0);
				KNOTI_LOG(p->id, "RESET TALENT SKILL\t[%u %u %u]", p_p->petid, p_p->skills[loop].skill_id, p_p->skills[loop].skill_level);
			}
		}
	}
}

int update_pet_from_db(sprite_t* p, pet_t *p_p, pet_db_t* p_pdb, bool resetskill)
{
	//KDEBUG_LOG(p->id,"xxxx %u %u %u",p_p->petid,p_p->action_state,p_pdb->action_state);
	memset(p_p, 0, sizeof(pet_t));
	p_p->petid = p_pdb->petid;
	p_p->pettype = p_pdb->pettype;
	p_p->race = p_pdb->race;
	p_p->flag = p_pdb->flag;
	
	memcpy(p_p->nick, p_pdb->nick, USER_NICK_LEN);
	
	p_p->level = p_pdb->level;
	p_p->experience = p_pdb->experience;

	memcpy(&p_p->base_attr_db, &p_pdb->base_attr_db, sizeof(pet_attr_db_t));

	calc_pet_5attr(p_p);
	
	p_p->attr_addition = p_pdb->attr_addition;
	p_p->hp = p_pdb->hp;
	p_p->mp = p_pdb->mp;
	p_p->earth = p_pdb->earth;
	p_p->water = p_pdb->water;
	p_p->fire = p_pdb->fire;
	p_p->wind = p_pdb->wind;
	p_p->injury_lv = p_pdb->injury_lv;
	p_p->action_state = p_pdb->action_state;

	calc_pet_second_level_attr(p_p);
	p_p->hp = p_p->hp > p_p->hp_max ? p_p->hp_max : p_p->hp;
	p_p->mp = p_p->mp > p_p->mp_max ? p_p->mp_max : p_p->mp;
	p_p->skill_cnt = p_pdb->skill_cnt > MAX_SKILL_BAR ? MAX_SKILL_BAR : p_pdb->skill_cnt;

	int talent_skill_cnt = 0;
	for (int loop = 0; loop < p_p->skill_cnt; loop ++) {
		p_p->skills[loop].skill_id = p_pdb->skills[loop].skill_id;
		p_p->skills[loop].skill_level = p_pdb->skills[loop].skill_level;
		p_p->skills[loop].default_lv = p_pdb->skills[loop].default_lv;

		if (p_p->skills[loop].skill_level == 0xFF) {
			p_p->skills[loop].skill_level = p_p->level / 10 + 1;
			p_p->skills[loop].skill_exp = 1;
			if (talent_skill_cnt >= 2 && !resetskill) {
				p_p->skills[loop].skill_exp = 0;
			}
			talent_skill_cnt ++;
		} else {
			p_p->skills[loop].skill_exp = 0;
		}
	}

	if (talent_skill_cnt < 2)
		compensate_pet_talent_skill(p, p_p);

	if (resetskill && talent_skill_cnt >= 2) {
		reset_pet_talent_skill(p, p_p);
	}

	return 0;
}

int update_pet_from_db_new(sprite_t* p, pet_t *p_p, stru_pet_info* p_pdb)
{
	memset(p_p, 0, sizeof(pet_t));
	p_p->petid = p_pdb->gettime;
	p_p->pettype = p_pdb->pettype;
	p_p->race = p_pdb->race;
	p_p->flag = p_pdb->flag;
	
	memcpy(p_p->nick, p_pdb->nick, USER_NICK_LEN);
	
	p_p->level = p_pdb->level;
	p_p->experience = p_pdb->exp;

	memcpy(&p_p->base_attr_db, &p_pdb->physiqueinit, sizeof(pet_attr_db_t));

	calc_pet_5attr(p_p);
	
	p_p->attr_addition = p_pdb->attr_addition;
	p_p->hp = p_pdb->hp;
	p_p->mp = p_pdb->mp;
	p_p->earth = p_pdb->earth;
	p_p->water = p_pdb->water;
	p_p->fire = p_pdb->fire;
	p_p->wind = p_pdb->wind;
	p_p->injury_lv = p_pdb->injury_state;
	p_p->action_state = p_pdb->location;

	calc_pet_second_level_attr(p_p);
	p_p->hp = p_p->hp > p_p->hp_max ? p_p->hp_max : p_p->hp;
	p_p->mp = p_p->mp > p_p->mp_max ? p_p->mp_max : p_p->mp;
	p_p->skill_cnt = p_pdb->skills.size() > MAX_SKILL_BAR ? MAX_SKILL_BAR : p_pdb->skills.size();

	int compensation = 1, talent_skill_cnt = 0;
	for (int loop = 0; loop < p_p->skill_cnt; loop ++) {
		p_p->skills[loop].skill_id = p_pdb->skills[loop].skillid;
		p_p->skills[loop].skill_level = p_pdb->skills[loop].level;
		//p_p->skills[loop].default_lv = p_pdb->skills[loop];

		if (p_p->skills[loop].skill_level == 0xFF) {
			p_p->skills[loop].skill_level = p_p->level / 10 + 1;
			p_p->skills[loop].skill_exp = 1;
			if (talent_skill_cnt >= 2) {
				p_p->skills[loop].skill_exp = 0;
			}
			compensation = 0;
			talent_skill_cnt ++;
		} else {
			p_p->skills[loop].skill_exp = 0;
		}
	}

	return 0;
}
int get_g_book_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	return send_request_to_db(p, p->id, get_all_pet_type_cmd, NULL, 0);
}

static void pkg_gbook(gpointer key, gpointer value, gpointer data)
{
	beast_kill_t* pbk = (beast_kill_t *)value;
	if ((pbk->state & 0x4) == 0 || pbk->beastid > 100000)
		return;
	PKG_UINT32(msg, pbk->beastid, *(uint32_t*)data);
	PKG_UINT16(msg, !!(pbk->state & 0x8), *(uint32_t*)data);
	PKG_UINT16(msg, !!(pbk->state & 0x10), *(uint32_t*)data);
	pbk->state &= ~0x8;
}

int get_all_pet_type(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_all_pet_type_out* p_out = P_OUT;
	for (uint32_t loop = 0; loop < p_out->petlist.size(); loop ++) {
		uint32_t pettype = p_out->petlist[loop];
		if (get_beast(pettype)) {
			beast_kill_t* pbk = cache_get_beast_kill_info(p, pettype);
			if (!(pbk->state & 0x4)) {
				db_set_monster_handbook(NULL, p->id, pettype, 0x4, 0, 0);
				pbk->state |= 0xC;
			}
		}
	}

	int slen = sizeof(protocol_t) + 4;
	g_hash_table_foreach(p->beast_handbook, pkg_gbook, &slen);
	
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, ((slen - sizeof(protocol_t) - 4) / 8), j);
	
	init_proto_head(msg, p->waitcmd, slen);
	return send_to_self(p, msg, slen, 1);
}

int get_g_book_xiaomee_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t beastid;
	UNPKG_UINT32(body, beastid, j);

	if(beastid >= 100000) {
		return send_to_self_error(p,p->waitcmd,cli_err_invalid_grpid, 1);
	}
	beast_kill_t* pbk = (beast_kill_t *)g_hash_table_lookup(p->beast_handbook, &beastid);
	if(!pbk) {
		return send_to_self_error(p,p->waitcmd,cli_err_invalid_grpid, 1);
	}

	if(pbk->state & 0x10) {
		return send_to_self_error(p,p->waitcmd,cli_err_already_get_this, 1);
	}

	p->xiaomee += 200;
	pbk->state |= 0x10;
	db_add_xiaomee(NULL,p->id,200);
	db_set_monster_handbook(NULL, p->id, beastid, 0x10, 0, 0);
	response_proto_uint32_uint32(p, p->waitcmd, 50001, 200, 1, 0);
	return 0;
}

int cli_get_petlist_type(sprite_t *p,  Cmessage * c_in)
{
	cli_get_petlist_type_in* p_in = P_IN;
	beast_t* pb = get_beast(p_in->pettype);
	if (!pb) {
		KERROR_LOG(p->id, "invalid pettype\t[%u]", p_in->pettype);
		return -1;
	}

	*(uint32_t *)p->session = p_in->pettype;

	mole2_get_type_pets_in db_in;
	db_in.start = p_in->start;
	db_in.limit = p_in->limits;
	db_in.pettype = p_in->pettype;
	return send_msg_to_db(p, p->id, mole2_get_type_pets_cmd, &db_in);

}

int mole2_get_type_pets(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_type_pets_out *p_out=P_OUT;
	cli_get_petlist_type_out out;
	out.total = p_out->total;
	out.start = p_out->start;
	out.pettype = *(uint32_t *) p->session;
	KDEBUG_LOG(p->id, "GET TYPE\t[%u]", out.pettype);

	for (uint32_t loop = 0; loop < p_out->pets.size(); loop ++) {
		pet_t tmp_pet;
		update_pet_from_db_new(p, &tmp_pet, &p_out->pets[loop]);
		stru_type_pet_info type_pet;
		type_pet.petid = tmp_pet.petid;
		type_pet.level = tmp_pet.level;
		type_pet.hp = tmp_pet.hp;
		type_pet.hpmax = tmp_pet.hp_max;
		type_pet.mp = tmp_pet.mp;
		type_pet.mpmax = tmp_pet.mp_max;
		type_pet.injurylv = tmp_pet.injury_lv;
		memcpy(type_pet.nick, tmp_pet.nick, USER_NICK_LEN);
		type_pet.physique = ROUND_100(tmp_pet.physi100);
		type_pet.strength = ROUND_100(tmp_pet.stren100);
		type_pet.endurance = ROUND_100(tmp_pet.endur100);
		type_pet.quick = ROUND_100(tmp_pet.quick100);
		type_pet.intelligence = ROUND_100(tmp_pet.intel100);
		type_pet.para_all = (tmp_pet.base_attr_db.physi_para10 + tmp_pet.base_attr_db.stren_para10 \
			+ tmp_pet.base_attr_db.endur_para10 + tmp_pet.base_attr_db.quick_para10 + tmp_pet.base_attr_db.intel_para10) / 10;
		type_pet.spirit = tmp_pet.spirit;
		type_pet.resume = tmp_pet.resume;
		out.petlist.push_back(type_pet);
		KDEBUG_LOG(p->id, "GET TYPE\t[%u %u]", type_pet.petid, out.pettype);
	}

	return send_msg_to_self(p, p->waitcmd, &out, 1);
}

int mole2_petfight_get_rank(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	return 0;
}

int mole2_petfight_get_user_rank(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	return 0;
}

int db_set_pet_flag(sprite_t *p,pet_t *p_p)
{
	cli_devote_medals_out cli_out;
	cli_out.count=p_p->flag;
	cli_out.exp=p_p->petid;
	return send_msg_to_db(0,p->id,mole2_set_pet_flag_cmd,&cli_out);
}

/**
 * @brief 宠物产生精灵蛋 分四级  有不同的概率 
 *
 * @param petid[2] 精灵id   itemid 使用药品的id
 *
 * @return 精灵蛋id 
 */
int cli_pet_exchange_egg(sprite_t *p,  Cmessage * c_in)
{
	cli_pet_exchange_egg_in *p_in=P_IN;	
	std::map<uint32_t,pet_simple_t>::iterator it1=p->pets_inhouse->find(p_in->petid[0]);
	std::map<uint32_t,pet_simple_t>::iterator it2=p->pets_inhouse->find(p_in->petid[1]);
	uint32_t rate[MAX_PET_EX_EGG_CNT]={ };
	if( !check_pet_inhouse(p, p_in->petid[0]) || !check_pet_inhouse(p, p_in->petid[1])){
		KERROR_LOG(p->id,"pet not in house %u %u", p_in->petid[0], p_in->petid[1]);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_not_inbag, 1);
	}
	normal_item_t *pni=NULL;
	if( p_in->itemid ){
		pni=get_item(p_in->itemid);
		CHECK_ITEM_VALID(p, pni, p_in->itemid);
		CHECK_ITEM_EXIST(p, p_in->itemid, 1);
		CHECK_ITEM_FUNC(p, pni, item_for_pet_exchange_egg);
	}
	if( it1->second.level <5 || it2->second.level <5 ){
		return send_to_self_error(p, p->waitcmd, cli_err_level_not_fit, 1);
	}
	for( uint32_t loop=0 ; loop<MAX_PET_EX_EGG_CNT ; loop++ ){
		rate[loop]=exchange_egg[loop].rate;
		//DEBUG_LOG("rate %u",rate[loop]);
	}
	DEBUG_LOG("rate %u %u %u %u",rate[0],rate[1],rate[2],rate[3]);
	if(pni && p_in->itemid){
		//使用稳定剂提升4级的概率   从1级中减去提升了的
		 rate[MAX_PET_EX_EGG_CNT-1] *= pni->add_times;
		 //DEBUG_LOG("xxx %u %u %u",p_in->itemid,pni->item_id,pni->add_times);
		 rate[0] -= rate[MAX_PET_EX_EGG_CNT-1] *(pni->add_times - 1); 
	}
	DEBUG_LOG("rate1111 %u %u %u %u",rate[0],rate[1],rate[2],rate[3]);
	uint32_t idx=rand_type_idx(MAX_PET_EX_EGG_CNT,rate,1000);
	item_t* pday = cache_get_day_limits(p, ssid_fishing_cnt);
	if(idx==3){
		if( pday->count >= 10){
			idx=1;
		}else{
			pday->count++;
			db_day_add_ssid_cnt(NULL, p->id, ssid_pet_exchange_rare_egg, 1, -1);
		}
	}
	uint32_t egg_idx=rand_type_idx(exchange_egg[idx].egg_cnt, exchange_egg[idx].egg_rate, 1000);
	uint32_t itemid=exchange_egg[idx].egg_info[egg_idx].itemid;
	if( idx == 3 ){
		noti_all_online_get_item(p, itemid);
	}
	//DEBUG_LOG("xxxx :%u %u",egg_idx,exchange_egg[idx].egg_cnt);
	cache_add_kind_item(p, itemid, 1);
	db_add_item(NULL, p->id, itemid, 1);
	del_pet_inhouse(p, p_in->petid[0]);
	del_pet_inhouse(p, p_in->petid[1]);
	if(p_in->itemid ){
		cache_reduce_kind_item(p, p_in->itemid, 1);
		db_add_item(NULL, p->id, p_in->itemid, -1);
	}
	msg_log_pet_exchange_egg(itemid, p->id);
	KDEBUG_LOG(p->id,"cli_pet_exchange_egg %u %u %u", idx, egg_idx, itemid);
	response_proto_uint32(p, p->waitcmd,itemid, 1, 0);
	return 0;
}
