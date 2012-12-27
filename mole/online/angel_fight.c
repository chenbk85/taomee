#include "proto.h"
#include "item.h"
#include "angel_fight.h"
#include "dbproxy.h"
#include "benchapi.h"
#include "fight_prize.h"
#include "central_online.h"
#include "message.h"
#include "swap.h"
#include "fire_cup.h"

#define MAX_AF_SKILL_CNT  50
#define MAX_AF_TASK_CNT	  60
#define MAX_DAILY_TASK_CNT 3
#define PVP_PRIZE_FLAG_CNT	6

af_skill_t all_skill[MAX_AF_SKILL_CNT];
af_task_t  all_task[MAX_AF_TASK_CNT];
uint32_t   all_task_cnt = 0;

int add_base_attr_from_type(af_user_info_t* add_attr, uint32_t type, uint32_t value)
{
	switch (type) {
	case 1:
		add_attr->power += value;
		break;
	case 2:
		add_attr->iq += value;
		break;
	case 3:
		add_attr->smart += value;
		break;
	case 4:
		add_attr->strong += value;
		break;
	default:
		break;
	}
	return 0;
}

int add_base_attr_from_equip(af_user_info_t* add_attr, uint32_t equip_id)
{
	item_t *itm = get_item_prop(equip_id);
	if (itm) {
		add_attr->power += itm->u.af_cloth_tag.power;
		add_attr->iq += itm->u.af_cloth_tag.iq;
		add_attr->smart += itm->u.af_cloth_tag.smart;
		add_attr->strong += itm->u.af_cloth_tag.strong;
	}
	return 0;
}

/*
 * @prief 拉取基本属性 8200
 */
int af_get_user_base_info_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t userid = 0;
	int i = 0;
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);
	*(uint32_t*)p->session = userid;

	return send_request_to_db(SVR_PROTO_AF_GET_USER_BASE_INFO, p, 0, NULL, userid);
}

int af_get_user_base_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4 + sizeof(af_user_info_t));

	//用户总经验
	uint32_t sum_exp = *(uint32_t*)buf;
	af_user_info_t* p_user_info = (af_user_info_t*)(buf + 4);

	int l = sizeof(protocol_t);
	uint8_t* p_msg = msg;
	uint32_t msglen = l;
	l += (sizeof(af_user_info_t) + sizeof(uint32_t));
/*	PKG_UINT32(msg, p_user_info->cur_exp, l);
	PKG_UINT32(msg, p_user_info->max_exp, l);
	PKG_UINT32(msg, p_user_info->level, l);
	PKG_UINT32(msg, p_user_info->iq, l);
	PKG_UINT32(msg, p_user_info->smart, l);
	PKG_UINT32(msg, p_user_info->power, l);
	PKG_UINT32(msg, p_user_info->strong, l);
	PKG_UINT32(msg, p_user_info->energy, l);
	PKG_UINT32(msg, p_user_info->max_energy, l);
	PKG_UINT32(msg, p_user_info->vigour, l);
	PKG_UINT32(msg, p_user_info->max_vigour, l);
	PKG_UINT32(msg, p_user_info->get_point, l);
*/

	uint32_t add_strong = 0;//收集点数加的强壮，给前端显示
	if (p_user_info->get_point >= 100 && p_user_info->get_point < 2000) {
		add_strong = (p_user_info->get_point / 100) * 3 + 3;
	}
	PKG_UINT32(msg, add_strong, l);

	uint32_t j = 4 + sizeof(af_user_info_t);
	typedef struct {
		uint32_t equip_id;		//装备
		uint32_t day_first;		//今天是否是第一次进来 0不是1是
		uint32_t day_pvp_cnt;	//今天PVP的次数
		uint32_t use_item_cnt;
		uint32_t skill_cnt;
	}__attribute__((packed)) user_other_info_cbk_head_t;

	user_other_info_cbk_head_t* cbk_head = (user_other_info_cbk_head_t*)(buf + j);
	j += sizeof(user_other_info_cbk_head_t);

	CHECK_BODY_LEN(len, 4 + sizeof(af_user_info_t) + sizeof(user_other_info_cbk_head_t) + cbk_head->use_item_cnt * 8 + cbk_head->skill_cnt * 8);
	//DEBUG_LOG("af_get_user_base_info_callback %d, %d", use_item_cnt, skill_cnt);

	PKG_UINT32(msg, cbk_head->equip_id, l);
	PKG_UINT32(msg, cbk_head->day_pvp_cnt, l);
	af_user_info_t add_attr = {0};
	add_base_attr_from_equip(&add_attr, cbk_head->equip_id);

	int i;
	PKG_UINT32(msg, cbk_head->use_item_cnt, l);
	for (i = 0; i < cbk_head->use_item_cnt; i++) {//增益道具使用效果
		uint32_t type;
		uint32_t round;
		UNPKG_H_UINT32(buf, type, j);
		UNPKG_H_UINT32(buf, round, j);

		PKG_UINT32(msg, type, l);
		PKG_UINT32(msg, round, l);
		add_base_attr_from_type(&add_attr, type, 15);
	}

	PKG_UINT32(msg, cbk_head->skill_cnt, l);
	for (i = 0; i < cbk_head->skill_cnt; i++) {//已装备的技能
		uint32_t skill_id;
		uint32_t skill_lvl;
		UNPKG_H_UINT32(buf, skill_id, j);
		UNPKG_H_UINT32(buf, skill_lvl, j);

		//计算技能
		af_skill_t* skill= af_get_skill_from_skill_id(skill_id);
		if (!skill|| skill_lvl > 10) {
			return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
		}
		PKG_UINT32(msg, skill_id, l);
		PKG_UINT32(msg, skill_lvl, l);

		int j;
		for (j = 1; j <= 4; j++) {//大力、智慧、灵活、强壮
			add_base_attr_from_type(&add_attr, j, skill->skill_level[skill_lvl - 1].add_val[j - 1]);
		}
	}

	PKG_UINT32(p_msg, *(uint32_t*)p->session, msglen);
	PKG_UINT32(p_msg, p_user_info->cur_exp, msglen);
	PKG_UINT32(p_msg, p_user_info->max_exp, msglen);
	PKG_UINT32(p_msg, p_user_info->level, msglen);
	PKG_UINT32(p_msg, p_user_info->iq + add_attr.iq, msglen);
	PKG_UINT32(p_msg, p_user_info->smart + add_attr.smart, msglen);
	PKG_UINT32(p_msg, p_user_info->power + add_attr.power, msglen);
	PKG_UINT32(p_msg, p_user_info->strong + add_attr.strong, msglen);
	PKG_UINT32(p_msg, p_user_info->hp + 8 * (p_user_info->strong + add_attr.strong), msglen);
	PKG_UINT32(p_msg, p_user_info->mp, msglen);
	PKG_UINT32(p_msg, p_user_info->energy, msglen);
	PKG_UINT32(p_msg, p_user_info->max_energy, msglen);
	PKG_UINT32(p_msg, p_user_info->vigour, msglen);
	PKG_UINT32(p_msg, p_user_info->max_vigour, msglen);
	PKG_UINT32(p_msg, p_user_info->get_point, msglen);
	//DEBUG_LOG("user_info:%d, %d, %d, %d, %d, %d", p_user_info->cur_exp, p_user_info->level, p_user_info->iq + add_attr.iq,
		//p_user_info->smart + add_attr.smart,p_user_info->power + add_attr.power,p_user_info->strong + add_attr.strong);

#ifndef TW_VER
	if ((p->id == *(uint32_t*)p->session) && p->team_id != 0 && cbk_head->day_first == 1 && (get_today() >= 20110803 && get_today() < 20110819)) {
#else
	if ((p->id == *(uint32_t*)p->session) && p->team_id != 0 && cbk_head->day_first == 1 && (get_today() >= 20110810 && get_today() < 20110826)) {
#endif
		uint32_t event_id = 6;//如果是每天第一次进入，并且报名了火神杯，设置好友可以为自己加油
		event_id = event_id - 1;//传给DB要-1
		send_request_to_db(SVR_PROTO_AF_USER_SET_EVENT_CNT, NULL, sizeof(event_id), &event_id, p->id);
	}

	if (p->id == *(uint32_t*)p->session) {
		p->fight_level = p_user_info->level;
		if (p->fight_exp != sum_exp) {
			p->fight_exp = sum_exp;
			//全局表同步自己经验
			send_request_to_db(SVR_PROTO_AF_SET_WHOLE_EXP, NULL, sizeof(uint32_t), &sum_exp, *(uint32_t*)p->session);
		}
	} else {
		//全局表同步好友经验
		send_request_to_db(SVR_PROTO_AF_SET_WHOLE_EXP, NULL, sizeof(uint32_t), &sum_exp, *(uint32_t*)p->session);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取角色背包 8201
 */
int af_get_user_package_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_GET_USER_PACKAGE, p, 0, NULL, p->id);
}

int af_get_user_package_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
/*
	Count_1	4	Uint32	卡片种类数
	Count_2	4	Uint32	道具种类数
	Count_3	4	Uint32	携带的装备数量
	………	………	………	………
	card_id	4	Uint32	卡片id
	count	4	Uint32	个数
	………	………	………	………
	props_id 4	Uint32	道具id
	count	 4	Uint32	个数
	………	………	………	………
	equip_id 4	Uint32	装备id
	………	………	………	………
*/
	CHECK_BODY_LEN_GE(len, 8);
	uint32_t card_cnt;
	uint32_t item_cnt;
	uint32_t equip_cnt;
	int j = 0;
	int l = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, card_cnt, j);
	UNPKG_H_UINT32(buf, item_cnt, j);
	UNPKG_H_UINT32(buf, equip_cnt, j);
	CHECK_BODY_LEN(len, 3 * sizeof(uint32_t) + 8 * (card_cnt + item_cnt) + 4 * equip_cnt);

	DEBUG_LOG("af_get_user_package_callback:%d %d %d", card_cnt, item_cnt, equip_cnt);

	int i;
	PKG_UINT32(msg, card_cnt, l);
	for (i = 0; i < card_cnt; i++) {
		uint32_t card_id;
		uint32_t count;
		UNPKG_H_UINT32(buf, card_id, j);
		UNPKG_H_UINT32(buf, count, j);

//		DEBUG_LOG("af_get_user_package_callback:cardid=%d count=%d", card_id, count);

		PKG_UINT32(msg, card_id, l);
		PKG_UINT32(msg, count, l);
	}

	PKG_UINT32(msg, item_cnt, l);
	for (i = 0; i < item_cnt; i++) {
		uint32_t item_id;
		uint32_t count;
		UNPKG_H_UINT32(buf, item_id, j);
		UNPKG_H_UINT32(buf, count, j);

//		DEBUG_LOG("af_get_user_package_callback:item_id=%d count=%d", item_id, count);
		PKG_UINT32(msg, item_id, l);
		PKG_UINT32(msg, count, l);
	}

	PKG_UINT32(msg, equip_cnt, l);
	for (i = 0; i < equip_cnt; i++) {
		uint32_t equip_id;
		UNPKG_H_UINT32(buf, equip_id, j);
//		UNPKG_H_UINT32(buf, count, j);

		PKG_UINT32(msg, equip_id, l);
		PKG_UINT32(msg, 1, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取卡册  8203
 */
int af_get_fight_card_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t userid = 0;
	int i = 0;
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);

	return send_request_to_db(SVR_PROTO_AF_GET_ALL_CARD, p, 0, NULL, userid);
}

int af_get_fight_card_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t card_cnt;
	int j = 0;
	int l = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, card_cnt, j);
	PKG_UINT32(msg, card_cnt, l);

	int i;
	for(i = 0; i < card_cnt; i++) {
		uint32_t card_id;
		uint32_t count;
		UNPKG_H_UINT32(buf, card_id, j);
		UNPKG_H_UINT32(buf, count, j);

		PKG_UINT32(msg, card_id, l);
		PKG_UINT32(msg, count, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取技能 8202
 */
int af_get_fight_skill_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t userid = 0;
	int i = 0;
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);
	return send_request_to_db(SVR_PROTO_AF_GET_ALL_SKILL, p, 0, NULL, userid);
}

