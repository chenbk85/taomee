#include "pet.h"
#include "items.h"
#include "battle.h"
#include "honor.h"
#include "sns.h"
#include "task_new.h"
#include "skill.h"
#include "npc.h"
#include "beast.h"
#include "buff.h"
#include "viponly.h"
#include "mail.h"

void init_all_clothes()
{
	all_clothes = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_cloth_by_key(gpointer key, gpointer pcloth, gpointer userdata)
{
	g_slice_free1(sizeof(cloth_t), pcloth);
	return TRUE;
}

void fini_all_clothes()
{
	g_hash_table_destroy(all_clothes);
}

cloth_t* get_cloth(uint32_t clothtype)
{
	return (cloth_t *)g_hash_table_lookup(all_clothes, &clothtype);
}

int gen_cloth_attr_new(cloth_attr_t* p_attr,body_cloth_t *pbc)
{
	p_attr->clothtype = pbc->clothtype;
	p_attr->grid = 0;
	p_attr->cloth_level = pbc->clothlv;
	p_attr->duration = pbc->duration;
	p_attr->duration_max  = pbc->duration_max;
	p_attr->hp_max = pbc->hp_max;
	p_attr->mp_max = pbc->mp_max;
	p_attr->attack = pbc->attack;
	p_attr->mattack = pbc->mattack;
	p_attr->defense = pbc->defense;
	p_attr->mdefense = pbc->mdefense;
	p_attr->speed = pbc->speed;
	p_attr->spirit = pbc->spirit;
	p_attr->resume = pbc->resume;
	p_attr->hit = pbc->hit;
	p_attr->dodge = pbc->dodge;
	p_attr->crit = pbc->crit;
	p_attr->fightback = pbc->fightback;
	p_attr->rpoison = pbc->rpoison;
	p_attr->rlithification = pbc->rlithification;
	p_attr->rlethargy = pbc->rlethargy;
	p_attr->rinebriation = pbc->rinebriation;
	p_attr->rconfusion = pbc->rconfusion;
	p_attr->roblivion = pbc->roblivion;
	p_attr->quality = pbc->quality;
	p_attr->validday = pbc->validday;
	p_attr->crystal_attr = pbc->crystal_attr;
	p_attr->bless_type = pbc->bless_type;
	return 0;
}

int gen_cloth_attr(cloth_t* pc, uint16_t lv, cloth_lvinfo_t* pclv, cloth_attr_t* p_attr, uint32_t validday)
{
	p_attr->clothtype = pc->id;
	p_attr->grid = 0;
	p_attr->cloth_level = lv;
	p_attr->duration = p_attr->duration_max = pclv->duration[0] + rand () % (pclv->duration[1] - pclv->duration[0] + 1);
	p_attr->hp_max = pclv->hp_max + rand() % (pclv->r_hp_max + 1);
	p_attr->mp_max = pclv->mp_max + rand() % (pclv->r_mp_max + 1);
	p_attr->attack = pclv->attack + rand() % (pclv->r_attack + 1);
	p_attr->mattack = pclv->mattack + rand() % (pclv->r_mattack + 1);
	p_attr->defense = pclv->defense + rand() % (pclv->r_defense + 1);
	p_attr->mdefense = pclv->mdefense + rand() % (pclv->r_mdefense + 1);
	p_attr->speed = pclv->speed + rand() % (pclv->r_speed + 1);
	p_attr->spirit = pclv->spirit + rand() % (pclv->r_spirit + 1);
	p_attr->resume = pclv->resume + rand() % (pclv->r_resume + 1);
	p_attr->hit = pclv->hit + rand() % (pclv->r_hit + 1);
	p_attr->dodge = pclv->dodge + rand() % (pclv->r_dodge + 1);
	p_attr->crit = pclv->crit + rand() % (pclv->r_crit + 1);
	p_attr->fightback = pclv->fightback + rand() % (pclv->r_fightback + 1);
	p_attr->rpoison = pclv->rpoison + rand() % (pclv->r_rpoison + 1);
	p_attr->rlithification = pclv->rlithification + rand() % (pclv->r_rlithification + 1);
	p_attr->rlethargy = pclv->rlethargy + rand() % (pclv->r_rlethargy + 1);
	p_attr->rinebriation = pclv->rinebriation + rand() % (pclv->r_rinebriation + 1);
	p_attr->rconfusion = pclv->rconfusion + rand() % (pclv->r_rconfusion + 1);
	p_attr->roblivion = pclv->roblivion + rand() % (pclv->r_roblivion + 1);

	p_attr->quality = pclv->quality;
	p_attr->validday = validday;

	int percents[5] = {0, 500, 700, 900, 0};
	p_attr->crystal_attr = 0;
	if (pc->minlv >= 10 && pc->equip_part >= part_hat && pc->equip_part <= part_shoe \
		&& pc->id / 10000 == 14 && rand() % 1000 < percents[pclv->quality])
		p_attr->crystal_attr = rand() % 4 + 1;
	p_attr->bless_type = p_attr->crystal_attr ? (rand() % (attr_id_max - 1) + 1) : 0;
	return 0;
}

exchange_info_t* get_exchange_info_no_check(uint32_t id)
{
	return id && id <= MAX_EXCHANGE_INFO_CNT ? &exchange_infos[id - 1] : NULL;
}

exchange_info_t* get_exchange_info(uint32_t id)
{
	exchange_info_t* pei = get_exchange_info_no_check(id);
	return pei && pei->id ? pei : NULL;
}


int load_cloth_attr_16(int16_t* p_attr, uint16_t* p_rattr, xmlNodePtr cur, const char* name)
{
	int attr_cnt = 0;
	int attr[2];

	attr_cnt = decode_xml_prop_arr_int_default(attr, 2, cur, name, 0);
	(*p_attr) = attr[0];
	if (attr_cnt != 2) 
		(*p_rattr) = 0;
	else 
		(*p_rattr) = attr[1] - attr[0];

	if ((*p_rattr) > 10000) 
		ERROR_RETURN(("cloth attr rand too big\t[%u]", *p_rattr), -1);
	return 0;
}

int load_cloth_attr_32(int32_t* p_attr, uint32_t* p_rattr, xmlNodePtr cur, const char* name)
{
	int attr_cnt = 0;
	int attr[2];

	attr_cnt = decode_xml_prop_arr_int_default(attr, 2, cur, name, 0);
	(*p_attr) = attr[0];
	if (attr_cnt != 2) 
		(*p_rattr) = 0;
	else 
		(*p_rattr) = attr[1] - attr[0];

	if ((*p_rattr) > 10000) 
		ERROR_RETURN(("cloth attr rand too big\t[%u]", *p_rattr), -1);
	return 0;

}

void load_cloth_lvinfo(xmlNodePtr cur, cloth_lvinfo_t* pcl)
{
	// difference cloth attr
	decode_xml_prop_uint32_default (&(pcl->price), cur, "Price", 0);
	
	decode_xml_prop_uint32_default (&(pcl->sell_price), cur, "SellPrice", 0);
	decode_xml_prop_uint32_default (&(pcl->repair_price), cur, "RepairPrice", 0);
	decode_xml_prop_uint32_default (&(pcl->tradablity), cur, "Tradability", 3);
	decode_xml_prop_uint32_default (&(pcl->vip_tradablity), cur, "VipTradability", 3);
	//decode_xml_prop_uint32_default (&(pcl->del_forbid), cur, "Del", 1);
	decode_xml_prop_uint32_default (&(pcl->quality), cur, "EquipColor", 0);
	DECODE_XML_PROP_INT_DEFAULT(pcl->earth, cur, "Earth", 0);
	DECODE_XML_PROP_INT_DEFAULT (pcl->water, cur, "Water", 0);
	DECODE_XML_PROP_INT_DEFAULT (pcl->fire, cur, "Fire", 0);
	DECODE_XML_PROP_INT_DEFAULT (pcl->wind, cur, "Wind", 0);
	decode_xml_prop_uint16_default (&(pcl->vendibility), cur, "Vendibility", 0);
	decode_xml_prop_arr_int_default(pcl->duration, 2, cur, "Duration", 300);
	if (!pcl->duration[0]) pcl->duration[0] = 300;
	//if (pcl->duration[1] < pcl->duration[0]) 
	pcl->duration[1] = pcl->duration[0];
	
	load_cloth_attr_32(&pcl->hp_max, &pcl->r_hp_max, cur, "HpMax");
	load_cloth_attr_32(&pcl->mp_max, &pcl->r_mp_max, cur, "MpMax");
	load_cloth_attr_16(&pcl->attack, &pcl->r_attack, cur, "Atk");
	load_cloth_attr_16(&pcl->mattack, &pcl->r_mattack, cur, "Matk");
	load_cloth_attr_16(&pcl->defense, &pcl->r_defense, cur, "Def");
	load_cloth_attr_16(&pcl->mdefense, &pcl->r_mdefense, cur, "Mdef");
	load_cloth_attr_16(&pcl->speed, &pcl->r_speed, cur, "Speed");
	load_cloth_attr_16(&pcl->spirit, &pcl->r_spirit, cur, "Spirit");
	load_cloth_attr_16(&pcl->resume, &pcl->r_resume, cur, "Resume");
	load_cloth_attr_16(&pcl->hit, &pcl->r_hit, cur, "Hit");
	load_cloth_attr_16(&pcl->dodge, &pcl->r_dodge, cur, "Dodge");
	load_cloth_attr_16(&pcl->crit, &pcl->r_crit, cur, "Crit");
	load_cloth_attr_16(&pcl->fightback, &pcl->r_fightback, cur, "FightBack");
	load_cloth_attr_16(&pcl->rpoison, &pcl->r_rpoison, cur, "ResistPoison");
	load_cloth_attr_16(&pcl->rlithification, &pcl->r_rlithification, cur, "ResistLithification");
	load_cloth_attr_16(&pcl->rlethargy, &pcl->r_rlethargy, cur, "ResistLethargy");
	load_cloth_attr_16(&pcl->rinebriation, &pcl->r_rinebriation, cur, "ResistInebriation");
	load_cloth_attr_16(&pcl->rconfusion, &pcl->r_rconfusion, cur, "ResistConfusion");
	load_cloth_attr_16(&pcl->roblivion, &pcl->r_roblivion, cur, "ResistOblivion");
	
	KTRACE_LOG(0, "CLOTH:%d %d %d %d %d \t%d %d %d %d %d \t%d %d %d %d %d %d\t%d %d %d %d %d \t%d %d %d %d %d \t%d %d %d %d %d", \
				pcl->price, pcl->sell_price, pcl->repair_price, pcl->tradablity, pcl->vip_tradablity, \
				pcl->duration[0], pcl->hp_max, pcl->r_hp_max, pcl->mp_max, pcl->r_mp_max, \
				pcl->attack, pcl->r_attack, pcl->mattack, pcl->r_mattack, pcl->defense, pcl->r_defense, \
				pcl->mdefense, pcl->r_mdefense, pcl->speed, pcl->spirit, pcl->resume, \
				pcl->hit, pcl->dodge, pcl->crit, pcl->fightback, pcl->rpoison, \
				pcl->rlithification, pcl->rlethargy, pcl->rinebriation, pcl->rconfusion, pcl->roblivion);
}

void load_cloth_proinfo(xmlNodePtr cur, cloth_lvinfo_t* pcl)
{
	decode_xml_prop_arr_int_default(pcl->duration, 2, cur, "Duration", 300);
	if (!pcl->duration[0]) pcl->duration[0] = 300;
	//if (pcl->duration[1] < pcl->duration[0]) 
	pcl->duration[1] = pcl->duration[0];

	decode_xml_prop_uint32_default (&(pcl->quality), cur, "EquipColor", 0);
	
	load_cloth_attr_32(&pcl->hp_max, &pcl->r_hp_max, cur, "HpMax");
	load_cloth_attr_32(&pcl->mp_max, &pcl->r_mp_max, cur, "MpMax");
	load_cloth_attr_16(&pcl->attack, &pcl->r_attack, cur, "Atk");
	load_cloth_attr_16(&pcl->mattack, &pcl->r_mattack, cur, "Matk");
	load_cloth_attr_16(&pcl->defense, &pcl->r_defense, cur, "Def");
	load_cloth_attr_16(&pcl->mdefense, &pcl->r_mdefense, cur, "Mdef");
	load_cloth_attr_16(&pcl->speed, &pcl->r_speed, cur, "Speed");
	load_cloth_attr_16(&pcl->spirit, &pcl->r_spirit, cur, "Spirit");
	load_cloth_attr_16(&pcl->resume, &pcl->r_resume, cur, "Resume");
	load_cloth_attr_16(&pcl->hit, &pcl->r_hit, cur, "Hit");
	load_cloth_attr_16(&pcl->dodge, &pcl->r_dodge, cur, "Dodge");
	load_cloth_attr_16(&pcl->crit, &pcl->r_crit, cur, "Crit");
	load_cloth_attr_16(&pcl->fightback, &pcl->r_fightback, cur, "FightBack");
	load_cloth_attr_16(&pcl->rpoison, &pcl->r_rpoison, cur, "ResistPoison");
	load_cloth_attr_16(&pcl->rlithification, &pcl->r_rlithification, cur, "ResistLithification");
	load_cloth_attr_16(&pcl->rlethargy, &pcl->r_rlethargy, cur, "ResistLethargy");
	load_cloth_attr_16(&pcl->rinebriation, &pcl->r_rinebriation, cur, "ResistInebriation");
	load_cloth_attr_16(&pcl->rconfusion, &pcl->r_rconfusion, cur, "ResistConfusion");
	load_cloth_attr_16(&pcl->roblivion, &pcl->r_roblivion, cur, "ResistOblivion");
	
	KTRACE_LOG(0, "CLOTH:%d %d %d %d %d \t%d %d %d %d %d \t%d %d %d %d %d %d\t%d %d %d %d %d \t%d %d %d %d %d \t%d %d %d %d %d", \
				pcl->price, pcl->sell_price, pcl->repair_price, pcl->tradablity, pcl->vip_tradablity, \
				pcl->duration[0], pcl->hp_max, pcl->r_hp_max, pcl->mp_max, pcl->r_mp_max, \
				pcl->attack, pcl->r_attack, pcl->mattack, pcl->r_mattack, pcl->defense, pcl->r_defense, \
				pcl->mdefense, pcl->r_mdefense, pcl->speed, pcl->spirit, pcl->resume, \
				pcl->hit, pcl->dodge, pcl->crit, pcl->fightback, pcl->rpoison, \
				pcl->rlithification, pcl->rlethargy, pcl->rinebriation, pcl->rconfusion, pcl->roblivion);
}


int load_clothes(xmlNodePtr cur)
{
	//KF_LOG(111111,"trade","hello");
	g_hash_table_foreach_remove(all_clothes, free_cloth_by_key, 0);
	memset(crystal_attr, 0, sizeof(crystal_attr));
	uint32_t id, lv;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Kind")) {
			xmlNodePtr chl1 = cur->xmlChildrenNode;
			while (chl1) {
				if (!xmlStrcmp(chl1->name, (const xmlChar*)"Item")) {
					DECODE_XML_PROP_UINT32(id, chl1, "ID");
					if (id < MIN_CLOTH_ID || id > MAX_CLOTH_ID || get_cloth(id)) {
						ERROR_RETURN(("invalid or duplicate cloth id=%u", id), -1);
					}

					cloth_t* pcloth = (cloth_t *)g_slice_alloc0(sizeof(cloth_t));
					pcloth->id = id;
					g_hash_table_insert(all_clothes, &(pcloth->id), pcloth);

					DECODE_XML_PROP_UINT32(pcloth->prof, chl1, "Prof");
					decode_xml_prop_uint32_default(&pcloth->proflv, chl1, "ProfLv", 0);
					DECODE_XML_PROP_UINT32(pcloth->minlv, chl1, "UseLv");
					DECODE_XML_PROP_UINT32(pcloth->equip_part, chl1, "EquipPart");
					decode_xml_prop_uint32_default (&(pcloth->weapon_type), chl1, "Weapon", 0);
					decode_xml_prop_uint32_default (&(pcloth->honor_id), chl1, "TitleID", 0);
					decode_xml_prop_uint32_default (&(pcloth->del_forbid), cur, "Del", 0);
					pcloth->activatecnt = decode_xml_prop_arr_int_default((int *)pcloth->activateid, MAX_PROF_CNT, chl1, "ActivateID", 0);
					if (pcloth->honor_id && !IS_TEMP_HONOR(pcloth->honor_id)) {
						ERROR_RETURN(("invalid honor_id[%u %u]", id, pcloth->honor_id), -1);
					}
					if (pcloth->equip_part == 0 || pcloth->equip_part > 108) {
						ERROR_RETURN(("invalid EquipPart\t[%u %u]", id, pcloth->equip_part), -1);
					}
					if (pcloth->weapon_type > 9) {
						ERROR_RETURN(("invalid weapon type\t[%u %u]", id, pcloth->weapon_type), -1);
					}

					xmlNodePtr chl2 = chl1->xmlChildrenNode;
					while (chl2) {
						if ((!xmlStrcmp(chl2->name, (const xmlChar *)"Level"))) {
							DECODE_XML_PROP_INT (lv, chl2, "Lv");
							if (lv >= MAX_CLOTH_LEVL_CNT || pcloth->clothlvs[lv].valid) {
								ERROR_RETURN(("invalid or duplicate cloth lvl: id=%u lvl=%u", id, lv), -1);
							}

							pcloth->clothlvs[lv].valid = 1;
							load_cloth_lvinfo(chl2, &pcloth->clothlvs[lv]);
						}

						if ((!xmlStrcmp(chl2->name, (const xmlChar *)"Produce"))) {
							DECODE_XML_PROP_INT (lv, chl2, "Extent");
							if (lv > PRODUCT_LV_CNT || pcloth->pro_attr[lv - 1].valid) {
								ERROR_RETURN(("invalid or duplicate cloth lvl: id=%u lvl=%u %u", id, lv, pcloth->pro_attr[lv - 1].valid), -1);
							}

							switch (lv) {
								case 1:
									pcloth->pro_rate[lv - 1] = 1000;
									break;
								case 2:
									if (!pcloth->pro_attr[0].valid) {
										KERROR_LOG(0, "ttt\t[%u]", pcloth->id);
										return -1;
									}
									pcloth->pro_rate[lv - 1] = 350;
									pcloth->pro_rate[0] -= 350;
									break;
								case 3:
									if (!pcloth->pro_attr[0].valid) {
										KERROR_LOG(0, "ttt\t[%u]", pcloth->id);
										return -1;
									}
									pcloth->pro_rate[lv - 1] = 150;
									pcloth->pro_rate[0] -= 150;
									break;
								case 4:
									break;
								default:
									return -1;
							}

							pcloth->pro_attr[lv - 1].valid = 1;
							load_cloth_proinfo(chl2, &pcloth->pro_attr[lv - 1]);
						}
						
						chl2 = chl2->next;
					}
				
					if (!pcloth->clothlvs[0].valid) {
						ERROR_RETURN(("no 0 lv cloth\t[%u]", id), -1);
					}
				}
				
				chl1 = chl1->next;
			}
		}

		if (!xmlStrcmp(cur->name, (const xmlChar*)"CrystalAttr")) {
			xmlNodePtr chl1 = cur->xmlChildrenNode;
			while (chl1) {
				if (!xmlStrcmp(chl1->name, (const xmlChar*)"Type")) {
					uint32_t id;
					DECODE_XML_PROP_UINT32(id, chl1, "ID");
					if (!id || id >= attr_id_max)
						ERROR_RETURN(("crystal attr id invalid\t[%u]", id), -1);
					xmlNodePtr chl2 = chl1->xmlChildrenNode;
					while (chl2) {
						if (!xmlStrcmp(chl2->name, (const xmlChar*)"Attr")) {
							uint32_t id2;
							DECODE_XML_PROP_UINT32(id2, chl2, "EquipColor");
							if (id2 >= CLOTH_QUALITY_CNT)
								ERROR_RETURN(("equip color invalid\t[%u %u]", id, id2), -1);

							decode_xml_prop_arr_int_default((int *)crystal_attr[id - 1][id2], CLOTH_LV_PHASE,chl2, "AddValue", 0);
						}
						chl2 = chl2->next;
					}
				}
				chl1 = chl1->next;
			}
		}
		cur = cur->next;
	}


	memset (drop_clothes, 0, sizeof(drop_clothes));
	memset (cloth_drop_rate, 0, sizeof(cloth_drop_rate));

	for (uint32_t loop = 0; loop < 3; loop ++) {
		cloth_drop_rate[loop][3] = CLOTH_COMMON_RAND_BASE;
	}


	cloth_drop_rate[3][0] = 250;
	cloth_drop_rate[3][1] = 150;
	cloth_drop_rate[3][2] = 50;
	cloth_drop_rate[3][3] = 0;
	cloth_drop_rate[3][4] = 9550;
	/*
	cloth_drop_rate[3][0] = 500;
	cloth_drop_rate[3][1] = 300;
	cloth_drop_rate[3][2] = 100;
	cloth_drop_rate[3][3] = 0;
	cloth_drop_rate[3][4] = 100;
	*/


	for (uint32_t loop = 4; loop < CLOTH_LV_PHASE; loop ++) {
		cloth_drop_rate[loop][0] = cloth_drop_rate[loop - 1][0] >= 25 ? (cloth_drop_rate[loop - 1][0] - 25) : 0;
		cloth_drop_rate[loop][1] = cloth_drop_rate[loop - 1][1] >= 15 ? (cloth_drop_rate[loop - 1][1] - 15) : 0;
		cloth_drop_rate[loop][2] = cloth_drop_rate[loop - 1][2] >= 5 ? (cloth_drop_rate[loop - 1][2] - 5) : 0;
		cloth_drop_rate[loop][3] = cloth_drop_rate[loop - 1][3];
		cloth_drop_rate[loop][4] = cloth_drop_rate[loop - 1][4] + 45;
	}

	uint32_t clothratio[MAX_ITEMS_WITH_BODY] = {7, 5, 7, 7, 5, 4, 0, 0};
	uint32_t clothcnt[CLOTH_LV_PHASE] = {0};
	GList* pcloths = g_hash_table_get_values(all_clothes);
	GList* head = pcloths;
	while (pcloths) {
		cloth_t* pc = (cloth_t *)pcloths->data;
		if(pc->id >= 140001 && pc->id <= 149999 && pc->minlv >= 15 \
			&& pc->equip_part <= MAX_ITEMS_WITH_BODY && clothratio[pc->equip_part - 1]) {
			for (uint32_t loop = 0; loop < clothratio[pc->equip_part - 1] && clothcnt[pc->minlv / 5] < 100; loop ++) {
				drop_clothes[pc->minlv / 5][clothcnt[pc->minlv / 5] ++] = pc->id;
			}
		}
		pcloths = pcloths->next;
	}
	g_list_free(head);

	return 0;
}

suit_t* get_suit_no_check(uint32_t sid)
{
	return (!sid || sid > MAX_SUIT_NUM) ? NULL : &suits[sid - 1];
}

suit_t* get_suit(uint32_t sid)
{
	suit_t* ps = get_suit_no_check(sid);
	return ps && ps->sid ? ps : NULL;
}

void load_add_attr(base_attr_t* pba, xmlNodePtr cur)
{
	decode_xml_prop_uint32_default((uint32_t *)&pba->hp_max, cur, "HpMax", 0);
	decode_xml_prop_uint32_default((uint32_t *)&pba->mp_max, cur, "Mp", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->attack, cur, "Atk", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->defense, cur, "Def", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->mdefense, cur, "Mdef", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->speed, cur, "Speed", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->spirit, cur, "Spirit", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->resume, cur, "Resume", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->hit, cur, "Hit", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->dodge, cur, "Dodge", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->crit, cur, "Crit", 0);
	decode_xml_prop_uint16_default((uint16_t *)&pba->fightback, cur, "FightBack", 0);
}

