#include "npc.h"
#include "global.h"
#include "items.h"
#include "skill.h"
#include "beast.h"
#include "pet.h"

void init_npcs()
{
	npcs = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_npc_by_key(gpointer key, gpointer p, gpointer userdata)
{
	g_slice_free1(sizeof(sprite_t), p);
	return TRUE;
}

void fini_npcs(int destroy)
{
	g_hash_table_foreach_remove(npcs, free_npc_by_key, 0);
	if (destroy)
		g_hash_table_destroy(npcs);
}

sprite_t* get_npc(uint32_t npcid)
{
	return (sprite_t *)g_hash_table_lookup(npcs, &npcid);
}

int init_npc_with_beast(beast_t* pb, sprite_t* p)
{
	p->pettype = 		pb->id;
	memcpy(p->nick, pb->nick, USER_NICK_LEN);
	p->race = 			pb->race;
 	p->level = 			beast_group_lvtype_ave;
	
	p->earth = 			pb->earth;
	p->water = 			pb->water;
	p->fire = 			pb->fire;
	p->wind = 			pb->wind;

	p->physique = 		pb->physi_factor;
	p->strength = 		pb->stren_factor;
	p->endurance = 		pb->endur_factor;
	p->quick = 			pb->quick_factor;
	p->intelligence =	pb->intel_factor;

	return 0;
}


int load_npc(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Npc")) { 
			uint32_t id = 0, type;
			DECODE_XML_PROP_UINT32 (id, cur, "ID");
			DECODE_XML_PROP_UINT32 (type, cur, "TypeID");
			if (id >MAX_NPC_TYPE || id <= MAX_BEAST_TYPE) {
				KERROR_LOG(0, "invalid npc id\t[%u]", id);
				return -1;
			}

			beast_t* pb = get_beast(type);
			if (!pb) {
				KERROR_LOG(0, "invalid beast id\t[%u %u]", id, type);
				return -1;
			}
			
			sprite_t* p = get_npc(id);
			if (p) {
				KERROR_LOG(0, "duplicate npc id\t[%u]", id);
				return -1;
			}

			p = (sprite_t *)g_slice_alloc(sizeof(sprite_t));
			memset(p, 0, sizeof(sprite_t));
			p->id = id;
			g_hash_table_insert(npcs, &p->id, p);

			init_npc_with_beast(pb, p);

			DECODE_XML_PROP_STR(p->nick, cur, "Nick");
			DECODE_XML_PROP_INT_DEFAULT(p->hp_max, cur, "HPCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->mp_max, cur, "MPCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->attack, cur, "AtkCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->mattack, cur, "MatkCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->defense, cur, "DefCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->speed, cur, "SpeedCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->spirit, cur, "SpiritCoefficient", RAND_COMMON_RAND_BASE);
			DECODE_XML_PROP_INT_DEFAULT(p->resume, cur, "ResumeCoefficient", RAND_COMMON_RAND_BASE);

			DECODE_XML_PROP_INT_DEFAULT(p->mdefense, cur, "MDef", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->bisha, cur, "Crit", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->fight_back, cur, "FightBack", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->hit_rate, cur, "Hit", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->avoid_rate, cur, "Avd", 0);

			DECODE_XML_PROP_INT_DEFAULT(p->rpoison, cur, "Rpoison", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->rlithification, cur, "Rlithification", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->rlethargy, cur, "Rlethargy", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->rinebriation, cur, "Rcurse", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->rconfusion, cur, "Rconfusion", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->roblivion, cur, "Roblivion", 0);

			KDEBUG_LOG(0, "NPC INFO\t[%u %s %u %u %u %u]", p->id, p->nick, p->hp, p->mp, p->attack, p->defense);
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Skill")) {
					int skilllist[MAX_SKILL_BAR], percent[MAX_SKILL_BAR];
					p->skill_cnt = decode_xml_prop_arr_int_default(skilllist, MAX_SKILL_BAR, chl, "ID", 0);
					decode_xml_prop_arr_int_default(percent, MAX_SKILL_BAR, chl, "Percent", 0);
					int loop;
					for (loop = 0; loop < p->skill_cnt; loop ++) {
						p->skills[loop].skill_id = skilllist[loop];
						p->skills[loop].skill_level = 1;
						p->skills[loop].skill_exp = percent[loop];
					}
				}
				chl = chl->next;
			}
		}
		cur = cur->next;
	}

	return 0;
}