int af_get_fight_skill_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t skill_cnt;
	int j = 0;
	int l = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, skill_cnt, j);
	PKG_UINT32(msg, skill_cnt, l);

	int i;
	for(i = 0; i < skill_cnt; i++) {
		uint32_t skill_id;
		uint32_t level;
		uint32_t is_used;
		uint32_t type;
		UNPKG_H_UINT32(buf, skill_id, j);
		UNPKG_H_UINT32(buf, level, j);
		UNPKG_H_UINT32(buf, is_used, j);
		UNPKG_H_UINT32(buf, type, j);

		PKG_UINT32(msg, skill_id, l);
		PKG_UINT32(msg, level, l);
//		PKG_UINT32(msg, is_used, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
* @brief 使用道具 8204
*/
int af_use_item_cmd(sprite_t* p, const uint8_t *body, int len)
{
	//使用道具
	typedef struct {
		uint32_t item_id;	//物品id
		uint32_t type;		//物品类型：1恢复道具、2增益道具、3其他、4礼包
		uint32_t flag;		//恢复道具：1能量，2活力 增益道具：1大力、2智慧、3灵活、4强壮、5主动、6被动 礼包：...  其他：1幸运卡，2许愿心，3任务刷新卡，4任务终结卡，5摩摩挑战卡,6boss挑战卡，7宝石卡
		uint32_t add_val;	//影响的值
		uint32_t round;		//持续轮数
		uint32_t limit_lvl; //限制等级
	}__attribute__((packed)) db_use_item_package_t;

	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t item_id = 0;
	int i = 0;
	UNPKG_UINT32(body, item_id, i);
	DEBUG_LOG("af_use_item_cmd: uid=%u, item_id=%d", p->id, item_id);

	item_kind_t* kind = find_kind_of_item(item_id);
	if (kind == NULL || kind->kind != FIGHT_ITEM_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	item_t *itm = get_item_prop(item_id);
	if (p->fight_level < itm->u.af_item_tag.limit_lvl) {//使用等级判断
		return send_to_self_error(p, p->waitcmd, -ERR_af_limit_fight_level, 1);
	}

	db_use_item_package_t package = {0};
	package.item_id = item_id;
	package.type = itm->u.af_item_tag.type;
	package.flag = itm->u.af_item_tag.flag;
	package.add_val = itm->u.af_item_tag.add_val;
	package.round = itm->u.af_item_tag.round;
	package.limit_lvl = itm->u.af_item_tag.limit_lvl;
	DEBUG_LOG("af_use_item_cmd: id=%u item_id=%d type=%d flag=%d add_val=%d rount=%d limit=%d", p->id, item_id, package.type,package.flag,package.add_val,package.round,package.limit_lvl);

	*(uint32_t*)p->session = item_id;
	if (package.type == 4) {//gift package 使用交换
		if (itm->u.af_item_tag.swap_id >= 453 && itm->u.af_item_tag.swap_id <= 457) {
			return user_use_gift_package_item(p, itm->u.af_item_tag.swap_id);
		} else {
			return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
		}
	} else if (package.type == 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_USE_ITEM, p, sizeof(package), &package, p->id);
}

/*
 * @brief 使用道具callback
 */
int af_use_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t type;		//1:使用成功 2:达到每日使用上限	(有新的返回状态继续添加)
		uint32_t day_add;	//恢复性道具今日总共加的值
	}__attribute__((packed)) use_item_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(use_item_cbk_pack_t));
	use_item_cbk_pack_t* cbk_pack = (use_item_cbk_pack_t*)buf;
	uint32_t item_id = *(uint32_t*)p->session;

	DEBUG_LOG("af_use_item_callback: type=%d", cbk_pack->type);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->type, l);
	PKG_UINT32(msg, item_id, l);
	PKG_UINT32(msg, cbk_pack->day_add, l);
	PKG_UINT32(msg, 0, l);

	p->waitcmd = PROTO_AF_USE_ITEM;
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int af_use_gift_package_callback(sprite_t* p)
{
	uint32_t item_id = *(uint32_t*)p->session;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, 1, l);
	PKG_UINT32(msg, item_id, l);
	PKG_UINT32(msg, 0, l);

	item_t *itm = get_item_prop(item_id);
	uint32_t buff[256] = {0};
	get_gift_swap_items(itm->u.af_item_tag.swap_id, buff);

	PKG_UINT32(msg, buff[0], l);
	int i = 0;
	for (i = 0; i < buff[0]; i++) {
		PKG_UINT32(msg, buff[1 + 2 * i], l);
		PKG_UINT32(msg, buff[1 + (2 * i + 1)], l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

static int parse_single_skill_level(af_skill_level_t *skill_level, xmlNodePtr cur)
{
	int i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Level"))) {
			DECODE_XML_PROP_UINT32(skill_level->skill_lvl, cur, "Skill_lvl");
			if (skill_level->skill_lvl < 1 && skill_level->skill_lvl >10) {
				return -1;
			}

			DECODE_XML_PROP_UINT32(skill_level[i].exp_lvl, cur, "Exp_lvl");
			DECODE_XML_PROP_UINT32(skill_level[i].need_cnt, cur, "Card_cnt");
			DECODE_XML_PROP_UINT32(skill_level[i].need_money, cur, "Money");
			DECODE_XML_PROP_UINT32(skill_level[i].rate, cur, "Rate");
			DECODE_XML_PROP_UINT32(skill_level[i].add_val[0], cur, "Str");//大力
			DECODE_XML_PROP_UINT32(skill_level[i].add_val[1], cur, "Int");//智慧
			DECODE_XML_PROP_UINT32(skill_level[i].add_val[2], cur, "Ali");//灵活
			DECODE_XML_PROP_UINT32(skill_level[i].add_val[3], cur, "Hab");//强壮
			DECODE_XML_PROP_UINT32(skill_level[i].add_hp, cur, "Hp");
			i++;
		}
		cur = cur->next;
	}
	if (i != 10) {
		return -1;
	}
	return 0;
}

int load_af_skill(const char* file)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_RETURN (("load angel fight skill config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		err = -1;
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Skill"))) {
			if (i > MAX_AF_SKILL_CNT) {
				err = -1;
				ERROR_LOG("too many skill error");
				goto exit;
			}
			DECODE_XML_PROP_UINT32(all_skill[i].skill_id, cur, "Skill_id");
			DECODE_XML_PROP_UINT32(all_skill[i].skill_type, cur, "Skill_type");
			DECODE_XML_PROP_UINT32(all_skill[i].card_id, cur, "Card_id");
			//DEBUG_LOG("ANGEL_FIGHT %d: %d %d %d", i, all_skill[i].skill_id, all_skill[i].skill_type, all_skill[i].card_id);
			if (parse_single_skill_level(all_skill[i].skill_level, cur->xmlChildrenNode)) {
				return -1;
			}
			i++;
		}
		cur = cur->next;
	}
	err = 0;

exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load angel fight skill file %s Size[%u]", file, i);
}

/*
 * @brief 通过技能卡片ID得到技能
 */
af_skill_t* af_get_skill_from_card_id(uint32_t card_id)
{
	int i;
	for (i = 0; i < MAX_AF_SKILL_CNT && all_skill[i].skill_id != 0; i++) {
		if (card_id == all_skill[i].card_id) {
			return (all_skill + i);
		}
	}
	return NULL;
}

/*
 * @brief 同过skillID得到skill
 */
af_skill_t* af_get_skill_from_skill_id(uint32_t skill_id)
{
	int i;
	for (i = 0; i < MAX_AF_SKILL_CNT && all_skill[i].skill_id != 0; i++) {
		if (skill_id == all_skill[i].skill_id) {
			return (all_skill + i);
		}
	}
	return NULL;
}

/*
* @brief 通过装备ID得到装扮ID，使用装备卡时用到(目前没有用到)
*/
static int af_get_attire_id_from_equip_id(uint32_t *attire_id, uint32_t equip_id)
{
	int ret = 0;
	if (equip_id == 1) {
		*attire_id = 19012;
	} else if (equip_id == 2) {
		*attire_id = 19012;
	} else {
		ret = -1;
	}
	return ret;
}

/*
* @brief 使用卡片 8205
*/
int af_use_card_cmd(sprite_t* p, const uint8_t *body, int len)
{
	typedef struct {
		uint32_t card_id;		//卡片ID
		uint32_t card_type;		//卡片类型:1天使卡 2成就卡 3被动技能卡 4地图卡 5怪物卡 6装扮卡
		uint32_t map_item_id;	//对应的物品id
		uint32_t add_type;		//增加属性类型1大力 2智慧 3灵活 4强壮 5能量上限 6活力上限
		uint32_t add_val;		//增加的属性值
		uint32_t point;			//收集点数
		uint32_t max_point;		//最大收集点数
		uint32_t add_power;		//如果是被动技能，第一次解封出的1级技能也会属性，大力加成
		uint32_t add_iq;		//智慧加成
		uint32_t add_smart;		//灵活加成
		uint32_t add_strong;	//强壮加成
		uint32_t add_hp;		//hp加成
	}__attribute__((packed)) db_use_cards_package_t;

	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t card_id = 0;
	int i = 0;
	UNPKG_UINT32(body, card_id, i);
	item_kind_t* kind = find_kind_of_item(card_id);
	if (kind == NULL && kind->kind != FIGHT_CARD_KIND) {	//卡片类型判断
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	item_t *itm = get_item_prop(card_id);
	if (p->fight_level < itm->u.af_card_tag.level) {	//使用等级判断
		return send_to_self_error(p, p->waitcmd, -ERR_af_limit_fight_level, 1);
	}

	db_use_cards_package_t package = {0};
	package.card_id = card_id;
	package.card_type = itm->u.af_card_tag.card_type;
	package.map_item_id = itm->u.af_card_tag.map_item_id;
	package.add_type = itm->u.af_card_tag.add_type;
	package.add_val = itm->u.af_card_tag.add_val;
	package.point = itm->u.af_card_tag.point;
	package.max_point = itm->u.af_card_tag.max_collect * itm->u.af_card_tag.point;
	DEBUG_LOG("af_use_card_cmd:uid=%u skill=%u type=%u value=%u", p->id, card_id, package.add_type, package.add_val);

	//如果是被动技能，属性加成直接算到数据库中
	if (package.card_type == 3) {
		af_skill_t* skill = af_get_skill_from_card_id(card_id);
		if (!skill) {
			return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
		}
		package.add_power = skill->skill_level[0].add_val[0];
		package.add_iq = skill->skill_level[0].add_val[1];
		package.add_smart = skill->skill_level[0].add_val[2];
		package.add_strong = skill->skill_level[0].add_val[3];
		package.add_hp = skill->skill_level[0].add_hp;
	}
	//DEBUG_LOG("af_use_card_cmd:%d %d %d %d %d %d %d", package.card_id, package.card_type, package.map_item_id, package.add_type, package.add_val, package.point, package.max_point);

	memcpy(p->session, &card_id, sizeof(card_id));
	return send_request_to_db(SVR_PROTO_AF_USE_CARD, p, sizeof(package), &package, p->id);
}

/*
* @brief 使用卡片callback
*/
int af_use_card_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t use_cnt;	//第几次收集
		uint32_t achieve_card;//成就卡
	}__attribute__((packed)) use_card_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(use_card_cbk_pack_t));
	uint32_t card_id = *(uint32_t*)p->session;
	use_card_cbk_pack_t* cbk_pack = (use_card_cbk_pack_t*)buf;

	DEBUG_LOG("af_use_card_callback: %d %d", cbk_pack->use_cnt, cbk_pack->achieve_card);

	//卡片使用成功,前端要刷新基本信息
	if (cbk_pack->use_cnt >= 1) {
		item_t *itm = get_item_prop(card_id);
		uint32_t attire_id = 0;
		//如果是装扮卡，第一次解封要给装扮 (策划不确定给不给)
		if (itm->u.af_card_tag.card_type == 4 && cbk_pack->use_cnt == 1) {
			if (af_get_attire_id_from_equip_id(&attire_id, itm->u.af_card_tag.map_item_id) != -1) {
				//db_exchange_single_item_op(p, 0, attire_id, 1, 0);
			}
		}
	}

	//state 0到达收集点数 1第一次使用 2多次收集
	uint32_t state = (cbk_pack->use_cnt > 1) ? 2 : cbk_pack->use_cnt;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, state, l);
	PKG_UINT32(msg, cbk_pack->use_cnt, l);
	PKG_UINT32(msg, card_id, l);
	PKG_UINT32(msg, cbk_pack->achieve_card, l);

	//p->waitcmd = PROTO_AF_USE_CARD;
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
* @brief 技能升级 8206
*/
int af_user_skill_level_up_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);
	uint32_t skill_id;
	uint32_t skill_lvl;
	int j = 0;
	UNPKG_UINT32(body, skill_id, j);
	UNPKG_UINT32(body, skill_lvl, j);

	DEBUG_LOG("af_user_skill_level_up_cmd:uid=%u skill_id=%u skill_lvl=%d level=%d", p->id, skill_id, skill_lvl, p->fight_level);

	af_skill_t* skill = af_get_skill_from_skill_id(skill_id);
	if (!skill) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}
	if ((skill_lvl) < 2 || (skill_lvl) > 10 || (p->fight_level < skill->skill_level[skill_lvl - 1].exp_lvl)) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_limit_fight_level, 1);
	}
	if (p->yxb < skill->skill_level[skill_lvl - 1].need_money) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	typedef struct {
		uint32_t skill_id;
		uint32_t skill_type;
		uint32_t skill_lvl;
		uint32_t card_id;
		uint32_t card_cnt;
		uint32_t exp_lvl;
		uint32_t money;
		uint32_t rate;
		uint32_t add_power;
		uint32_t add_iq;
		uint32_t add_strong;
		uint32_t add_smart;
		uint32_t add_hp;
	}__attribute__((packed)) db_skill_level_up_package_t;

	db_skill_level_up_package_t package = {0};
	package.skill_id = skill->skill_id;
	package.skill_type = skill->skill_type;
	package.skill_lvl = skill_lvl;
	package.card_id = skill->card_id;
	package.card_cnt = skill->skill_level[skill_lvl - 1].need_cnt;
	package.exp_lvl = skill->skill_level[skill_lvl - 1].exp_lvl;
	package.money = skill->skill_level[skill_lvl - 1].need_money;
	package.rate = skill->skill_level[skill_lvl - 1].rate;
	package.add_power = skill->skill_level[skill_lvl - 1].add_val[0] - skill->skill_level[skill_lvl - 2].add_val[0];
	package.add_iq = skill->skill_level[skill_lvl - 1].add_val[1] - skill->skill_level[skill_lvl - 2].add_val[1];
	package.add_smart = skill->skill_level[skill_lvl - 1].add_val[2] - skill->skill_level[skill_lvl - 2].add_val[2];
	package.add_strong = skill->skill_level[skill_lvl - 1].add_val[3] - skill->skill_level[skill_lvl - 2].add_val[3];
	package.add_hp = skill->skill_level[skill_lvl - 1].add_hp - skill->skill_level[skill_lvl - 2].add_hp;

	*(uint32_t*)p->session = skill->skill_id;
	*(uint32_t*)(p->session + 4) = skill_lvl;
	*(uint32_t*)(p->session + 8) = skill->skill_level[skill_lvl - 1].need_money;

	return send_request_to_db(SVR_PROTO_AF_LEVEL_UP, p, sizeof(package), &package, p->id);
}

int af_user_skill_level_up_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t is_succ;		//是否升级成功
		uint32_t achieve_card;	//获得成就(为0表示没有获得成就)
	}__attribute__((packed)) level_up_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(level_up_cbk_pack_t));
	level_up_cbk_pack_t* cbk_pack = (level_up_cbk_pack_t*)buf;

	uint32_t skill_id = *(uint32_t*)(p->session);
	uint32_t skill_lvl = *(uint32_t*)(p->session + 4);
	uint32_t need_money = *(uint32_t*)(p->session + 8);
	p->yxb -= need_money;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->is_succ, l);
	PKG_UINT32(msg, skill_id, l);
	PKG_UINT32(msg, skill_lvl, l);
	PKG_UINT32(msg, cbk_pack->achieve_card, l);

	if (cbk_pack->is_succ) {
		uint32_t msgbuf[2] = {id, skill_lvl};
		uint32_t msg_id;
		if (skill_id < 1563017) {
			msg_id = 0x0409B3F2 + (skill_id - 1563000);
			if (skill_id == 1563000) {
				msg_id = 0x0409B403;
			}
		} else if (skill_id >= 1563017 && skill_id < 1563022){
			msg_id = 0x0409B405 + (skill_id - 1563017);
		} else if (skill_id >= 1563022 && skill_id < 1563029){
			msg_id = 0x0409B417 + (skill_id - 1563022);
		} else if (skill_id >= 1563029 && skill_id < 1563031) {
			msg_id = 0x0409B428 + (skill_id - 1563029);
		}
		msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p,msg,l,1);
}