int add_cloth_suit(cloth_t* psc, uint32_t suitid)
{
	int loop = 0;
	for (loop = 0; loop < psc->suitcnt; loop ++) {
		if (psc->suitid[loop] == suitid)
			return 0;
	}
	
	if (psc->suitcnt >= MAX_ITEMS_WITH_BODY)
		return -1;
	
	psc->suitid[psc->suitcnt ++] = suitid;
	return 0;
}

int load_suit(xmlNodePtr cur)
{
	memset(suits, 0, sizeof(suits));
	uint32_t id;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Suit")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			suit_t* ps = get_suit_no_check(id);
			if (!ps || ps->sid)
				ERROR_RETURN(("invalid or duplicate suit id\t[%u]", id), -1);
			ps->sid = id;
			DECODE_XML_PROP_UINT32(ps->prof, cur, "Prof");
			ps->clothcnt = decode_xml_prop_arr_int_default((int *)ps->clothids, MAX_ITEMS_WITH_BODY, cur, "ItemID", 0);
			uint32_t loop;
			for (loop = 0; loop < ps->clothcnt; loop ++) {
				cloth_t* psc = get_cloth(ps->clothids[loop]);
				if (!psc) {
					ERROR_RETURN(("invalid cloth id\t[%u %u]", ps->sid, ps->clothids[loop]), -1);
				}
				if (add_cloth_suit(psc, id)) {
					KERROR_LOG(0, "cloth suit max\t[%u %u]", psc->id, id);
				}
			}
			load_add_attr(&ps->attr, cur);
		}
		cur = cur->next;
	}

	return 0;
}

int load_handbook(xmlNodePtr cur)
{
	uint32_t id;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Materials")) {
			DECODE_XML_PROP_UINT32(id, cur, "ItemID");

			cloth_t* pcloth = NULL;
			normal_item_t* pitem = NULL;
			int loop;
			if ((pcloth = get_cloth(id)) != NULL) {
				decode_xml_prop_uint32_default(&pcloth->lv, cur, "Level", 1);
				decode_xml_prop_uint16_default(&pcloth->itemlv, cur, "SyntheseLevel", 1);
				pcloth->material_cnt = 0;
				memset(pcloth->materials, 0, sizeof(pcloth->materials));
				int itmids[MAX_MATERIAL_CNT], cnts[MAX_MATERIAL_CNT];
				pcloth->material_cnt = decode_xml_prop_arr_int_default(itmids, MAX_MATERIAL_CNT, cur, "SyntheseMeterial", 0);
				if (pcloth->material_cnt != decode_xml_prop_arr_int_default(cnts, MAX_MATERIAL_CNT, cur, "Count", 0))
					ERROR_RETURN(("itm cnt not match\t[%u %u]", id, pcloth->material_cnt), -1);
				int loop;
				for (loop = 0; loop < pcloth->material_cnt; loop ++) {
					pcloth->materials[loop].itemid = itmids[loop];
					pcloth->materials[loop].count = cnts[loop];
				}
				int rate=0;
				for (loop = 0; loop < PRODUCT_LV_CNT; loop ++) {
					rate += pcloth->pro_rate[loop];
					if (pcloth->pro_rate[loop] && !pcloth->pro_attr[loop].valid)
						ERROR_RETURN(("rate attr not match"), -1);
				}
				if (rate != RAND_COMMON_RAND_BASE)
					ERROR_RETURN(("rate attr not match\t[%u %u]", id, rate), -1);
			} else if ((pitem = get_item(id)) != NULL) {
				decode_xml_prop_uint16_default(&pitem->itemlv, cur, "SyntheseLevel", 1);
				pitem->material_cnt = 0;
				memset(pitem->materials, 0, sizeof(pitem->materials));
				int itmids[MAX_MATERIAL_CNT], cnts[MAX_MATERIAL_CNT];
				pitem->material_cnt = decode_xml_prop_arr_int_default(itmids, MAX_MATERIAL_CNT, cur, "SyntheseMeterial", 0);
				if (pitem->material_cnt != decode_xml_prop_arr_int_default(cnts, MAX_MATERIAL_CNT, cur, "Count", 0))
					ERROR_RETURN(("itm cnt not match\t[%u %u]", id, pcloth->material_cnt), -1);
				for (loop = 0; loop < pitem->material_cnt; loop ++) {
					pitem->materials[loop].itemid = itmids[loop];
					pitem->materials[loop].count = cnts[loop];
				}
			} else {
				KERROR_LOG(0, "invalid itemid\t[%u]", id);
				//return -1;
			}
		}
		cur = cur->next;
	}

	return 0;
}


void init_all_items()
{
	all_items = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_item_by_key(gpointer key, gpointer p_item, gpointer userdata)
{
	g_slice_free1(sizeof(normal_item_t), p_item);
	return TRUE;
}

void fini_all_items()
{
	g_hash_table_destroy(all_items);
}

normal_item_t* get_item(uint32_t itemid)
{
	return (normal_item_t *)g_hash_table_lookup(all_items, &itemid);
}

int load_item_for_normal(gift_info_t* pgi, xmlNodePtr cur, uint32_t itemid)
{
	xmlNodePtr chll = cur->xmlChildrenNode; 
	while (chll) {
		if (!xmlStrcmp(chll->name, (const xmlChar *)"Goods")) {
			pgi->itemcnt = decode_xml_prop_arr_int_default((int *)pgi->itemids, MAX_GIFT_CNT, chll, "ID", 0);
			KTRACE_LOG(0, "LOAD ITEM GOOD\t[%u %u]", itemid, pgi->itemcnt);
			if (pgi->itemcnt != decode_xml_prop_arr_int_default((int *)pgi->item_cnt, MAX_GIFT_CNT, chll, "Cnt", 0))
				ERROR_RETURN(("itm cnt not match\t[%u]", itemid), -1);
		}
		
		if (!xmlStrcmp(chll->name, (const xmlChar *)"Clothes")) {
			pgi->clothcnt = decode_xml_prop_arr_int_default((int *)pgi->cloth_type, MAX_GIFT_CNT, chll, "ID", 0);
			decode_xml_prop_uint16_default(&pgi->tag, chll, "Tag",0);
			if (pgi->clothcnt != 
						decode_xml_prop_arr_int_default((int *)pgi->cloth_lv, MAX_GIFT_CNT, chll, "Lv", 0)){
				if(pgi->clothcnt != 
						decode_xml_prop_arr_int_default((int *)pgi->cloth_lv, MAX_GIFT_CNT, chll, "Extent", 0))
					ERROR_RETURN(("itm cnt not match\t[%u]", itemid), -1);
			}
		}

		chll = chll->next;
	}
	return 0;
}

int load_item_for_spec(normal_item_t* pnis, xmlNodePtr cur)
{
	xmlNodePtr chll = cur->xmlChildrenNode; 
	while (chll) {
		if (!xmlStrcmp(chll->name, (const xmlChar *)"Goods")) {
			DECODE_XML_PROP_UINT32(pnis->specgift.ids[pnis->specgift.count], chll, "ID");
			DECODE_XML_PROP_UINT32(pnis->specgift.rates[pnis->specgift.count], chll, "Rate");
			int count[2];
			decode_xml_prop_arr_int_default(count, 2, chll, "Cnt", 1);
			count[1] = count[0] > count[1] ? count[0] : count[1];
			pnis->specgift.min[pnis->specgift.count] = count[0];
			pnis->specgift.max[pnis->specgift.count] = count[1];
			pnis->specgift.count ++;
		}
		chll = chll->next;
	}
	return 0;
}

int load_item_for_rand(normal_item_t* pnis, xmlNodePtr cur)
{
	xmlNodePtr chll = cur->xmlChildrenNode; 
	while (chll) {
		if (!xmlStrcmp(chll->name, (const xmlChar *)"RandGoods")) {
			DECODE_XML_PROP_UINT32(pnis->randgift.percents[pnis->randgift.grp_cnt], chll, "Percent");
			if (load_item_for_normal(&pnis->randgift.gifts[pnis->randgift.grp_cnt], chll, pnis->item_id))
				return -1;
			pnis->randgift.grp_cnt ++;
		}
		chll = chll->next;
	}

	uint32_t all_percent = 0;
	for (uint32_t loop = 0; loop < pnis->randgift.grp_cnt; loop ++) {
		all_percent += pnis->randgift.percents[loop];
	}
	if (all_percent != RAND_COMMON_RAND_BASE) {
		ERROR_RETURN (("percent not 1000\t[%u %u]", pnis->item_id, all_percent), -1);
	}
	return 0;
}

int load_item_for_rand_prof(normal_item_t* pnis, xmlNodePtr cur)
{
	xmlNodePtr chll = cur->xmlChildrenNode; 
	while (chll) {
		if (!xmlStrcmp(chll->name, (const xmlChar *)"RandGoods")) {
			DECODE_XML_PROP_UINT32(pnis->rand_prof_gift.prof[pnis->rand_prof_gift.grp_cnt], chll, "Prof");
			if (load_item_for_normal(&pnis->rand_prof_gift.gifts[pnis->rand_prof_gift.grp_cnt], chll, pnis->item_id))
				return -1;
			pnis->rand_prof_gift.grp_cnt ++;
		}
		chll = chll->next;
	}
	
	return 0;
}

int load_item_for_shapeshifting(normal_item_t* pnis, xmlNodePtr cur)
{
	xmlNodePtr chll = cur->xmlChildrenNode; 
	while (chll) {
		if (!xmlStrcmp(chll->name, (const xmlChar *)"Attr")) {
			DECODE_XML_PROP_UINT32(pnis->shapeshifting_info.pettype, chll, "BeastType");
			decode_xml_prop_int32_default(&pnis->shapeshifting_info.attr.hp_max, chll, "HpMax",0);
			decode_xml_prop_int32_default(&pnis->shapeshifting_info.attr.mp_max, chll, "MpMax",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.attack, chll, "Atk",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.mattack, chll, "Matk",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.defense, chll, "Def",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.mdefense, chll, "Mdef",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.speed, chll, "Speed",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.spirit, chll, "Spirit",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.resume, chll, "Resume",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.hit, chll, "Hit",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.dodge, chll, "Dodge",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.crit, chll, "Crit",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.fightback, chll, "FightBack",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.rpoison, chll, "ResistPoison",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.rlithification, chll, "ResistLithification",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.rlethargy, chll, "ResistLethargy",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.rinebriation, chll, "ResistInebriation",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.rconfusion, chll, "ResistConfusion",0);
			decode_xml_prop_int16_default(&pnis->shapeshifting_info.attr.roblivion, chll, "ResistOblivion",0);
		}
		chll = chll->next;
	}
	return 0;
}


int load_items(xmlNodePtr cur)
{
	g_hash_table_foreach_remove(all_items, free_item_by_key, 0);
	uint32_t itmid, overlay_max, total_max, type;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Kind")) {
			uint32_t kid;
			DECODE_XML_PROP_UINT32(kid, cur, "ID");
			if (kid < 10) {
				cur = cur->next;
				continue;
			}
			DECODE_XML_PROP_UINT32(overlay_max, cur, "Max");
			DECODE_XML_PROP_UINT32(type, cur, "Team");
			decode_xml_prop_uint32_default(&total_max, cur, "Maximum", -1);		
			xmlNodePtr chl = cur->xmlChildrenNode; 
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Item")) {
					DECODE_XML_PROP_INT (itmid, chl, "ID");
					if (itmid < 150000) {
						chl = chl->next;
						continue;
					}
					normal_item_t* pnis  = get_item(itmid);
					if (pnis) {
						ERROR_RETURN(("duplicate item id [iid=%u]", itmid), -1);
					}
					pnis = (normal_item_t *)g_slice_alloc0(sizeof(normal_item_t));
					pnis->item_id = itmid;
					g_hash_table_insert(all_items, &(pnis->item_id), pnis);
					pnis->overlay_max = overlay_max;
					pnis->total_max = total_max;
					pnis->type = type;
					
					decode_xml_prop_uint32_default (&pnis->day_max, chl, "DayMax", 0);
					decode_xml_prop_uint32_default (&pnis->price, chl, "Price", 0);
					decode_xml_prop_uint32_default (&pnis->sale_price, chl, "SalePrice", 0);
					if (pnis->price < pnis->sale_price) {
						KERROR_LOG(0, "item price invalid\t[%u %u %u]", pnis->item_id, pnis->price, pnis->sale_price);
						return -1;
					}
					decode_xml_prop_uint32_default (&pnis->tradablity, chl, "Tradability", 3);
					decode_xml_prop_uint32_default (&pnis->vip_tradablity, chl, "VipTradability", 3);
					decode_xml_prop_uint16_default (&pnis->vip_only, chl, "VipOnly", 0);
					decode_xml_prop_uint16_default (&pnis->vendibility, chl, "Vendibility", 0);
					decode_xml_prop_uint32_default (&pnis->minlv, chl, "UseLv", 1);
					decode_xml_prop_uint32_default (&pnis->maxlv, chl, "MaxLv", MAX_SPRITE_LEVEL);
					decode_xml_prop_uint32_default (&pnis->del_forbid, chl, "Del", 0);
					decode_xml_prop_uint32_default (&pnis->function, chl, "Type", 0);
					if (pnis->item_id >= 290007 && pnis->item_id <= 290010)
						pnis->function = item_for_exchange_normal;
					decode_xml_prop_uint32_default (&pnis->target, chl, "Target", 1);
					KTRACE_LOG(0, "ITEM ID\t[%u %u %u]", itmid, pnis->function, pnis->target);
					switch (pnis->function) {
						case item_for_hpmp:
							decode_xml_prop_uint32_default (&pnis->medinfo.hp_up, chl, "HpUp", 0);
							decode_xml_prop_uint32_default (&pnis->medinfo.mp_up, chl, "MpUp", 0);
							if (!pnis->medinfo.hp_up && !pnis->medinfo.mp_up)
								ERROR_RETURN(("item for hp mp no effect\t[%u]", itmid), -1);
							break;
						case item_for_reset_attr:
							decode_xml_prop_uint16_default ((uint16_t *)&pnis->attrinfo.physique, chl, "Physique", 0);
							decode_xml_prop_uint16_default ((uint16_t *)&pnis->attrinfo.strength, chl, "Strength", 0);
							decode_xml_prop_uint16_default ((uint16_t *)&pnis->attrinfo.endurance, chl, "Endurance", 0);
							decode_xml_prop_uint16_default ((uint16_t *)&pnis->attrinfo.quick, chl, "Quick", 0);
							decode_xml_prop_uint16_default ((uint16_t *)&pnis->attrinfo.intelligence, chl, "Intelligence", 0);
							pnis->attrinfo.attr_add = (!pnis->attrinfo.physique && !pnis->attrinfo.strength \
								&& !pnis->attrinfo.endurance && !pnis->attrinfo.quick && !pnis->attrinfo.intelligence) ? -1 : \
								-(pnis->attrinfo.physique + pnis->attrinfo.strength + pnis->attrinfo.endurance \
								+ pnis->attrinfo.quick + pnis->attrinfo.intelligence);
							if (pnis->attrinfo.attr_add < 0 && pnis->attrinfo.attr_add != -1)
								return -1;
							break;
						case item_for_handbook:
							DECODE_XML_PROP_UINT32 (pnis->handbook_id, chl, "HandBookID");
							if (!pnis->handbook_id) {
								KERROR_LOG(0, "hbid zero\t[%u]", pnis->handbook_id);
								return -1;
							}
							break;
						case item_for_pet_egg:
						case item_for_pet_exchange:
							pnis->function = item_for_pet_exchange;
							DECODE_XML_PROP_UINT32(pnis->petinfo.beastid, chl, "BeastType");
							DECODE_XML_PROP_UINT32(pnis->petinfo.beastlv, chl, "BeastLv");
							decode_xml_prop_hex_default(&pnis->petinfo.feed_type, chl, "FeedType",0);
							decode_xml_prop_uint32_default((uint32_t*)&pnis->petinfo.feed_count, chl, "FeedCount",0);
							decode_xml_prop_uint32_default((uint32_t*)&pnis->petinfo.rand_diff, chl, "RandDiff", rand_diff_default);
							decode_xml_prop_uint32_default((uint32_t*)&pnis->petinfo.rand_growth, chl, "RandGrowth", 0);
							break;
						case item_for_pet_catch:
							pnis->petcatch.racecnt = decode_xml_prop_arr_int_default(pnis->petcatch.races, MAX_RACE_TYPE, chl, "Race", MAX_RACE_TYPE);
							DECODE_XML_PROP_UINT32 (pnis->petcatch.cardlv, chl, "CardLV");
							break;
						case item_for_set_color:
							DECODE_XML_PROP_UINT32 (pnis->color, chl, "Color");
							break;
						case item_for_energy:
							DECODE_XML_PROP_UINT32 (pnis->energy_factor, chl, "EnergyFactor");
							break;
						case item_for_exp_buff:
						case item_for_pet_buff:
							DECODE_XML_PROP_UINT32 (pnis->exp_factor.factor, chl, "ExpFactor");
							DECODE_XML_PROP_UINT32 (pnis->exp_factor.count, chl, "ValidCount");
							break;
						case item_for_skill_buff:
							DECODE_XML_PROP_UINT32 (pnis->exp_factor.factor, chl, "SkillFactor");
							DECODE_XML_PROP_UINT32 (pnis->exp_factor.count, chl, "ValidCount");
							break;
						case item_for_exchange_normal:
							if (load_item_for_normal(&pnis->giftinfo, chl, pnis->item_id)) return -1;
							break;
						case item_for_exchange_spec:
							decode_xml_prop_uint16_default(&(pnis->specgift.quantities),chl,"Quantities",0);
							load_item_for_spec(pnis, chl);
							break;
						case item_for_exchange_rand:
							if (load_item_for_rand(pnis, chl)) return -1;
							break;
						case item_for_auto_hpmp:
							DECODE_XML_PROP_INT_DEFAULT(pnis->hpmp_facotr.hpfactor, chl, "MhpFactor", 0);
							DECODE_XML_PROP_INT_DEFAULT(pnis->hpmp_facotr.mpfactor, chl, "MmpFactor", 0);
							if (!pnis->hpmp_facotr.hpfactor && !pnis->hpmp_facotr.mpfactor)
								return -1;
							break;
						case item_for_auto_fight:
							DECODE_XML_PROP_UINT32(pnis->auto_fight_cnt, chl, "ValidCount");
							break;
						case item_for_broadcast:
							decode_xml_prop_uint32_default(&pnis->cast_usage.grpid, chl, "BeastGrp", 0);
							break;
						case item_for_race_skill:
							{
								decode_xml_prop_arr_int_default((int*)&pnis->race_skill_info.order, 3, chl, "SkillPercent", 0);
								xmlNodePtr son= chl->xmlChildrenNode;
								uint32_t loop=0;
								while(son){
									if(loop >= MAX_RACE_SKILL_TYPE)
										break;
									if (!xmlStrcmp(son->name, (const xmlChar *)"Race")){
										DECODE_XML_PROP_UINT32(pnis->race_skill_info.type_info[loop].raceid, son, "ID");
										decode_xml_prop_arr_int_default((int*)&pnis->race_skill_info.type_info[loop].skillid, 3, son, "Skill", 0);
										decode_xml_prop_arr_int_default((int*)&pnis->race_skill_info.type_info[loop].percent, 3, son, "SkillRate", 0);
									}
									son=son->next;
									loop++;
								}
							}
							break;
						case item_for_precious_stone:
							decode_xml_prop_uint32_default(&pnis->rate, chl, "AddRate", 0);
							break;
						case item_for_recover_duration:
						case item_for_recover_mduration:
							DECODE_XML_PROP_INT_DEFAULT(pnis->duration, chl, "Value", 0);
							break;
						case item_for_comp_clothes:
							DECODE_XML_PROP_ARR_INT((pnis->comp_clothes_info.proc_rate), PRODUCT_LV_CNT, chl, "ProcRate"); 
							DECODE_XML_PROP_ARR_INT((pnis->comp_clothes_info.limit_lv), 2, chl, "LimitLv");
							break;
						case item_for_exchange_rand_prof:
							if (load_item_for_rand_prof(pnis, chl)) return -1;
							break;
						case item_for_shapeshifting:
							if (load_item_for_shapeshifting(pnis, chl)) return -1;
							break;
						case item_for_pet_exchange_egg:
							decode_xml_prop_uint32_default(&pnis->add_times, chl, "AddTimes", 1);
							break;
					}
					if (pnis->function == item_for_exchange_normal \
						&& (!pnis->giftinfo.itemcnt && !pnis->giftinfo.clothcnt))
						KERROR_LOG(0, "item for exchange not effect\t[%u]", itmid);
				}
				chl = chl->next;
			}
		}
	
		cur = cur->next;
	}

	return 0;
}


int load_exchange_info(xmlNodePtr cur)
{
	memset(exchange_infos, 0, sizeof(exchange_infos));
	uint32_t id;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Exchange")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			exchange_info_t* pei = get_exchange_info_no_check(id);
			if (!pei || pei->id)
				ERROR_RETURN(("invalid or duplicate exchange id\t[%u]", id), -1);
			pei->id = id;
			DECODE_XML_PROP_UINT32(pei->minlv, cur, "RoleLv");
			int itmids[MAX_EXCHANGE_ITEM_TYPE], cnts[MAX_EXCHANGE_ITEM_TYPE];
			pei->paycnt = decode_xml_prop_arr_int_default(itmids, MAX_EXCHANGE_ITEM_TYPE, cur, "PayItemID", 0);
			if (pei->paycnt != decode_xml_prop_arr_int_default(cnts, MAX_EXCHANGE_ITEM_TYPE, cur, "PayItemCnt", 0))
				ERROR_RETURN(("itemid cnt not match\t[%u %u]", id, pei->paycnt), -1);
			int loop;
			for (loop = 0; loop < pei->paycnt; loop ++) {
				if (!IS_ITEM_SPECIAL(itmids[loop]) && !get_item(itmids[loop]))
					ERROR_RETURN(("invalid itemid\t[%u %u]", id, itmids[loop]), -1);
				pei->payitems[loop].itemid = itmids[loop];
				pei->payitems[loop].count = cnts[loop];
			}
			decode_xml_prop_uint32_default(&pei->just_once, cur, "JustOnce", 0);
			decode_xml_prop_uint32_default(&pei->day_limit, cur, "Day", 0);
			decode_xml_prop_uint32_default(&pei->limit_cnt, cur, "MaxCount", 0);
		    uint32_t tmp=0;
			decode_xml_prop_uint32_default(&pei->start_time, cur, "StartHour", 0);
			pei->start_time *= 3600;
			decode_xml_prop_uint32_default(&tmp, cur, "StartMinute", 0);
		    pei->start_time += tmp*60;
			decode_xml_prop_uint32_default(&pei->end_time, cur, "EndHour", 24);
		    pei->end_time *= 3600;
			decode_xml_prop_uint32_default(&tmp, cur, "EndMinute", 0);
		    pei->end_time += tmp*60;
			pei->getcnt = decode_xml_prop_arr_int_default(itmids, MAX_EXCHANGE_ITEM_TYPE, cur, "GetItemID", 0);
			if (pei->getcnt != decode_xml_prop_arr_int_default(cnts, MAX_EXCHANGE_ITEM_TYPE, cur, "GetItemCnt", 0))
				ERROR_RETURN(("itemid cnt not match\t[%u %u]", id, pei->getcnt), -1);
			pei->type = IS_ITEM_SPECIAL(itmids[0]) ? item_special : 0;
			pei->type = get_item(itmids[0]) ? item_normal : pei->type;
			pei->type = get_cloth(itmids[0]) ? item_cloth : pei->type;
			if (pei->type == item_cloth) {
				decode_xml_prop_arr_int_default((int *)pei->extents, MAX_EXCHANGE_ITEM_TYPE, cur, "Extent", 0);
				for (loop = 0; loop < pei->getcnt; loop ++) {
					cloth_t* pc = get_cloth(itmids[loop]);
					if (!pc || pei->extents[loop] > PRODUCT_LV_CNT+1 || (pei->extents[loop] && !pc->pro_attr[pei->extents[loop] - 1].valid)) {
						ERROR_RETURN(("pro extent invalid\t[%u %u]", id, pei->extents[loop]), -1);
					}
				}
			}
			for (loop = 0; loop < pei->getcnt; loop ++) {
				if ((pei->type == item_special && !IS_ITEM_SPECIAL(itmids[loop])) \
					|| (pei->type == item_normal && !get_item(itmids[loop])))
					ERROR_RETURN(("invalid itemid\t[%u %u]", id, itmids[loop]), -1);
				pei->getitems[loop].itemid = itmids[loop];
				pei->getitems[loop].count = cnts[loop];
			}
			KTRACE_LOG(0, "EXCHANGE INFO\t[%u %u %u %u %u]", pei->id, pei->type, pei->minlv, pei->getcnt, pei->paycnt);

		}
		cur = cur->next;
	}

	return 0;
}