void init_shops()
{
	int loop = 0;
	for (loop = 0; loop < MAX_SHOP_CNT; loop ++) {
		shops[loop].shop_id = 0;
		shops[loop].item_list = g_hash_table_new(g_int_hash, g_int_equal);
		shops[loop].skill_list = g_hash_table_new(g_int_hash, g_int_equal);
		shops[loop].pet_skill_list = g_hash_table_new(g_int64_hash, g_int64_equal);
		shops[loop].item_map_list = g_hash_table_new(g_int64_hash, g_int64_equal);
		shops[loop].skill_map_list = g_hash_table_new(g_int64_hash, g_int64_equal);
	}
}

void fini_shops()
{
	int loop = 0;
	for (loop = 0; loop < MAX_SHOP_CNT; loop ++) {
		g_hash_table_destroy(shops[loop].item_list);
		g_hash_table_destroy(shops[loop].skill_list);
		g_hash_table_destroy(shops[loop].pet_skill_list);
		g_hash_table_destroy(shops[loop].item_map_list);
		g_hash_table_destroy(shops[loop].skill_map_list);
	}
}

shop_t* get_shop_no_chk(uint32_t shopid)
{
	return (shopid && shopid < MAX_SHOP_CNT) ? &shops[shopid - 1] : NULL;
}

int load_shop_item(xmlNodePtr cur)
{
	int id = 0, loop;
	for (loop = 0; loop < MAX_SHOP_CNT; loop ++) {
		g_hash_table_remove_all(shops[loop].item_list);
		g_hash_table_remove_all(shops[loop].item_map_list);
	}
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Npc")) { 
			DECODE_XML_PROP_INT (id, cur, "ID");
			shop_t* pshop = get_shop_no_chk(id);
			if (!pshop)
				ERROR_RETURN(("shop id invalid\t[%u]", id), -1);
			pshop->shop_id = id;

			int count, ids[1000];;
			count = decode_xml_prop_arr_int_default(ids, 1000, cur, "ItemIds", 0);

			for (loop = 0; loop < count; loop ++) {
				if (get_item(ids[loop])) {
					normal_item_t* pni = get_item(ids[loop]);
					g_hash_table_insert(pshop->item_list, &pni->item_id, pni);
				} else if (get_cloth(ids[loop])) {
					cloth_t* pc = get_cloth(ids[loop]);
					g_hash_table_insert(pshop->item_list, &pc->id, pc);
				} else {
					ERROR_RETURN(("item id invalid\t[%u %u]", id, ids[loop]), -1);
				}
			}
			count = decode_xml_prop_arr_int_default(ids, 1000, cur, "MapID", 0);
			for (loop = 0; loop < count; loop ++) {
				map_t* pm = get_map(ids[loop]);
				if (!pm) 
					ERROR_RETURN(("invalid mapid\t[%u %u]", id, ids[loop]), -1);
				g_hash_table_insert(pshop->item_map_list, &pm->id, pm);
			}
		}
		cur = cur->next;
	}

	return 0;
}