/*
 * @brief 更换技能或装备 8207
 */
int af_user_change_equip_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);
	typedef struct {
		uint32_t equip_id;		//装备的技能或衣服
		uint32_t state;			//
		uint32_t type;			//1天使2装扮
	}__attribute__((packed)) db_change_equip_package_t;

	db_change_equip_package_t package = {0};
	int j = 0;
	UNPKG_UINT32(body, package.equip_id, j);
	UNPKG_UINT32(body, package.state, j);
	if (package.state >= 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}
	item_kind_t* itm_kind = find_kind_of_item(package.equip_id);
	if (!itm_kind || !(itm_kind->kind == FIGHT_CLOTHES_KIND || itm_kind->kind == FIGHT_ANGEL_KIND)) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	package.type = 1;	//天使
	if (itm_kind->kind == FIGHT_CLOTHES_KIND) {
		package.type = 2;
	}

	*(uint32_t*)p->session = package.equip_id;
	return send_request_to_db(SVR_PROTO_AF_CHANGE_EQUIP, p, sizeof(package), &package, p->id);
}

int af_user_change_equip_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t type;
	}__attribute__((packed)) change_equip_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(change_equip_cbk_pack_t));
	change_equip_cbk_pack_t* cbk_pack = (change_equip_cbk_pack_t*)buf;

	if (cbk_pack->type == 0) {

	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)p->session, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p,msg,l,1);
}

/*
 * @brief 全局表拉取所有好友信息（经验、等级等） 8208
 */
int af_get_all_friends_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));
	CHECK_VALID_ID(p->id);

	uint32_t count = 0;
	uint32_t buff[1024] = {0};
	int i = 0;
	UNPKG_UINT32(body, count, i);
	if (count > 250) {
		ERROR_RETURN(("too many friends count = %u", count), -1);
	}
	CHECK_BODY_LEN(len, count * 4 + 4);

	len = 0;
	buff[len++] = count;

	//DEBUG_LOG("af_get_all_friends_cmd:count=%u", count);
	int j;
	uint32_t uid = 0;
	for (j = 0; j < count; j++) {
		UNPKG_UINT32(body, uid, i);
		//DEBUG_LOG("af_get_all_friends_cmd:uid=%u", uid);
		if (!uid || IS_NPC_ID(uid) || IS_GUEST_ID(uid)) {
            continue;
        }
		int k;
		for (k = 0; k < j && uid != buff[k]; k++);
		if (k == j) {
			buff[len++] = uid;
		}
	}
	buff[0] = len - 1;

	return send_request_to_db(SVR_PROTO_AF_GET_ALL_FRIENDS, p, len * sizeof(uint32_t), buff, p->id);
}

int af_get_all_friends_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t uid;
		uint32_t exp;
		uint32_t level;
	}__attribute__((packed)) one_friend_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, 4);
    uint32_t count = *(uint32_t*)buf;
    CHECK_BODY_LEN(len, 4 + sizeof(one_friend_cbk_pack_t) * count);

	uint32_t cnt = 0;
	uint32_t l = sizeof(protocol_t) + sizeof(uint32_t);
	one_friend_cbk_pack_t* p_cbk_pack = (one_friend_cbk_pack_t*)(buf + 4);
	int i;
	for (i = 0; i < count; i++) {
		if ((p_cbk_pack + i)->uid != p->id && (p_cbk_pack + i)->exp == 0) {	//这儿只要进去就算玩过
			continue;
		}
		cnt++;
		PKG_UINT32(msg, (p_cbk_pack + i)->uid, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->exp, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->level, l);
	}
	uint32_t protocol_len = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, protocol_len);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取所有好友今日对战情况 8209
 */
int af_get_all_friends_fight_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_GET_ALL_FRIENDS_FIGHT, p, 0, NULL, p->id);
}

int af_get_all_friends_fight_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t uid;
		uint32_t fight;
	}__attribute__((packed)) one_fight_cbk_t;

	CHECK_BODY_LEN_GE(len, 4);
    uint32_t count = *(uint32_t*)buf;
    CHECK_BODY_LEN(len, 4 + sizeof(one_fight_cbk_t) * count);

	uint32_t cnt = 0;
	uint32_t l = sizeof(protocol_t) + sizeof(uint32_t);
	one_fight_cbk_t* p_cbk_pack = (one_fight_cbk_t*)(buf + 4);
	//DEBUG_LOG("af_get_all_friends_fight_callback: count=%u", count);
	int i;
	for (i = 0; i < count; i++) {
		if ((p_cbk_pack + i)->uid != p->id && (p_cbk_pack + i)->fight == 0) {
			continue;
		}
		cnt++;
		PKG_UINT32(msg, (p_cbk_pack + i)->uid, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->fight, l);
		//DEBUG_LOG("af_get_all_friends_fight_callback: uid=%u fight=%u", (p_cbk_pack + i)->uid, (p_cbk_pack + i)->fight);
	}
	uint32_t protocol_len = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, protocol_len);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取某一地图的所有关卡完成度 8210
 */
int af_get_map_pass_point_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t map_id = 0;
	int i = 0;
	UNPKG_UINT32(body, map_id, i);

	map_barrier_info_t* map_info = get_map_barrier_from_id(map_id);
	if (!IS_MONSTER_MAP(map_id) || map_info == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	uint32_t db_buff[128] = {0};
	uint32_t buff_len = 0;

	db_buff[buff_len++] = map_info->barrier_cnt;
	for (i = 0; (uint32_t)i < map_info->barrier_cnt; i++) {
		db_buff[buff_len++] = map_info->barrier_id[i];
	}

	return send_request_to_db(SVR_PROTO_AF_GET_MAP_PASS_POINT, p, buff_len * sizeof(uint32_t), db_buff, p->id);
}

int af_get_map_pass_point_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t point_id;
		uint32_t point_finish;
	}__attribute__((packed)) map_point_cbk_t;

	uint32_t point_cnt = 0;
	CHECK_BODY_LEN_GE(len, sizeof(point_cnt));
	point_cnt = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(point_cnt) + sizeof(map_point_cbk_t) * point_cnt);

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, point_cnt, l);
	map_point_cbk_t* p_cbk_pack = (map_point_cbk_t*)(buf + 4);
	int i;
	for (i = 0; i < point_cnt; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->point_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->point_finish, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取一个技能的等级 8211
 */
int af_get_one_skill_level_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	typedef struct {
		uint32_t skill_id;
	}__attribute__((packed)) db_get_one_skill_package_t;
	db_get_one_skill_package_t package = {0};

	int i  = 0;
	UNPKG_UINT32(body, package.skill_id, i);
	return send_request_to_db(SVR_PROTO_AF_GET_ONE_SKILL, p, sizeof(package), &package, p->id);
}

int af_get_one_skill_level_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t skill_id;
		uint32_t skill_lvl;
	}__attribute__((packed)) get_one_skill_cbk_t;

	CHECK_BODY_LEN(len, sizeof(get_one_skill_cbk_t));
	get_one_skill_cbk_t* cbk_pack = (get_one_skill_cbk_t*)buf;
	//DEBUG_LOG("af_get_one_skill_level_callback:skill_id=%u, skill_lvl=%u", cbk_pack->skill_id, cbk_pack->skill_lvl);

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->skill_id, l);
	PKG_UINT32(msg, cbk_pack->skill_lvl, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief check是否可以翻牌 8212
 */
int af_check_user_get_prize_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_CHECK_GET_PRIZE, p, 0, NULL, p->id);
}

int af_check_user_get_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t barrier_id;	//关卡ID
		uint32_t win;			//是否获胜
		uint32_t vs_level;		//对手等级
		uint32_t exp;			//本轮战斗获得经验
		uint32_t fight_type;	//战斗类型 FRIGHT_TYPE
		uint32_t get_prize;		//战斗结束后是否翻牌,0不能1能
		uint32_t vs_apprentice_id;	//徒弟的userid
		uint32_t vs_master_id;	//徒弟当前的师傅id
		uint32_t apprentice_cnt;//已有徒弟的数量
	}__attribute__((packed)) get_one_skill_cbk_head_t;

	typedef struct {
		uint32_t apprentice_id;	//徒弟ID
	}__attribute__((packed)) get_one_skill_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, sizeof(get_one_skill_cbk_head_t));
	get_one_skill_cbk_head_t* cbk_head = (get_one_skill_cbk_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(get_one_skill_cbk_head_t) + cbk_head->apprentice_cnt * sizeof(get_one_skill_cbk_pack_t));
	get_one_skill_cbk_pack_t* p_cbk_pack = (get_one_skill_cbk_pack_t*)(buf + sizeof(get_one_skill_cbk_head_t));

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->exp, l);
	PKG_UINT32(msg, cbk_head->fight_type, l);
	PKG_UINT32(msg, cbk_head->win, l);

	if (cbk_head->fight_type == FT_GET_APPRENTICE || cbk_head->fight_type == FT_GRAB_APPRENTICE) {	//如果是收徒作战
		p->get_apprentice = 0;
		p->old_master_id = 0;
		p->apprentice_id = 0;
		if (cbk_head->win == 1) {
			//修改徒弟的师傅为自己
			CHECK_VALID_ID(cbk_head->vs_apprentice_id);
			if (cbk_head->vs_master_id != 0) {
				CHECK_VALID_ID(cbk_head->vs_master_id);
			}

			p->get_apprentice = 1;//设置为可以收徒
			p->old_master_id = cbk_head->vs_master_id;
			p->apprentice_id = cbk_head->vs_apprentice_id;
		}

		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	} else if (cbk_head->fight_type == FT_TRAIN) {	//师徒切磋武艺
		uint32_t msgbuf[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B430, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	} else if (cbk_head->fight_type == FT_PK) {
		if (cbk_head->get_prize == 0) {
			PKG_UINT32(msg, 0, l);
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}
	}

	int i = 0;
	for (i = 0; i < cbk_head->apprentice_cnt; i++) {	//给徒弟经验分成
		typedef struct {
			uint32_t add_exp;
		}__attribute__((packed)) db_add_exp_package_t;
		db_add_exp_package_t package = {0};
		package.add_exp = (uint32_t)(cbk_head->exp * 0.2);
		if (package.add_exp > 0) {
			send_request_to_db(SVR_PROTO_AF_ADD_FIGHT_EXP, NULL, sizeof(package), &package, (p_cbk_pack + i)->apprentice_id);
			af_add_master_apprentice_msg((p_cbk_pack + i)->apprentice_id, p->id, (p_cbk_pack + i)->apprentice_id, 104, package.add_exp, 0);
		}
	}

	one_card_t card[5];
	int ret = get_prize_list(&card, cbk_head->barrier_id, cbk_head->win, p->fight_level);
	if (ret == -1) {
		ERROR_LOG("get prize list error");
		return send_to_self_error(p, p->waitcmd, -ERR_af_limit_fight_level, 1);
		//return -1;
	}

	PKG_UINT32(msg, 5, l);
	for (i = 0; i < 5; i++) {
		PKG_UINT32(msg, card[i].prize_id, l);
		PKG_UINT32(msg, card[i].prize_cnt, l);
		//PKG_UINT32(msg, card[i].rate, l);
	}
	memcpy(p->card, card, sizeof(card));

	p->get_prize = 1;//标记为可以翻牌
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 用户翻牌 8213
 */
int af_user_get_prize_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t count = 0;
	int body_len = 0;
	UNPKG_UINT32(body, count, body_len);

	if (p->get_prize != 1 || count >= 5) {
		ERROR_LOG("you can not get prize!");
		return send_to_self_error(p, p->waitcmd, -ERR_af_can_not_get_prize, 1);
	}
	CHECK_BODY_LEN(len, 4 + count * sizeof(len));

	int i, j;
	uint32_t card_list[5] = {0};//前端拆掉的卡片
	for (j = 0; j < count; j++) {
		UNPKG_UINT32(body, card_list[j], body_len);
		for (i = 0; i < 5 && card_list[j] != p->card[i].prize_id; i++);
		if (i == 5) {
			ERROR_LOG("get prize id not in card[5] error: %u", card_list[j]);
			return send_to_self_error(p, p->waitcmd, -ERR_af_can_not_get_prize, 1);
		}
	}

	typedef struct {
		uint32_t card_id;
		uint32_t card_cnt;
		uint32_t item_id;
		uint32_t item_cmt;
	}__attribute__((packed)) db_get_prize_package_t;

	db_get_prize_package_t package = {0};
	package.card_id = 1301023;//拆牌卡ID
	package.card_cnt = count;

	int sum_rate = 0;
	uint32_t nidx[5] = {0};
	uint32_t nrate[5] = {0};
	uint32_t idx = 0;
	for (i = 0; i < 5; i++) {
		int j = 0;
		for (j = 0; j < count && p->card[i].prize_id != card_list[j]; j++);
		if (j == count) {
			sum_rate += p->card[i].rate;
			nrate[idx] = sum_rate;
			nidx[idx++] = i;
		}
	}

	int rate = rand() % sum_rate;
	for (i = 0; i < idx; i++) {
		if (rate < nrate[i]) {
			package.item_id = p->card[nidx[i]].prize_id;
			package.item_cmt = p->card[nidx[i]].prize_cnt;
			break;
		}
	}
	DEBUG_LOG("user get prize:%u %u %u %u", p->id, package.card_cnt, package.item_id, package.item_cmt);
	*(uint32_t*)p->session = package.item_id;
	*(uint32_t*)(p->session + 4) = package.item_cmt;

	return send_request_to_db(SVR_PROTO_AF_USER_GET_PRIZE, p, sizeof(package), &package, p->id);
}