void init_rand_infos()
{
	rand_infos = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_rand_by_key(gpointer key, gpointer rand, gpointer userdata)
{
	g_slice_free1(sizeof(rand_info_t), rand);
	return TRUE;
}

void fini_rand_infos()
{
	g_hash_table_destroy(rand_infos);
}

rand_info_t* get_rand_info(uint32_t rand_id)
{
	return (rand_info_t *)g_hash_table_lookup(rand_infos, &rand_id);
}

int load_rand_item(xmlNodePtr cur)
{
	g_hash_table_foreach_remove(rand_infos, free_rand_by_key, 0);
	uint32_t id;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Rand")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			rand_info_t* p_ri = get_rand_info(id);
			if (p_ri) ERROR_RETURN(("invalid or duplicate item kind id: %u", id), -1);
			p_ri = (rand_info_t *)g_slice_alloc0(sizeof(rand_info_t));
			p_ri->rand_id = id;
			g_hash_table_insert(rand_infos, &p_ri->rand_id, p_ri);
			DECODE_XML_PROP_UINT32(p_ri->max_cnt, cur, "Time");
			DECODE_XML_PROP_UINT32(p_ri->repeattype, cur, "RepeatType");
			decode_xml_prop_uint32_default(&p_ri->startday, cur, "StartDay", 0);
			decode_xml_prop_uint32_default(&p_ri->needitem, cur, "ItemID", 0);
			decode_xml_prop_uint16_default(&p_ri->minlv, cur, "MinRoleLv", 1);
			decode_xml_prop_uint16_default(&p_ri->maxlv, cur, "MaxRoleLv", MAX_SPRITE_LEVEL);
			decode_xml_prop_uint16_default(&p_ri->speccnt, cur, "SpecCnt", 0);
			decode_xml_prop_uint32_default(&p_ri->redirectlv, cur, "Lv", 0);
			decode_xml_prop_uint32_default(&p_ri->redirectid, cur, "To", 0);
			decode_xml_prop_uint32_default(&p_ri->beastgrp, cur, "GrpID", 0);
			p_ri->mapcnt=(uint8_t)decode_xml_prop_arr_int_default((int*)p_ri->mapid, MAX_RAND_ITEM_MAPID, cur, "MapID", 0);
			if(p_ri->mapcnt>MAX_RAND_ITEM_MAPID)
			{
				ERROR_RETURN(("rand info: mapid cnt > [%u] randid [%u]",MAX_RAND_ITEM_MAPID,id),-1);
			}
			if (p_ri->needitem) {
				DECODE_XML_PROP_INT(p_ri->needcnt, cur, "ItemCnt");
				p_ri->needcnt = -p_ri->needcnt;
			}

			xmlNodePtr chl = cur->xmlChildrenNode;
			uint32_t share_rate = 0;
			uint8_t have_cloth = 0, have_item = 0;
			while (chl) {
				if (p_ri->type_cnt >= MAX_RAND_ITEM_TYPE)
					ERROR_RETURN(("item id or count invalid\t[%u]", id), -1);
				if (!xmlStrcmp(chl->name, (const xmlChar*)"Item")) {
					rand_item_t* pitem = &p_ri->items[p_ri->type_cnt];
					DECODE_XML_PROP_UINT32(pitem->item_id, chl, "ID");
					if (get_item(pitem->item_id)) {
						DECODE_XML_PROP_UINT32(pitem->item_cnt, chl, "ItemCnt");
						have_item = 1;
					} else {
						DECODE_XML_PROP_INT_DEFAULT(pitem->item_cnt, chl, "EquipLv", 0);
						cloth_t* pcloth = get_cloth(pitem->item_id);
						if (!pcloth || pitem->item_cnt >= MAX_CLOTH_LEVL_CNT || !pcloth->clothlvs[pitem->item_cnt].valid) {
							ERROR_RETURN(("item id or count invalid\t[%u %u]", pitem->item_id, pitem->item_cnt), -1);
						}
						have_cloth = 1;
					}
					DECODE_XML_PROP_UINT32(p_ri->rates[p_ri->type_cnt], chl, "ShareRate");
					DECODE_XML_PROP_UINT32(p_ri->items[p_ri->type_cnt].get_rate, chl, "GetRate");
					share_rate += p_ri->rates[p_ri->type_cnt];
					p_ri->type_cnt ++;
				}
				chl = chl->next;
			}

			KTRACE_LOG(0, "RAND ITEM SP\t[%u %u %u %u %u]", p_ri->rand_id, id, p_ri->needitem, p_ri->needcnt, p_ri->type_cnt);
			if (share_rate != RAND_COMMON_RAND_BASE) {
				ERROR_RETURN(("share rate err\t[%u %u %u]", p_ri->rand_id, p_ri->type_cnt, share_rate), -1);
			}

			if (have_cloth && have_item) 
				p_ri->rand_type = mix_item_cloth;
			else if (have_cloth)
				p_ri->rand_type = only_cloth;
			else if (have_item)
				p_ri->rand_type = only_item;
			else
				return -1;
		}
	
		cur = cur->next;
	}

	return 0;
}

int buy_cloth_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t shopid, cloth_type, cloth_levl;
	int j = 0;
	UNPKG_UINT32(body, shopid, j);
	UNPKG_UINT32(body, cloth_type, j);
	UNPKG_UINT32(body, cloth_levl, j);

	KDEBUG_LOG(p->id, "BUY CLOTH\t[%u %u]", cloth_type, cloth_levl);
	cloth_t* psc = get_cloth(cloth_type);
	CHECK_CLOTH_LV_VALID(p, psc, cloth_type, cloth_levl);
	CHECK_USER_IN_MAP(p, 0);

	map_id_t mapid = p->tiles->id;
	if (IS_HOME_MAP(mapid)) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	} else if (IS_MAZE_MAP(mapid)) {
		if( p->tiles->shop_id != shopid) {
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		mapid = 0;
	}

	if (!item_shop_existed(shopid, cloth_type, mapid)) {
		KERROR_LOG(p->id, "item shop not exsited\t[%u %u %lu]", shopid, cloth_type, mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	
	cloth_lvinfo_t* pclv = &psc->clothlvs[cloth_levl];
	if (!itm_buyable(pclv->tradablity)
		&& !(ISVIP(p->flag) && itm_vipbuyable(pclv->tradablity))) {
		KERROR_LOG(p->id, "cloth can't buy\t[%u %u %u]", cloth_type, cloth_levl, p->flag);
		return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_buy, 1);
	}

	if (get_bag_cloth_cnt(p) >= get_max_grid(p)) {
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}

	CHECK_XIAOMEE_ENOUGH(p, pclv->price);

	return db_add_cloth(p, p->id, pclv->price, psc, cloth_levl, pclv);
}

int rsp_get_hero_team_reward_cloth(sprite_t* p, body_cloth_t* pc)
{
	cli_get_hero_team_reward_out cli_out;
	cli_out.rank = *(uint32_t *)p->session;

/*
	cli_out->cloth.clothid = pc->clothid;
	cli_out->cloth.clothtype = pc->clothtype;
	cli_out->cloth.grid = pc->grid;
	cli_out->cloth.clothlv = pc->clothlv;
	cli_out->cloth.quality = pc->quality;
	cli_out->cloth.validday = pc->validday;
	cli_out->cloth.duration_max = pc->duration_max;
	cli_out->cloth.duration = pc->duration;
	cli_out->cloth.hpmax = pc->hp_max;
	cli_out->cloth.mpmax = pc->mp_max;
	cli_out->cloth.attack = pc->attack;
	cli_out->cloth.mattack = pc->mattack;
	cli_out->cloth.defense = pc->defense;
	cli_out->cloth.mdef = pc->mdefense;
	cli_out->cloth.speed = pc->speed;
	cli_out->cloth.spirit = pc->spirit;
	cli_out->cloth.resume = pc->resume;
	cli_out->cloth.hit = pc->hit;
	cli_out->cloth.dodge = pc->dodge;
	cli_out->cloth.crit = pc->crit;
	cli_out->cloth.fightback = pc->fightback;
	cli_out->cloth.rpoison = pc->rpoison;
	cli_out->cloth.rcommination = pc->rinebriation;
	cli_out->cloth.rconfusion = pc->rconfusion;
	cli_out->cloth.rlethargy = pc->rlethargy;
	cli_out->cloth.rlithification = pc->rlithification;
	cli_out->cloth.roblivion = pc->roblivion;
	cli_out->cloth.crystal_attr = 0;
	cli_out->cloth.blesstype = 0;
	cli_out->cloth.addval = 0;
	cli_out->cloth.isactivate = 0;
	*/
	cli_out.clothtype = pc->clothtype;

	if (cli_out.rank == 1) {
		cache_add_kind_item(p, 310008, 1);
		db_add_item(NULL, p->id, 310008, 1);
		stru_item tmpitem;
		tmpitem.itemid = 310008;
		tmpitem.count = 1;
		cli_out.itemlist.push_back(tmpitem);
	}

	db_set_flag(NULL, p->id, flag_herocup_reward, 1);
	p->flag |= 1 << flag_herocup_reward;
	
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int rsp_draw_once_get_item(sprite_t* p, body_cloth_t* pc)
{

	if (p->vip_draw_cnt_day >= DRAW_GIFT_CNT) {
		p->vip_draw_cnt_gift_use ++;
		db_day_add_ssid_cnt(NULL, p->id, item_id_vip_draw_give, 1, -1);
	} else {
		p->vip_draw_cnt_day ++;
		p->vip_draw_cnt_total ++;
		db_day_add_ssid_cnt(NULL, p->id, item_id_vip_draw_day, 1, -1);
	}

	item_t* pday = cache_get_day_limits(p, ssid_moqi_ride);
	pday->count ++;
	db_day_add_ssid_cnt(NULL, p->id, ssid_moqi_ride, 1, -1);
	cli_draw_once_out cli_out;
	stru_cloth_detail tmpcloth;

	tmpcloth.clothid = pc->clothid;
	tmpcloth.clothtype = pc->clothtype;
	tmpcloth.grid = pc->grid;
	tmpcloth.clothlv = pc->clothlv;
	tmpcloth.quality = pc->quality;
	tmpcloth.validday = pc->validday;
	tmpcloth.duration_max = pc->duration_max;
	tmpcloth.duration = pc->duration;
	tmpcloth.hpmax = pc->hp_max;
	tmpcloth.mpmax = pc->mp_max;
	tmpcloth.attack = pc->attack;
	tmpcloth.mattack = pc->mattack;
	tmpcloth.defense = pc->defense;
	tmpcloth.mdef = pc->mdefense;
	tmpcloth.speed = pc->speed;
	tmpcloth.spirit = pc->spirit;
	tmpcloth.resume = pc->resume;
	tmpcloth.hit = pc->hit;
	tmpcloth.dodge = pc->dodge;
	tmpcloth.crit = pc->crit;
	tmpcloth.fightback = pc->fightback;
	tmpcloth.rpoison = pc->rpoison;
	tmpcloth.rcommination = pc->rinebriation;
	tmpcloth.rconfusion = pc->rconfusion;
	tmpcloth.rlethargy = pc->rlethargy;
	tmpcloth.rlithification = pc->rlithification;
	tmpcloth.roblivion = pc->roblivion;
	tmpcloth.crystal_attr = 0;
	tmpcloth.blesstype = 0;
	tmpcloth.addval = 0;
	tmpcloth.isactivate = 0;
	cli_out.clothlist.push_back(tmpcloth);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int rsp_get_pet_skill_reward_cloth(sprite_t* p, body_cloth_t* pc)
{
	cli_get_pet_skill_reward_out cli_out;
	stru_item tmpitem;
	tmpitem.itemid = pc->clothtype;
	tmpitem.count = 1;
	cli_out.itemlist.push_back(tmpitem);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}


int buy_cloth_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{	
	CHECK_DBERR(p, ret);
	uint32_t xiaomee_add;
	int j = 0;
	UNPKG_H_UINT32(buf, p->xiaomee, j);
	UNPKG_H_UINT32(buf, xiaomee_add, j);

	monitor_sprite(p, "BUY CLOTH");
	
	body_cloth_t* pc = (body_cloth_t *)(buf + j);
	cache_add_bag_cloth (p, pc);

	cloth_t* pcloth = NULL;
	switch (p->waitcmd) {
	case proto_cli_buy_cloth:
		break;
	case cli_get_rand_item_cmd:
	case cli_get_rand_item_spec_cmd:
		return send_request_to_db(p, p->id, proto_db_rand_info_update, &((rand_info_t *)(p->session + 8))->rand_id, 4);
	case proto_cli_get_vip_item:
		return send_request_to_db(p, p->id, proto_db_rand_info_update, &((vip_item_t *)(p->session + 8))->vid, 4);
	case proto_cli_composite_cloth:
		pcloth = *(cloth_t **)p->session;
		*(uint32_t *)p->session = pc->clothid;
		*(uint32_t *)(p->session + 4) = pc->clothtype;
		msg_log_composite_cloth(pc->clothtype);
		return db_reduce_item_list(p, p->id, pcloth->materials, pcloth->material_cnt);
	case proto_cli_just_for_online:
		p->waitcmd = 0;
		return 0;
	case cli_get_hero_team_reward_cmd:
		return rsp_get_hero_team_reward_cloth(p, pc);
	case cli_draw_once_cmd:
		return rsp_draw_once_get_item(p, pc);
	case cli_get_pet_skill_reward_cmd:
		return rsp_get_pet_skill_reward_cloth(p, pc);
	default:
		return -1;
	}

	int k = sizeof (protocol_t);
	k += pkg_cloth(pc, msg + k);
	PKG_UINT32(msg, p->xiaomee, k);
	PKG_UINT32(msg, xiaomee_add, k);
	init_proto_head (msg, p->waitcmd, k);
	return send_to_self (p, msg, k, 1);
}

int cli_sell_clothes(sprite_t* p, Cmessage* c_in)
{
	cli_sell_clothes_in* p_in = P_IN;
	int k = 4;
	uint8_t out[32] = {0};
	uint32_t* p_xiaomee = (uint32_t *)out;

	CHECK_NOT_IN_TRADE(p); 
	CHECK_INT_GE(p_in->clothlist.size(), 1);
	PKG_H_UINT32(out, p_in->clothlist.size(), k);

	for (uint32_t i = 0; i < p_in->clothlist.size(); i ++) {
		body_cloth_t* p_bc = cache_get_bag_cloth(p, p_in->clothlist[i]);
		if (!p_bc) {
			return send_to_self_error(p, p->waitcmd, cli_err_cloth_id_invalid, 1);
		}
		cloth_t* pcloth = get_cloth(p_bc->clothtype);
		CHECK_CLOTH_LV_VALID(p, pcloth, p_bc->clothtype, p_bc->clothlv);
		cloth_lvinfo_t* pclv = &pcloth->clothlvs[p_bc->clothlv];
		if (!itm_salable(pclv->tradablity)
			&& !(ISVIP(p->flag) && itm_vipsalable(pclv->tradablity))) {
			return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_sell, 1);
		}
		PKG_H_UINT32(out, p_in->clothlist[i], k);
		p_bc->duration_max = p_bc->duration_max == 0 ? 1 : p_bc->duration_max;
		*p_xiaomee += pclv->sell_price * p_bc->duration / p_bc->duration_max;
		KDEBUG_LOG(p->id,"SELL CLOTH [id=%u]",p_in->clothlist[i]);
	}

	return send_request_to_db(p, p->id, proto_db_sell_cloths, out, k);
}

int sell_cloths_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t xiaomee, add_value, cloth_cnt, cloth_id;
	int i = sizeof(protocol_t), j = 0;

	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, add_value, j);
	UNPKG_H_UINT32(buf, cloth_cnt, j);

	CHECK_BODY_LEN(len, 12 + 4 * cloth_cnt);

	p->xiaomee = xiaomee;
	monitor_sprite(p, "SELL CLOTH");
	for (uint32_t loop = 0; loop < cloth_cnt; loop ++) {
		UNPKG_H_UINT32(buf, cloth_id, j);
		cache_del_bag_cloth(p, cloth_id);
		body_cloth_t* pbc = cache_get_cloth(p, cloth_id);
		if (pbc) {
			log_cloth_new(p,pbc,"sell_cloth");
		}
	}

	PKG_UINT32(msg, xiaomee, i);
	PKG_UINT32(msg, add_value, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int get_pakt_clothes_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	return db_get_packet_cloth_list(p);
	/*
	int i = sizeof(protocol_t) + 4;
	uint32_t clothcnt = 0;
	GList* pcloths = g_hash_table_get_values(p->all_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t *)pcloths->data;
		if (pc->grid < BASE_BODY_TYPE) {
			i += pkg_cloth(pc, msg + i);
			clothcnt ++;
		}
		pcloths = pcloths->next;
	}
	g_list_free(head);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, clothcnt, j);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
	*/
}

int response_pakt_cloth(sprite_t* p)
{
	int i = sizeof(protocol_t) + 4;
	uint32_t clothcnt = 0;
	GList* pcloths = g_hash_table_get_values(p->all_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t *)pcloths->data;
		if (pc->grid < BASE_BODY_TYPE) {
			i += pkg_cloth(pc, msg + i);
			clothcnt ++;
		}
		pcloths = pcloths->next;
	}
	g_list_free(head);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, clothcnt, j);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_pakt_clothes_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t cnt = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, cnt * sizeof(body_cloth_t) + 4);
	KDEBUG_LOG( p->id, "GET PAKT CLOTHES CALLBACK\t[%u]",cnt);
	add_user_bag_cloths(p, (body_cloth_t *)(buf + 4), cnt);
	switch(p->waitcmd) {
	case proto_cli_exchange_gift_normal:
		response_proto_uint32(p, p->waitcmd, *(uint32_t *)p->session, 1, 0);
		return 0;
	case cli_exchange_cmd:
		response_proto_uint32(p, p->waitcmd, p->pkpoint_total, 1, 0);
		return 0;
	case prof_compensation_cmd:
		response_proto_head(p, p->waitcmd, 1, 0);
		return 0;
	case proto_cli_get_pakt_cloth:
		return response_pakt_cloth(p);
	case cli_get_vip_active_2_cmd:
		p->waitcmd = 0;
		return 0;
	default:
		return -1;
	}
}

int get_body_clothes_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, g_hash_table_size(p->body_cloths), i);

	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t *)pcloths->data;
		i += pkg_cloth(pc, msg + i);
		pcloths = pcloths->next;
	}
	g_list_free(head);
	
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

#define PKG_H_BODY_CLOTH(b, v, j) \
		do { \
			*(body_cloth_t* *)((b)+(j)) = (v); (j) += sizeof(body_cloth_t *); \
		} while (0)

#define UNPKG_H_BODY_CLOTH(b, v, j) \
		do { \
			(v) = *(body_cloth_t* *)((b) + (j));(j) += sizeof(body_cloth_t *); \
		} while (0)

int cli_set_body_clothes(sprite_t* p, Cmessage* c_in)
{
	cli_set_body_clothes_in* p_in = P_IN;
	uint32_t buff[MAX_ITEMS_WITH_BODY * 8 + 1] = {0};
	uint8_t part[2][MAX_ITEMS_WITH_BODY] = {{0}, {0}};
	
	int addcnt = 0, bufidx = 4;
	*(uint32_t *)p->session = 0;
	uint32_t loop;
	for (loop = 0; loop < p_in->cloth_list.size(); loop++) {
		DEBUG_LOG("xxx %u %u",p_in->cloth_list[loop].clothid,p_in->cloth_list[loop].clothtype);
		cloth_t* psc = get_cloth(p_in->cloth_list[loop].clothtype);

		CHECK_CLOTH_LV_VALID(p, psc, p_in->cloth_list[loop].clothtype, p_in->cloth_list[loop].clothlv);
		CHECK_PROF_FIT(p, psc->prof, psc->proflv);
		CHECK_LV_FIT(p, p, psc->minlv, MAX_SPRITE_LEVEL);

		if (part[psc->equip_part / 100][psc->equip_part % 100 - 1]) {
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}

		part[psc->equip_part / 100][psc->equip_part % 100 - 1] = 1;

		body_cloth_t* pbc = cache_get_cloth(p, p_in->cloth_list[loop].clothid);
		if (!pbc) {
			KERROR_LOG(p->id, "cloth not exsit\t[%u]", p_in->cloth_list[loop].clothid);
			return send_to_self_error(p, p->waitcmd, cli_err_cloth_not_existed, 1);
		}
		
		buff[2 * buff[0] + 1] = p_in->cloth_list[loop].clothid;
		buff[2 * buff[0] + 2] = psc->equip_part + BASE_BODY_TYPE;
		buff[0] ++;

		if (pbc->grid > BASE_BODY_TYPE) {
			continue;
		}
		
		PKG_H_BODY_CLOTH(p->session, pbc, bufidx);
		PKG_H_UINT32(p->session, psc->equip_part + BASE_BODY_TYPE, bufidx);
		(*(uint32_t *)p->session) ++;

		if (!cache_get_body_cloth(p, psc->equip_part + BASE_BODY_TYPE)) {
			addcnt --;
		}
		
	}

	for (loop = 0; loop < MAX_ITEMS_WITH_BODY; loop ++) {
		body_cloth_t* pbodyc = cache_get_body_cloth(p, loop + 1 + BASE_BODY_TYPE);
		if (pbodyc && !part[0][loop]) {
			addcnt ++;
			PKG_H_BODY_CLOTH(p->session, pbodyc, bufidx);
			PKG_H_UINT32(p->session, 0, bufidx);
			(*(uint32_t *)p->session) ++;
		}

		pbodyc = cache_get_body_cloth(p, loop + 1 + BASE_BODY_TYPE + 100);
		if (pbodyc && !part[1][loop]) {
			addcnt ++;
			PKG_H_BODY_CLOTH(p->session, pbodyc, bufidx);
			PKG_H_UINT32(p->session, 0, bufidx);
			(*(uint32_t *)p->session) ++;
		}
	}

	KDEBUG_LOG(p->id, "SET CLOTHES\t[%u %d]", buff[0], addcnt);
	uint32_t bag_cloth_cnt = get_bag_cloth_cnt(p);
	if (bag_cloth_cnt + addcnt > get_max_grid(p)) {
		KERROR_LOG(p->id, "set cloth cnt err\t[%d %d]", bag_cloth_cnt, addcnt);
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}

	return send_request_to_db(p, p->id, proto_db_set_clothes_where, buff, buff[0] * 8 + 4);
}

