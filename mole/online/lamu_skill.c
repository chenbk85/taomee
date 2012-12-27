#include <libtaomee/time/time.h>
#include <statistic_agent/msglog.h>
#include <libtaomee/log.h>

#include "exclu_things.h"
#include "lamu_skill.h"

static struct skill_item skill_items[MAX_SKILL_NUM];

/* @breif load skill.xml file
 */
int load_skill_config(xmlNodePtr cur_node)
{
	uint32_t skill_id;
	struct skill_item *skill = NULL;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Item")) {
			DECODE_XML_PROP_UINT32(skill_id, cur_node, "ID");
			if ((skill_id > MAX_SKILL_NUM) || (skill_id < 1)) {
				ERROR_RETURN(("Failed to Parse skill File, skill_id %u", skill_id), -1);
			}
			skill = &(skill_items[skill_id - 1]);
			skill->id = skill_id;

			xmlChar* str;
			DECODE_XML_PROP(cur_node, "ItemID", str);
			int i = 0, cnt = 0, k, itemid;
			size_t slen = strlen((const char*)str);
			for (; (i != MAX_ITEM_NUM) && (cnt != slen); ++i, cnt += k) {
				sscanf((const char*)str + cnt, "%d%n", &itemid, &k);
				if (!get_item_prop(itemid)) {
					ERROR_RETURN(("invalid itemid %u", itemid), -1);
				}
				skill->item_id[i] = itemid;
			}
			xmlFree(str);

			DECODE_XML_PROP_INT(skill->vip_limit, cur_node, "VIPLimit");
			DECODE_XML_PROP_INT(skill->pl_limit, cur_node, "PLLimit");
			DECODE_XML_PROP_INT(skill->sub_attr, cur_node, "SUB");
			DECODE_XML_PROP_INT(skill->add_change, cur_node, "AddChange");
			DECODE_XML_PROP_INT(skill->need_change, cur_node, "NeedChange");
		}
		cur_node = cur_node->next;
	}
	return 0;
}

/* @brief get chagne level
 */
uint32_t get_change_level(uint32_t change_value)
{
	switch(change_value) {
		case 0 ... 39:
			return 0;
		case 40 ... 179:
			return 1;
		case 180 ... 659:
			return 2;
		case 660 ... 1339:
			return 3;
		case 1340 ... 2659:
			return 4;
		case 2660 ... 4279:
			return 5;
		case 4280 ... 6839:
			return 6;
		case 6840 ... 9799:
			return 7;
		case 9800 ... 13999:
			return 8;
		case 14000 ... 18699:
			return 9;
		default:
			return 10;
	}
	return 0;
}

/* @brief get add change day limit
 */
uint32_t get_day_limit(pet_t *pet)
{
	uint32_t pl_day_limit[] = {10, 10, 20, 20, 30, 30, 40, 40, 50, 50, 60};
	uint32_t sl_day_limit[] = {20, 20, 40, 40, 60, 60, 80, 80, 100, 100, 120};
	uint32_t level = get_change_level(pet->skill_value);
	if (PET_IS_SUPER_LAHM(pet)) {
		return sl_day_limit[level];
	} else {
		return pl_day_limit[level];
	}
	return 0;
}

/* @breif get skill info according lamu type and  skill type
 */
struct skill_item*  get_lahm_skill(uint32_t type, uint32_t bits)
{
	if (type < 3 && bits < 12) {
		uint32_t idx = bits * 3 + type;
		if(idx < MAX_SKILL_NUM && skill_items[idx].id == (idx + 1)) {
			return (skill_items + idx);
		}
	}
	return NULL;
}

/* @breif check if id is valid, 1 sucess, 0 fail
 */