int af_user_get_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t new_exp;
		uint32_t level_up;
		uint32_t achieve_cnt;
		uint32_t event_cnt;
	}__attribute__((packed)) get_prize_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, sizeof(get_prize_cbk_pack_t));
	get_prize_cbk_pack_t *p_cbk_pack = (get_prize_cbk_pack_t*)buf;
	CHECK_BODY_LEN(len, sizeof(get_prize_cbk_pack_t) + (p_cbk_pack->achieve_cnt + p_cbk_pack->event_cnt) * sizeof(uint32_t));
	uint32_t* p_list = (uint32_t*)(buf + sizeof(get_prize_cbk_pack_t));

	uint32_t item_id = *(uint32_t*)p->session;
	uint32_t item_cnt = *(uint32_t*)(p->session + 4);

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, p_cbk_pack->level_up, l);
	PKG_UINT32(msg, item_id, l);
	PKG_UINT32(msg, item_cnt, l);
	PKG_UINT32(msg, p_cbk_pack->achieve_cnt, l);
	DEBUG_LOG("af_user_get_prize_callback:[%u %u %u %u %u %u]", p->id, p_cbk_pack->level_up, item_id, item_cnt, p_cbk_pack->achieve_cnt, p_cbk_pack->event_cnt);
	int i;
	for (i = 0; i < p_cbk_pack->achieve_cnt; i++) {
		PKG_UINT32(msg, p_list[i], l);
	}

	if (item_id == 0) {
		p->yxb += item_cnt;
	}
	p->get_prize = 0;
	//send_request_to_db(SVR_PROTO_AF_SET_WHOLE_EXP, NULL, sizeof(uint32_t), &(p_cbk_pack->new_exp), p->id);

	int j = 0;
	for (j = 0; j < p_cbk_pack->event_cnt; j++) {
		send_request_to_db(SVR_PROTO_AF_USER_SET_EVENT_CNT, NULL, sizeof(uint32_t), &(p_list[i++]), p->id);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 用户许愿 8214
 */
int af_user_set_wishing_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t wish_item_id;

	int i = 0;
	UNPKG_UINT32(body, wish_item_id, i);
	item_kind_t* kind = find_kind_of_item(wish_item_id);
	DEBUG_LOG("af_user_set_wishing_cmd:uid[%u] item_id[%u]", p->id, wish_item_id);
	if (!kind || kind->kind != FIGHT_COLLECT_KIND) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}
	item_t *itm = get_item_prop(wish_item_id);
	if (!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	*(uint32_t*)p->session = wish_item_id;
	*(uint32_t*)(p->session + 4) = itm->u.af_collect_tag.wishing_cnt;
	if (p->fight_level < itm->u.af_collect_tag.limit_lvl) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}
	//221E
	return send_request_to_db(SVR_PROTO_AF_USER_CHECK_WISHING, p, 0, NULL, p->id);
}

static int af_user_set_wishing(sprite_t* p, uint32_t id, uint32_t wish_cnt)
{
	if (wish_cnt > 0) {	//表示正在许愿中
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}
	uint32_t wish_item_id = *(uint32_t*)p->session;
	//3351
	return send_request_to_db(SVR_PROTO_AF_USER_SET_WISHING, p, 4, &wish_item_id, p->id);
}

/*
 * @brief 全局表查看用户是否能够许愿callback
 */
int af_user_check_wishing_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t wish_cnt;//0可以许愿 大于0不能许愿
	}__attribute__((packed)) check_wishing_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(check_wishing_cbk_pack_t));
	check_wishing_cbk_pack_t* cbk_pack = (check_wishing_cbk_pack_t*)buf;

	switch (p->waitcmd) {
	case PROTO_AF_USER_SET_WISHING:
		return af_user_set_wishing(p, id, cbk_pack->wish_cnt);
	case PROTO_AF_GET_WISHING_STATE:
		*(uint32_t*)p->session = cbk_pack->wish_cnt;
		return send_request_to_db(SVR_PROTO_AF_GET_WISH_ITEMID, p, 0, NULL, p->id);
	case PROTO_AF_GET_WISHING_COLLECT:
		return af_user_get_wishing_collect(p, cbk_pack->wish_cnt);
	default:
		return 0;
	}
	return 0;
}

/*
 * @brief USER表检查好友是否可以许愿callback
 */
int af_user_set_wishing_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t is_wishing;//1可以许愿 0不能许愿 2今天已经许愿了
	}__attribute__((packed)) set_wishing_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(set_wishing_cbk_pack_t));
	set_wishing_cbk_pack_t* cbk_pack = (set_wishing_cbk_pack_t*)buf;
	uint32_t state = 1;	//返回给前端，能够许愿返回1，今天已经许愿返回0

	if (cbk_pack->is_wishing == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	} else if(cbk_pack->is_wishing == 2) {
		state = 0;
	} else {
		uint32_t wishing_cnt = *(uint32_t*)(p->session + 4);
		send_request_to_db(SVR_PROTO_AF_USER_SET_WISHING_CNT, 0, 4, &wishing_cnt, p->id);
		uint32_t msgbuf[2] = {id, 1};
		msglog(statistic_logfile, 0x0409B40A, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	}

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, state, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 祝福好友,帮助好友实现愿望 8215
 */
int af_deal_friend_wishing_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	typedef struct {
		uint32_t friend_id;
	}__attribute__((packed)) db_get_wishing_package_t;
	db_get_wishing_package_t package;

	int i = 0;
	UNPKG_UINT32(body, package.friend_id, i);
	CHECK_VALID_ID(package.friend_id);
	if (p->id == package.friend_id) { //自己不能给自己祝福
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_GET_FRIEND_WISHING_CNT, p, sizeof(package), &package, p->id);
}

/*
 * @brief 全局表得到好友的许愿次数和当前许愿的计数callback
 */
int af_get_friend_wishing_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t friend_id;		//好友ID

		uint32_t wish_cnt;		//许愿剩余次数
		uint32_t wish_idx;		//许愿计数器
	}__attribute__((packed)) friend_wishing_cnt_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(friend_wishing_cnt_cbk_pack_t));

	friend_wishing_cnt_cbk_pack_t* cbk_pack = (friend_wishing_cnt_cbk_pack_t*)buf;

	if (cbk_pack->wish_cnt == 0) {//不能祝福好友
		uint32_t l = sizeof(protocol_t);
		uint32_t state = 0;
		PKG_UINT32(msg, state, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}

	typedef struct {
		uint32_t friend_id;
		uint32_t wish_idx;
	}__attribute__((packed)) db_deal_friend_wish_package_t;
	db_deal_friend_wish_package_t package = {0};
	package.friend_id = cbk_pack->friend_id;
	package.wish_idx = cbk_pack->wish_idx;

	return send_request_to_db(SVR_PROTO_AF_DEAL_FRIEND_WISHING, p, sizeof(package), &package, p->id);
}

/*
 * @brief USER表处理用户为好友祝福callback
 */
int af_deal_friend_wishing_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t friend_id;
		uint32_t state;
		uint32_t item_id;	//祝福好友获得的奖励
		uint32_t item_cnt;	//祝福好友获得的奖励数量
		//uint32_t is_levelup;//是否升级
		//uint32_t achieve_cnt;//获得成就卡数量
	}__attribute__((packed)) friend_wishing_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(friend_wishing_cbk_pack_t));

	friend_wishing_cbk_pack_t* cbk_pack = (friend_wishing_cbk_pack_t*)buf;

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->state, l);

	if (cbk_pack->state == 1) {
		send_request_to_db(SVR_PROTO_AF_SET_FRIEND_WISHING_CNT, NULL, 0, NULL, cbk_pack->friend_id);
		PKG_UINT32(msg, cbk_pack->item_id, l);//祝福成功送的东西
		PKG_UINT32(msg, cbk_pack->item_cnt, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 用户当前许愿状态 8216
 */
int af_user_get_wishing_state_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_USER_CHECK_WISHING, p, 0, NULL, p->id);
}

int af_user_get_wishing_itemid_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		int32_t wish_item;
	}__attribute__((packed)) get_wishing_item_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(get_wishing_item_cbk_pack_t));
	get_wishing_item_cbk_pack_t* cbk_pack = (get_wishing_item_cbk_pack_t*)buf;

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->wish_item, l);
	PKG_UINT32(msg, *(uint32_t*)p->session, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 用户得到许愿收集品 8220
 */
int af_user_get_wishing_collect_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_USER_CHECK_WISHING, p, 0, NULL, p->id);
}

int af_user_get_wishing_collect(sprite_t* p, uint32_t wish_cnt)
{
	if (wish_cnt > 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}
	return send_request_to_db(SVR_PROTO_AF_USER_GET_WISHING_COLLECT, p, 0, NULL, p->id);
}

/*
 * @brief 获得许愿收集品callback
 */
int af_user_get_wishing_collect_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t collect_id;//收集品ID
	}__attribute__((packed)) get_wishing_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(get_wishing_cbk_pack_t));

	get_wishing_cbk_pack_t* cbk_pack = (get_wishing_cbk_pack_t*)buf;
	if (cbk_pack->collect_id == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->collect_id, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 处理好友事件 8217
 */
int af_deal_friend_event_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	typedef struct {
		uint32_t friend_id;		//好友ID
		uint32_t event_id;		//事件类型
	}__attribute__((packed)) db_get_wishing_package_t;
	db_get_wishing_package_t package;

	int i = 0;
	UNPKG_UINT32(body, package.friend_id, i);
	UNPKG_UINT32(body, package.event_id, i);
	CHECK_VALID_ID(package.friend_id);
	if (p->id == package.friend_id) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}

	//如果有新上事件，记得要改,事件6已经用了，用于好友交互徽章领取(下掉)
	if (package.event_id < 2 || package.event_id > 5) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}
	*(uint32_t*)p->session = package.event_id;
	*(uint32_t*)(p->session + 4) = package.friend_id;
	return send_request_to_db(SVR_PROTO_AF_GET_FRIEND_EVENT_CNT, p, sizeof(package), &package, p->id);
}

/*
 * @brief 全局表拉取单个好友事件callback
 */
int af_get_friend_event_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t friend_id;		//好友ID
		uint32_t is_deal;		//召唤好友时，时间校验不过时为0，其他情况都为1
		uint32_t event_id;		//事件类型
		uint32_t event_cnt;		//事件计数
	}__attribute__((packed)) friend_event_cnt_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(friend_event_cnt_cbk_pack_t));

	friend_event_cnt_cbk_pack_t* cbk_pack = (friend_event_cnt_cbk_pack_t*)buf;
	if (cbk_pack->is_deal == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_deal_event_wrong, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_DEAL_FRIEND_EVENT, p, sizeof(friend_event_cnt_cbk_pack_t), cbk_pack, p->id);
}

/*
 * @brief USER处理好友事件状态callback
 */
int af_deal_friend_event_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
	//	uint32_t friend_id;
		uint32_t state;
		uint32_t item_id;
		uint32_t item_cnt;
		uint32_t is_levelup;
		uint32_t achieve_cnt;
	}__attribute__((packed)) friend_event_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(friend_event_cbk_pack_t));

	friend_event_cbk_pack_t* cbk_pack = (friend_event_cbk_pack_t*)buf;
	CHECK_BODY_LEN(len, sizeof(friend_event_cbk_pack_t) + cbk_pack->achieve_cnt * sizeof(uint32_t));
	uint32_t* plist = (uint32_t*)(buf + sizeof(friend_event_cbk_pack_t));

	uint32_t event_id = *(uint32_t*)p->session;
	uint32_t friend_id = *(uint32_t*)(p->session + 4);

	//如果是事件6，并且是时间处理成功，事件6不输出摩尔豆和经验值
#ifndef TW_VER
	if (event_id == 6 && cbk_pack->state == 1 && p->team_id != 0 && (get_today() >= 20110803 && get_today() < 20110819)) {
#else
	if (event_id == 6 && cbk_pack->state == 1 && p->team_id != 0 && (get_today() >= 20110810 && get_today() < 20110826)) {
#endif
		uint32_t day_type = 31122;	//勇者之战 好友交互 徽记限制
		return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &day_type, p->id);
	}

	//DEBUG_LOG("af_deal_friend_event_callback:cbk[%u]", cbk_pack->state);
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->state, l);
	if (cbk_pack->state != 0) {
		PKG_UINT32(msg, cbk_pack->item_id, l);
		PKG_UINT32(msg, cbk_pack->item_cnt, l);
		PKG_UINT32(msg, cbk_pack->is_levelup, l);
		PKG_UINT32(msg, cbk_pack->achieve_cnt, l);
		int i;
		for (i = 0; i < cbk_pack->achieve_cnt; i++) {
			PKG_UINT32(msg, plist[i], l);
		}
	}

	if (cbk_pack->state != 0 && event_id == 2) {
		char buff[2048] = {0};
#ifndef TW_VER
		sprintf(buff, "你的好友%.16s(米米号:%u)在摩摩怪大作战中有出色的表现，希望能够和你并肩战斗，快来黑森林吧！他（她）在那里等你!", p->nick, p->id);
		send_postcard(p->nick, p->id, friend_id, 1000242, buff, 0);
#else
		sprintf(buff, "你的好友%.16s(米米號:%u)在摩摩怪大作戰中有出色的表現，希望能夠和你並肩戰鬥，快來黑森林吧！他（她）在那裏等你!", p->nick, p->id);
		send_postcard(p->nick, p->id, friend_id, 1000242, buff, 0);
#endif
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 勇者之战，好友交互送东西
 */
int af_check_friend_event_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t event_cnt = *(uint32_t*)buf;
	uint32_t item_id = 0;
	int32_t item_cnt = 0;
	if (event_cnt < 30) {
		item_id = 190872;	//送徽章
		item_cnt = 1;
		//给自己加徽章
		db_exchange_single_item_op(p, 0, 190872, 1, 0);

		//t_day中设置几天的次数
		uint32_t db_buff[] = {31122, 30, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, 12, db_buff, p->id);

		//每个队伍的总量增加
		add_fire_cup_medal_count(p, item_id, item_cnt);
	}
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, 1, l);
	PKG_UINT32(msg, item_id, l);
	PKG_UINT32(msg, item_cnt, l);
	PKG_UINT32(msg, 0, l);
	PKG_UINT32(msg, 0, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取指定数量的好友对战、事件信息 8218
 */
int af_user_get_some_friends_info_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));
	CHECK_VALID_ID(p->id);

	uint32_t friend_cnt = 0;
	uint32_t buff[256] = {0};
	int i = 0;
	UNPKG_UINT32(body, friend_cnt, i);
	if (friend_cnt > 14) {
		ERROR_RETURN(("too many friends count = %u", friend_cnt), -1);
	}
	CHECK_BODY_LEN(len, friend_cnt * 4 + 4);

	len = 0;
	buff[len++] = friend_cnt;

	int j;
	uint32_t uid = 0;
	for (j = 0; j < friend_cnt; j++) {
		UNPKG_UINT32(body, uid, i);
		if (!uid || IS_NPC_ID(uid) || IS_GUEST_ID(uid) || uid == p->id) {
			continue;
        }
		int k;
		for (k = 0; k < j && uid != buff[k]; k++);
		if (k == j) {
			buff[len++] = uid;
		}
	}
	buff[0] = len - 1;
	if (buff[0] == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_USER_GET_SOME_FRIENDS, p, len * sizeof(uint32_t), buff, p->id);
}

