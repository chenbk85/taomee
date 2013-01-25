#include "honor.h"
#include "sns.h"

honor_attr_t* get_honor_attr_no_check(uint32_t honor_id)
{
	if (honor_id < BEGIN_HONOR_ID || honor_id > MAX_HONOR_NUM)
		return NULL;
	return &honors[honor_id - BEGIN_HONOR_ID];
}

honor_attr_t* get_honor_attr(uint32_t honor_id)
{
	honor_attr_t* p_ha = get_honor_attr_no_check(honor_id);
	return p_ha && p_ha->honor_id == honor_id ? p_ha : NULL;
}

int load_honor_titles(xmlNodePtr cur)
{
	memset(honors, 0, sizeof(honors));
	uint32_t id;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Title")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			honor_attr_t* p_ha = get_honor_attr_no_check(id);
			if (!p_ha || p_ha->honor_id) {
				ERROR_RETURN(("err or duplicate honor title id: %u", id), -1);
			}

			p_ha->honor_id = id;
			p_ha->pre_honor_cnt = decode_xml_prop_arr_int_default((int *)p_ha->pre_honor_id, MAX_PRE_HONOR_NUM, cur, "PreTitle", 0);
			decode_xml_prop_uint32_default(&p_ha->proflv, cur, "ProfLv", 0);
			decode_xml_prop_uint32_default(&p_ha->prof, cur, "Prof", -1);
			decode_xml_prop_uint32_default((uint32_t *)&p_ha->clear_type, cur, "ClearType", 0);

			decode_xml_prop_uint32_default((uint32_t *)&p_ha->attr.hp_max, cur, "HpMax", 0);
			decode_xml_prop_uint32_default((uint32_t *)&p_ha->attr.mp_max, cur, "MpMax", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.attack, cur, "Atk", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.defense, cur, "Def", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.speed, cur, "Speed", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.spirit, cur, "Spirit", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.resume, cur, "Resume", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.hit, cur, "Hit", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.dodge, cur, "Dodge", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.crit, cur, "Crit", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.fightback, cur, "FightBack", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.mdefense, cur, "Mdef", 0);
			decode_xml_prop_uint16_default((uint16_t *)&p_ha->attr.mattack, cur, "Matk", 0);
		}
	
		cur = cur->next;
	}

	return 0;
}

void pkg_user_honor(gpointer key, gpointer value, gpointer data)
{
	cli_get_honor_list_out* p_out = (cli_get_honor_list_out *)data;
	p_out->honorlist.push_back(*(uint32_t *)key);
}

void pkg_cloth_honor(sprite_t* p, cli_get_honor_list_out* p_out)
{
	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t*)pcloths->data;
		cloth_t* psc = get_cloth(pc->clothtype);
		if (psc && psc->honor_id && get_honor_attr(psc->honor_id))
			p_out->honorlist.push_back(psc->honor_id);
		pcloths = pcloths->next;
	}
	g_list_free(head);
}

int cli_get_honor_list(sprite_t *p, Cmessage* c_in)
{
	cli_get_honor_list_out cli_out;
	pkg_cloth_honor(p, &cli_out);
	g_hash_table_foreach(p->user_honors, pkg_user_honor, &cli_out);

	if (p->relationship.flag == relation_master) {
		uint32_t masterlv = calc_relation_level(p->relationship.exp);
		cli_out.honorlist.push_back(relation_level[masterlv - 1][1]);
	}

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_use_honor(sprite_t* p, Cmessage* c_in)
{
	cli_use_honor_in* p_in = P_IN;
	KDEBUG_LOG(p->id, "USE HONOR CMD\t[%u]", p_in->honorid);
	if(!ISVIP(p->flag) && p_in->honorid==VIP_TITLE ){
		DEBUG_LOG("not vip ,ignore HONOR:%u", p_in->honorid);
		return send_to_self_error(p, p->waitcmd, cli_err_not_vip, 1);
	}
	if (p_in->honorid && !check_honor_exist(p, p_in->honorid)) {
		return send_to_self_error(p, p->waitcmd, cli_err_honor_not_existed, 1);
	}

	return db_set_user_honor(p, p->id, p_in->honorid);
}

int use_honor_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t honor_id = *(uint32_t *)buf;
	
	set_user_honor(p, honor_id);
	modify_sprite_second_attr(p);
	notify_user_honor_up(p, 0, 1, 1);

	return 0;
}

int check_honor_exist(sprite_t* p, uint32_t honor_id)
{
	if (IS_TEMP_HONOR(honor_id) && honor_id != VIP_TITLE) {
		GList* pcloths = g_hash_table_get_values(p->body_cloths);
		GList* head = pcloths;
		while (pcloths) {
			body_cloth_t* pc = (body_cloth_t*)pcloths->data;
			cloth_t* psc = get_cloth(pc->clothtype);
			if (psc && psc->honor_id == honor_id)
				return 1;
			pcloths = pcloths->next;
		}
		g_list_free(head);
	} else if (IS_MASTER_HONOR(honor_id)) {
		if (p->relationship.flag == relation_master) {
			uint32_t masterlv = calc_relation_level(p->relationship.exp);
			return relation_level[masterlv - 1][1] >= honor_id;
		}
		return 0;
	} else {
		return g_hash_table_lookup(p->user_honors, &honor_id) != NULL;
	}
	return 0;
}