int check_itemid(sprite_t *p, uint32_t itemid, int skill_type, int *item_skill_type)
{
	int loop;
	int i = 0;
	int skill_sub = 0;
	for (loop = 0; loop < skill_type; loop++)
	{
		for (i = 0; i < MAX_ITEM_NUM; i++)
		{
			if (itemid == skill_items[loop].item_id[i])
			{
				*item_skill_type = loop + 1;
				skill_sub = skill_type - (*item_skill_type);

				DEBUG_LOG("%d %d %d", skill_sub, skill_type, (*item_skill_type));

				if (skill_sub%3 == 0)
				{
				    return 1;
				}
			}
		}
	}

	return 0;
}


/* @brief according to skill type, check if lamu have got five stage
 */
int check_five_stage_accord_skill(sprite_t *p, uint32_t skill_type)
{
	if (p->followed == NULL) {
		ERROR_RETURN(("no lamu followed %u", p->id), -1);
	}
	int bit_shift = (skill_type - 1) % 3;
	return (p->followed->skill_bits & (0x01 << bit_shift));
}

/* @brief check if this lamu enter five stage
 */
int enter_five_stage(pet_t *pet)
{
	if (pet == NULL) {
		ERROR_RETURN(("no lamu followed"), -1);
	}
	return (pet->skill_bits & 7);
}

/* @brief check if have this skill, 1 sucess, 0 fail
 */
int check_skill(sprite_t *p, uint32_t skill_type)
{
	if (p->followed == NULL) {
		ERROR_RETURN(("no lamu followed %u", p->id), -1);
	}
	int array_index = (skill_type - 1) % 3;
	int bit_index = (skill_type - 1) / 3;
	if (bit_index >= 32) {
		ERROR_RETURN(("bit index too large %u", bit_index), -1);
	}
	uint32_t have_skill = (p->followed->skill_ex[array_index] & (0x01 << bit_index));
	if (have_skill && (skill_type > 3)) {
		return check_five_stage_accord_skill(p, skill_type);
	}
	return have_skill;
}

/* @breif add item by use skill
 */
int get_item_by_skill_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	/*pet id, skill type, itemid*/
	CHECK_BODY_LEN(bodylen, 12);

	if (p->followed == NULL) {
			ERROR_RETURN(("not followed lamu %u", p->id), -1);
	}

	uint32_t petid, skill_type, itemid;
	int j = 0;
	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, skill_type, j);
	if ((skill_type > MAX_SKILL_NUM) || (skill_type < 1) || (skill_items[skill_type - 1].id ==0)) {
		ERROR_RETURN(("skill type wrong %u %u %u", p->id, skill_type, MAX_SKILL_NUM), -1);
	}
	UNPKG_UINT32(body, itemid, j);
	int item_skill_type = 0;
	if (!check_itemid(p, itemid, skill_type, &item_skill_type)) {
		ERROR_RETURN(("wrong itemid %u %u %u %u", p->id, skill_type, itemid, item_skill_type), -1);
	}
	/*ID号为零直接返回*/
	if (itemid == 0) {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, p->followed->skill_value, i);
		init_proto_head(msg, p->waitcmd, i);
		return  send_to_self(p, msg, i, 1);
	}

	/*save item id, add change value, skill type*/
	*(uint32_t *)(p->session) = itemid;
	*(uint32_t *)(p->session + 8) = skill_type;
	/*check if have this skill, 126 map not check*/
	uint32_t not_check = (p->tiles->id == 126 && itemid == 190591)
						|| (p->tiles->id == 130 && itemid == 190611)
						|| (p->tiles->id == 134 && itemid == 190625);


	if (!not_check) {
		/*lamu should do action*/
		if (p->lamu_action == 0) {
			DEBUG_LOG("lamu action zero %u", p->id);
			return send_to_self_error(p, p->waitcmd, -ERR_action_have_fini, 1);
		}
		if (!check_skill(p, skill_type)) {
			DEBUG_LOG("have not this skill %u %u", p->id, skill_type);
			return send_to_self_error(p, p->waitcmd, -ERR_lahm_have_not_this_skill, 1);
		}
	}
	uint32_t day_limit = 0;
	if (ISVIP(p->flag)) {
		day_limit = skill_items[item_skill_type - 1].vip_limit;
	} else {
		day_limit = skill_items[item_skill_type -1].pl_limit;
	}

	uint32_t db_buf[7];
	db_buf[0] = item_skill_type;
	db_buf[1] = itemid;
	db_buf[2] = day_limit;
	db_buf[3] = p->followed->id;
	if (check_skill(p, skill_type)) {
		int add_change = skill_items[skill_type - 1].add_change;
		if (PET_IS_SUPER_LAHM(p->followed)) {
			add_change *= 2;
		}
		db_buf[4] = add_change;
		db_buf[5] = get_day_limit(p->followed);
		*(uint32_t *)(p->session + 4) = add_change;
	} else {
		*(uint32_t *)(p->session + 4) = 0;
		db_buf[4] = 0;
		db_buf[5] = 0;
	}
	db_buf[6] = 0;
	return send_request_to_db(SVR_PROTO_GET_ITEM_BY_USER_SKILL, p, sizeof(db_buf), db_buf, p->id);
}