typedef struct {
	uint32_t friend_id;	//好友ID
	uint32_t wish_cnt;//是否满足许愿的条件(数量)
	uint32_t is_date;	//是否满足召唤好友的10天条件
	uint8_t	 event_idx[8];//事件计数器
}__attribute__((packed)) get_some_friends_cbk_pack_t;

/*
 * @brief 全局表拉取用户指定数量的好友信息callback
 */
int af_user_get_some_friends_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));

	uint32_t friend_cnt = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, friend_cnt * sizeof(get_some_friends_cbk_pack_t) + 4);
	get_some_friends_cbk_pack_t* p_cbk_pack = (get_some_friends_cbk_pack_t*)(buf + 4);

	uint32_t db_buf[256] = {0};
	db_buf[0] = friend_cnt;
	int i = 0;
	for (i = 0; i < friend_cnt; i++) {
		db_buf[i + 1] = (p_cbk_pack + i)->friend_id;
	}

	if (friend_cnt == 0) {
		uint32_t l = sizeof(protocol_t);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	//*(uint32_t*)p->session = friend_cnt;
	memcpy(p->session, buf, friend_cnt * sizeof(get_some_friends_cbk_pack_t) + 4);

	return send_request_to_db(SVR_PROTO_AF_USER_GET_SOME_FRIENDS_INFO, p, friend_cnt * sizeof(uint32_t) + 4, db_buf, p->id);
}

/*
 * @brief USER表拉取用户指定数量的好友信息callback
 */
int af_user_get_some_friends_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t friend_id;
		uint32_t is_fight;		//是否可以对战
		uint8_t	 event_idx[8];	//事件buff
	}__attribute__((packed)) get_friends_info_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));

	uint32_t friend_cnt = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, friend_cnt * sizeof(get_friends_info_cbk_pack_t) + 4);
	get_friends_info_cbk_pack_t* p_cbk_pack = (get_friends_info_cbk_pack_t*)(buf + 4);

	uint32_t tmp_cnt = *(uint32_t*)p->session;
	get_some_friends_cbk_pack_t* p_tmp_info = (get_some_friends_cbk_pack_t*)(p->session + 4);

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, tmp_cnt, l);

	int i = 0;
	int j = 0;

	for (i = 0; i < tmp_cnt; i++) {
		uint32_t event_cnt = 0;
		uint32_t event_id[8] = {0};
		for (j = 0; j < friend_cnt; j++) {
			if ((p_tmp_info + i)->friend_id == (p_cbk_pack + j)->friend_id) {
				//判断是否可以许愿
				if ((p_tmp_info + i)->wish_cnt > 0 && ((p_tmp_info + i)->event_idx[0] != (p_cbk_pack + j)->event_idx[0])) {
					event_id[event_cnt++] = 1;
				}
				//判断是否可以召唤
				if ((p_tmp_info + i)->is_date == 1 && ((p_tmp_info + i)->event_idx[1] != (p_cbk_pack + j)->event_idx[1])) {
					event_id[event_cnt++] = 2;
				}
				if ((p_tmp_info + i)->event_idx[2] != (p_cbk_pack + j)->event_idx[2]) {
					event_id[event_cnt++] = 3;
				}
				if ((p_tmp_info + i)->event_idx[3] != (p_cbk_pack + j)->event_idx[3]) {
					event_id[event_cnt++] = 4;
				}
				/*if ((p_tmp_info + i)->event_idx[4] != (p_cbk_pack + j)->event_idx[4]) {
					event_id[event_cnt++] = 5;
				}*/
				if ((p_tmp_info + i)->event_idx[5] != (p_cbk_pack + j)->event_idx[5]) {
					//event_id[event_cnt++] = 6; 好友交互兑换徽章下掉
				}
				PKG_UINT32(msg, (p_cbk_pack + j)->friend_id, l);
				PKG_UINT32(msg, (p_cbk_pack + j)->is_fight, l);
				break;
			}
		}
		if (j == friend_cnt) {
			if ((p_tmp_info + i)->wish_cnt > 0) {
				event_id[event_cnt++] = 1;
			}
			if ((p_tmp_info + i)->is_date == 1 && (p_tmp_info + i)->event_idx[1] != 0) {
				event_id[event_cnt++] = 2;
			}
			if ((p_tmp_info + i)->event_idx[2] != 0) {
				event_id[event_cnt++] = 3;
			}
			if ((p_tmp_info + i)->event_idx[3] != 0) {
				event_id[event_cnt++] = 4;
			}
			PKG_UINT32(msg, (p_tmp_info + i)->friend_id, l);
			PKG_UINT32(msg, 0, l);
		}
		//打包事件
		PKG_UINT32(msg, event_cnt, l);
		for (j = 0; j < event_cnt; j++) {
			PKG_UINT32(msg, event_id[j], l);
		}
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}


/*
 * @brief 拉取PVP对战记录 8219
 */
int af_user_get_fight_record_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_USER_GET_FIGHT_RECORD, p, 0, NULL, p->id);
}

int af_user_get_fight_record_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t msg_type;	//0PVE对战信息，1PVP对战信息
		uint32_t fight_uid;	//对战的用户ID
		uint32_t fight_type;//1主动挑战0被挑战
		uint32_t is_win;	//1获胜0失败
		uint32_t get_exp;	//获得经验
		uint32_t fight_time;//对战时间
	}__attribute__((packed)) fight_record_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t record_cnt = *(uint32_t*)buf;
	fight_record_cbk_pack_t* p_cbk_pack = (fight_record_cbk_pack_t*)(buf + sizeof(record_cnt));

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, record_cnt, l);
	int i;
	for (i = 0; i < record_cnt; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->msg_type, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->fight_uid, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->fight_type, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->is_win, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->get_exp, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->fight_time, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 配置表每日任务奖励
 */
static int parse_single_item(item_unit_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int id, j = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"ItemOut"))) {
			if (j == MAX_TASK_ITEM_CNT) {
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT(id, cur, "ID");
			if (!(iut[j].itm = get_item_prop (id))) {
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			if (iut[j].count <= 0) {
				ERROR_RETURN(("error count=%d, item=%d", iut[j].count, id), -1);
			}

			j++;
		}
		cur = cur->next;
	}
	*cnt = j;

	return 0;
}

/*
 * @breif 读取每日任务配置文件
 */
int load_af_task(const char* file)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_RETURN (("load angel fight daily task config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		err = -1;
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	uint32_t level_cnt[41] = {0};
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Task"))) {
			if (i > MAX_AF_TASK_CNT) {
				err = -1;
				ERROR_LOG("too many task error");
				goto exit;
			}
			DECODE_XML_PROP_UINT32(all_task[i].task_id, cur, "ID");
			if (all_task[i].task_id != (i + 1)) {	//ID要连续，并且从1开始
				err = -1;
				goto exit;
			}
			DECODE_XML_PROP_UINT32(all_task[i].task_type, cur, "Type");
			DECODE_XML_PROP_UINT32(all_task[i].low_lvl, cur, "Low_lvl");
			DECODE_XML_PROP_UINT32(all_task[i].high_lvl, cur, "High_lvl");
			DECODE_XML_PROP_UINT32(all_task[i].max_cnt, cur, "Max_cnt");
			DECODE_XML_PROP_UINT32(all_task[i].event_id, cur, "Event_id");

			int lvl;
			for (lvl = all_task[i].low_lvl; lvl <= all_task[i].high_lvl; lvl++) {
				level_cnt[lvl]++;
			}

			if (parse_single_item(all_task[i].item_out, &(all_task[i].out_cnt), cur->xmlChildrenNode)) {
				err = -1;
				goto exit;
			}
			i++;
		}
		cur = cur->next;
	}
	all_task_cnt = i;

	char buf[256] = {0};
	int lvl = 1;
	for (lvl = 1; lvl <= 40; lvl++) {
		sprintf(buf, "%s,%d", buf, level_cnt[lvl]);
	}
	//DEBUG_LOG("load_af_task evert level can get task: %s", buf);
	err = 0;

exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load angel fight daily task file %s Size[%u]", file, i);
}

/*
 * @brief 随机生成get_cnt个新的任务，并且这个ID不在ptask中，同时将ID放在ptask的task_idx位
 * @param ptask: ID数组(ID=idx+1)
 * @param task_idx: 随机出的任务ID的idx位
 * @param get_cnt: 随机出的任务的数量
 * @param level: 等级限制
 * @param cur_task_id: 刷新任务时，不能生成到与要刷新的任务一样的task_id
 */
int get_some_task_without_in_array(uint32_t* ptask, uint32_t task_idx, uint32_t get_cnt, uint32_t level, uint32_t cur_task_id)
{
	if (task_idx + get_cnt > MAX_DAILY_TASK_CNT || level == 0 || level > 40) {
		return -1;
	}

	uint32_t task_list[MAX_AF_TASK_CNT] = {0};
	uint8_t is_select[MAX_AF_TASK_CNT] = {0};
	if (cur_task_id != 0) {
		is_select[cur_task_id - 1] = 1;
	}
	int len = 0;

	int i = 0;
	for (i = 0; i < all_task_cnt; i++) {
		if (level >= all_task[i].low_lvl && level <= all_task[i].high_lvl) {
			task_list[len++] = i;
		}
	}

	if (len == 0) {
		ERROR_LOG("get_some_task_without_in_array: error len[%u]", len);
		return -1;
	}

	for (i = 0; i < task_idx; i++) {
		is_select[ptask[i] - 1] = 1;
	}

	while (get_cnt--) {
		uint32_t idx = rand() % len;
		while (is_select[task_list[idx]]) {
			idx = (idx + 1) % len;
		}

		ptask[task_idx++] = task_list[idx] + 1;
		//DEBUG_LOG("get_some_task_without_in_array: %u", task_list[idx] + 1);
		is_select[task_list[idx]] = 1;
	}

	return 0;
}

/*
 * @brief 根据任务id找到任务
 */
af_task_t* get_task_from_task_id(uint32_t task_id)
{
	if (task_id > all_task_cnt || task_id == 0) {
		return NULL;
	}
	return all_task + task_id - 1;
}

static int db_add_af_task_prize(sprite_t* p, af_task_t* task)
{
	int i = 0;
	uint8_t buf[pagesize];
	int j = 16;

	int out_cnt = 0;
	for (i = 0; i < task->out_cnt; i++) {
		if (task->item_out[i].itm->id != 13) {	//不是战斗经验时，用114D协议加物品
			out_cnt ++;
			if (pkg_item_kind(p, buf, task->item_out[i].itm->id, &j) == -1) {
				return -1;
			}
			PKG_H_UINT32(buf, task->item_out[i].itm->id, j);
			PKG_H_UINT32(buf, task->item_out[i].count , j);
			PKG_H_UINT32(buf, task->item_out[i].itm->max, j);
		}
	}
	if (out_cnt == 0) {
		return 0;
	}

	int k = 0;
	PKG_H_UINT32(buf, 0, k);
    PKG_H_UINT32(buf, out_cnt, k);
    PKG_H_UINT32(buf, 0, k);
    PKG_H_UINT32(buf, 0, k);
//	DEBUG_LOG("db_add_af_task_prize: len[%u]", j);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buf, p->id);
}

/*
 * @brief 拉取每日任务状态 8221
 */
int af_user_get_daily_task_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_GET_DAILY_TASK, p, 0, NULL, p->id);
}

int af_user_get_daily_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;	//0当天第一次拉取，需要生成3个任务 1拉取成功
		uint32_t refrench_cnt;	//当天刷新任务的次数
		uint32_t count;	//任务数量,state=0时为0，state=1时固定为3
	}__attribute__((packed)) get_daily_task_cbk_head_t;
	CHECK_BODY_LEN_GE(len, sizeof(get_daily_task_cbk_head_t));
	get_daily_task_cbk_head_t* cbk_head = (get_daily_task_cbk_head_t*)buf;

	typedef struct {
		uint32_t task_id;
		uint32_t task_state;
		uint32_t task_cnt;
		uint32_t max_cnt;
	}__attribute__((packed)) get_daily_task_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(get_daily_task_cbk_head_t) + cbk_head->count * sizeof(get_daily_task_cbk_pack_t));

	if (cbk_head->state == 1 && cbk_head->count != 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
	}

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->refrench_cnt, l);
	PKG_UINT32(msg, 3, l);

	int i = 0;
	if (cbk_head->state == 0) {
		//随机生成3个任务
		uint32_t task_id[3] = {0};
		uint32_t db_buf[32] = {0};
		int buf_len = 0;
		db_buf[buf_len++] = 3;

		if (get_some_task_without_in_array(task_id, 0, 3, p->fight_level, 0) == -1) {
			return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
		}
		DEBUG_LOG("get new daily task: uid[%u] task[%u %u %u]", p->id, task_id[0], task_id[1], task_id[2]);

		for (i = 0; i < 3; i++) {
			af_task_t* task = get_task_from_task_id(task_id[i]);
			if (task == NULL) {
				return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
			}
			PKG_UINT32(msg, task->task_id, l);
			PKG_UINT32(msg, 1, l);
			PKG_UINT32(msg, 0, l);
			//PKG_UINT32(msg, task->max_cnt, l);

			db_buf[buf_len++] = task->task_id;
			db_buf[buf_len++] = task->task_type;
			db_buf[buf_len++] = task->event_id;
			db_buf[buf_len++] = task->max_cnt;
		}
		send_request_to_db(SVR_PROTO_AF_SET_DAILY_TASK, NULL, buf_len * 4, db_buf, p->id);
	} else {
		get_daily_task_cbk_pack_t* cbk_pack = (get_daily_task_cbk_pack_t*)(buf + sizeof(get_daily_task_cbk_head_t));
		for (i = 0; i < 3; i++) {
			PKG_UINT32(msg, (cbk_pack + i)->task_id, l);
			PKG_UINT32(msg, (cbk_pack + i)->task_state, l);
			PKG_UINT32(msg, (cbk_pack + i)->task_cnt, l);
			//PKG_UINT32(msg, (cbk_pack + i)->max_cnt, l);
		}
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 完成任务 8222
 */
int af_user_finish_one_task_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	typedef struct {
		uint32_t task_id;
		uint32_t itemid;
	}__attribute__((packed)) db_accept_task_package_t;
	db_accept_task_package_t package = {0};

	int i = 0;
	uint32_t use_item;
	UNPKG_UINT32(body, package.task_id, i);
	UNPKG_UINT32(body, use_item, i);

	af_task_t* task = get_task_from_task_id(package.task_id);
	if (!task) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
	}

	if (use_item == 1) {
		package.itemid = 1301026;	//需要策划配任务终结卡ID
	}
	*(uint32_t*)p->session = package.task_id;

	return send_request_to_db(SVR_PROTO_AF_CHECK_FINISH_TASK, p, sizeof(package), &package, p->id);
}