int set_clothes_where_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int j = 0;
	uint32_t count;
	UNPKG_H_UINT32(p->session, count, j);
	for (uint32_t loop = 0; loop < count; loop ++) {
		body_cloth_t* pc;
		uint32_t grid;
		UNPKG_H_BODY_CLOTH(p->session, pc, j);
		UNPKG_H_UINT32(p->session, grid, j);
		if (grid) {
			body_cloth_t* pbc = cache_get_body_cloth(p, grid);
			if (pbc) {
				g_hash_table_remove(p->body_cloths, &pbc->grid);
				pbc->grid = 0;
			}

			pc->grid = grid;
			g_hash_table_insert(p->body_cloths, &pc->grid, pc);
		} else {
			g_hash_table_remove(p->body_cloths, &pc->grid);
			pc->grid = grid;
		}
	}
	cache_set_cloth_attr(p);
	modify_sprite_second_attr(p);
	notify_user_clothes(p);

	//uint8_t buff[256];
	//int i = sizeof(protocol_t), len = i + 8;
	//uint32_t player_cnt =p->btr_team?p->btr_team->count:1;
	//PKG_UINT32(buff, p->id, i);

	//PKG_UINT32(buff, p->id, len);
	//PKG_UINT32(buff, p->level, len);
	//PKG_UINT32(buff, p->hp, len);
	//PKG_UINT32(buff, p->hp_max, len);
	//PKG_UINT32(buff, p->mp, len);
	//PKG_UINT32(buff, p->mp_max, len);
	//PKG_UINT32(buff, p->injury_lv, len);
	//if(p->btr_team ){
		//for (int loop = 1; loop < p->btr_team->count; loop ++) {
			//sprite_t* lp = p->btr_team->players[loop];
			//if (lp) {
				//PKG_UINT32(buff, lp->id, len);
				//PKG_UINT32(buff, lp->level, len);
				//PKG_UINT32(buff, lp->hp, len);
				//PKG_UINT32(buff, lp->hp_max, len);
				//PKG_UINT32(buff, lp->mp, len);
				//PKG_UINT32(buff, lp->mp_max, len);
				//PKG_UINT32(buff, lp->injury_lv, len);
			//}
		//}
	//}
	//PKG_UINT32(buff, player_cnt, i);
	//init_proto_head(buff, proto_cli_noti_team_state, len);
	//send_to_team(p,buff,len);

	p->waitcmd = 0;
	return 0;
}

int del_cloth_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t cloth_id;
	int j = 0;
	CHECK_NOT_IN_TRADE(p); 
	UNPKG_UINT32(body, cloth_id, j);
	cloth_t* pc = get_cloth(cloth_id);
	if (!pc || pc->del_forbid) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_del, 1);
	}

	KDEBUG_LOG(p->id, "DEL CLOTH\t[%u]", cloth_id);

	return db_del_cloth(p, p->id, cloth_id);
}

int del_cloth_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t cloth_id;
	int j = 0;
	UNPKG_H_UINT32 (buf, cloth_id, j);
	response_proto_head(p, p->waitcmd, 1, 0);
	body_cloth_t* pbc = cache_get_bag_cloth(p, cloth_id);
	if (pbc) {
		uint32_t grid = pbc->grid;
		cache_del_bag_cloth(p, cloth_id);
		if (grid > BASE_BODY_TYPE) {
			calc_sprite_second_level_attr(p);
			cache_set_cloth_attr(p);
			notify_user_clothes(p);
		}
	}
	return 0;
}

int del_normal_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid;
	int32_t count;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, count, j);

	KDEBUG_LOG(p->id, "DEL NORMAL ITEM\t[%u %u]", itemid, count);
	CHECK_INT_GE(count, 1);

	CHECK_NOT_IN_TRADE(p); 
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);

	if (pni->del_forbid) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_del, 1);
	}
	
	CHECK_ITEM_EXIST(p, itemid, count);
	db_add_item(NULL, p->id, itemid, -count);
	cache_reduce_kind_item(p, itemid, count);
	KF_LOG("del_item",p->id,"itemid:%u itemcnt:%u",itemid,count);
	response_proto_uint32_uint32(p, p->waitcmd, itemid, get_item_cnt(p, pni), 1, 0);
	return 0;
}

int del_house_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	item_t itm;
	int j = 0;
	UNPKG_UINT32(body, itm.itemid, j);
	UNPKG_UINT32(body, itm.count, j);
	CHECK_INT_GE(itm.count, 1);

	KDEBUG_LOG(p->id, "DEL HOUSE NORMAL ITEM\t[%u  %u]", itm.itemid, itm.count);

	itm.count = 0 - itm.count;

	normal_item_t* pni = get_item(itm.itemid);
	CHECK_ITEM_VALID(p, pni, itm.itemid);

	if (pni->del_forbid) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_del, 1);
	}

	KF_LOG("del_item",p->id,"itemid:%u itemcnt:%u",itm.itemid,itm.count);
	return send_request_to_db(p, p->id, proto_db_add_house_item, &itm, sizeof(itm));
}


int buy_normal_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid, shopid, xiaomee;
	int32_t count;
	uint8_t buf[1024];
	int i = 0, j = 0;

	UNPKG_UINT32(body, shopid, j);
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, count, j);

	CHECK_INT_GE(count, 1);
	CHECK_USER_IN_MAP(p, 0);

	map_id_t mapid = p->tiles->id;
	if (IS_HOME_MAP(mapid)) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	} else if (IS_MAZE_MAP(mapid)) {
		if( p->tiles->shop_id != shopid) {
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		mapid = 0;
	}

	if (!item_shop_existed(shopid, itemid, mapid)) {
		KERROR_LOG(p->id, "item shop not exsited\t[%u %u %lu]", shopid, itemid, mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	normal_item_t* pni = get_item(itemid);
	KDEBUG_LOG(p->id, "BUY NORMAL ITEM\t[%u %u]", itemid, count);
	CHECK_ITEM_VALID(p, pni, itemid);

	uint32_t item_cnt = get_item_cnt(p, pni);
	if (count + item_cnt > pni->total_max) {
		return send_to_self_error (p, p->waitcmd, cli_err_item_cnt_max, 1);
	}

	if (!can_have_item(p, pni, get_need_grid(pni, item_cnt, item_cnt + count))) {
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}

	if (!itm_buyable(pni->tradablity)
		&& !(ISVIP(p->flag) && itm_vipbuyable(pni->tradablity))) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_buy, 1);
	}

	xiaomee = pni->price * count;
	CHECK_XIAOMEE_ENOUGH(p, xiaomee);
	
	PKG_H_UINT32(buf, itemid, i);
	PKG_H_UINT32(buf, count, i);
	PKG_H_UINT32(buf, -xiaomee, i);
	msg_log_tuzhi(itemid,count );
	
	return send_request_to_db(p, p->id, proto_db_buy_normal_item, buf, i);
}

int buy_normal_item_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t itemid, count, xiaomee;
	int32_t cost;
	uint8_t out[64];
	int i = sizeof(protocol_t), j = 0;

	CHECK_DBERR(p, ret);

	UNPKG_H_UINT32(buf, itemid, j);
	UNPKG_H_UINT32(buf, count, j);
	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, cost, j);

	p->xiaomee = xiaomee;
	monitor_sprite(p, "BUY ITEM");
	cache_update_kind_item(p, itemid, count);

	KDEBUG_LOG(p->id, "BUY ITEM CALLBACK[%u %u %u %u %d]", id, itemid, count, xiaomee, cost);

	PKG_UINT32(out, itemid, i);
	PKG_UINT32(out, count, i);
	PKG_UINT32(out, xiaomee, i);
	PKG_UINT32(out, -cost, i);

	if (itemid == ITEM_ID_SIGNAL)
		msg_log_signal(p->id, 1, 0);

	init_proto_head(out, p->waitcmd, i);
	return send_to_self(p, out, i, 1);
}

void gen_task_use_item_target(sprite_t* p, normal_item_t* pni)
{
	if (!pni->cast_usage.taskid)
		return;

	task_db_t* ptr = cache_get_task_info(p, pni->cast_usage.taskid);
	if (!ptr)
		return;

	task_node_t* ptn = get_task_node(ptr->taskid, ptr->nodeid + 1);

	if (!ptn || !ptn->usetarget.itemid || !ptn->usetarget.count \
		|| ptr->svr_buf[svr_buf_item] >= ptn->usetarget.count)
		return;

	ptr->svr_buf[svr_buf_item] += 1;
	db_set_task_svr_buf(NULL, p->id, ptr);
}

void cache_add_firework_beast(sprite_t* p, uint32_t beasttype)
{
	item_t* pi = (item_t *)g_hash_table_lookup(p->fireworks, &beasttype);
	if (!pi) {
		pi = (item_t *)g_slice_alloc(sizeof(item_t));
		pi->itemid = beasttype;
		pi->count = 0;
		g_hash_table_insert(p->fireworks, &pi->itemid, pi);
	}
	pi->count ++;
}

uint32_t get_beastid_for_firework(normal_item_t* pni)
{
	if (!pni->cast_usage.grpid)
		return 0;
	
	beast_group_t* pbg = get_beast_grp(pni->cast_usage.grpid);
	if (!pbg || !pbg->idcnt)
		return 0;
	
	int idx = rand_type_idx(BATTLE_POS_CNT, pbg->percents, RAND_COMMON_RAND_BASE);
	TRACE_LOG("FIRE WORK\t[%d %u %u %u %u %u %u %u %u %u %u %u %u]", \
		idx, pbg->idlist[idx].id, pbg->idcnt, pni->cast_usage.grpid, pbg->percents[0], pbg->percents[1], pbg->percents[2], 
		pbg->percents[3], pbg->percents[4], pbg->percents[5], pbg->percents[6], pbg->percents[7], pbg->percents[8]);
	return pbg->idlist[idx].id % SPEC_BEAST_BASE;

}

int add_normal_item_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t itemid, count, type, beastid;
	uint8_t out[128];
	int i = 0, j = 0;

	CHECK_DBERR(p, ret);

	UNPKG_H_UINT32(buf, type, j);
	UNPKG_H_UINT32(buf, itemid, j);
	UNPKG_H_UINT32(buf, count, j);

	cache_update_kind_item(p, itemid, count);
	normal_item_t* pni = get_item(itemid);
	KDEBUG_LOG(p->id, "ADD ITEM CALLBACK[%u %u]", itemid, count);

	rand_info_t* pri = NULL;
	vip_item_t* pvi = NULL;
	
	switch (p->waitcmd) {
		case cli_get_rand_item_cmd:
		case cli_get_rand_item_spec_cmd:
			if (pni && pni->function == item_for_pet_exchange)
				msg_log_pet_egg(pni->petinfo.beastid, pet_egg_lucky_draw, 1);
			pri = (rand_info_t *)(p->session + 8);
			return send_request_to_db(p, p->id, proto_db_rand_info_update, &pri->rand_id, 4);
		case use_broadcast_item_cmd:
			i = sizeof(protocol_t);
			PKG_UINT32(out, p->id, i);
			PKG_UINT32(out, *(uint32_t *)p->session, i); /* des uid */
			PKG_UINT32(out, itemid, i);
			PKG_UINT32(out, *(uint32_t *)(p->session + 4), i); /* way */
			PKG_UINT16(out, *(uint16_t *)(p->session + 8), i);
			PKG_UINT16(out, *(uint16_t *)(p->session + 10), i);

			beastid = get_beastid_for_firework(pni);
			
			if (beastid)
				cache_add_firework_beast(p, beastid);

			PKG_UINT32(out, beastid, i);
			
			init_proto_head(out, p->waitcmd, i);
			send_to_map(p, out, i, 1, 1);
			return 0;
		case proto_cli_use_energy_item:
			response_proto_uint32_uint32(p, p->waitcmd, itemid, p->energy, 1, 0);
			return 0;
		case proto_cli_get_vip_item:
			pvi = (vip_item_t *)(p->session + 8);
			db_log_vip_item(NULL, p->id, 0, itemid, *(uint32_t *)(p->session + 4));
			return send_request_to_db(p, p->id, proto_db_rand_info_update, &pvi->vid, 4);
		case proto_cli_use_buff_item:
			i = sizeof(protocol_t);
			PKG_UINT32(out, itemid, i);
			i += pkg_ratio_buff(p->vip_buffs[0], out + i);
			i += pkg_ratio_buff(p->vip_buffs[1], out + i);
			i += pkg_ratio_buff(p->vip_buffs[2], out + i);
			PKG_UINT32(out, get_auto_fight_count(p), i);
			PKG_UINT32(out, p->vip_buffs[buffid_hpmp][0], i);
			PKG_UINT32(out, p->vip_buffs[buffid_hpmp][1], i);
			PKG_UINT32(out, p->hp, i);
			PKG_UINT32(out, p->mp, i);

			init_proto_head(out, p->waitcmd, i);
			return send_to_self(p, out, i, 1);
		default:
			ERROR_RETURN(("unkown cmd id when add item callback\t[%u %u]", p->id, p->waitcmd), -1);		
	}
	return 0;
}

static sprite_t* g_pkg_bag_item_p;
static void pkg_bag_item(gpointer key, gpointer value, gpointer data)
{
	item_t* pitem = (item_t *)value;
	if (!pitem->count)
		return;
	if (g_pkg_bag_item_p->last_use_medical_id==pitem->itemid ){
		uint32_t count=pitem->count-1;
		if(count>0){
			PKG_UINT32(msg, pitem->itemid, (*(int *)data));
			PKG_UINT32(msg, 0, (*(int *)data));
			PKG_UINT32(msg, count, (*(int *)data));
		}

	}else{
		PKG_UINT32(msg, pitem->itemid, (*(int *)data));
		PKG_UINT32(msg, 0, (*(int *)data));
		PKG_UINT32(msg, pitem->count, (*(int *)data));
	}
	//DEBUG_LOG("itemid=%u,count=%u",pitem->itemid,pitem->count  );
}

int get_normal_item_list_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int k = sizeof(protocol_t) + 4;
	g_pkg_bag_item_p=p;
	g_hash_table_foreach(p->bag_items, pkg_bag_item, &k);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, ((k - sizeof(protocol_t) - 4) / 12), i);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int sell_normal_item_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t type_cnt, itemid, item_cnt;
	int j = 0, k = 4;
	uint8_t out[256];
	uint32_t* p_xiaomee = (uint32_t*)out;
	*p_xiaomee = 0;
	UNPKG_UINT32 (body, type_cnt, j);
	CHECK_INT_GE(type_cnt, 1);

	CHECK_NOT_IN_TRADE(p); 
	KDEBUG_LOG(p->id, "SELL NORMAL ITEM LIST\t[%u]", type_cnt);
	CHECK_BODY_LEN(bodylen, type_cnt * 8 + 4);

	PKG_H_UINT32(out, type_cnt, k);

	for (uint32_t i = 0; i < type_cnt; i ++) {
		UNPKG_UINT32(body, itemid, j);
		UNPKG_UINT32(body, item_cnt, j);
		normal_item_t* pni = get_item(itemid);	
		CHECK_ITEM_VALID(p, pni, itemid);

		if (!itm_salable(pni->tradablity)
			&& !(ISVIP(p->flag) && itm_vipsalable(pni->tradablity))) {
			return send_to_self_error(p, p->waitcmd, cli_err_item_cannot_sell, 1);
		}

		KF_LOG("sell_item",p->id,"itemid:%u itemcnt:%u",itemid,item_cnt);
		*p_xiaomee += pni->sale_price * item_cnt;
		item_cnt = 0 - item_cnt;
		PKG_H_UINT32(out, 0, k);
		PKG_H_UINT32(out, itemid, k);
		PKG_H_UINT32(out, item_cnt, k);
	}

	return send_request_to_db(p, p->id, proto_db_add_item_list, out, k);
}

int add_item_list_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t xiaomee, add_value, count;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, xiaomee, j);
	UNPKG_H_UINT32(buf, add_value, j);
	UNPKG_H_UINT32(buf, count, j);

	CHECK_BODY_LEN(len, 12 + count * 12);
	uint32_t loop;
	for(loop = 0; loop < count; loop ++) {
		int type, itemid, item_cnt;
		UNPKG_H_UINT32(buf, type, j);
		UNPKG_H_UINT32(buf, itemid, j);
		UNPKG_H_UINT32(buf, item_cnt, j);
		KDEBUG_LOG(p->id, "ITEM\t[%u %u %u]", type, itemid, item_cnt);
		if (item_cnt > 0)
			cache_add_kind_item(p, itemid, item_cnt);
		else if (item_cnt < 0)
			cache_reduce_kind_item(p, itemid, -item_cnt);
	}

	p->xiaomee = xiaomee;
	monitor_sprite(p, "SELL ITEM");
	switch (p->waitcmd) {
		case proto_cli_sell_items:
			PKG_UINT32(msg, xiaomee, i);
			PKG_UINT32(msg, add_value, i);
			init_proto_head(msg, p->waitcmd, i);
			return send_to_self(p, msg, i, 1);
		case proto_cli_composite_item:
			//response_proto_uint32_uint32(p, p->waitcmd, p->energy, p->cache_skill_exp, 1, 0);
			//return 0;
			PKG_UINT32(msg, 0, i);
			PKG_UINT32(msg, *(uint32_t *)p->session, i);
			PKG_UINT32(msg, p->xiaomee, i);
			PKG_UINT32(msg, p->energy, i);
			PKG_UINT32(msg, p->cache_skill_exp, i);
			init_proto_head(msg, p->waitcmd, i);
			return send_to_self(p, msg, i, 1);
		case proto_cli_composite_cloth:
			/* rsp clothid and cloth type see add cloth callback */
			PKG_UINT32(msg, *(uint32_t *)p->session, i);
			PKG_UINT32(msg, *(uint32_t *)(p->session + 4), i);
			PKG_UINT32(msg, p->xiaomee, i);
			PKG_UINT32(msg, p->energy, i);
			PKG_UINT32(msg, p->cache_skill_exp, i);
			init_proto_head(msg, p->waitcmd, i);
			return send_to_self(p, msg, i, 1);
		default:
			ERROR_RETURN(("invalid cmd\t[%u %u]", p->id, p->waitcmd), -1);
	}
	return 0;
	
}

int composite_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int32_t itemid, count;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, count, j);
	KDEBUG_LOG(p->id, "COMPOSITE ITEM\t[%u %d]", itemid, count);

	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	if (!count) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	
	skill_t *ps=(pni->item_id>230000) ?
			cache_get_skill(p, skill_life_food_cook):cache_get_skill(p, skill_life_medical_mix);
	if (!ps) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	
	CHECK_CAN_HAVE_MORE_ITEM(p, pni, count);
	if (!pni->material_cnt) {
		return send_to_self_error(p, p->waitcmd, cli_err_cannot_composite, 1);
	}

	uint32_t xiaomee = pni->sale_price * count ;
	CHECK_XIAOMEE_ENOUGH(p, xiaomee);

	int loop;
	item_t items[MAX_MATERIAL_CNT + 1];
	for (loop = 0; loop < pni->material_cnt; loop ++) {
		CHECK_ITEM_EXIST(p, pni->materials[loop].itemid, pni->materials[loop].count * count);
		items[loop].itemid = pni->materials[loop].itemid;
		items[loop].count = -pni->materials[loop].count * count;
	}
	items[pni->material_cnt].itemid = itemid;
	items[pni->material_cnt].count = count;
//consume energy	
	uint32_t consume_energy=pow(20*pni->itemlv, (2.0/3))*count;
	if(consume_energy >=p->energy){
		KERROR_LOG(p->id, "energy not enough\t[%u]", p->energy);
		return send_to_self_error(p, p->waitcmd, cli_err_need_rest, 1);
	}
	p->energy-=consume_energy;
	db_set_energy(NULL, p->id, p->energy);
//add exp
	skill_info_t * psi=get_skill_info(ps->skill_id);
	uint32_t get_exp=pni->itemlv*100/ps->skill_level;
	ps->skill_exp+=get_exp;
	cache_update_skill_level(p, ps, psi, ps->skill_exp);
	p->cache_skill_exp=ps->skill_exp;
	db_update_skill_info(NULL, p->id, ps);
	KDEBUG_LOG(p->id,"comp clothes --energy[%u]  ++exp [%u]",consume_energy,get_exp);
	
	p->xiaomee -= xiaomee;
	db_add_xiaomee(NULL, p->id, -xiaomee);
	monitor_sprite(p, "COMP ITEM");

	*(uint32_t*)p->session = itemid;
	return db_add_item_list(p, p->id, items, pni->material_cnt + 1);
}

int composite_cloth_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t clothtype,itemid=0;
	int j = 0;
	UNPKG_UINT32(body, clothtype, j);
	UNPKG_UINT32(body, itemid, j);
	normal_item_t* pni=NULL;
	if(itemid !=0){
		CHECK_ITEM_EXIST(p, itemid, 1);
		pni=get_item(itemid);
		CHECK_ITEM_VALID(p, pni, itemid);
		CHECK_ITEM_FUNC(p, pni, item_for_comp_clothes);
	}
	cloth_t* pcloth = get_cloth(clothtype);
	if (!pcloth) {
		KERROR_LOG(p->id, "cloth id invalid\t[%u]", clothtype);
		return send_to_self_error(p, p->waitcmd, cli_err_cloth_id_invalid, 1);
	}
	if (!pcloth->material_cnt) {
		return send_to_self_error(p, p->waitcmd, cli_err_cannot_composite, 1);
	}
	if (get_bag_cloth_cnt(p) >= get_max_grid(p)) {
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}

	uint32_t xiaomee = pcloth->clothlvs[0].sell_price;
	CHECK_XIAOMEE_ENOUGH(p, xiaomee);
	for (int loop = 0; loop < pcloth->material_cnt; loop ++) {
		CHECK_ITEM_EXIST(p, pcloth->materials[loop].itemid, pcloth->materials[loop].count);
	}
	*(cloth_t**)p->session = pcloth;
	skill_t* ps = cache_get_skill(p, skill_life_weapon_make);
	if (!ps) {
		KERROR_LOG(p->id, "hav not skill\t[%u]", skill_life_weapon_make);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
//compute product_rates
	uint32_t rates[PRODUCT_LV_CNT]={};
	if(!pni){
		uint32_t rate_white=pcloth->pro_rate[0],rate_blue=(ps->skill_level-1)*30+pcloth->pro_rate[1],
					rate_purple=(ps->skill_level-1)*10+pcloth->pro_rate[2];
		uint32_t total=rate_white+rate_blue+rate_purple;
		rates[0]=rate_white*RAND_COMMON_RAND_BASE/total;
		rates[1]=rate_blue*RAND_COMMON_RAND_BASE/total;
		rates[2]=RAND_COMMON_RAND_BASE-rates[0]-rates[1];
		//KDEBUG_LOG(p->id,"composite clothes without itemxxxxrate[%u %u %u    %u %u %u %u]",pcloth->pro_rate[0],
		//	pcloth->pro_rate[1],pcloth->pro_rate[2],rates[0],rates[1],rates[2],rates[3]);
	}
	else{
		if(pcloth->lv >= pni->comp_clothes_info.limit_lv[0] && pcloth->lv <= pni->comp_clothes_info.limit_lv[1]){
			for(uint8_t loop=0; loop<PRODUCT_LV_CNT; loop++)
				rates[loop]=pni->comp_clothes_info.proc_rate[loop];
			//KDEBUG_LOG(p->id,"composite clothes with itemxxxxrate[%u %u %u %u]",rates[0],rates[1],rates[2],rates[3]);
		}
		else{
			KERROR_LOG(p->id,"item can't be used,lv not fit[%u]",pni->item_id);
			return send_to_self_error(p, p->waitcmd, cli_err_level_not_fit, 1);
		}
	}
	int idx = rand_type_idx(PRODUCT_LV_CNT, rates, RAND_COMMON_RAND_BASE);
	KDEBUG_LOG(p->id, "COMPOSITE CLOTH\t[%u %u]", clothtype, idx);

	if (!pcloth->pro_attr[idx].valid) {
		KERROR_LOG(p->id, "no comp info\t[%u %u]", clothtype, idx);
		return send_to_self_error(p, p->waitcmd, cli_err_cannot_composite, 1);
	}

	uint32_t consume_energy=pow(20*pcloth->itemlv, (2.0/3));
	if(consume_energy >=p->energy){
		KERROR_LOG(p->id, "energy not enough\t[%u]", p->energy);
		return send_to_self_error(p, p->waitcmd, cli_err_need_rest, 1);
	}
	p->energy-=consume_energy;
	db_set_energy(NULL, p->id, p->energy);
	
	skill_info_t * psi=get_skill_info(ps->skill_id);
	uint32_t get_exp=pcloth->itemlv*100/ps->skill_level;
	ps->skill_exp+=get_exp;
	cache_update_skill_level(p, ps, psi, ps->skill_exp);
	p->cache_skill_exp=ps->skill_exp;
	db_update_skill_info(NULL, p->id, ps);
	//KDEBUG_LOG(p->id,"comp clothes --energy[%u]  ++exp [%u] cache exp[%u]",consume_energy,get_exp,p->cache_skill_exp);
	
	if(itemid != 0)
		del_item(p, itemid,	1);
	return db_add_cloth(p, p->id, xiaomee, pcloth, 0, &pcloth->pro_attr[idx]);
}