int lahm_use_skill_do_work_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	/*pet id, skill type, itemid*/
	CHECK_BODY_LEN(bodylen, 8);

	int j = 0;
	uint32_t skill_type;
	uint32_t skill_flag;
	UNPKG_UINT32(body, skill_type, j);

	if (p->followed == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	DEBUG_LOG("LAHM ACTION = %u %u %u %u", p->id, p->followed->id, p->lamu_action, skill_type);

	if (!PET_IS_SUPER_LAHM(p->followed)) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}

	if (!check_skill(p, skill_type)) {
		return send_to_self_error(p, p->waitcmd, -ERR_lahm_have_not_this_skill, 1);
	}

	skill_flag = (skill_type - 1) % 3;
	if (p->lamu_action == 0 || (p->lamu_action -1) % 3 != skill_flag) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_action_not_fit, 1);
	}

	uint32_t db_buf[7];

	db_buf[0] = skill_type;
	db_buf[1] = 0;
	db_buf[2] = 0;
	db_buf[3] = p->followed->id;
	db_buf[4] = 5;
	db_buf[5] = get_day_limit(p->followed);
	db_buf[6] = 1;

	*(uint32_t *)(p->session) = 0;
	*(uint32_t *)(p->session + 4) = db_buf[4];
	*(uint32_t *)(p->session + 8) = skill_type;

	return send_request_to_db(SVR_PROTO_GET_ITEM_BY_USER_SKILL, p, sizeof(db_buf), db_buf, p->id);
}

/* @breif add item
 */
int get_item_by_skill_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	/*get if out of day change limit*/
	uint32_t day_change_limit = *(uint32_t *)buf;
	uint32_t times_today = *(uint32_t *)(buf + 4);

	pet_t *pet = p->followed;
	uint32_t itemid = *(uint32_t *)(p->session);
	uint32_t add_change = *(uint32_t *)(p->session + 4);

	if(pet == NULL) {
		return send_to_self_error(p, p->waitcmd, ERR_the_lahm_not_followed, 1);
	}

	switch(p->waitcmd) {
		case PROTO_GET_ITEM_BY_USE_SKILL:
			db_single_item_op(0, p->id, itemid, 1, 1);
			break;
		case PROTO_PET_USE_SKILL_DO_WORK:
			if (times_today == 3) {
				itemid = 180071;
				db_single_item_op(0, p->id, itemid, 1, 1);
			}
			break;
	}

	if (!day_change_limit) {
		uint32_t db_buf[2];
		pet->skill_value += add_change;
		db_buf[0] = pet->id;
		db_buf[1] = pet->skill_value;
		send_request_to_db(SVR_PROTO_SET_LAMU_CHANGE_VALUE, 0, 8, db_buf, p->id);
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, pet->skill_value, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

/* @breif use skill after 20 second call this funciton
 */
int lamu_end_action(void* owner, void* data)
{
	int i;
	pet_t *pet = (pet_t *)data;
	sprite_t* p = owner;
	p->lamu_action = 0;
	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, pet->id, i);
	init_proto_head(msg, PROTO_REMOVE_PET_ACTION, i);
	send_to_map(p, msg, i, 0);
	p->lamu_action_timer = NULL;
	return 0; // returns 0 means the `timer scanner` should remove this event
}