/*
 * @brief 校验是否可以完成任务callback
 */
int af_user_check_finish_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t task_state;	//如果返回为2，表示可以完成
	}__attribute__((packed)) check_finish_task_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(check_finish_task_cbk_pack_t));
	check_finish_task_cbk_pack_t* cbk_pack = (check_finish_task_cbk_pack_t*)buf;

	if (cbk_pack->task_state != 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
	}
	uint32_t task_id = *(uint32_t*)p->session;
	af_task_t* task = get_task_from_task_id(task_id);
	db_add_af_task_prize(p, task);

	typedef struct {
		uint32_t add_exp;
	}__attribute__((packed)) db_add_exp_package_t;
	db_add_exp_package_t package = {0};
	int i = 0;
	for (i = 0; i < task->out_cnt; i++) {
		if (task->item_out[i].itm->id == 13) {
			package.add_exp = task->item_out[i].count;
		}
	}

	if (package.add_exp == 0) {
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_ADD_FIGHT_EXP, p, sizeof(package), &package, p->id);
}

/*
 * @brief 完成每日任务加经验callback
 */
int af_user_add_fight_exp_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t is_levelup;
		uint32_t achieve_cnt;
	}__attribute__((packed)) add_exp_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(add_exp_cbk_pack_t));

	add_exp_cbk_pack_t* cbk_pack = (add_exp_cbk_pack_t*)buf;
	CHECK_BODY_LEN(len, sizeof(add_exp_cbk_pack_t) + cbk_pack->achieve_cnt * sizeof(uint32_t));
	uint32_t* plist = (uint32_t*)(buf + sizeof(add_exp_cbk_pack_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->is_levelup, l);
	PKG_UINT32(msg, cbk_pack->achieve_cnt, l);
	int i = 0;
	for (i = 0; i < cbk_pack->achieve_cnt; i++) {
		PKG_UINT32(msg, plist[i], l);
	}

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B416, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 刷新任务 8223
 */
int af_user_refresh_one_task_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	typedef struct {
		uint32_t task_id;	//任务ID
		uint32_t itemid;	//任务刷新卡的id
	}__attribute__((packed)) db_refresh_task_package_t;
	db_refresh_task_package_t package = {0};

	int i = 0;
	UNPKG_UINT32(body, package.task_id, i);
	package.itemid = 1301025;

	af_task_t* task = get_task_from_task_id(package.task_id);
	if (!task) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
	}

	*(uint32_t*)p->session = package.task_id;

	return send_request_to_db(SVR_PROTO_AF_CHECK_REFRESH_TASK, p, sizeof(package), &package, p->id);
}

/*
 * @brief 校验是否可以刷新任务callback
 */
int af_user_check_refresh_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t task_id[2];
	}__attribute__((packed)) check_refresh_task_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(check_refresh_task_cbk_pack_t));
	check_refresh_task_cbk_pack_t* cbk_pack = (check_refresh_task_cbk_pack_t*)buf;

	uint32_t task_id[3] = {cbk_pack->task_id[0], cbk_pack->task_id[1], 0};
	//DEBUG_LOG("af_user_check_refresh_task_callback: tid1[%u] tid2[%u]", task_id[0], task_id[1]);
	if (get_some_task_without_in_array(task_id, 2, 1, p->fight_level, *(uint32_t*)p->session) == -1) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
	}

	typedef struct {
		uint32_t old_task_id;
		uint32_t task_id;
		uint32_t task_type;
		uint32_t max_cnt;
		uint32_t event_id;
	}__attribute__((packed)) db_refresh_task_package_t;
	af_task_t* task = get_task_from_task_id(task_id[2]);
	if (!task) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_task_wrong, 1);
	}

	db_refresh_task_package_t package = {0};
	package.old_task_id = *(uint32_t*)p->session;
	package.task_id = task->task_id;
	package.task_type = task->task_type;
	package.max_cnt = task->max_cnt;
	package.event_id = task->event_id;
	send_request_to_db(SVR_PROTO_AF_REFRESH_ONE_TASK, NULL, sizeof(package), &package, p->id);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, task->task_id, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取每日boss对战次数 8224
 */
int af_user_get_fight_boss_count_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_AF_GET_FIGHT_BOSS_COUNT, p, 0, NULL, p->id);
}

int af_user_get_fight_boss_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t normal_cnt;	//普通boss对战数量
		uint32_t vip_cnt;		//VIP boss对战数量
		uint32_t limit_time_cnt;//限时boss对战数量
		uint32_t active_cnt;	//活动boss对战数量
	}__attribute__((packed)) fight_boss_count_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(fight_boss_count_cbk_pack_t));

	fight_boss_count_cbk_pack_t* cbk_pack = (fight_boss_count_cbk_pack_t*)buf;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->normal_cnt, l);
	PKG_UINT32(msg, cbk_pack->vip_cnt, l);
	PKG_UINT32(msg, cbk_pack->limit_time_cnt, l);
	PKG_UINT32(msg, cbk_pack->active_cnt, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取徒弟市场信息 8225
 */
int af_user_get_apprentice_market_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t friend_cnt = 0;
	uint32_t buff[256] = {0};
	int i = 0;
	UNPKG_UINT32(body, friend_cnt, i);
	if (friend_cnt > 200) {		//好友最大为200
		ERROR_RETURN(("too many friends count = %u", friend_cnt), -1);
	}
	CHECK_BODY_LEN(len, friend_cnt * 4 + 4);

	len = 0;
	buff[len++] = friend_cnt;

	int j;
	uint32_t uid = 0;
	for (j = 0; j < friend_cnt; j++) {
		UNPKG_UINT32(body, uid, i);
		if (!uid || IS_NPC_ID(uid) || IS_GUEST_ID(uid) || uid == p->id) {
			continue;
        }
		int k;
		for (k = 1; k < len && uid != buff[k]; k++);
		if (k == len) {
			//DEBUG_LOG("In userid: index[%u] userid[%u]", len, uid);
			buff[len++] = uid;
		}
	}
	buff[0] = len - 1;
	if (buff[0] == 0) {
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}

	memcpy(p->session, buff, len * 4);

	return send_request_to_db(SVR_PROTO_AF_GET_MASTER_AND_APPRENTICE, p, 0, NULL, p->id);
}

/*
 * @brief 拉取师傅和徒弟的ID callback
 */
int af_get_master_and_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;	//师傅和徒弟的数量
	}__attribute__((packed)) master_apprentice_cbk_head_t;
	typedef struct {
		uint32_t userid;
	}__attribute__((packed)) master_apprentice_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, sizeof(master_apprentice_cbk_head_t));
	master_apprentice_cbk_head_t* cbk_head = (master_apprentice_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(master_apprentice_cbk_head_t) + cbk_head->count * sizeof(master_apprentice_cbk_pack_t));
	master_apprentice_cbk_pack_t* p_cbk_pack = (master_apprentice_cbk_pack_t*)(buf + sizeof(master_apprentice_cbk_head_t));

	uint32_t friend_cnt = *(uint32_t*)p->session;
	uint32_t* plist = (uint32_t*)(p->session + 4);

	uint32_t db_buf[200] = {0};
	len = 0;
	db_buf[len++] = 0;
	int i;
	for (i = 0; i < friend_cnt; i++) {
		int l = 0;
		for (l = 0; l < cbk_head->count && plist[i] != (p_cbk_pack + l)->userid; l++);
		if (l != cbk_head->count) {	//去除师傅和徒弟的ID
			continue;
		}
		//DEBUG_LOG("out userid: index[%u] userid[%u]", len, plist[i]);
		db_buf[len++] = plist[i];
	}
	db_buf[0] = len - 1;
	if (db_buf[0] == 0) {
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_GET_APPRENTICE_MARKET, p, len * 4, db_buf, p->id);
}

/*
 * @brief 拉取徒弟市场信息 callback
 */
int af_user_get_apprentice_market_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;	//徒弟市场人数
	}__attribute__((packed)) apprentice_market_cbk_head_t;

	typedef struct {
		uint32_t userid;	//用户id
		uint32_t exp;		//用户exp
		uint32_t level;		//等级
		uint32_t master_id;	//师傅ID
		uint32_t master_level;//师傅等级
	}__attribute__((packed)) apprentice_market_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, sizeof(apprentice_market_cbk_head_t));
	apprentice_market_cbk_head_t* cbk_head = (apprentice_market_cbk_head_t*) buf;

	CHECK_BODY_LEN(len, sizeof(apprentice_market_cbk_head_t) + cbk_head->count * sizeof(apprentice_market_cbk_pack_t));
	apprentice_market_cbk_pack_t* p_cbk_pack = (apprentice_market_cbk_pack_t*)(buf + sizeof(apprentice_market_cbk_head_t));

	int l = sizeof(protocol_t) + sizeof(uint32_t);
	//PKG_UINT32(msg, cbk_head->count, l);
	int i = 0;
	int cnt = 0;
	for (i = 0; i < cbk_head->count; i++) {
		if ((p_cbk_pack + i)->userid != p->id && (p_cbk_pack + i)->exp == 0) {	//经验为0的取消掉
			continue;
		}
		cnt++;
		PKG_UINT32(msg, (p_cbk_pack + i)->userid, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->level, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->master_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->master_level, l);
	}
	uint32_t protocol_len = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, protocol_len);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取师徒信息 8226
 */
int af_get_master_apprentice_info_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t userid;
	int i = 0;
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);

	return send_request_to_db(SVR_PROTO_AF_GET_APPRENTICE_INFO, p, 0, NULL, userid);
}

typedef struct {
	uint32_t userid;	//要查询的userid
	uint32_t merit;		//功德值
	uint32_t finish_cnt;//已出师的人数
	uint32_t level;		//拜师时候的等级
	uint32_t master_id;	//师傅ID
	uint32_t can_cash;	//是否可以孝敬师傅
	uint32_t apprentice_cnt;	//徒弟个数
}__attribute__((packed)) get_apprentice_cbk_head_t;

typedef struct {
	uint32_t apprentice_id;	//徒弟ID
	uint32_t level;			//收徒时候的等级
	uint32_t get_cash;		//是否可以领取徒弟的感谢金：0不能 1可以
}__attribute__((packed)) get_apprentice_cbk_pack_t;

typedef struct {
	uint32_t master_out_cnt;//师傅的出师人数
	uint32_t master_level;	//师傅等级，没有是0
	uint32_t master_merit;	//师傅功德值，没有是0
	uint32_t apprentice_cnt;//师傅的徒弟个数，没有是0
}__attribute__((packed)) get_master_cbk_pack_t;

static int send_master_apprentice_info_to_self(sprite_t* p, get_master_cbk_pack_t* pack)
{
	get_apprentice_cbk_head_t* cbk_head = (get_apprentice_cbk_head_t*)(p->session);
	get_apprentice_cbk_pack_t* p_cbk_pack = (get_apprentice_cbk_pack_t*)(p->session + sizeof(get_apprentice_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->userid, l);
	PKG_UINT32(msg, cbk_head->merit, l);
	PKG_UINT32(msg, cbk_head->finish_cnt, l);
	PKG_UINT32(msg, cbk_head->level, l);
	PKG_UINT32(msg, cbk_head->master_id, l);
	if (pack == NULL) {
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
	} else {
		PKG_UINT32(msg, pack->master_out_cnt, l);
		PKG_UINT32(msg, pack->master_level, l);
		PKG_UINT32(msg, pack->master_merit, l);
		PKG_UINT32(msg, pack->apprentice_cnt, l);
	}
	PKG_UINT32(msg, cbk_head->can_cash, l);
	PKG_UINT32(msg, cbk_head->apprentice_cnt, l);
	int i = 0;
	for (i = 0; i < cbk_head->apprentice_cnt; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->apprentice_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->level, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->get_cash, l);
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 得到个人师徒信息及徒弟信息 SVR_AF_GET_APPRENTICE_INFO callback
 */
int af_user_get_apprentice_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(get_apprentice_cbk_head_t));
	get_apprentice_cbk_head_t* cbk_head = (get_apprentice_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(get_apprentice_cbk_head_t) + cbk_head->apprentice_cnt * sizeof(get_apprentice_cbk_pack_t));
	//get_apprentice_cbk_pack_t* p_cbk_pack = (get_apprentice_cbk_pack_t*)(buf + sizeof(get_apprentice_cbk_head_t));
	memcpy(p->session, buf, len);

	if (cbk_head->master_id != 0) {
		CHECK_VALID_ID(cbk_head->master_id);
		return send_request_to_db(SVR_PROTO_AF_GET_MASTER_INFO, p, 0, NULL, cbk_head->master_id);
	}
	return send_master_apprentice_info_to_self(p, NULL);
}

/*
 * @brief 得到师傅信息 SVR_AF_GET_MASTER_INFO callback
 */
int af_user_get_master_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(get_master_cbk_pack_t));
	get_master_cbk_pack_t* cbk_pack = (get_master_cbk_pack_t*) buf;

	return send_master_apprentice_info_to_self(p, cbk_pack);
}

/*
 * @brief 判断是否可以收徒 8227
 */
int af_check_get_an_apprentice_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t apprentice_id = 0;
	int i = 0;
	UNPKG_UINT32(body, apprentice_id, i);
	CHECK_VALID_ID(apprentice_id);

	if (apprentice_id == p->id) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	*(uint32_t*)p->session = apprentice_id;
	return send_request_to_db(SVR_PROTO_AF_CHECK_GET_APPRENTICE, p, 0, NULL, p->id);
}

/*
 * @brief 判断是否可以收徒 callback
 */