int get_house_item_list_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	return send_request_to_db(p, p->id, proto_db_get_house_item_list, 0, 0);
}

int get_house_item_list_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int j = 0;
	uint32_t item_cnt;
	UNPKG_H_UINT32(buf, item_cnt, j);
	CHECK_BODY_LEN(len, item_cnt * sizeof(item_t) + 4);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, item_cnt, i);
	uint32_t loop, itemid, count;
	for (loop = 0; loop < item_cnt; loop ++) {
		UNPKG_H_UINT32(buf, itemid, j);
		UNPKG_H_UINT32(buf, count, j);
		PKG_UINT32(msg, itemid, i);
		PKG_UINT32(msg, count, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int cli_get_house_cloth(sprite_t* p, Cmessage* c_in)
{
	cli_get_house_cloth_in* p_in = P_IN;
	uint8_t out[16];
	int i = 0;
	PKG_H_UINT32(out, p_in->start, i);
	PKG_H_UINT32(out, p_in->count, i);
	return send_request_to_db(p, p->id, proto_db_get_house_cloth_list, out, i);
}

int get_house_cloth_list_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int j = 0;
	uint32_t total_cnt, start, cloth_cnt;
	UNPKG_H_UINT32(buf, total_cnt, j);
	UNPKG_H_UINT32(buf, start, j);
	UNPKG_H_UINT32(buf, cloth_cnt, j);
	KDEBUG_LOG(p->id, "HOUSE CLOTH CALLBACK\t[%u]", cloth_cnt);
	CHECK_BODY_LEN(len, cloth_cnt * sizeof(body_cloth_t) + 12);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, total_cnt, i);
	PKG_UINT32(msg, start, i);
	PKG_UINT32(msg, cloth_cnt, i);
	for (uint32_t loop = 0; loop < cloth_cnt; loop ++) {
		body_cloth_t* pc = (body_cloth_t *)(buf + 12 + loop * sizeof(body_cloth_t));
		i += pkg_cloth(pc, msg + i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int item_in_out_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int32_t flag, itemid, count;
	int j = 0;
	UNPKG_UINT32(body, flag, j);
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, count, j);

	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);

	KDEBUG_LOG(p->id, "ITEM IN OUT\t[%u %u %d]", flag, itemid, count);

	switch(flag) {
		case 0:		/* get out */
			CHECK_CAN_HAVE_MORE_ITEM(p, pni, count);
			break;
		case 1:		/* put in */
			CHECK_ITEM_EXIST(p, itemid, count);
			break;
		default:
			ERROR_RETURN(("flag invalid\t[%u %u]", p->id, flag), -1);
	}

	count = flag ? count : -count;
	int i = 0;
	uint8_t out[16];
	PKG_H_UINT32(out, itemid, i);
	PKG_H_UINT32(out, count, i);
	return send_request_to_db(p, p->id, proto_db_item_in_out, out, i);
		
}

int item_in_out_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t itemid = *(uint32_t*)buf;
	int32_t count = *(int32_t *)(buf + 4);
	KDEBUG_LOG(p->id, "ITEM IN OUT CALLBACK\t[%u %d]", itemid, count);
	if (count < 0) 
		cache_add_kind_item(p, itemid, - count);
	else if (count > 0)
		cache_reduce_kind_item(p, itemid, count);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int cloth_in_out_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t flag, clothid;
	int j = 0;
	UNPKG_UINT32(body, flag, j);
	UNPKG_UINT32(body, clothid, j);
	body_cloth_t* p_bc = cache_get_bag_cloth(p, clothid);
	KDEBUG_LOG(p->id, "CLOTH IN OUT CMD\t[%u %u %p]", flag, clothid, p_bc);
	switch(flag) {
		case 0:		/* get out */
			if (p_bc) {
				KERROR_LOG(p->id, "cloth already in bag\t[%u %u]", p_bc->clothid, p_bc->clothtype);
				return send_to_self_error(p, p->waitcmd, cli_err_already_in_bag, 1);
			}
			if (get_bag_cloth_cnt(p) >= get_max_grid(p))
				return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
			break;
		case 1:		/* put in */
			if (!p_bc)
				return send_to_self_error(p, p->waitcmd, cli_err_cloth_not_existed, 1);
			break;
		default:
			ERROR_RETURN(("flag invalid\t[%u %u]", p->id, flag), -1);
	}

	int i = 0;
	uint8_t out[16];
	PKG_H_UINT32(out, clothid, i);
	PKG_H_UINT32(out, flag ? START_GRID_CLOTH_WAREHOUSE : 0, i);
	return send_request_to_db(p, p->id, proto_db_cloth_in_out, out, i);
}

int cloth_in_out_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	body_cloth_t* pc = (body_cloth_t *)buf;
	if (GRID_FOR_CLOTH_WAREHOUSE(pc->grid)) 
		cache_del_bag_cloth(p,  pc->clothid);
	else cache_add_bag_cloth(p, pc);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int check_team_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
    KDEBUG_LOG(p->id, "CHECK TEAM ITEM\t[%u]", itemid);
	if (!IS_ITEM_KEY(itemid))
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	CHECK_SELF_TEAM_LEADER(p, itemid, 1);

	sprite_t* s = NULL;
	if (p->btr_team) {
		for (int loop = 0; loop < p->btr_team->count; loop ++) {
			if (cache_item_have_cnt(p->btr_team->players[loop], itemid, 1)) {
				s = p->btr_team->players[loop];
				break;
			}
		}
	} else if (cache_item_have_cnt(p, itemid, 1)) {
		s = p;
	}

	uint32_t have = 0;
	if (s) {
		have = 1;
		cache_reduce_kind_item(s, itemid, 1);
		db_add_item(0, s->id, itemid, -1);
	}

	response_proto_uint32_uint32(p, p->waitcmd, itemid, have, 1, 0);
	return 0;
}

int cli_check_team_item(sprite_t* p, Cmessage* c_in)
{
	cli_check_team_item_in* p_in = P_IN;
	cli_check_team_item_out cli_out;
	if (!p->btr_team) {
		if (!cache_item_have_cnt(p, p_in->itemid, p_in->count))
			cli_out.uidlist.push_back(p->id);
	} else {
		for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (!cache_item_have_cnt(s, p_in->itemid, p_in->count))
				cli_out.uidlist.push_back(s->id);
		}
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out ,1);
}

int item_enough_check(sprite_t* p, uint32_t itemid, uint32_t count)
{
	if (itemid == 0) {
		return 0;
	} else if (itemid == item_id_energy) {
		return p->energy < count ? cli_err_energy_not_enough : 0;
	} else if (itemid == item_id_pkpt) {
		return p->pkpoint_total < count ? cli_err_pkpt_not_enough : 0;
	} else if (itemid == item_id_vacancy) {
		return p->vacancy_cnt < count ? cli_err_vacancy_no_enough : 0;
	} else if (itemid == item_id_lucky_star) {
		DEBUG_LOG("xxxx %u %u %u",itemid,p->luckystar_count,count);
		return p->luckystar_count < count ? cli_err_havnot_this_item: 0;
	} else if (itemid && !cache_item_have_cnt(p, itemid, count)) {
		return cli_err_havnot_this_item;
	} 	return 0;
}

int item_enough_del(sprite_t* p, uint32_t itemid, uint32_t count)
{
	if (itemid == item_id_energy) {
		p->energy -= count;
		db_set_energy(NULL, p->id, p->energy);
	} else if (itemid == item_id_pkpt) {
		p->pkpoint_total -= count;
		db_update_pkinfo(NULL, p->id, p->pkpoint_total, p->pkpoint_day, 2);
	} else if (itemid == item_id_vacancy) {
		p->vacancy_cnt -= count;
		db_sub_ssid_totalcnt(NULL, p->id, itemid, count);
		msg_log_vacancy(p->id, p->vacancy_cnt);
	} else if (get_item(itemid)) {
		cache_reduce_kind_item(p, itemid, count);
		db_add_item(NULL, p->id, itemid, -count);
	}
	return 0;
}

int cli_get_rand_item_pvp(sprite_t* p,uint32_t itemid, 
		uint32_t pvp_pk_can_get_item_need_value ){
	KDEBUG_LOG(p->id,"cli_get_rand_item_pvp_succ %u %u ",p->pvp_pk_can_get_item, pvp_pk_can_get_item_need_value );
	if (p->pvp_pk_can_get_item==pvp_pk_can_get_item_need_value ){
		cli_get_rand_item_out cli_out;
		cli_out.item.count=p->add_item(itemid,1 );
		cli_out.item.itemid=itemid;
		//p->pvp_pk_can_get_item=0;
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	}else{
		return send_to_self_error (p, p->waitcmd,  cli_err_client_not_proc, 1);
		return 0;	
	}
}

int cli_get_rand_item_pvp_fail(sprite_t* p,uint32_t itemid, 
		uint32_t pvp_pk_can_get_item_need_value ){

	KDEBUG_LOG(p->id,"cli_get_rand_item_pvp_fail=%u ",p->pvp_pk_can_get_item );
	if (p->pvp_pk_can_get_item>pvp_pk_can_get_item_need_value ){
		cli_get_rand_item_out cli_out;
		cli_out.item.count=p->add_item(itemid,1 );
		cli_out.item.itemid=itemid;
		DEBUG_LOG(" cli_get_rand_item_pvp");

		p->pvp_pk_can_get_item++;
		if(p->pvp_pk_can_get_item>3){
			p->pvp_pk_can_get_item=0;
		}
		//KDEBUG_LOG(p->id,"cli_get_rand_item_pvp_fail=%u ",p->pvp_pk_can_get_item );
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	}else{
		return send_to_self_error (p, p->waitcmd,  cli_err_client_not_proc, 1);
	}
}



int cli_get_rand_item(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_TIRED(p);
	cli_get_rand_item_in* p_in = P_IN;
	if (p_in->randid==713){//PVP冠军奖励
		return  cli_get_rand_item_pvp(p,350013,1);
	}

	if (p_in->randid==714){//PVP鼓励奖
		return  cli_get_rand_item_pvp_fail(p,300045,1);
	}


	//得到随机物品信息
	rand_info_t* pri = get_rand_info(p_in->randid);
	KDEBUG_LOG(p->id, "GET RAND ITEM\t[%u %u %u %u]", p_in->randid, p->collection_grid, p->card_grid, p->chemical_grid);
	if (!pri || !pri->max_cnt) {
		KERROR_LOG(p->id, "rand id invalid\t[%u]", p_in->randid);
		return send_to_self_error (p, p->waitcmd, cli_err_rand_id_invalid, 1);
	}

	if (IS_GRASS_GAME_TYPE(pri->rand_id )) {
		if (!p->box_get_rand) {
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
	}else if(IS_ROULETTE_GAME_TYPE(pri->rand_id)){
		msg_log_play_roulette(p->id);
	}else if(IS_PUMPKIN_QUESTION_TYPE(pri->rand_id)){
			if(p->pumpkin_lantern_activity_type != 1){
				p->pumpkin_lantern_activity_type = 0;
				return send_to_self_error(p, p->waitcmd, cli_err_rand_id_invalid, 1);
			}
			p->pumpkin_lantern_activity_type = 0; 
			msg_log_pumpkin_reward(1);
	}else if(IS_PUMPKIN_GAME_TYPE(pri->rand_id)){
			if(p->pumpkin_lantern_activity_type != 3){
				p->pumpkin_lantern_activity_type = 0;
				return send_to_self_error(p, p->waitcmd, cli_err_rand_id_invalid, 1);
			}
			p->pumpkin_lantern_activity_type = 0; 
			msg_log_pumpkin_reward(1);
	}

	//检查所在位置
	if(pri->mapcnt != 0)
		CHECK_USER_IN_MAPS(p, pri->mapid, MAX_RAND_ITEM_MAPID);

	if (pri->beastgrp && p->last_beastgrp != pri->beastgrp) {
		KERROR_LOG(p->id, "beast grp kill not match\t[%u %u]", p->last_beastgrp, pri->beastgrp);
		return send_to_self_error(p, p->waitcmd, cli_err_not_win_beastgrp, 1);
	}
	
	//检查等级
	CHECK_LV_FIT(p, p, pri->minlv, pri->maxlv);

	if (pri->redirectid && p->level >= pri->redirectlv && get_rand_info(pri->redirectid)) {
		KDEBUG_LOG(p->id, "REDIRECT RAND\t[%u %u %u %u]", pri->rand_id, p->level, pri->redirectid, pri->redirectlv);
		pri = get_rand_info(pri->redirectid);
	}

	uint32_t bag_cloth_cnt = get_bag_cloth_cnt(p);
	switch (pri->rand_type) {
		case only_cloth:
			if (bag_cloth_cnt >= get_max_grid(p)) {
				KERROR_LOG(p->id, "bag full\t[%u]", bag_cloth_cnt);
				return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1); 
			}
			break;
		case only_item:
			if (p->collection_grid >= get_max_grid(p) || p->chemical_grid >= get_max_grid(p)) {
				KERROR_LOG(p->id, "bag full\t[%u %u]", p->collection_grid, p->chemical_grid);
				return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1); 
			}
			break;
		case mix_item_cloth:
			if (p->collection_grid >= get_max_grid(p) || p->chemical_grid >= get_max_grid(p) \
					|| bag_cloth_cnt >= get_max_grid(p)) {
				KERROR_LOG(p->id, "bag full\t[%u %u]", p->collection_grid, p->chemical_grid);
				return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1); 
			}
			break;
	}

	int ret = item_enough_check(p, pri->needitem, pri->needcnt);
	if (ret) {
		return send_to_self_error (p, p->waitcmd, ret, 1);
	}

	int idx = rand_type_idx(pri->type_cnt, pri->rates, RAND_COMMON_RAND_BASE);

	uint32_t itemid = 0, count = 0;
	uint32_t get_rand = rand() % RAND_COMMON_RAND_BASE;
	normal_item_t* pni = NULL;
	cloth_t *pc=NULL;
	if (get_rand < pri->items[idx].get_rate) {
		itemid = pri->items[idx].item_id;
		count = pri->items[idx].item_cnt;
		pni = get_item(itemid);
		pc = get_cloth(itemid);
		if ((!pni && !pc) || (pni && !can_have_more_item(p, itemid, count)) ){
			KERROR_LOG(p->id, "bag full\t[%u %u %u %u]", itemid, count,!pni,!pc);
			itemid = count = 0;
		}
	}

	if (IS_GRASS_GAME_TYPE(pri->rand_id )) {
		p->box_get_rand = 0;
		msg_log_active_item(idx);
	}else if(IS_ROULETTE_GAME_TYPE(pri->rand_id)){
		int i= get_roulette_outitem_idx(itemid);
		if(i >= 0)
			msg_log_roulette_item(i,itemid);
	}

	if (count && get_ol_time(p) > 3 * 60 * 60)
		count = (count + 1) / 2;

	if (pri->max_cnt == -1) {
		item_enough_del(p, pri->needitem, pri->needcnt);
		if(IS_ITEM(itemid)){
			db_add_item(NULL, p->id, itemid, count);
			cache_add_kind_item(p, itemid, count);
			if (pni && pni->function == item_for_pet_exchange)
				msg_log_pet_egg(pni->petinfo.beastid, pet_egg_lucky_draw, 1);
		}
		else if(IS_CLOTHES(itemid)){
			if(pc->clothlvs[0].valid == 1)
				db_add_cloth(0, p->id, 0, pc, 0, &pc->clothlvs[0]);
			else
				send_to_self_error(p, p->waitcmd, cli_err_cloth_id_invalid, 1);
		}

		response_proto_uint32_uint32(p, p->waitcmd, itemid, count, 1, 0);
		return 0;
	} else {
		*(uint32_t *)p->session = itemid;
		*(uint32_t *)(p->session + 4) = count;
		memcpy(p->session + 8, pri, sizeof(rand_info_t));
		return db_check_rand_info(p, p->id, pri->rand_id, pri->repeattype, pri->max_cnt);
	}
}

int cli_get_rand_item_spec(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_TIRED(p);
	cli_get_rand_item_spec_in* p_in = P_IN;
	rand_info_t* pri = get_rand_info(p_in->randid);
	

	if (!pri || !pri->speccnt) {
		KERROR_LOG(p->id, "rand id invalid\t[%u]", p_in->randid);
		return send_to_self_error(p, p->waitcmd, cli_err_rand_id_invalid, 1);
	}

	KDEBUG_LOG(p->id, "GET RAND ITEM SPEC\t[%u %u %u %u]", p_in->randid, p_in->itemid, p_in->count, pri->type_cnt);
	if(pri->mapcnt != 0)
		CHECK_USER_IN_MAPS(p, pri->mapid, MAX_RAND_ITEM_MAPID);
	CHECK_LV_FIT(p, p, pri->minlv, pri->maxlv);

	uint32_t loop;
	for (loop = 0; loop < pri->type_cnt; loop ++) {
		if (!p_in->itemid || !p_in->count) {
			p_in->itemid = p_in->count = 0;
			break;
		}
		if (pri->items[loop].item_id == p_in->itemid) {
			if (int (pri->items[loop].item_cnt) < p_in->count)
				return -1;
			break;
		}
	}
	if (loop == pri->type_cnt) {
		KERROR_LOG(p->id, "item id invalid\t[%u %u]", p_in->itemid, p_in->count);
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}

	if (p_in->count > 0 && !can_have_more_item(p, p_in->itemid, p_in->count))
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	else if (p_in->count < 0 && !cache_item_have_cnt(p, p_in->itemid, -p_in->count))
		return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1);

	int ret = item_enough_check(p, pri->needitem, pri->needcnt);
	if (ret) {
		return send_to_self_error (p, p->waitcmd, ret, 1);
	}

	if (get_ol_time(p) > 3 * 60 * 60)
		p_in->count = (p_in->count + 1) / 2;

	if (pri->max_cnt == -1) {
		item_enough_del(p, pri->needitem, pri->needcnt);
		db_add_item(NULL, p->id, p_in->itemid, p_in->count);
		if (p_in->count > 0)
			cache_add_kind_item(p, p_in->itemid, p_in->count);
		else
			cache_reduce_kind_item(p, p_in->itemid, -p_in->count);
		response_proto_uint32_uint32(p, p->waitcmd, p_in->itemid, p_in->count, 1, 0);
		return 0;
	} else {
		*(uint32_t *)p->session = p_in->itemid;
		*(uint32_t *)(p->session + 4) = p_in->count;
		memcpy(p->session + 8, pri, sizeof(rand_info_t));
		return db_check_rand_info(p, p->id, pri->rand_id, pri->repeattype, pri->max_cnt);
	}
}

int rand_info_update_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t itemid = *(uint32_t *)p->session;
	uint32_t itemcnt = *(uint32_t *)(p->session + 4);
	KDEBUG_LOG(p->id, "RAND INFO UPDATE GET ITEM\t[%u %u]", itemid, itemcnt);
	response_proto_uint32_uint32(p, p->waitcmd, itemid, itemcnt, 1, 0);
	return 0;
}

int check_rand_info_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t randid;
	int i = 0;
	UNPKG_UINT32(body, randid, i);
	rand_info_t* pri = get_rand_info(randid);
	KDEBUG_LOG(p->id, "CHECK RAND INFO\t[%u]", randid);
	if (!pri) 
		return send_to_self_error(p, p->waitcmd, cli_err_rand_id_invalid, 1);
	memcpy(p->session + 8, pri, sizeof(rand_info_t));
	return db_check_rand_info(p, p->id, randid, pri->repeattype, pri->max_cnt);
}
int check_vip_rand_info(sprite_t* p, Cmessage *c_in)
{
	check_vip_rand_info_in* p_in=P_IN;
	uint32_t vid=p_in->vid+100000;
	vip_item_t* pvi = get_vip_item(vid);
	KDEBUG_LOG(p->id, "CHECK VIP RAND INFO\t[%u]", p_in->vid);
	if (!pvi) 
		return send_to_self_error(p, p->waitcmd, cli_err_rand_id_invalid, 1);
	memcpy(p->session + 8, pvi, sizeof(vip_item_t));
	return db_check_rand_info(p, p->id, vid, pvi->repeattype, pvi->repeatcnt);
}

int rand_info_check_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t randid, uptime, itemid, itemcnt;
	int i = 0, randcnt;
	UNPKG_H_UINT32(buf, randid, i);
	UNPKG_H_UINT32(buf, uptime, i);
	UNPKG_H_UINT32(buf, randcnt, i);
	KDEBUG_LOG(p->id, "RAND INFO CHECK CALLBACK\t[%u %u %u]", randid, uptime, randcnt);
	cloth_t* psc = NULL;
	rand_info_t* pri = NULL;
	vip_item_t* pvi = NULL;

	switch (p->waitcmd) {
		case cli_get_rand_item_cmd:
		case cli_get_rand_item_spec_cmd:
			pri = (rand_info_t*)(p->session + 8);
			randcnt = check_same_period(uptime, pri->repeattype, pri->startday) ? randcnt : 0;
			if (randcnt >= pri->max_cnt) {
				switch (pri->repeattype) {
				case repeat_day:
					return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
				case repeat_none:
					return send_to_self_error(p, p->waitcmd, cli_err_rand_all_max, 1);
				case repeat_life:
					return send_to_self_error(p, p->waitcmd, cli_err_rand_life_max, 1);
				default:
					return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
				}
			}
			item_enough_del(p, pri->needitem, pri->needcnt);
			itemid = *(uint32_t *)p->session;
			itemcnt = *(uint32_t *)(p->session + 4);

			if (itemid && get_item(itemid)) {
				return db_add_item(p, p->id, itemid, itemcnt);
			} else if (itemid && (psc = get_cloth(itemid))) {
				return db_add_cloth(p, p->id, 0, psc, itemcnt, &psc->clothlvs[itemcnt]);
			} else {
				if (pri->rand_id > 5000 && pri->rand_id < 6000) {
					db_day_add_ssid_cnt(NULL, p->id, item_id_vacancy, 1, -1);
					p->vacancy_cnt ++;
					msg_log_vacancy(p->id, p->vacancy_cnt);
				}
				return send_request_to_db(p, p->id, proto_db_rand_info_update, &pri->rand_id, 4);
			}
		case proto_cli_check_rand_info:
			{
 				pri=(rand_info_t*)(p->session + 8);
				randcnt = check_same_period(uptime, pri->repeattype, pri->startday) ? randcnt : 0;
				//KDEBUG_LOG(p->id, "11111\t[%u]", randid);
				response_proto_uint32_uint32(p, p->waitcmd, randid, randcnt, 1, 0);
				return 0;
			}
		case check_vip_rand_info_cmd:
			{
 				pvi=(vip_item_t*)(p->session + 8);
				randcnt = check_same_period(uptime, pvi->repeattype, pvi->startday) ? randcnt : 0;
			//	KDEBUG_LOG(p->id, "11111\t[%u]", randid);
				response_proto_uint32_uint32(p, p->waitcmd, randid, randcnt, 1, 0);
				return 0;
			}
		case proto_cli_get_vip_item:
			{
				pvi = (vip_item_t*)(p->session + 8);
				randcnt = check_same_period(uptime,pvi->repeattype, pvi->startday) ? randcnt : 0;
				if (randcnt >= pvi->repeatcnt) {
					switch (pvi->repeattype) {
						case repeat_day:
							return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
						case repeat_none:
							return send_to_self_error(p, p->waitcmd, cli_err_rand_all_max, 1);
						case repeat_life:
							return send_to_self_error(p, p->waitcmd, cli_err_rand_life_max, 1);
						default:
							return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
					}
				}
				/*
				itemid = *(uint32_t *)p->session;
				itemcnt = *(uint32_t *)(p->session + 4);
				if (get_item(itemid)) {
					return db_add_item(p, p->id, itemid, itemcnt);
				} else if ((psc = get_cloth(itemid)) != NULL) {
					return db_add_cloth(p, p->id, 0, psc, itemcnt, &psc->clothlvs[itemcnt]);
				}
				changed by francisco 2012-5-18  原来只能一次添加一个东西 改成可以一次添加完毕
				*/
				cli_get_vip_item_out out;	
				for( uint32_t loop=0;  loop<pvi->type_cnt; loop++ ){
					itemid=pvi->items[loop].item_id;
					itemcnt=pvi->items[loop].item_cnt;
					cache_add_kind_item(p, itemid, itemcnt);
					db_add_item(NULL, p->id, itemid, itemcnt);
					db_log_vip_item(NULL, p->id, 0, itemid, itemcnt);
					stru_item tmp;
					tmp.itemid=itemid;
					tmp.count=itemcnt;
					out.items.push_back(tmp);
				}
				send_request_to_db(NULL, p->id, proto_db_rand_info_update, &pvi->vid, 4);
				return send_msg_to_self(p, p->waitcmd, &out, 1);
			}
		default:
			ERROR_RETURN(("invalid cmd\t[%u %u]", p->id, p->waitcmd), -1);
	}
}