/* @brief lamu use skill broadcast
 */
int lamu_do_action_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t action, petid;
	unpkg_uint32_uint32(body, bodylen, &petid, &action);

	if ((action > MAX_SKILL_NUM) || (action < 1) || (skill_items[action - 1].id ==0)) {
		ERROR_RETURN(("skill type wrong %u %u %u", p->id, action, MAX_SKILL_NUM), -1);
	}

	if (p->followed == NULL) {
		ERROR_RETURN(("no followed pet %u", p->id), -1);
	}
	pet_t *pet = p->followed;
	if ((pet->hungry == 0) || (pet->thirsty ==0)
		|| (pet->sanitary == 0) || (pet->spirit == 0)) {
		return send_to_self_error(p, p->waitcmd, -ERR_lahm_basic_attire_not_enough, 1);
	}
	/*check if have this skill*/
	if ((p->tiles->id != 126) && (p->tiles->id != 130) && (p->tiles->id != 134) )  {
		if (!check_skill(p, action)) {
			DEBUG_LOG("have not this skill %u %u", p->id, action);
			return send_to_self_error(p, p->waitcmd, -ERR_lahm_have_not_this_skill, 1);
		}
	}

	sub_lamu_basic_attire(p, p->followed, skill_items[action -1].sub_attr);

	p->lamu_action = action;
	p->lamu_action_timer = add_event(&(p->timer_list), lamu_end_action, p, p->followed,
							get_now_tv()->tv_sec + 20, ADD_EVENT_REPLACE_UNCONDITIONALLY);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, petid, i);
	PKG_UINT32(msg, action, i);
	PKG_UINT8(msg, pet->hungry, i);
	PKG_UINT8(msg, pet->thirsty, i);
	PKG_UINT8(msg, pet->sanitary, i);
	PKG_UINT8(msg, pet->spirit, i);
	init_proto_head(msg, PROTO_LAMU_ACTION, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

/* @breif sub lamu basic attribute
 */
int sub_lamu_basic_attire(sprite_t *p, pet_t *pet, int change_value)
{
	#define ATTR_ADD(a, v) \
		a = a + v; \
		if (v > 0) a = ((a) > 100 ? 100 : (a)); \
		else a = ((a) > 100 ? 0 : (a));

	/*sub basic attirbute*/
	ATTR_ADD(pet->hungry, change_value);
	ATTR_ADD(pet->thirsty, change_value);
	ATTR_ADD(pet->sanitary, change_value);
	ATTR_ADD(pet->spirit, change_value);
	pet->stamp = now.tv_sec;
	db_update_pet_attr(0, pet->stamp, change_value, change_value, change_value, change_value, pet->id, p->id);
	return 0;
}


/* @brief end action by self
 */
int remove_lamu_action_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t action, petid;
	unpkg_uint32_uint32(body, bodylen, &petid, &action);
	if (p->lamu_action_timer != NULL) {
		REMOVE_TIMER(p->lamu_action_timer);
		p->lamu_action_timer = NULL;
	}
	p->lamu_action = 0;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, petid, i);
	init_proto_head(msg, PROTO_REMOVE_PET_ACTION, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

/* @breif check if lamu have three basic skill
 */
int check_three_basic_skill(pet_t *pet)
{
	return !((pet->skill_ex[0] & 0x01) && (pet->skill_ex[1] & 0x01) && (pet->skill_ex[2] & 0x01));
}