int af_check_get_an_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;	//1可以收徒，2每天3次限制，3等级不够 4徒弟数到达上限
	}__attribute__((packed)) check_apprentice_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(check_apprentice_cbk_pack_t));
	check_apprentice_cbk_pack_t* cbk_pack = (check_apprentice_cbk_pack_t*)buf;

	if (cbk_pack->state == 1) {	//还要判断徒弟今天能不能拜师
		uint32_t apprentice_id = *(uint32_t*)p->session;
		return send_request_to_db(SVR_PROTO_AF_CHECK_APPRENTICE_GET_MASTER, p, 0, NULL, apprentice_id);
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->state, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 判断徒弟能否拜师 callback
 */
int af_check_apprentice_get_master_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;	//1可以拜师 5不能拜师
	}__attribute__((packed)) check_apprentice_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(check_apprentice_cbk_pack_t));
	check_apprentice_cbk_pack_t* cbk_pack = (check_apprentice_cbk_pack_t*)buf;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->state, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 拉取训练信息 8228
 */
int af_get_apprentice_trainning_info_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	typedef struct {
		uint32_t apprentice_id;	//徒弟ID
	}__attribute__((packed)) db_trainning_info_package_t;
	db_trainning_info_package_t package = {0};

	int i = 0;
	UNPKG_UINT32(body, package.apprentice_id, i);
	CHECK_VALID_ID(package.apprentice_id);

	return send_request_to_db(SVR_PROTO_AF_GET_APPRENTICE_TRAINING, p, sizeof(package), &package, p->id);
}

/*
 * @brief 拉取训练信息callback
 */
int af_get_apprentice_trainning_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;	//训练状态（返回已经训练过的id的数量）
	}__attribute__((packed)) trainning_info_cbk_head_t;
	typedef struct {
		uint32_t training_id;	//训练id
	}__attribute__((packed)) trainning_info_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(trainning_info_cbk_head_t));
	trainning_info_cbk_head_t* cbk_head = (trainning_info_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(trainning_info_cbk_head_t) + cbk_head->count * sizeof(trainning_info_cbk_pack_t));
	trainning_info_cbk_pack_t* p_cbk_pack = (trainning_info_cbk_pack_t*)(buf + sizeof(trainning_info_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->count, l);
	int i = 0;
	for (i = 0; i < cbk_head->count; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->training_id, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 训练徒弟8229
 */
int af_user_train_apprentice_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	uint32_t apprentice_id;	//徒弟ID
	uint32_t training_id;	//训练ID
	int i = 0;
	UNPKG_UINT32(body, apprentice_id, i);
	UNPKG_UINT32(body, training_id, i);
	CHECK_VALID_ID(apprentice_id);

	if (training_id > 6 || training_id <= 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	*(uint32_t*)p->session = apprentice_id;
	*(uint32_t*)(p->session + 4) = training_id;

	//要先得到徒弟等级，判断他是否能够出师
	return send_request_to_db(SVR_PROTO_AF_USER_GET_LEVEL, p, 0, NULL, apprentice_id);
}

/*
 * @brief 得到徒弟等级 callback
 */
int af_user_train_apprentice(sprite_t* p, uint32_t level)
{
	typedef struct {
		uint32_t apprentice_id;	//徒弟ID
		uint32_t training_id;	//训练ID
	}__attribute__((packed)) db_train_apprentice_package_t;
	db_train_apprentice_package_t package = {0};

	package.apprentice_id = *(uint32_t*)p->session;
	package.training_id = *(uint32_t*)(p->session + 4);
	*(uint32_t*)(p->session + 8) = level;
	return send_request_to_db(SVR_PROTO_AF_USER_TRAIN_APPRENTICE, p, sizeof(package), &package, p->id);
}

/*
 * @brief 训练徒弟 callback
 */
int af_user_train_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t level;
	}__attribute__((packed)) train_apprentice_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(train_apprentice_cbk_pack_t));
	train_apprentice_cbk_pack_t* cbk_pack = (train_apprentice_cbk_pack_t*)buf;

	uint32_t apprentice_id = *(uint32_t*)p->session;
	uint32_t training_id = *(uint32_t*)(p->session + 4);
	uint32_t level = *(uint32_t*)(p->session + 8);
	if ((int32_t)(level - cbk_pack->level) >= 5) {	//满足出师条件,直接返回
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	uint32_t add_exp[] = {0, 0, 20, 25, 30, 40, 50};	//训练徒弟所加的经验

	//徒弟消息
	af_add_master_apprentice_msg(apprentice_id, p->id, apprentice_id, training_id, add_exp[training_id], 0);
	//增加徒弟经验
	send_request_to_db(SVR_PROTO_AF_ADD_FIGHT_EXP, NULL, 4, &(add_exp[training_id]), apprentice_id);
	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B431 + (training_id - 2), get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, add_exp[training_id], l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 逐出师门 8230
 */
int af_user_expel_apprentice_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	db_expel_apprentice_package_t package = {0};
	int i = 0;
	UNPKG_UINT32(body, package.apprentice_id, i);
	CHECK_VALID_ID(package.apprentice_id);
	package.expel_type = 1;	//师傅主动逐徒弟

	return send_request_to_db(SVR_PROTO_AF_USER_EXPEL_APPRENTICE, p, sizeof(package), &package, p->id);
}

/*
 * @brief 逐出师门 callback
 */
int af_user_expel_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t apprentice_id;	//徒弟ID
	}__attribute__((packed)) expel_apprentice_cbk_pack_t;
	expel_apprentice_cbk_pack_t* cbk_pack = (expel_apprentice_cbk_pack_t*)buf;

	//徒弟的USER表中删除师傅
	db_retray_master_package_t package = {0};
	package.retray_type = 2;
	send_request_to_db(SVR_PROTO_AF_USER_DELETE_MASTER, NULL, sizeof(package), &package, cbk_pack->apprentice_id);
	af_add_master_apprentice_msg(cbk_pack->apprentice_id, p->id, cbk_pack->apprentice_id, 101, 0, 0);

	//徒弟的全局表中删除师傅
	uint32_t new_master_id = 0;
	send_request_to_db(SVR_PROTO_AF_MONIFY_MASTER, NULL, sizeof(new_master_id), &new_master_id, cbk_pack->apprentice_id);

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B42D, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->apprentice_id, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 徒弟孝敬师傅（感谢金） 8231
 */
int af_apprentice_respect_master_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_APPRENTICE_RESPECT_MASTER, p, 0, NULL, p->id);
}

/*
 * @brief 徒弟孝敬师傅（感谢金） callback
 */
int af_apprentice_respect_master_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;		//0送感谢金成功 1不能送感谢金
		uint32_t master_id;
	}__attribute__((packed)) respect_master_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(respect_master_cbk_pack_t));
	respect_master_cbk_pack_t* cbk_pack = (respect_master_cbk_pack_t*)buf;

	if (cbk_pack->master_id == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}
	CHECK_VALID_ID(cbk_pack->master_id);
	//师傅表中设置可以领取感谢金
	send_request_to_db(SVR_PROTO_AF_MASTER_SET_RESPECT, NULL, 4, &(p->id), cbk_pack->master_id);

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B42E, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 师傅领取感谢金 8232
 */
int af_user_get_respect_cash_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	typedef struct {
		uint32_t apprentice_id;	//徒弟ID
	}__attribute__((packed)) db_get_respect_cash_t;
	db_get_respect_cash_t package = {0};
	int i = 0;
	UNPKG_UINT32(body, package.apprentice_id, i);
	CHECK_VALID_ID(package.apprentice_id);

	return send_request_to_db(SVR_PROTO_AF_GET_RESPECT_CASH, p, sizeof(package), &package, p->id);
}

/*
 * @brief 师傅领取感谢金 callback
 */
int af_user_get_respect_cash_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	typedef struct {
		uint32_t state;
	}__attribute__((packed)) get_cash_cbk_pack_t;

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B42F, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	int l = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 背叛师门 8233
 */
int af_apprentice_retray_master_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	db_retray_master_package_t package = {0};
	package.retray_type = 1;

	return send_request_to_db(SVR_PROTO_AF_USER_DELETE_MASTER, p, sizeof(package), &package, p->id);
}

/*
 * @brief 背叛师门 callback
 */
int af_apprentice_retray_master_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t master_id;	//师傅ID
	}__attribute__((packed)) retray_master_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(retray_master_cbk_pack_t));
	retray_master_cbk_pack_t* cbk_pack = (retray_master_cbk_pack_t*)buf;

	if (cbk_pack->master_id != 0) {
		db_expel_apprentice_package_t package = {0};
		package.expel_type = 2;	//徒弟背叛师傅
		package.apprentice_id = p->id;
		send_request_to_db(SVR_PROTO_AF_USER_EXPEL_APPRENTICE, NULL, sizeof(package), &package, cbk_pack->master_id);
		af_add_master_apprentice_msg(cbk_pack->master_id, cbk_pack->master_id, p->id, 102, 0, 0);

		//徒弟的全局表中删除师傅
		uint32_t new_master_id = 0;
		send_request_to_db(SVR_PROTO_AF_MONIFY_MASTER, NULL, sizeof(new_master_id), &new_master_id, p->id);
	}

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B42C, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	int l = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 出师（师傅和徒弟出师都用这条） 8234
 */
int af_finish_apprentice_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);
	uint32_t finish_type = 0;
	uint32_t userid = 0;
	int i = 0;
	UNPKG_UINT32(body, finish_type, i);
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);

	uint32_t master_id;
	uint32_t apprentice_id;
	if (finish_type == 1) {
		master_id = p->id;
		apprentice_id = userid;
	} else if (finish_type == 2) {
		master_id = userid;
		apprentice_id = p->id;
	} else {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}

	*(uint32_t*)p->session = finish_type;
	*(uint32_t*)(p->session + 4) = master_id;
	*(uint32_t*)(p->session + 8) = apprentice_id;

	//校验徒弟是否可以出师
	return send_request_to_db(SVR_PROTO_AF_APPRENTICE_CHECK_FINISH, p, 4, &(master_id), apprentice_id);
}

/*
 * @brief 校验徒弟是否可以出师 callback
 */
int af_apprentice_check_finish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t apprentice_level;	//徒弟当前等级
	}__attribute__((packed)) check_finish_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(check_finish_cbk_pack_t));
	check_finish_cbk_pack_t* cbk_pack = (check_finish_cbk_pack_t*)buf;

	uint32_t finish_type = *(uint32_t*)p->session;
	uint32_t master_id = *(uint32_t*)(p->session + 4);
	uint32_t apprentice_id = *(uint32_t*)(p->session + 8);

	typedef struct {
		uint32_t finish_type;		//1师傅点出师 2徒弟点出师
		uint32_t apprentice_id;		//徒弟ID
		uint32_t apprentice_level;	//徒弟当前等级
	}__attribute__((packed)) db_finish_apprentice_package_t;
	db_finish_apprentice_package_t package = {0};
	package.finish_type = finish_type;
	package.apprentice_id = apprentice_id;
	package.apprentice_level = cbk_pack->apprentice_level;

	return send_request_to_db(SVR_PROTO_AF_MASTER_FINISH_APPRENTICE, p, sizeof(package), &package, master_id);
}

typedef struct {
	uint32_t item_id;		//出师成功获得的物品id
	uint32_t item_cnt;		//出师成功获得的物品id
}__attribute__((packed)) finish_apprentice_cbk_pack_t;

static int send_finish_gift_to_self(sprite_t* p, uint32_t apprentice_cnt, uint32_t count, finish_apprentice_cbk_pack_t* p_cbk_pack)
{
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, 0, l);
	PKG_UINT32(msg, *(uint32_t*)(p->session + 4), l);
	PKG_UINT32(msg, *(uint32_t*)(p->session + 8), l);
	PKG_UINT32(msg, apprentice_cnt, l);
	PKG_UINT32(msg, count, l);
	int i;
	for (i = 0; i < count; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->item_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->item_cnt, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 出师（师傅和徒弟出师都用这条） callback
 */
int af_master_finish_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;			//0可以出师，1等级不够
		uint32_t apprentice_cnt;//出师的人数
		uint32_t count;			//出师成功师傅获得的物品数量
	}__attribute__((packed)) finish_apprentice_cbk_head_t;

	CHECK_BODY_LEN_GE(len, sizeof(finish_apprentice_cbk_head_t));
	finish_apprentice_cbk_head_t* cbk_head = (finish_apprentice_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(finish_apprentice_cbk_head_t) + cbk_head->count * sizeof(finish_apprentice_cbk_pack_t));
	finish_apprentice_cbk_pack_t* p_cbk_pack = (finish_apprentice_cbk_pack_t*)(buf + sizeof(finish_apprentice_cbk_head_t));

	if (cbk_head->state != 0) {
		uint32_t l = sizeof(protocol_t);
		PKG_UINT32(msg, cbk_head->state, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}

	uint32_t finish_type = *(uint32_t*)p->session;
	//uint32_t master_id = *(uint32_t*)(p->session + 4);
	uint32_t apprentice_id = *(uint32_t*)(p->session + 8);

	uint32_t msgbuf[2] = {apprentice_id, 1};
	msglog(statistic_logfile, 0x0409B42B, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	//全局表删除师傅ID
	uint32_t new_master_id = 0;
	send_request_to_db(SVR_PROTO_AF_MONIFY_MASTER, NULL, sizeof(new_master_id), &new_master_id, apprentice_id);
	if (finish_type == 2) {	//徒弟点出师
		//*(uint32_t*)(p->session + 12) = cbk_head->apprentice_cnt;
		return send_request_to_db(SVR_PROTO_AF_APPRENTICE_FINISH_APPRENTICE, p, 0, NULL, apprentice_id);
	}

	send_request_to_db(SVR_PROTO_AF_APPRENTICE_FINISH_APPRENTICE, NULL, 0, NULL, apprentice_id);
	return send_finish_gift_to_self(p, cbk_head->apprentice_cnt, cbk_head->count, p_cbk_pack);
}

/*
 * @breif 徒弟点出师返回的物品 callback
 */
int af_apprentice_finish_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;		//出师成功徒弟获得的物品数量
	}__attribute__((packed)) finish_apprentice_cbk_head_t;
	CHECK_BODY_LEN_GE(len, sizeof(finish_apprentice_cbk_head_t));
	finish_apprentice_cbk_head_t* cbk_head = (finish_apprentice_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(finish_apprentice_cbk_head_t) + cbk_head->count * sizeof(finish_apprentice_cbk_pack_t));
	finish_apprentice_cbk_pack_t* p_cbk_pack = (finish_apprentice_cbk_pack_t*)(buf + sizeof(finish_apprentice_cbk_head_t));

	//如果cbk_head->count不为0，则认为是第一次徒弟出师，前端需要提示给东西
	return send_finish_gift_to_self(p, cbk_head->count ? 1 : 0, cbk_head->count, p_cbk_pack);
}