void use_item_on_pet(sprite_t* p, pet_t* p_p, normal_item_t* p_ni, uint32_t* p_hp, uint32_t* p_mp)
{
	uint8_t out[16];
	uint32_t hp = 0, mp = 0;

	int luck = rand() % 41 + 80;
	if (p_ni->medinfo.hp_up) {
		hp = p_p->hp + p_p->resume * p_ni->medinfo.hp_up * luck / 10000;
		hp = hp > p_p->hp_max ? p_p->hp_max : hp;
		hp -= p_p->hp;
		p_p->hp += hp;
	} 

	if (p_ni->medinfo.mp_up) {
		mp = p_p->mp + p_ni->medinfo.mp_up * luck / 100;
		mp = mp > p_p->mp_max ? p_p->mp_max : mp;
		mp -= p_p->mp;
		p_p->mp += mp;
	}
	
	*p_hp = hp;
	*p_mp = mp;

	int i = 0;
	PKG_H_UINT32(out, p_p->petid, i);
	PKG_H_UINT32(out, hp, i);
	PKG_H_UINT32(out, mp, i);
	PKG_H_UINT32(out, p_p->injury_lv, i);

	KDEBUG_LOG(p->id, "SET PET HP MP\t[%u %u %u]", p_p->petid, p_p->hp, p_p->mp);

	send_request_to_db(0, p->id, proto_db_set_pet_hp_mp, out, i);
}

void use_item_on_sprite(sprite_t* p, normal_item_t* p_ni, uint32_t* p_hp, uint32_t* p_mp)
{
	uint32_t hp = 0, mp = 0;
	if (p_ni->medinfo.hp_up) {
		int luck = rand() % 41 + 80;
		hp = p->hp + p->resume * p_ni->medinfo.hp_up * luck / 10000;
		hp = hp > p->hp_max ? p->hp_max : hp;
		hp -= p->hp;
		p->hp += hp;
	} 
	
	if (p_ni->medinfo.mp_up) {
		int luck = rand() % 41 + 80;
		mp = p->mp + p_ni->medinfo.mp_up * luck / 100;
		mp = mp > p->mp_max ? p->mp_max : mp;
		mp -= p->mp;
		p->mp += mp;
	}
	
	*p_hp = hp;
	*p_mp = mp;

	KDEBUG_LOG(p->id, "SET SPRITE HP MP\t[%u %u]", p->hp, p->mp);
	db_add_hp_mp(NULL, p->id, hp, mp, p->injury_lv);
}

int use_item_for_sprite_on_pet(sprite_t* p, pet_t* p_p, normal_item_t* p_ni, uint32_t* p_hp, uint32_t* p_mp)
{
	if ((p_ni->medinfo.hp_up && p_p->hp >= p_p->hp_max) \
		&& (p_ni->medinfo.mp_up && p_p->mp >= p_p->mp_max))
		return cli_err_not_need_cure;
	use_item_on_pet(p, p_p, p_ni, p_hp, p_mp);
	return 0;
}
#define     IS_REVIVING_ITEM(id)  ((id)>=210012 && (id)<=210014)
int use_medical_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t uid, petid, itemid, hp = 0, mp = 0;
	int i = 0;
	UNPKG_UINT32(body, uid, i);
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, itemid, i);

	KDEBUG_LOG(p->id, "USE MEDICAL ITEM\t[%u %u %u]", itemid, petid, itemid);

	if (p->team_state == wait_animation || (p->btr_info && p->btr_info->state != bt_state_init)) {
		KERROR_LOG(p->id, "in battle\t [%u]", itemid);
		return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
	}

	normal_item_t* p_ni = get_item(itemid);
	CHECK_ITEM_VALID(p, p_ni, itemid);
	CHECK_ITEM_FUNC(p, p_ni, item_for_hpmp);
	CHECK_ITEM_EXIST(p, itemid, 1);

	//if(IS_REVIVING_ITEM(itemid)){ 
		//KERROR_LOG(p->id, "item id invalid\t[%u]", itemid); 
		//return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1); 
	//}
	pet_t* p_p = NULL;

	if (p->id == uid) {
		if (petid) {
			p_p = get_pet_inbag(p, petid);
			CHECK_PET_INBAG(p, p_p, petid);
			if (use_item_for_sprite_on_pet(p, p_p, p_ni, &hp, &mp))
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
		} else {
			if ((p_ni->medinfo.hp_up && p->hp >= p->hp_max) || (p_ni->medinfo.mp_up && p->mp >= p->mp_max))
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
			use_item_on_sprite(p, p_ni, &hp, &mp);
		}
	} else {
		sprite_t* lp = get_sprite(uid);
		if (!lp) {
			return send_to_self_error(p, p->waitcmd, cli_err_user_offline, 1);
		}
		
		if (!p->btr_team || p->btr_team != lp->btr_team) {
			return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_team, 1);
		}

		if (petid) {
			p_p = get_pet_inbag(lp, petid);
			CHECK_PET_INBAG(p, p_p, petid);
			if (use_item_for_sprite_on_pet(lp, p_p, p_ni, &hp, &mp))
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
		} else {
			if ((p_ni->medinfo.hp_up && lp->hp >= lp->hp_max) || (p_ni->medinfo.mp_up && lp->mp >= lp->mp_max))
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
			use_item_on_sprite(lp, p_ni, &hp, &mp);
		}
	}

	db_add_item(0, p->id, itemid, -1);
	cache_reduce_kind_item(p, itemid, 1);
	db_set_item_handbook(0, p->id, itemid, 1);

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, uid, i);
	PKG_UINT32(msg, petid, i);
	PKG_STR(msg, p_p ? p_p->nick : p->nick, i, USER_NICK_LEN);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, hp, i);
	PKG_UINT32(msg, mp, i);
	KDEBUG_LOG(p->id, "USE MEDICAL RET\t[%u %u %u %u %u]", uid, petid, itemid, hp, mp);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_team(p, msg, i, 1);
}

int use_broadcast_item(sprite_t* p, Cmessage* c_in)
{
	use_broadcast_item_in* p_in = P_IN;
	KDEBUG_LOG(p->id, "BROAD ITEM\t[%u %u %u %u %u]", p_in->uid, p_in->itemid, p_in->way, p_in->posx, p_in->posy);

	CHECK_POS_VALID(p, p_in->posx, p_in->posy);

	if (p_in->uid) {
		sprite_t* s = get_sprite(p_in->uid);
		if (!s || s->tiles != p->tiles)
			return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_map, 1);
	}

	normal_item_t* pni = get_item(p_in->itemid);
	CHECK_ITEM_VALID(p, pni, p_in->itemid);
	CHECK_ITEM_FUNC(p, pni, item_for_broadcast);
	CHECK_ITEM_EXIST(p, p_in->itemid, 1);
	*(uint32_t *)p->session = p_in->uid;
	*(uint32_t *)(p->session + 4) = p_in->way;
	*(uint16_t *)(p->session + 8) = p_in->posx;
	*(uint16_t *)(p->session + 10) = p_in->posy;

	uint32_t nowtime = get_now_sec();
	if (p_in->itemid == ITEM_ID_SIGNAL) {
		if (nowtime < 13.5 * 3600)
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
		if (beast_cnt == -1) {
			//report_global_beast_kill(0);
			return send_to_self_error(p, p->waitcmd, cli_err_rand_all_max, 1);
		}
		if (beast_cnt >= MAX_KILL_BEAST_CNT)
			return send_to_self_error(p, p->waitcmd, cli_err_rand_all_max, 1);

		item_t* pday = cache_get_day_limits(p, ssid_global_beast_kill);
		pday->count ++;
		db_day_add_ssid_cnt(NULL, p->id, ssid_global_beast_kill, 1, -1);
		msg_log_signal(p->id, 0, 1);
	} //else if (nowtime < 14 * 3600 || nowtime > 16 * 3600)
		//return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
	gen_task_use_item_target(p, pni);
	return db_add_item(p, p->id, p_in->itemid, -1);
}

int use_handbook_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid;
	uint8_t out[32];
	int i = 0, j = 0;
	UNPKG_UINT32(body, itemid, i);
	KDEBUG_LOG(p->id, "USE HANDBOOK ITEM\t[%u]", itemid);
	normal_item_t* p_ni = get_item(itemid);
	CHECK_ITEM_VALID(p, p_ni, itemid);
	CHECK_ITEM_FUNC(p, p_ni, item_for_handbook);
	CHECK_ITEM_EXIST(p, itemid, 1);

	PKG_H_UINT32(out, p_ni->handbook_id, j);
	PKG_H_UINT32(out, 1, j);
	PKG_H_UINT32(out, itemid, j);
	PKG_H_UINT32(out, -1, j);

	*(uint32_t *)p->session = p_ni->handbook_id;

	if (IS_ITEM_HANDBOOK_ITEM(itemid)) {
		return send_request_to_db(p, p->id, proto_db_use_item_hb_item, out, j);
	} else if (IS_ITEM_HANDBOOK_BEAST(itemid)) {
		return send_request_to_db(p, p->id, proto_db_use_beast_hb_item, out, j);
	}

	KERROR_LOG(p->id, "unexpected itemid when use handbook item: %u", itemid);
	return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
}

int use_handbook_item_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t itemid = *(uint32_t *)buf;
	cache_reduce_kind_item(p, itemid, 1);
	if (IS_ITEM_HANDBOOK_BEAST(itemid)) {
		uint32_t beastid = *(uint32_t *)p->session;
		beast_kill_t* pbk = cache_get_beast_kill_info(p, beastid);
		pbk->state |= 1;
	}
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int use_energy_item_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t itemid;
	int i = 0;
	UNPKG_UINT32(body, itemid, i);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_EXIST(p, itemid, 1);
	CHECK_ITEM_FUNC(p, pni, item_for_energy);

	*(uint32_t *)p->session = itemid;

	uint32_t max_energy = max_user_energy(p);
	uint32_t user_energy = p->energy + max_energy * pni->energy_factor / 100;
	user_energy = user_energy > max_energy ? max_energy : user_energy;
	return db_set_energy(p, p->id, user_energy);
}

int use_ratio_buff_item(sprite_t* p, normal_item_t* pni, uint32_t buffid)
{
	uint32_t* buff = p->vip_buffs[buffid];
	buff_factor_t* pbf = &pni->exp_factor;

	uint32_t exp_factor = get_exp_factor(p);
	if (!exp_factor)
		return cli_err_need_rest;

	if (exp_factor >= 300)
		return cli_err_expfactor_top;

	for (uint32_t loop = 0; loop < buff[0]; loop ++) {
		exp_factor = exp_factor * buff[1 + loop * 2] / 100;
		if (exp_factor >= 300)
			return cli_err_expfactor_top;
	}

	buff[1 + 2 * buff[0]] = pbf->factor;
	buff[2 + 2 * buff[0]] = pbf->count;
	buff[0] ++;

	db_set_vip_buff(p, buffid);
	return 0;
}

int use_hpmp_buff_item(sprite_t* p, normal_item_t* pni)
{
	const uint32_t max_autohpmp = 999999;
	if (pni->hpmp_facotr.hpfactor) {
		if (p->vip_buffs[buffid_hpmp][0] + pni->hpmp_facotr.hpfactor > max_autohpmp)
			return cli_err_rand_all_max;
		p->vip_buffs[buffid_hpmp][0] += pni->hpmp_facotr.hpfactor;
	}

	if (pni->hpmp_facotr.mpfactor) {
		if (p->vip_buffs[buffid_hpmp][1] + pni->hpmp_facotr.mpfactor > max_autohpmp)
			return cli_err_rand_all_max;
		p->vip_buffs[buffid_hpmp][1] += pni->hpmp_facotr.mpfactor;
	}

	if (cache_auto_hpmp_for_user(p)) {
		if (p->btr_team)
			notify_team_state(p->btr_team);
		db_add_exp_level(p);
	}

	for (uint32_t loop = 0; loop < p->pet_cnt; loop ++) {
		if (cache_auto_hpmp_for_pet(p, &p->pets_inbag[loop]))
			db_add_pet_exp_level(p, &p->pets_inbag[loop]);
	}

	db_set_vip_buff(p, buffid_hpmp);
	return 0;
}

int use_fight_buff_item(sprite_t* p, normal_item_t* pni)
{
	if (get_auto_fight_count(p) + pni->auto_fight_cnt > 1000)
		return cli_err_rand_all_max;
	p->vip_buffs[buffid_auto_fight][2] += pni->auto_fight_cnt;
	db_set_vip_buff(p, buffid_auto_fight);
	return 0;
}

int use_buff_item_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t itemid, petid;
	int i = 0;
	UNPKG_UINT32(body, itemid, i);
	UNPKG_UINT32(body, petid, i);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_EXIST(p, itemid, 1);

	int ret = 0;
	switch (pni->function) {
	case item_for_exp_buff:
		ret = use_ratio_buff_item(p, pni, buffid_exp);
		break;
	case item_for_pet_buff:
		ret = use_ratio_buff_item(p, pni, buffid_pet_exp);
		break;
	case item_for_skill_buff:
		ret = use_ratio_buff_item(p, pni, buffid_skill_exp);
		break;
	case item_for_auto_fight:
		ret = use_fight_buff_item(p, pni);
		break;
	case item_for_auto_hpmp:
		ret = use_hpmp_buff_item(p, pni);
		break;
	default:
		KERROR_LOG(p->id, "not for exp or skill\t[%u %u]", itemid, pni->function);
		return -1;
	}

	if (ret) {
		return send_to_self_error(p, p->waitcmd, ret, 1);
	}

	return db_add_item(p, p->id, itemid, -1);
}

int db_add_item_list(sprite_t* p, uint32_t id, item_t* items, uint32_t item_cnt)
{
	uint8_t out[1024];
	int j = 0;
	PKG_H_UINT32 (out, 0, j);
	PKG_H_UINT32 (out, item_cnt, j);

	for (uint32_t i = 0; i < item_cnt; i ++) {
		PKG_H_UINT32(out, 0, j);
		PKG_H_UINT32 (out, items[i].itemid, j);
		PKG_H_UINT32 (out, items[i].count, j);
		KDEBUG_LOG(id, "DB ADD ITEM\t[id=%u cnt=%d]", items[i].itemid, items[i].count);
		msg_log_tuzhi(  items[i].itemid, items[i].count );
	}


	return send_request_to_db(p, id, proto_db_add_item_list, out, j);
}

int db_reduce_item_list(sprite_t* p, uint32_t id, item_t* items, uint32_t item_cnt)
{
	uint8_t out[1024];
	int j = 0;
	PKG_H_UINT32 (out, 0, j);
	PKG_H_UINT32 (out, item_cnt, j);

	for (uint32_t i = 0; i < item_cnt; i ++) {
		PKG_H_UINT32(out, 0, j);
		PKG_H_UINT32 (out, items[i].itemid, j);
		PKG_H_UINT32 (out, -items[i].count, j);
		KDEBUG_LOG(id, "DB REDUCE ITEM\t[id=%u cnt=%d]", items[i].itemid, items[i].count);
	}

	return send_request_to_db(p, id, proto_db_add_item_list, out, j);
}


static inline void add_temp_suit_info(item_t* asuit, cloth_t* psc)
{
	for (int loop = 0; loop < psc->suitcnt; loop ++) {
		for (int l = 0; l < MAX_ITEMS_WITH_BODY * MAX_ITEMS_WITH_BODY; l ++) {
			if (asuit[l].itemid == psc->suitid[loop]) {
				asuit[l].count ++;
				break;
			} else if (!asuit[l].itemid) {
				asuit[l].itemid = psc->suitid[loop];
				asuit[l].count = 1;
				break;
			}
		}
	}
}

int cache_update_body_cloths(sprite_t* p, body_cloth_t* apc, uint32_t cloth_cnt)
{
	g_hash_table_remove_all(p->body_cloths);
	g_hash_table_remove_all(p->cloth_timelist);
	g_hash_table_foreach_remove(p->all_cloths, free_body_cloth, 0);
	for (uint32_t loop = 0; loop < cloth_cnt; loop++) {
		/*if (try_disappear_cloth_before(p, &apc[loop]))
			continue;
			*/
		
		cloth_t* p_sc = get_cloth(apc[loop].clothtype);
		if (!p_sc || apc[loop].clothlv >= MAX_CLOTH_LEVL_CNT || !p_sc->clothlvs[apc[loop].clothlv].valid) {
			KERROR_LOG(p->id, "Invalid cloth id\t[%u %u]", apc[loop].clothtype, apc[loop].clothlv);
			continue;
		}

		body_cloth_t* pc = (body_cloth_t *)g_slice_alloc(sizeof(body_cloth_t));
		memcpy(pc, apc + loop, sizeof(body_cloth_t));
		g_hash_table_insert(p->all_cloths, &pc->clothid, pc);
		g_hash_table_replace(p->body_cloths, &pc->grid, pc);
		if (pc->validday)
			g_hash_table_insert(p->cloth_timelist, &pc->clothid, pc);
	}

	return cloth_cnt * sizeof(body_cloth_t);
}

void cache_set_cloth_attr(sprite_t* p)
{
	uint32_t honor = p->honor;
	int remove_honor = IS_TEMP_HONOR(p->honor) ? 1 : 0;

	typedef struct tmp_cloth {
		body_cloth_t*	body_cloth;
		cloth_t*		meta_cloth;	
	}__attribute__((packed))tmp_cloth_t;

	p->earth = 0;
	p->water = 0;
	p->fire = 0;
	p->wind = 0;
	p->honor = 0;
	calc_sprite_second_level_attr(p);

	item_t body_suits[MAX_ITEMS_WITH_BODY * MAX_ITEMS_WITH_BODY];
	memset(body_suits, 0, sizeof(body_suits));
	tmp_cloth_t crystal_cloth[4] = {{NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}};
	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t *)pcloths->data;
		cloth_t* p_sc = get_cloth(pc->clothtype);
		if (p_sc && pc->clothlv < MAX_CLOTH_LEVL_CNT && p_sc->clothlvs[pc->clothlv].valid) {
			if (p_sc->honor_id == p->honor)
				remove_honor = 0;
			add_temp_suit_info(body_suits, p_sc);
			if (p_sc->equip_part == part_crystal) {
				p->earth = p_sc->clothlvs[pc->clothlv].earth;
				p->water = p_sc->clothlvs[pc->clothlv].water;
				p->fire = p_sc->clothlvs[pc->clothlv].fire;
				p->wind = p_sc->clothlvs[pc->clothlv].wind;
			} else if (p_sc->equip_part >= part_hat && p_sc->equip_part <= part_shoe) {
				crystal_cloth[p_sc->equip_part - 1].body_cloth = pc;
				crystal_cloth[p_sc->equip_part - 1].meta_cloth = p_sc;
				pc->bless_type &= 0xFFFF;			
			}
			set_body_cloth(p, pc);
		}
		pcloths = pcloths->next;
	}
	g_list_free(head);

	if (!remove_honor)
		set_user_honor(p, honor);
	for (int loop = 0; loop < MAX_ITEMS_WITH_BODY * MAX_ITEMS_WITH_BODY; loop ++) {
		if (!body_suits[loop].itemid)
			break;
		
		suit_t* ps = get_suit(body_suits[loop].itemid);
		if (ps && ps->clothcnt == body_suits[loop].count)
			add_second_attr(p, &ps->attr,2);
	}

	for (int loop = 0; loop < 4; loop ++) {
		int next = (loop + 1) % 4;
		body_cloth_t* pbc = crystal_cloth[next].body_cloth;
		cloth_t* psc = crystal_cloth[next].meta_cloth;
		
		if (!pbc || !crystal_cloth[loop].body_cloth || !psc->activatecnt)
			continue;
		
		bool is_activated = false;
		for (uint32_t loopi = 0; loopi < psc->activatecnt; loopi ++) {
			if (crystal_cloth[loop].body_cloth->clothtype == psc->activateid[loopi]) {
				is_activated = true;
				break;
			}
		}
		if (is_activated && pbc->crystal_attr && crystal_cloth[loop].body_cloth->crystal_attr \
			&& (pbc->crystal_attr + 1) % 4 == crystal_cloth[loop].body_cloth->crystal_attr % 4) {
			switch (pbc->bless_type) {
				case attr_id_hpmax:
					p->hp_max += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_mpmax:
					p->mp_max += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_spirit:
					p->spirit += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_resume:
					p->resume += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_dodge:
					p->bisha += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_hit:
					p->hit_rate += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_avoid:
					p->avoid_rate += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_mdef:
					p->mdefense += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_rconfusion:
					p->rconfusion += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_rpoison:
					p->rpoison += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_rlethargy:
					p->rlethargy += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_roblivion:
					p->roblivion += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_rcommination:
					p->rinebriation += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
				case attr_id_rlithification:
					p->rlithification += crystal_attr[pbc->bless_type - 1][pbc->quality][psc->minlv / 5];
					break;
			}
			pbc->bless_type |= 0x10000;
		}
	}

	p->hp = p->hp > p->hp_max ? p->hp_max : p->hp;
	p->mp = p->mp > p->mp_max ? p->mp_max : p->mp;

	for (uint32_t loop = 0; loop < p->skill_cnt; loop ++) {
		switch (p->skills[loop].skill_id) {
		case skill_resist_poison:
			p->rpoison = p->rpoison * (100 + 5 * p->skills[loop].skill_level) / 100;
			break;
		case skill_resist_commination:
			p->rinebriation = p->rinebriation * (100 + 5 * p->skills[loop].skill_level) / 100;
			break;
		case skill_resist_confusion:
			p->rconfusion = p->rconfusion * (100 + 5 * p->skills[loop].skill_level) / 100;
			break;
		case skill_resist_lithification:
			p->rlithification = p->rlithification * (100 + 5 * p->skills[loop].skill_level) / 100;
			break;
		case skill_resist_lethargy:
			p->rlethargy = p->rlethargy * (100 + 5 * p->skills[loop].skill_level) / 100;
			break;
		case skill_resist_oblivion:
			p->roblivion = p->roblivion * (100 + 5 * p->skills[loop].skill_level) / 100;
			break;
		}
	}
}