int load_shop_skill(xmlNodePtr cur)
{
	int id = 0;
	uint32_t loop;
	for (loop = 0; loop < MAX_SHOP_CNT; loop ++) {
		g_hash_table_remove_all(shops[loop].skill_list);
		g_hash_table_remove_all(shops[loop].pet_skill_list);
		g_hash_table_remove_all(shops[loop].skill_map_list);
	}
	
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Npc")) { 
			DECODE_XML_PROP_INT (id, cur, "ID");
			shop_t* pshop = get_shop_no_chk(id);
			if (!pshop)
				ERROR_RETURN(("shop id invalid\t[%u]", id), -1);
			pshop->shop_id = id;

			int ids[1000], lvs[1000];
			uint32_t count = decode_xml_prop_arr_int_default(ids, 1000, cur, "RoleStudyList", 0);

			for (loop = 0; loop < count; loop ++) {
				skill_info_t* psi = get_skill_info(ids[loop]);
				if (!psi) {
					ERROR_RETURN(("invalid skill id\t[%u %u]", id, ids[loop]), -1);
				}
				g_hash_table_insert(pshop->skill_list, &psi->skill_id, psi);
			}

			count = decode_xml_prop_arr_int_default(ids, 1000, cur, "PetStudyList", 0);
			if (count != decode_xml_prop_arr_int_default(lvs, 1000, cur, "PetStudyLv", 0))
				ERROR_RETURN(("pet skill cnt lv not match\t[%u %u]", id, count), -1);
			for (loop = 0; loop < count; loop ++) {
				pet_skill_p_t* psp = get_pet_skill_price(ids[loop], lvs[loop]);
				if (!psp)
					ERROR_RETURN(("pet skill invalid\t[%u %u %u]", id, ids[loop], lvs[loop]), -1);
				g_hash_table_insert(pshop->pet_skill_list, &psp->skill_key, psp);
			}

			count = decode_xml_prop_arr_int_default(ids, 1000, cur, "MapID", 0);
			for (loop = 0; loop < count; loop ++) {
				map_t* pm = get_map(ids[loop]);
				if (!pm) 
					ERROR_RETURN(("invalid mapid\t[%u %u]", id, ids[loop]), -1);
				g_hash_table_insert(pshop->skill_map_list, &pm->id, pm);
			}
		}
		cur = cur->next;
	}

	return 0;
}

int item_shop_existed(uint32_t shopid, uint32_t itemid, map_id_t mapid)
{
	shop_t* pshop = get_shop_no_chk(shopid);
	if (!pshop || !pshop->shop_id)
		return 0;
	
	gpointer pi = g_hash_table_lookup(pshop->item_list, &itemid);
	if (!pi)
		return 0;
	
	if (mapid) {
		gpointer pm = g_hash_table_lookup(pshop->item_map_list, &mapid);
		return pm ? 1 : 0;
	}
	
	return 1;
}

int skill_shop_existed(uint32_t shopid, uint32_t skillid, map_id_t mapid)
{
	shop_t* pshop = get_shop_no_chk(shopid);
	if (!pshop || !pshop->shop_id)
		return 0;
	
	if (!g_hash_table_lookup(pshop->skill_list, &skillid))
		return 0;
	
	if (IS_NORMAL_MAP(mapid)) {
		gpointer pm = g_hash_table_lookup(pshop->skill_map_list, &mapid);
		return pm ? 1 : 0;
	}
	
	return 1;
}

int pet_skill_shop_existed(uint32_t shopid, uint32_t skillid, uint32_t lv, map_id_t mapid)
{
	shop_t* pshop = get_shop_no_chk(shopid);
	if (!pshop || !pshop->shop_id)
		return 0;
	uint64_t skill_key = skillid;
	skill_key <<= 32;
	skill_key += lv;
	gpointer ps = g_hash_table_lookup(pshop->pet_skill_list, &skill_key);
	gpointer pm = g_hash_table_lookup(pshop->skill_map_list, &mapid);
	KDEBUG_LOG(0, "tttttt[%u %lu %u]", shopid,skill_key ,lv);
	return ps && (IS_MAZE_MAP(mapid) || pm) ? 1 : 0;
}

int add_npc_to_team_cmd(sprite_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t npcid;
	int i = 0;
	UNPKG_UINT32(body, npcid, i);
	sprite_t* npc = get_npc(npcid);
	if (!npc) {
		KERROR_LOG(p->id, "npc id invalid\t[%u]", npcid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	CHECK_SELF_TEAM_LEADER(p, npcid, 1);
	CHECK_TEAM_FULL(p, p, 1);
	
	CHECK_USER_APPLY_PK(p, p);
	CHECK_SELF_BATTLE_INFO(p, npcid);

	p->battle_npc[p->npc_cnt ++] = npc;
	response_proto_head(p, p->waitcmd, 1, 0);
	response_team_info(p);
	return 0;
}

int dismiss_npc_cmd(sprite_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t index;
	int i = 0;
	UNPKG_UINT32(body, index, i);
	if (index >= p->npc_cnt) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	p->npc_cnt --;
	p->battle_npc[index] = p->battle_npc[p->npc_cnt];
	p->battle_npc[p->npc_cnt] = NULL;

	response_team_info(p);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