/*
 * @brief 师徒消息 8235
 */
int af_user_get_master_record_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_GET_MASTER_RECORD, p, 0, NULL, p->id);
}

int af_user_get_master_record_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;
	}__attribute__((packed)) master_record_cbk_head_t;
	typedef struct {
		uint32_t record_time;//时间
		uint32_t master_id;		//师傅ID
		uint32_t apprentice_id;	//徒弟ID
		uint32_t msg_id;	//训练id
		uint32_t get_exp;	//获得经验
		uint32_t get_merit;	//获得功德值
	}__attribute__((packed)) master_record_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, 4);
	master_record_cbk_head_t* cbk_head = (master_record_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(master_record_cbk_head_t) + cbk_head->count * sizeof(master_record_cbk_pack_t));
	master_record_cbk_pack_t* p_cbk_pack = (master_record_cbk_pack_t*)(buf + sizeof(master_record_cbk_head_t));

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->count, l);
	int i;
	for (i = 0; i < cbk_head->count; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->record_time, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->master_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->apprentice_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->msg_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->get_exp, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->get_merit, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 收徒对战结束后，用户收徒 8236
 */
int af_user_add_apprentice_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);
	if (p->get_apprentice != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_master_apprentice_wrong, 1);
	}

	return send_request_to_db(SVR_PROTO_AF_USER_GET_LEVEL, p, 0, NULL, p->apprentice_id);
}


/*
 * @brief 收徒对战胜利，得到徒弟的等级，发送DB添加徒弟
 */
static int af_user_add_apprentice(sprite_t* p, uint32_t level)
{
	typedef struct {
		uint32_t master_id;			//收的徒弟的当前师傅ID
		uint32_t apprentice_id;		//徒弟ID
		uint32_t apprentice_level;	//徒弟等级
	}__attribute__((packed)) db_add_apprentice_t;

	db_add_apprentice_t package = {0};
	package.master_id = p->id;
	package.apprentice_id = p->apprentice_id;
	package.apprentice_level = level;

	*(uint32_t*)p->session = level;

	return send_request_to_db(SVR_PROTO_AF_USER_ADD_APPRENTICE, p, sizeof(package), &package, p->id);
}

/*
 * @brief 拉取等级callback
 */

int af_user_get_level_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t fight_level;
	}__attribute__((packed)) get_level_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(get_level_cbk_pack_t));
	get_level_cbk_pack_t* cbk_pack = (get_level_cbk_pack_t*)buf;

	switch(p->waitcmd) {
	case PROTO_AF_USER_TRAIN_APPRENTICE:
		return af_user_train_apprentice(p, cbk_pack->fight_level);
	case PROTO_AF_USER_ADD_APPRENTICE:
		return af_user_add_apprentice(p, cbk_pack->fight_level);
	default:
		break;
	}
	return 0;
}

/*
 * @brief 收徒成功 callback 后，修改徒弟的师傅为自己、删除徒弟的师傅表中徒弟的信息、修改徒弟的全局表的师傅为自己
 */
int af_user_add_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;
	}__attribute__((packed)) add_apprentice_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(add_apprentice_cbk_pack_t));
	add_apprentice_cbk_pack_t* cbk_pack = (add_apprentice_cbk_pack_t*)buf;
	if (cbk_pack->state != 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_master_apprentice_wrong, 1);
	}

	//修改徒弟的师傅为自己
	uint32_t db_buf[2] = {p->id, *(uint32_t*)p->session};
	//DEBUG_LOG("apprentice[%u] level[%u]", p->apprentice_id, *(uint32_t*)p->session);
	send_request_to_db(SVR_PROTO_AF_MODIFY_USER_MASTER, NULL, sizeof(db_buf), db_buf, p->apprentice_id);

	af_add_master_apprentice_msg(p->apprentice_id, p->id, p->apprentice_id, 100, 0, 10);

	if (p->old_master_id != 0) {	//删除徒弟的师傅表中徒弟的信息
		db_expel_apprentice_package_t package = {0};
		package.expel_type = 2;		//徒弟被抢
		package.apprentice_id = p->apprentice_id;
		send_request_to_db(SVR_PROTO_AF_USER_EXPEL_APPRENTICE, NULL, sizeof(package), &package, p->old_master_id);
	}

	//修改徒弟的全局表的师傅为自己
	send_request_to_db(SVR_PROTO_AF_MONIFY_MASTER, NULL, sizeof(p->id), &(p->id), p->apprentice_id);

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B42A, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, p->old_master_id, l);
	PKG_UINT32(msg, p->apprentice_id, l);

	p->get_apprentice = 0;
	p->old_master_id = 0;
	p->apprentice_id = 0;

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 添加师徒交互消息
 */
int af_add_master_apprentice_msg(uint32_t id, uint32_t master, uint32_t apprentice, uint32_t msg_id, uint32_t exp, int32_t merit)
{
	typedef struct {
		uint32_t datetime;	//时间0
		uint32_t master;	//师傅ID
		uint32_t apprentice;//徒弟ID
		uint32_t msg_id;	//消息ID：1-6训练 100收小小 101逐出师门 102小小逃走 103出师 104经验加成 105徒弟被抢
		uint32_t exp;		//获得的经验
		int32_t  merit;		//获得的功德值
	}__attribute__((packed)) db_msg_package_t;

	db_msg_package_t package = {0};

	package.master = master;
	package.apprentice = apprentice;
	package.datetime = 0;
	package.msg_id = msg_id;
	package.exp = exp;
	package.merit = merit;

	return send_request_to_db(SVR_PROTO_AF_ADD_MASTER_APPRENTICE_MSG, NULL, sizeof(package), &package, id);
}

/*
 * @brief 拉取PVP对战次数及兑换记录 8237
 */
int af_get_pvp_fight_record_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_AF_GET_PVP_FIGHT_RECORD, p, 0, NULL, p->id);
}

int af_get_pvp_fight_record_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t day_pvp_cnt;	//PVP每日战斗次数
		uint32_t total_win_cnt;	//PVP总获胜次数
		uint32_t prize_flag;	//已领取的标志
	}__attribute__((packed)) pvp_fight_record_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(pvp_fight_record_cbk_pack_t));
	pvp_fight_record_cbk_pack_t* cbk_pack = (pvp_fight_record_cbk_pack_t*)buf;

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->day_pvp_cnt, l);
	PKG_UINT32(msg, cbk_pack->total_win_cnt, l);

	uint32_t temp_l = l;
	l += 4;

	DEBUG_LOG("af_get_pvp_fight_record_callback:%u", cbk_pack->total_win_cnt);

	uint32_t get_cnt = 0;
	int i = 0;
	for (i = 0; i < PVP_PRIZE_FLAG_CNT; i++) {
		if (cbk_pack->prize_flag & (1 << i) ) {
			get_cnt++;
			PKG_UINT32(msg, i + 1, l);
		}
	}
	PKG_UINT32(msg, get_cnt, temp_l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief PVP获胜次数兑换成就卡 8238
 */
int af_get_pvp_win_prize_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, sizeof(uint32_t));
	CHECK_VALID_ID(p->id);

	uint32_t swap_win_cnt = 0;
	int i = 0;
	UNPKG_UINT32(body, swap_win_cnt, i);

	if (swap_win_cnt != 10 && swap_win_cnt != 50 && swap_win_cnt != 100 && swap_win_cnt != 200 && swap_win_cnt != 500 && swap_win_cnt != 1000) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_fight_id_wrong, 1);
	}
	*(uint32_t*)p->session = swap_win_cnt;

	return send_request_to_db(SVR_PROTO_AF_GET_PVP_WIN_PRIZE, p, sizeof(uint32_t), &swap_win_cnt, p->id);
}

int af_get_pvp_win_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;	//1成功
		uint32_t count;	//获得物品的数量
	}__attribute__((packed)) pvp_win_prize_cbk_head_t;
	typedef struct {
		uint32_t item_id;
		uint32_t item_cnt;
	}__attribute__((packed)) pvp_win_prize_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, sizeof(pvp_win_prize_cbk_head_t));
	pvp_win_prize_cbk_head_t* cbk_head = (pvp_win_prize_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(pvp_win_prize_cbk_head_t) + cbk_head->count * sizeof(pvp_win_prize_cbk_pack_t));
	pvp_win_prize_cbk_pack_t* p_cbk_pack = (pvp_win_prize_cbk_pack_t*)(buf + sizeof(pvp_win_prize_cbk_head_t));

	uint32_t swap_win_cnt = *(uint32_t*)p->session;
	if (cbk_head->state == 1) {
		uint32_t msg_id = 0;
		uint32_t msgbuf[2] = {p->id, 1};
		if (swap_win_cnt == 10) {
			msg_id = 0x0409B456;
		} else if (swap_win_cnt == 50) {
			msg_id = 0x0409B457;
		} else if (swap_win_cnt == 100) {
			msg_id = 0x0409B458;
		} else if (swap_win_cnt == 200) {
			msg_id = 0x0409B459;
		} else if (swap_win_cnt == 500) {
			msg_id = 0x0409B45A;
		} else if (swap_win_cnt == 1000) {
			msg_id = 0x0409B463;
		}
		if (msg_id != 0) {
			msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
		}
	}

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->state, l);
	PKG_UINT32(msg, cbk_head->count, l);
	int i = 0;
	for (i = 0; i < cbk_head->count; i++) {
		PKG_UINT32(msg, (p_cbk_pack + i)->item_id, l);
		PKG_UINT32(msg, (p_cbk_pack + i)->item_cnt, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int af_exchange_card_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t item_id;
	uint32_t item_cnt;
	CHECK_BODY_LEN( len, sizeof(item_id) + sizeof(item_cnt) );
	int j = 0;
	UNPKG_UINT32( body, item_id, j );
	UNPKG_UINT32( body, item_cnt, j );
	if( 0 == item_cnt )
	{
		response_proto_head( p, p->waitcmd, 0 );
		return 0;
	}
	item_kind_t* ik = find_kind_of_item( item_id );

	if( !ik )
	{
		return send_to_self_error( p, p->waitcmd, ERR_invalid_item_id, 1 );
	}
	item_t* it = get_item( ik, item_id );
	if( !it )
	{
		return send_to_self_error( p, p->waitcmd, ERR_invalid_item_id, 1 );
	}

	if( ik->kind != FIGHT_CARD_KIND || 0 == it->u.af_card_tag.exchange_val )
	{
		return send_to_self_error( p, p->waitcmd, ERR_invalid_item_id, 1 );
	}
	int sendlen = 0;
	uint8_t buff[1024];
	PKG_H_UINT32( buff, 1, sendlen );
	PKG_H_UINT32( buff, 1, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );
	pkg_item_kind( p, buff, ANGEL_FIGHT_DONATE_ITEM_ID, &sendlen );
	PKG_H_UINT32( buff, ANGEL_FIGHT_DONATE_ITEM_ID, sendlen );
	PKG_H_UINT32( buff, it->u.af_card_tag.exchange_val * item_cnt, sendlen );

	pkg_item_kind( p, buff, item_id, &sendlen );
	PKG_H_UINT32( buff, item_id, sendlen );
	PKG_H_UINT32( buff, item_cnt, sendlen );
	PKG_H_UINT32( buff, it->max, sendlen );
	return send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, sendlen, buff, p->id );
}


int af_donate_card_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t item_id;
	uint32_t item_cnt;
	CHECK_BODY_LEN( len, sizeof(item_id) + sizeof(item_cnt) );
	int j = 0;
	UNPKG_UINT32( body, item_id, j );
	UNPKG_UINT32( body, item_cnt, j );
	if( 0 == item_cnt )
	{
		response_proto_head( p, p->waitcmd, 0 );
		return 0;
	}
	item_kind_t* ik = find_kind_of_item( item_id );
	if( !ik )
	{
		return send_to_self_error( p, p->waitcmd, ERR_invalid_item_id, 1 );
	}
	item_t* it = get_item( ik, item_id );
	if( !it )
	{
		return send_to_self_error( p, p->waitcmd, ERR_invalid_item_id, 1 );
	}

	if( ik->kind != FIGHT_CARD_KIND || 0 == it->u.af_card_tag.donate_val )
	{
		return send_to_self_error( p, p->waitcmd, ERR_invalid_item_id, 1 );
	}
	int sendlen = 0;
	uint8_t buff[1024];
	PKG_H_UINT32( buff, 1, sendlen );
	PKG_H_UINT32( buff, 1, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );

	pkg_item_kind( p, buff, item_id, &sendlen );
	PKG_H_UINT32( buff, item_id, sendlen );
	PKG_H_UINT32( buff, item_cnt, sendlen );

	pkg_item_kind( p, buff, ANGEL_FIGHT_DONATE_ITEM_ID, &sendlen );
	PKG_H_UINT32( buff, ANGEL_FIGHT_DONATE_ITEM_ID, sendlen );
	PKG_H_UINT32( buff, it->u.af_card_tag.donate_val * item_cnt, sendlen );
	item_t* it_donate = get_item_prop( ANGEL_FIGHT_DONATE_ITEM_ID );
	if( !it_donate )
	{
		ERROR_RETURN( ("Can not find the item ID:[%d]", ANGEL_FIGHT_DONATE_ITEM_ID) , -1 );
	}
	PKG_H_UINT32( buff, it_donate->max, sendlen );
	return send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, sendlen, buff, p->id );
}

int af_get_donate_info_cmd(sprite_t* p, const uint8_t *body, int len)
{
	CHECK_VALID_ID( p->id );

	int j = 0;
	uint8_t buff[13];
	pkg_item_kind( p, buff, ANGEL_FIGHT_DONATE_ITEM_ID, &j );
	PKG_H_UINT32(buff, ANGEL_FIGHT_DONATE_ITEM_ID, j);
	PKG_H_UINT32(buff, ANGEL_FIGHT_DONATE_ITEM_ID + 1, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db( SVR_PROTO_GET_ITEM, p, j, buff, p->id);
}