void cache_set_body_attr_temp(sprite_t* p, body_cloth_t* pac, uint32_t count)
{
	uint32_t honor = p->honor;
	int remove_honor = IS_TEMP_HONOR(p->honor) ? 1 : 0;

	p->earth = 0;
	p->water = 0;
	p->fire = 0;
	p->wind = 0;
	p->honor = 0;
	calc_sprite_second_level_attr(p);

	item_t body_suits[MAX_ITEMS_WITH_BODY * MAX_ITEMS_WITH_BODY];
	memset(body_suits, 0, sizeof(body_suits));
	uint32_t loop;
	for (loop = 0; loop < count; loop ++) {
		body_cloth_t* pc = &pac[loop];
		cloth_t* p_sc = get_cloth(pc->clothtype);
		if (p_sc && pc->clothlv >= MAX_CLOTH_LEVL_CNT && !p_sc->clothlvs[pc->clothlv].valid) {
			if (p_sc->honor_id == p->honor)
				remove_honor = 0;
			add_temp_suit_info(body_suits, p_sc);
			if (p_sc->equip_part == part_crystal) {
				p->earth = p_sc->clothlvs[pc->clothlv].earth;
				p->water = p_sc->clothlvs[pc->clothlv].water;
				p->fire = p_sc->clothlvs[pc->clothlv].fire;
				p->wind = p_sc->clothlvs[pc->clothlv].wind;
			}
			set_body_cloth(p, pc);
		}
	}

	if (!remove_honor)
		set_user_honor(p, honor);

	for (loop = 0; loop < MAX_ITEMS_WITH_BODY * MAX_ITEMS_WITH_BODY; loop ++) {
		if (!body_suits[loop].itemid)
			break;
		
		suit_t* ps = get_suit(body_suits[loop].itemid);
		if (ps && ps->clothcnt == body_suits[loop].count)
			add_second_attr(p, &ps->attr,2);
	}

	p->hp = p->hp > p->hp_max ? p->hp_max : p->hp;
	p->mp = p->mp > p->mp_max ? p->mp_max : p->mp;
}

int add_user_bag_cloths(sprite_t* p, body_cloth_t* apbc, uint32_t cloth_cnt)
{
	for (uint32_t loop = 0; loop < cloth_cnt; loop ++) {
		body_cloth_t* pc = cache_get_cloth(p, apbc[loop].clothid);
		if (!pc) {
			/*
			if (try_disappear_cloth_before(p, &apbc[loop]))
				continue;
			*/
			pc = (body_cloth_t *)g_slice_alloc(sizeof(body_cloth_t));
			memcpy(pc, apbc + loop, sizeof(body_cloth_t));
			g_hash_table_insert(p->all_cloths, &pc->clothid, pc);
			if (pc->validday)
				g_hash_table_insert(p->cloth_timelist, &pc->clothid, pc);
		}
	}
	return cloth_cnt * sizeof(body_cloth_t);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  修理装备耐久  最大耐久不变   耐久一次恢复到最大耐久
 */
/* ----------------------------------------------------------------------------*/
int repair_cloth_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t pos, cloth_id, clothtype,cost;
	int j = 0;
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT32(body, cloth_id, j);
	UNPKG_UINT32(body, clothtype, j);
	KDEBUG_LOG(p->id, "REPAIR CLOTH\t[%u %u %u]", pos, cloth_id, clothtype);
	body_cloth_t* pc = cache_get_cloth(p, cloth_id);;

	if (!pc) {
		return send_to_self_error(p, p->waitcmd, cli_err_cloth_not_existed, 1);
	}
	
	if (pc->duration == pc->duration_max  ) {
		return send_to_self_error(p, p->waitcmd, cli_err_cloth_needno_repair, 1);
	}
	*(body_cloth_t* *)p->session = pc;
	
	cloth_t* psc = get_cloth(pc->clothtype);
	CHECK_CLOTH_LV_VALID(p, psc, pc->clothtype, pc->clothlv);
	CHECK_XIAOMEE_ENOUGH(p, psc->clothlvs[pc->clothlv].repair_price);
	//DEBUG_LOG("1dur=%u mdur=%u",pc->duration,pc->duration_max);
	if(pc->duration == 0 && pc->duration_max <= 1){
		cloth_lvinfo_t* pclv = &psc->clothlvs[pc->clothlv];
		pc->duration_max = pclv->duration[0] + rand () % (pclv->duration[1] - pclv->duration[0] + 1);
	}
	//DEBUG_LOG("2dur=%u mdur=%u",pc->duration,pc->duration_max);
	uint32_t duration =  pc->duration_max;
	//free for vip
	cost = ISVIP(p->flag)?0:psc->clothlvs[pc->clothlv].repair_price;
	return db_repair_one_cloth(p, p->id,cost, cloth_id,(uint32_t)pc->duration_max, duration);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  物品修复耐久
 */
/* ----------------------------------------------------------------------------*/
int cli_recover_duration(sprite_t* p,Cmessage* c_in)
{
	cli_recover_duration_in *p_in=P_IN;
	KDEBUG_LOG(p->id, "REPAIR MAX DURARION\t[%u %u %u]", p_in->pos, p_in->clothid, p_in->clothtype);
	
	normal_item_t *pnis  = get_item(p_in->itemid);
	CHECK_ITEM_VALID(p,pnis,p_in->itemid);
	CHECK_ITEM_EXIST(p,p_in->itemid,1);
	
	body_cloth_t* pc = cache_get_cloth(p, p_in->clothid);
	if (!pc) {
		KERROR_LOG(p->id,"cloth not exist[%u]",p_in->clothid);
		return send_to_self_error(p, p->waitcmd, cli_err_cloth_not_existed, 1);
	}

	uint32_t duration = 0;
	switch (pnis->function) {
		case item_for_recover_duration:
			if (pc->duration >= pc->duration_max) {
				KERROR_LOG(p->id, "need not recovery\t[%u]", pc->duration_max);
				return send_to_self_error(p, p->waitcmd, cli_err_cloth_needno_repair, 1);
			}

			if (pnis->duration == -1) {
				duration = pc->duration_max;
			} else {
				duration = (pc->duration + pnis->duration) > pc->duration_max ? pc->duration_max : (pc->duration + pnis->duration);
			}

			del_item(p, p_in->itemid, 1);
			*(body_cloth_t* *)p->session = pc;
			return db_repair_one_cloth(p, p->id, 0, p_in->clothid, pc->duration_max, duration);
		case item_for_recover_mduration:
			{
				cloth_t* psc = get_cloth(pc->clothtype);
				CHECK_CLOTH_LV_VALID(p, psc, pc->clothtype, pc->clothlv);
				cloth_lvinfo_t* pclv = &psc->clothlvs[pc->clothlv];
				if(pnis->duration==-1){
			//user's max_duration is larger than one that we can recover
					if (pc->duration_max >=pclv->duration[1] ) {
						KERROR_LOG(p->id, "need not recovery\t[%u %u]", pc->duration_max, pclv->duration[1]);
						return send_to_self_error(p, p->waitcmd, cli_err_cloth_needno_repair, 1);
					}
					duration=pclv->duration[1];
				}else{
					duration=(pclv->duration[1] > pc->duration_max+pnis->duration)?(pc->duration_max+pnis->duration):pclv->duration[1];
				}

				del_item(p, p_in->itemid, 1);
				*(body_cloth_t* *)p->session = pc;
				return db_repair_one_cloth(p, p->id, 0, p_in->clothid, duration, duration);
			}
		default:
			KERROR_LOG(p->id, "invalid func\t[%u]", pnis->function);
			return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}
}

int repair_cloths_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int cost, j = 0, i = sizeof(protocol_t);
	uint32_t cloth_cnt, cloth_id, duration, duration_max;
	UNPKG_H_UINT32(buf, cost, j);
	UNPKG_H_UINT32(buf, cloth_cnt, j);
	p->xiaomee += cost;
	monitor_sprite(p, "REPAIR CLOTH");
	CHECK_INT_GE(cloth_cnt, 1);
	CHECK_BODY_LEN(len, 8 + cloth_cnt * 8);

	UNPKG_H_UINT32(buf, cloth_id, j);
	UNPKG_H_UINT16(buf, duration_max, j);
	UNPKG_H_UINT16(buf, duration, j);
	body_cloth_t* pc = *(body_cloth_t* *)p->session;
	int need_calc_attr = pc->duration * 2 < pc->duration_max;
	pc->duration = duration; 
	pc->duration_max = duration_max; 
	if (pc->grid > BASE_BODY_TYPE && need_calc_attr) {
		cache_set_cloth_attr(p);
	}

	PKG_UINT32(msg, -cost, i);
	PKG_UINT32(msg, p->xiaomee, i);
	PKG_UINT32(msg, cloth_id, i);
	PKG_UINT16(msg, duration_max, i);
	PKG_UINT16(msg, duration, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

void cache_update_kind_item(sprite_t* p, uint32_t itemid, uint32_t count)
{
	KDEBUG_LOG(p->id, "CACHE UPDATE ITEM\t[%u %u]", itemid, count);
	normal_item_t* pni = get_item(itemid);
	if (!pni) return;
	item_t* pitem = cache_get_bag_item(p, itemid);
	if (!pitem) {
		pitem = (item_t *)g_slice_alloc(sizeof(item_t));
		pitem->itemid = itemid;
		pitem->count = 0;
		g_hash_table_insert(p->bag_items, &pitem->itemid, pitem);
	}

	int need_grid = get_need_grid(pni, pitem->count, count);
	switch (pni->type) {
		case normal_item_medical:
			p->chemical_grid += need_grid;
			break;
		case normal_item_collection:
			p->collection_grid += need_grid;
			break;
		case normal_item_card:
			p->card_grid += need_grid;
			break;
	}

	pitem->count = count;
}

int cache_init_bag_items(sprite_t* p, item_t* api, uint32_t item_cnt)
{
	p->chemical_grid = 0;
	p->collection_grid = 0;
	p->card_grid = 0;

	uint32_t loop;
	for (loop = 0; loop < item_cnt; loop ++) {
		item_t* pi = api + loop;
		bool continue_flag=false;
		switch ( pi->itemid ){
			case VIP_TIME_ID_5 :
				p->vip_day_5=pi->count;
				KDEBUG_LOG(p->id, "VIP_TIME_ID_5 :%u",p->vip_day_5 );
				continue_flag=true;	
				break;
			case VIP_TIME_ID_6 :
				p->vip_day_6=pi->count;
				KDEBUG_LOG(p->id, "VIP_TIME_ID_6 :%u",p->vip_day_6 );
				continue_flag=true;	
				break;
			case VIP_NOTI_FLAGID_5:
				p->vip_noti_flag_5=pi->count;
				KDEBUG_LOG(p->id, "VIP_NOTI_FLAGID_5: %u",p->vip_noti_flag_5 );
				continue_flag=true;	
				break;
			case VIP_NOTI_FLAGID_6:
				p->vip_noti_flag_6=pi->count;
				KDEBUG_LOG(p->id, "VIP_NOTI_FLAGID_6: %u",p->vip_noti_flag_6 );
				continue_flag=true;	
				break;
			case FLAG_TYPE:
				p->flag2=pi->count;
				continue_flag=true;	
				break;
	
			default :
				break;
		}

		if (continue_flag ){
			continue;
		}

		normal_item_t* pni = get_item(pi->itemid);
		if (!pi->count || !pni)
			continue;

		item_t* pitem = (item_t *)g_slice_alloc(sizeof(item_t));
		pitem->itemid = pi->itemid;
		pitem->count = pi->count;
		g_hash_table_insert(p->bag_items, &pitem->itemid, pitem);

		uint32_t need_grid = get_need_grid(pni, 0, pitem->count);

		switch (pni->type) {
			case normal_item_medical:			
				p->chemical_grid += need_grid;
				break;
			case normal_item_collection:
				p->collection_grid += need_grid;
				break;
			case normal_item_card:
				p->card_grid += need_grid;
				break;
		}
	}
	KDEBUG_LOG(p->id, "USER ITEM\t[%u %u %u]", p->collection_grid, p->chemical_grid, p->card_grid);

	return item_cnt * sizeof (item_t);
}

int cache_add_kind_item(sprite_t* p, uint32_t itemid, uint32_t count)
{
	normal_item_t* pni = get_item(itemid);
	if (!pni) return 0;

	item_t* pitem = cache_get_bag_item(p, itemid);
	if (!pitem) {
		pitem = (item_t *)g_slice_alloc(sizeof(item_t));
		pitem->itemid = itemid;
		pitem->count = 0;
		g_hash_table_insert(p->bag_items, &pitem->itemid, pitem);
	}

	if (pitem->count + count > pni->total_max) {
		count = pitem->count > pni->total_max ? 0 : pni->total_max - pitem->count;
	} 
	if (!count)
		return 0;

	uint32_t need_grid = get_need_grid(pni, pitem->count, pitem->count + count);

	KDEBUG_LOG(p->id, "ADD ITEM\t[%u %d]", itemid, count);
	const uint32_t max_grid = get_max_grid(p);

	switch (pni->type) {
		case normal_item_medical:
			if (p->chemical_grid + need_grid > max_grid) {
				uint32_t last_grid_cnt = pitem->count % pni->overlay_max;
				if (p->chemical_grid > max_grid || (p->chemical_grid == max_grid && !last_grid_cnt))
					return 0;
				need_grid = max_grid - p->chemical_grid;
				if (need_grid == 0) {
					count = (count + last_grid_cnt < pni->overlay_max) ? count : (pni->overlay_max - last_grid_cnt);
				} else if (count + last_grid_cnt > pni->overlay_max * (1 + need_grid)) {
					count = count + last_grid_cnt - pni->overlay_max * (1 + need_grid);
				}
			} 

			p->chemical_grid += need_grid;
			break;
		case normal_item_collection:
			if (p->collection_grid + need_grid > max_grid) {
				uint32_t last_grid_cnt = pitem->count % pni->overlay_max;
				if (p->collection_grid > max_grid || (p->collection_grid == max_grid && !last_grid_cnt))
					return 0;
				need_grid = max_grid - p->collection_grid;
				if (need_grid == 0) {
					count = (count + last_grid_cnt < pni->overlay_max) ? count : (pni->overlay_max - last_grid_cnt);
				} else if (count + last_grid_cnt > pni->overlay_max * (1 + need_grid)) {
					count = count + last_grid_cnt - pni->overlay_max * (1 + need_grid);
				}
			} 
			p->collection_grid += need_grid;
			break;
		case normal_item_card:
			if (p->card_grid + need_grid > max_grid) {
				uint32_t last_grid_cnt = pitem->count % pni->overlay_max;
				if (p->card_grid > max_grid || (p->card_grid == max_grid && !last_grid_cnt))
					return 0;
				need_grid = max_grid - p->card_grid;
				if (need_grid == 0) {
					count = (count + last_grid_cnt < pni->overlay_max) ? count : (pni->overlay_max - last_grid_cnt);
				} else if (count + last_grid_cnt > pni->overlay_max * (1 + need_grid)) {
					count = count + last_grid_cnt - pni->overlay_max * (1 + need_grid);
				}
			} 
			p->card_grid += need_grid;
			break;
	}

	pitem->count += count;

	KDEBUG_LOG(p->id, "ADD ITEM SUCC\t[%u %d]", itemid, count);
	return count;
}

int cache_reduce_kind_item(sprite_t* p, uint32_t itemid, uint32_t count)
{
	normal_item_t* pni = get_item(itemid);
	if (!pni) {
		KERROR_LOG(p->id, "item id invalid\t[%u %u %u]", p->id, itemid, count);
		return -1;
	}

	item_t* pitem = cache_get_bag_item(p, itemid);
	if (!pitem || pitem->count < count) {
		KERROR_LOG(p->id, "item not enough\t[%u %u %u %u]", p->id, itemid, count, (pitem ? pitem->count : 0));
		return -1;
	}

	
	int need_grid = get_need_grid(pni, pitem->count, pitem->count - count);
	
	KDEBUG_LOG(p->id, "REDUCE ITEM\t[%u %u %u %u %u %u %d]", pni->type, itemid, count, \
		p->chemical_grid, p->collection_grid, p->card_grid, need_grid);

	pitem->count -= count;
	switch(pni->type) {
	case normal_item_medical:
		p->chemical_grid += need_grid;
		break;
	case normal_item_collection:
		p->collection_grid += need_grid;
		break;
	case normal_item_card:
		p->card_grid += need_grid;
		break;
	}
	
	KDEBUG_LOG(p->id, "REDUCE ITEM SUCC\t[%u %u %u %u %u %u]", pni->type, itemid, pitem->count, \
		p->chemical_grid, p->collection_grid, p->card_grid);

	return 0;
}

int cache_item_have_cnt(sprite_t* p, uint32_t itemid, uint32_t itemcnt)
{
	item_t* pitem = cache_get_bag_item(p, itemid);
	return pitem && pitem->count >= itemcnt;
}

int exchange_item_normal(sprite_t* p, gift_info_t* pgi, uint32_t itemid)
{
	item_t add_items[100];
	memset (add_items, 0, sizeof(add_items));
	int loop;
	
	for (loop = 0; loop < pgi->itemcnt; loop ++) {
		uint32_t itemid = pgi->itemids[loop];
		uint32_t count = pgi->item_cnt[loop];
		add_items[loop].itemid = itemid;
		add_items[loop].count = cache_add_kind_item(p, itemid, count);
		if (add_items[loop].count != count) {
			for (int i = 0; i < 100; i ++) {
				if (add_items[loop].itemid == 0) {
					break;
				}
				cache_reduce_kind_item(p, add_items[i].itemid, add_items[i].count);
			}
			KERROR_LOG(p->id, "add item fail\t[%u %u]", itemid, count);	
			return cli_err_bag_full;
		}
	}

	uint32_t bag_cloth_cnt = get_bag_cloth_cnt(p);
	if (bag_cloth_cnt + pgi->clothcnt > get_max_grid(p)) {
		KERROR_LOG(p->id, "add cloth fail\t[%u %u]", bag_cloth_cnt, pgi->clothcnt);
		return cli_err_bag_full;
	}
	cache_reduce_kind_item(p, itemid, 1);
	KDEBUG_LOG(p->id, "pgi->clothcnt\t[%u]", pgi->clothcnt);
	for (loop = 0; loop < pgi->clothcnt; loop ++) {
		KDEBUG_LOG(p->id, "ex normal clothes\t[%u %u %u]", pgi->tag,pgi->cloth_lv[loop], pgi->cloth_type[loop]);
		cloth_t* psc = get_cloth(pgi->cloth_type[loop]);
		cloth_lvinfo_t* pclv;
		switch(pgi->tag){
			case 0:
				CHECK_CLOTH_LV_VALID(p, psc, pgi->cloth_type[loop], pgi->cloth_lv[loop]);
				pclv = &psc->clothlvs[pgi->cloth_lv[loop]];
				break;
			case 1:
				CHECK_CLOTH_PROD_VALID(p, psc, pgi->cloth_type[loop], pgi->cloth_lv[loop]-1);
				pclv=&psc->pro_attr[pgi->cloth_lv[loop]-1];
				break;
			default:
				return send_to_self_error(p, p->waitcmd, cli_err_cloth_not_existed, 1);
		}
		db_add_cloth(NULL, p->id, 0, psc, 0, pclv);
	}

	item_t items[1 + MAX_GIFT_CNT];
	uint32_t itemcnt = 1;
	items[0].itemid = itemid;
	items[0].count = -1;
	for (loop = 0; loop < pgi->itemcnt; loop ++) {
		items[itemcnt].itemid = pgi->itemids[loop];
		items[itemcnt].count = pgi->item_cnt[loop];
		normal_item_t* pni = get_item(items[itemcnt].itemid);
		if (pni && pni->function == item_for_pet_exchange)
			msg_log_pet_egg(pni->petinfo.beastid, pet_egg_exchange, 1);
		itemcnt ++;
		//KDEBUG_LOG(p->id,"aaadd item item=%u cnt=%u",items[itemcnt].itemid,items[itemcnt].count);
	}
	db_add_item_list(NULL, p->id, items, itemcnt);
	return 0;
}

int exchange_gift_spec_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid,itemcnt;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, itemcnt, j);
	KDEBUG_LOG(p->id, "EXCHANGEL ITEM\t[%u %u]", itemid,itemcnt);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_LV_FIT(p, p, pni->minlv, pni->maxlv);
	CHECK_ITEM_EXIST(p, itemid, itemcnt);
	CHECK_ITEM_FUNC(p, pni, item_for_exchange_spec);
	if(pni->specgift.quantities==0 && itemcnt!=1){//quantities==1表示可以批量处理 
		KERROR_LOG(p->id,"itemcnt too many [%u  %u  %u]",pni->specgift.quantities,itemid,itemcnt);	
		return send_to_self_error(p, p->waitcmd, cli_err_cnt_too_many, 1);
	}
	int idx = rand_type_idx(pni->specgift.count, pni->specgift.rates, RAND_COMMON_RAND_BASE);
	int count=0 ;
	for( uint32_t loop=0 ; loop<itemcnt ; loop++ ){
		count += pni->specgift.min[idx] + rand() % (pni->specgift.max[idx] - pni->specgift.min[idx] + 1);
	}
	
	switch (pni->specgift.ids[idx]) {
		case item_id_xiaomee:	
			p->xiaomee += count;
			db_add_xiaomee(NULL, p->id, count);
			monitor_sprite(p, "GET GIFT SPEC");
			break;
		case item_id_pkpt:
			if (p->pkpoint_day + count > MAX_PKPT_DAY(p))
				return send_to_self_error(p, p->waitcmd, cli_err_pkpt_day_max, 1);
			p->pkpoint_day += count;
			p->pkpoint_total += count;
			db_update_pkinfo(NULL, p->id, p->pkpoint_total, p->pkpoint_day, 2);
			break;
		case item_id_exp:
			if (p->experience >= MAX_TEMP_EXP_EX - 1)
				return send_to_self_error(p, p->waitcmd, cli_err_exp_too_much, 1);
			count = add_exp_to_sprite(p, count);
			break;
		case item_id_skill_expbox:
			p->skill_expbox += count;
			db_add_skill_expbox(NULL, p->id, count);
			break;
		case item_id_vacancy:
			p->vacancy_cnt += count;
			db_day_add_ssid_cnt(NULL, p->id, item_id_vacancy, count, -1);
			msg_log_vacancy(p->id, p->vacancy_cnt);
			break;
		case item_id_pet_expbox:
			p->expbox += count;
			db_set_expbox(NULL, p->id, p->expbox);
			break;
		default:
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	cache_reduce_kind_item(p, itemid, itemcnt);
	db_add_item(NULL, p->id, itemid, -1*itemcnt);

	response_proto_tri_uint32(p, p->waitcmd, pni->specgift.ids[idx], count,itemcnt, 1, 0);
	return 0;
}

int exchange_gift_pet_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
	KDEBUG_LOG(p->id, "EXCHANGEL ITEM PET\t[%u]", itemid);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_EXIST(p, itemid, 1);
	CHECK_LV_FIT(p, p, pni->minlv, pni->maxlv);
	CHECK_ITEM_FUNC(p, pni, item_for_pet_exchange);
	//if(pni->vip_only)
		//CHECK_USER_VIP(p);
	if(pni->petinfo.feed_type != 0 && pni->petinfo.feed_count > 0) {
		return db_get_item_feed_count(p,p->id,itemid,pni->petinfo.feed_type);
	}

	if (user_add_pet(p, pni->petinfo.beastid, pni->petinfo.beastlv, 1, pni->petinfo.rand_growth, pni->petinfo.rand_diff))
		return send_to_self_error(p, p->waitcmd, cli_err_pet_bag_full, 1);

	cache_reduce_kind_item(p, itemid, 1);
	db_add_item(NULL, p->id, itemid, -1);
	response_proto_uint32_uint32(p, p->waitcmd, pni->petinfo.beastid, pni->petinfo.beastlv, 1, 0);
	return 0;
}

int exchange_gift_normal_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
	KDEBUG_LOG(p->id, "EXCHANGEL ITEM NORMAL\t[%u]", itemid);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_EXIST(p, itemid, 1);
	CHECK_LV_FIT(p, p, pni->minlv, pni->maxlv);
	gift_info_t* pgi = NULL;
	if (pni->function == item_for_exchange_normal) {
		pgi = &pni->giftinfo;
		*(uint32_t *)p->session = 0xFFFFFFFF;
	} else  if (pni->function == item_for_exchange_rand){
		int idx = rand_type_idx(pni->randgift.grp_cnt, pni->randgift.percents, RAND_COMMON_RAND_BASE);
		pgi = &pni->randgift.gifts[idx];
		*(uint32_t *)p->session = idx;
	}else  if (pni->function == item_for_exchange_rand_prof){
		uint8_t arr[10]={0},count=0;
		for( uint32_t loop=0 ; loop < pni->rand_prof_gift.grp_cnt ; loop++ ){
			//DEBUG_LOG("ppppp[loop=%u val=%u]",loop,(pni->rand_prof_gift.prof[loop]>>p->prof)&1);
			if(count<10 && ((pni->rand_prof_gift.prof[loop]>>p->prof)&1)==1){
				arr[count++]=loop;
			}
		}
		int idx =count ? arr[rand()%count] : 0;	
		KDEBUG_LOG(p->id,"ppppprof gift[idx=%u giftprof=%u prof=%u count=%u]",idx,pni->rand_prof_gift.prof[idx],p->prof,count);
		pgi = &pni->randgift.gifts[idx];
		*(uint32_t *)p->session = idx;
	}
	int ret = exchange_item_normal(p, pgi, itemid);
	if (ret)
		return send_to_self_error(p, p->waitcmd, ret, 1);

	return db_get_packet_cloth_list(p);
}

int spec_item_exchange_get(sprite_t* p, exchange_info_t* pei, uint32_t count)
{
	for (int loop = 0; loop < pei->getcnt; loop ++) {
		switch (pei->getitems[loop].itemid) {
			case item_id_xiaomee:
				p->xiaomee += pei->getitems[loop].count * count;
				db_add_xiaomee(NULL, p->id, pei->getitems[loop].count * count);
				monitor_sprite(p, "EXCHANGE");
				break;
			case item_id_pkpt:
				p->pkpoint_total += pei->getitems[loop].count * count;
				db_update_pkinfo(NULL, p->id, p->pkpoint_total, p->pkpoint_day, 2);
				break;
			case item_id_draw_max_cnt:
				p->draw_max_cnt +=pei->getitems[loop].count;
				KDEBUG_LOG(p->id,"dad draw_max_cnt:%u ",p->draw_max_cnt);
				db_day_add_ssid_cnt(0, p->id, item_id_draw_max_cnt, pei->getitems[loop].count, -1);
				break;
			default:
				KERROR_LOG(p->id, "exchange item special not supported\t[%u]", pei->id);
				return 0;
		}
	}
	return 0;
}

int cli_exchange(sprite_t* p, Cmessage* c_in)
{
	cli_exchange_in* p_in = P_IN;
	exchange_info_t* pei = get_exchange_info(p_in->eid);
	KDEBUG_LOG(p->id, "EXCHANGE\t[%u %u]", p_in->eid, p_in->cnt);
	if (!pei) {
		KERROR_LOG(p->id, "exchange id invalid\t[%u %u]", p_in->eid, p_in->cnt);
		return send_to_self_error(p, p->waitcmd, cli_err_exchange_id_invalid, 1);
	}
	uint32_t nowtime = get_now_sec();
	int day_idx = get_now_tm()->tm_wday;
	//KDEBUG_LOG(p->id,"---day limit [day=%u] [daylimit=%u] [starttime=%u]",day_idx,pei->day_limit, pei->start_time);
	if( (pei->day_limit>>day_idx)&1 || pei->start_time > nowtime || pei->end_time < nowtime){
		KERROR_LOG(p->id, "day limit\t[%u %u]", pei->day_limit,day_idx);
		return send_to_self_error(p, p->waitcmd, cli_err_times_limit, 1);
	}
	item_t* pday;
	if (pei->just_once) {
		pday = cache_get_day_limits(p, pei->id + ssid_exchange_base);
		if (p_in->cnt != 1 || pday->count){
			return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
		}
	}

	CHECK_LV_FIT(p, p, pei->minlv, MAX_SPRITE_LEVEL);
	int loop;
	for (loop = 0; loop < pei->paycnt; loop ++) {
		int ret = item_enough_check(p, pei->payitems[loop].itemid, pei->payitems[loop].count * p_in->cnt);
		if (ret)
			return send_to_self_error(p, p->waitcmd, ret, 1);
	}
	item_t add_items[100];
	memset(add_items, 0, sizeof(add_items));
	if(pei->limit_cnt != 0){
		if(get_now_sec() < 14*3600){
			KERROR_LOG(p->id, "not right time\t");
			return send_to_self_error(p, p->waitcmd, cli_err_times_limit, 1);
		}
		if(p_in->cnt!=1 || pei->getcnt==0){
			KERROR_LOG(p->id, "exchange cnt invalid\t[%u %u]", p_in->eid, p_in->cnt);
			return send_to_self_error(p, p->waitcmd, cli_err_item_cnt_max, 1);
		}
		mole2_add_val_in db_in;
		db_in.userid = pei->getitems[0].itemid;
		db_in.limit = pei->limit_cnt;
		*((uint32_t*)p->session)=pei->id;
		return send_msg_to_db(p, p->id, mole2_add_val_cmd,&db_in);
    }
	switch (pei->type) {
		case item_special:
			spec_item_exchange_get(p, pei, p_in->cnt);
			break;
		case item_normal:
			KDEBUG_LOG(p->id, "START EXCHANG ITEM\t[%u]", p_in->eid);
			for (loop = 0; loop < pei->getcnt; loop ++) {
				add_items[loop].itemid = pei->getitems[loop].itemid;
				add_items[loop].count = cache_add_kind_item(p, pei->getitems[loop].itemid, pei->getitems[loop].count * p_in->cnt);
				if (add_items[loop].count != pei->getitems[loop].count * p_in->cnt) {
					for (int i = 0; i < 100; i ++) {
						if (add_items[loop].itemid == 0) {
							break;
						}
						cache_reduce_kind_item(p, add_items[i].itemid, add_items[i].count);
					}
					return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
				}
			}
			for (loop = 0; loop < pei->getcnt; loop ++) {
				KDEBUG_LOG(p->id, "EXCHANGE ADD ITEM\t[%u %u]", pei->getitems[loop].itemid, pei->getitems[loop].count * p_in->cnt);
				db_add_item(NULL, p->id, pei->getitems[loop].itemid, pei->getitems[loop].count * p_in->cnt);
				sysinfo_t* psys = get_sys_info(sys_info_get_item, pei->getitems[loop].itemid);
				if (psys)
					noti_all_get_item(p, psys);
			}
			break;
		case item_cloth:
			KDEBUG_LOG(p->id, "START EXCHANG CLOTH\t[%u]", p_in->eid);
			if (pei->getcnt * p_in->cnt + get_bag_cloth_cnt(p) > get_max_grid(p))
				return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
			for (loop = 0; loop < pei->getcnt; loop ++) {
				for (uint32_t loopi = 0; loopi < p_in->cnt; loopi ++) {
					cloth_t* psc = get_cloth(pei->getitems[loop].itemid);
					if (!psc) {
						KERROR_LOG(p->id, "cloth info none\t[%u %u]", p_in->eid, pei->getitems[loop].itemid);
						return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
					}
					cloth_lvinfo_t* pcl =NULL;
					//KDEBUG_LOG(p->id,"lllllllllllllll extent=%u",pei->extents[loop]);
					if(pei->extents[loop] == 0){//0则从等级装备中选择
						pcl = &psc->clothlvs[0];
					}else if(pei->extents[loop]>PRODUCT_LV_CNT){//5则随机产生品质装备
						uint32_t rate=rand()%PRODUCT_LV_CNT;
						pcl = &psc->pro_attr[rate];
					}else{//1~4产生指定品质装备
						pcl = &psc->pro_attr[pei->extents[loop] - 1];
					}
					if (!pcl->valid) {
						KERROR_LOG(p->id, "cloth info none\t[%u %u]", p_in->eid, pei->getitems[loop].itemid);
						return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
					}
					db_add_cloth(NULL, p->id, 0, psc, 0, pcl);
				}
			}
			break;
		default:
			KERROR_LOG(p->id, "exchange type err\t[%u %u]", p_in->eid, pei->type);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	for (loop = 0; loop < pei->paycnt; loop ++) {
		item_enough_del(p, pei->payitems[loop].itemid, pei->payitems[loop].count * p_in->cnt);
	}

	msg_log_pkpt_exchange(p->id, p_in->eid);
	msg_log_exchange(p_in->eid,p_in->cnt);
	if (pei->just_once) {
		pday->count ++;
		db_day_add_ssid_cnt(NULL, p->id, ssid_exchange_base + pei->id, 1, 1);
	}

	if (pei->type == item_cloth)
		return db_get_packet_cloth_list(p);
	KDEBUG_LOG(p->id, "RESPONSE PROTO\t[%u %u]", p->pkpoint_total, p_in->eid);
	response_proto_uint32(p, p->waitcmd, p->pkpoint_total, 1, 0);
	return 0;
}

int get_exchange_cnt_info(sprite_t* p, Cmessage* c_in)
{
	get_exchange_cnt_info_out out;
	GList* pday = g_hash_table_get_values(p->day_limits);
	GList* head = pday;
	while (pday) {
		item_t* pi = (item_t *)pday->data;
		if (pi->itemid > ssid_exchange_base \
			&& pi->itemid < ssid_exchange_base + MAX_EXCHANGE_INFO_CNT \
			&& pi->count) {
			stru_id_cnt day;
			day.id = pi->itemid - ssid_exchange_base;
			day.count = pi->count;
			out.e_info.push_back(day);
		}
		pday = pday->next;
	}
	g_list_free(head);
	return send_msg_to_self(p, p->waitcmd, &out, 1);
}

int get_item_day_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t day_count;

	int i = 0;
	UNPKG_H_UINT32(buf, day_count, i);
	CHECK_BODY_LEN(len, 4 + day_count * 12);
	for (uint32_t loop = 0; loop < day_count; loop ++) {
		uint32_t ssid, totalcnt, count;
		UNPKG_H_UINT32(buf, ssid, i);
		UNPKG_H_UINT32(buf, totalcnt, i);
		UNPKG_H_UINT32(buf, count, i);

		if (ssid == survey_id_age_sex || ssid == item_id_help_boss || ssid == item_id_help_clean) {
			continue;
		} else if (ssid == ssid_vip_dragon_active) {
			p->vip_dragon_cnt = totalcnt;
		} else if (ssid == ssid_vip_dragon_2) {
			p->vip_dragon_2 = totalcnt;
		} else if (ssid == item_id_novip_draw) {
			p->draw_cnt_day = count;
			p->draw_cnt_total = totalcnt;
		} else if (ssid == item_id_vip_draw_day) {
			p->vip_draw_cnt_day = count;
			p->vip_draw_cnt_total = totalcnt;
		} else if (ssid == item_id_vip_draw_give) {
			p->vip_draw_cnt_gift_use = totalcnt;
		} else if (ssid == item_id_draw_max_cnt) {
			p->draw_max_cnt+=count;
		} else if (ssid == item_id_double_exp) {
			if (count) {
				p->double_exptime = count;
				p->start_dexp_time = p->login_time;
			} else {
				p->double_exptime = 0XFFFFFFFF;
			}
			KDEBUG_LOG(p->id, "DOUBLE EXP TIME\t[%u]", p->double_exptime);
		} else if (ssid == item_id_vacancy) {
			p->vacancy_cnt = totalcnt;
		} else if (ssid == item_id_hang_exp) {
			p->hang_exp_time = count;
		} else if (ssid == item_id_hang_pet) {
			p->hang_pet_time = count;
		} else if (ssid == item_id_lucky_star) {
			p->luckystar_count = totalcnt;
		} else if (ssid == item_id_hang_skill) {
			p->hang_skill_time = count;
		}else if(ssid == ssid_add_energy){
			p->recover_energy=count;
		}else if(ssid == ssid_precious_box){
			p->open_box_times=count;
		}else if(ssid == ssid_hangup_activity_time){
			p->hangup_activity_time=count;
		}else if(ssid == ssid_hangup_reward){
			p->hangup_rewards=count;
		}else if(ssid == ssid_hunt_rarebeast_times){
			p->times_for_hunting_rarebeast=count;
		}else if(ssid == ssid_trainning_last_time){
			p->trainning_last_time=count;
		}else if(ssid == ssid_trainning_least_time){
			p->trainning_least_time=count;
		}else if(ssid<61999 && ssid >61000){//for mail
			item_t* pi = (item_t *)g_slice_alloc(sizeof(item_t));
			pi->itemid = ssid;
			pi->count = totalcnt;
			//DEBUG_LOG("xxxx %u",totalcnt);
			g_hash_table_insert(p->day_limits, &pi->itemid, pi);
		}else if(ssid == ssid_week_pvp_scores){
			p->pvp_scores=count;
		}else if(ssid == ssid_pvp_pk_fail_times ){
			p->	pvp_pk_fail_times[0]=count;
		}else if(ssid == item_id_shape_shifting ){
			p->shapeshifting.itemid = count;
		}else if(ssid == item_id_shapeshifting_time){
			p->shapeshifting.lefttime = count;
		}else if(ssid == ssid_pvp_pk_fail_times_2 ){
			p->	pvp_pk_fail_times[1]=count;
		}else if(ssid == ssid_try_vip_activity ){
			item_t* pi = (item_t *)g_slice_alloc(sizeof(item_t));
			pi->itemid = ssid;
			pi->count = totalcnt;
			g_hash_table_insert(p->day_limits, &pi->itemid, pi);
		}else if (ssid > ssid_total_base && ssid < ssid_exchange_base + MAX_EXCHANGE_INFO_CNT) {
			//8000~30100
			item_t* pi = (item_t *)g_slice_alloc(sizeof(item_t));
			pi->itemid = ssid;
			pi->count = totalcnt;
			g_hash_table_insert(p->day_limits, &pi->itemid, pi);
		}else if(ssid == ITEM_ID_DRAGON_EGG_6){//焰鳞小白龙精灵蛋
			item_t* pi = (item_t *)g_slice_alloc(sizeof(item_t));
			pi->itemid = ssid;
			pi->count = totalcnt;
			//DEBUG_LOG("xxxx %u",totalcnt);
			g_hash_table_insert(p->day_limits, &pi->itemid, pi);
		}else {
			item_t* pi = (item_t *)g_slice_alloc(sizeof(item_t));
			pi->itemid = ssid;
			pi->count = count;
			g_hash_table_insert(p->day_limits, &pi->itemid, pi);
		}
	}
	try_to_send_for_noti(p);
	uint32_t buff[2] = {0,200000};
	shapeshifting_start_effect(p);
	return send_request_to_db(p, p->id, proto_db_get_monster_hb_range, buff, 8);
}

int update_cloth_duration(void* owner, void* data)
{
	sprite_t* p = (sprite_t *)owner;
	//g_hash_table_foreach_steal(p->cloth_timelist, try_disappear_cloth_after, p);
	db_sync_cloth_duration(p);
	ADD_ONLINE_TIMER(p, n_update_cloth_duration, NULL, 600);
	return 0;
}

int db_add_item_feed_count(sprite_t* p, userid_t uid,uint32_t itemid,uint32_t type,int count)
{
	int k = 0;
	int8_t out[32];
	
	PKG_H_UINT32(out, type, k);
	PKG_H_UINT32(out, itemid, k);
	PKG_H_UINT32(out, count, k);
	KDEBUG_LOG(uid, "DB ADD ITEM FEED\t[id=%u %X cnt=%d]",  itemid, type, count);
	return send_request_to_db(p, uid, proto_db_add_item_feed_count, out, k);
}

int db_get_item_feed_count(sprite_t* p, userid_t uid,uint32_t itemid,uint32_t type)
{
	int k = 0;
	int8_t out[32];
	
	PKG_H_UINT32(out, type, k);
	PKG_H_UINT32(out, itemid, k);
	KDEBUG_LOG(uid, "DB GET ITEM FEED\t[id=%u %X]",  itemid, type);
	return send_request_to_db(p, uid, proto_db_get_item_feed_count, out, k);
}

int add_item_feed_count_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t type, itemid, count;

	int i = 0;
	UNPKG_H_UINT32(buf, type, i);
	UNPKG_H_UINT32(buf, itemid, i);
	UNPKG_H_UINT32(buf, count, i);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, j);
	PKG_UINT32(msg, count, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_item_feed_count_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t type, itemid, count;

	int i = 0;
	UNPKG_H_UINT32(buf, type, i);
	UNPKG_H_UINT32(buf, itemid, i);
	UNPKG_H_UINT32(buf, count, i);

	normal_item_t* pni = get_item(itemid);
	mole2_init_feed_count_in db_in;
	switch(p->waitcmd) {
		case proto_cli_exchange_gift_pet:
			if (count < pni->petinfo.feed_count) {
				return send_to_self_error(p, p->waitcmd, cli_err_feed_count_err, 1);
			}
			if (user_add_pet(p, pni->petinfo.beastid, pni->petinfo.beastlv, 0, 0, rand_diff_default))
				return send_to_self_error(p, p->waitcmd, cli_err_pet_bag_full, 1);
			
			db_in.type = 0xE0000000;
			db_in.itemid = 290015;
			send_msg_to_db(NULL, p->id, mole2_init_feed_count_cmd, &db_in);
			msg_log_pet_activate_get(pni->petinfo.beastid);
			cache_reduce_kind_item(p, itemid, 1);
			db_add_item(NULL, p->id, itemid, -1);
			response_proto_uint32_uint32(p, p->waitcmd, pni->petinfo.beastid, pni->petinfo.beastlv, 1, 0);
			return 0;
		default:
			break;
	}

	itemid = *((uint32_t*)p->session);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, j);
	PKG_UINT32(msg, count, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int add_item_once_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t itemid;
	UNPKG_UINT32(body, itemid, j);

	uint32_t type = 0x70000000;
	if(itemid != 290015) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}

	msg_log_feed_pet(p->id);

	return db_add_item_feed_count(p, p->id,itemid,type,1);
}

int set_sth_done_for_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t type;
	uint32_t itemid;
	UNPKG_UINT32(body, itemid, j);

	switch(itemid) {
		case 1:
			type = 0xF0000001;
			break;
		case 3:
			type = 0xF0000000;
			break;
		default:
			return send_to_self_error(p,p->waitcmd,cli_err_item_id_invalid,1);
	}

	itemid = 290015;
	return db_add_item_feed_count(p,p->id,itemid,type,1);
}

int get_sth_done_for_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t itemid;
	uint32_t type = 0;
	UNPKG_UINT32(body, itemid, j);
	*((uint32_t*)p->session) = itemid;

	switch(itemid) {
		case 0:
			type = 0x70000000;
			break;
		case 1:
			type = 0xF0000001;
			break;
		case 2:
			type = 0xE0000000;
			break;
		case 3:
			type = 0xF0000000;
			break;
		default:
			return send_to_self_error(p,p->waitcmd,cli_err_item_id_invalid,1);
	}

	itemid = 290015;
	return db_get_item_feed_count(p,p->id,itemid,type);
}

int add_item_feed_count_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t itemid;
	UNPKG_UINT32(body, itemid, j);

	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);	

	if (pni->function != item_for_pet_exchange) {
		KERROR_LOG(p->id, "gift pet item id invalid\t[%u]", itemid);
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}

	if(pni->petinfo.feed_type == 0 || pni->petinfo.feed_count <= 0) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}

	if(!cache_item_have_cnt(p, itemid, 1)) {
		return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1);
	}

	return db_add_item_feed_count(p,p->id,itemid,pni->petinfo.feed_type,1);
}

/**
 * @brief 变身卡失效
 */
int shapeshifting_lose_effect(sprite_t* p)
{
	KDEBUG_LOG(p->id,"shapeshifting_lose_effect:%u %u",p->shapeshifting.itemid,p->shapeshifting.pettype,p->shapeshifting.lefttime);
	normal_item_t* pni = get_item(p->shapeshifting.itemid);
	if(pni && pni->function == item_for_shapeshifting){
		reduce_second_attr(p,&pni->shapeshifting_info.attr,2);
	}
	p->shapeshifting.itemid=0;
	p->shapeshifting.lefttime=0;
	p->shapeshifting.pettype=0;

	cli_user_shapeshifting_out out; 
	out.uid=p->id;
	out.petid=0;
	send_msg_to_map2(p->tiles,cli_user_shapeshifting_cmd,&out);
	return 0;
}

/**
 * @brief 变身卡生效
 */
int shapeshifting_start_effect(sprite_t* p)
{

	normal_item_t* pni = get_item(p->shapeshifting.itemid);
	KDEBUG_LOG(p->id,"shapeshifting_start_effect:%u %u %u",p->shapeshifting.itemid,p->shapeshifting.pettype,p->shapeshifting.lefttime);
	if(pni && pni->function == item_for_shapeshifting && p->shapeshifting.lefttime &&
			get_beast(pni->shapeshifting_info.pettype)){
		p->shapeshifting.pettype=pni->shapeshifting_info.pettype;
		KDEBUG_LOG(p->id,"shapeshifting_start_effect22:%u %u",p->shapeshifting.itemid,p->shapeshifting.pettype);
		add_second_attr(p,&pni->shapeshifting_info.attr,2);
	}else{
		p->shapeshifting.itemid=0;
		p->shapeshifting.lefttime=0;
		p->shapeshifting.pettype=0;
	}
	return 0;
}

//百变之术
#define     SHAPE_SHIFTING_SKILL 100008

/**
 * @brief 使用变身卡
 */
int cli_user_shapeshifting(sprite_t* p, Cmessage* c_in)
{
	cli_user_shapeshifting_in *p_in=P_IN;
	uint32_t itemid=p_in->itemid;
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	//CHECK_LV_FIT(p, p, pni->minlv, pni->maxlv);
	CHECK_ITEM_EXIST(p, itemid, 1);
	CHECK_ITEM_FUNC(p, pni, item_for_shapeshifting);
	beast_t *pb=get_beast(pni->shapeshifting_info.pettype);
	if(!pb ){
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	uint32_t itemlv=(itemid/1000)%10+1;
	skill_info_t* psi = get_skill_info(SHAPE_SHIFTING_SKILL);
	skill_t* ps = cache_get_skill(p, SHAPE_SHIFTING_SKILL);
	if (!psi || !ps || IS_OLD_PROF(p->prof)) {
		return send_to_self_error(p, p->waitcmd, cli_err_skill_not_have, 1);
	}
	if(itemlv > ps->skill_level){
		return send_to_self_error(p, p->waitcmd, cli_err_level_not_fit, 1);
	}
	
	if(p->shapeshifting.itemid && p->shapeshifting.lefttime){
		shapeshifting_lose_effect(p);
	}
	cache_reduce_kind_item(p, itemid, 1);
	db_add_item(NULL,p->id,itemid,-1);
	p->shapeshifting.itemid=itemid;
	
	p->shapeshifting.itemid=itemid;
	p->shapeshifting.lefttime=1800;

	db_day_set_ssid_cnt(NULL, p->id, item_id_shape_shifting , p->shapeshifting.itemid,p->shapeshifting.itemid);
	db_day_set_ssid_cnt(NULL, p->id, item_id_shapeshifting_time ,p->shapeshifting.lefttime,p->shapeshifting.lefttime);
	shapeshifting_start_effect(p);
	uint32_t add_exp=ps->skill_exp+itemlv*5+20;
	//KDEBUG_LOG(p->id,"xxx %u %u %u",ps->skill_exp,itemlv,add_exp);
	cache_update_skill_level(p, ps, psi, add_exp );
	db_update_skill_info(0, p->id, ps);
	cli_user_shapeshifting_out out; 
	out.uid=p->id;
	out.petid=pni->shapeshifting_info.pettype;
	send_msg_to_map2(p->tiles,p->waitcmd,&out);
	p->waitcmd=0;
	return 0;
}
