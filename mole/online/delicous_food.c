#include <statistic_agent/msglog.h>
#include<libtaomee/project/utilities.h>
#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "util.h"
#include "mole_car.h"
#include "delicous_food.h"

/* @brief 引逗小猫，按概率输出物品
 */
int tantalize_cat_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t id, i = 0;
	UNPKG_UINT32(body, id, i);
	return send_request_to_db(SVR_PROTO_CHECK_ANIMAL, p, sizeof(uint32_t), &id, p->id);
}

/* @brief 校验是否有猫
 */
int tantalize_cat_check_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t animal_id = *(uint32_t *)buf;
	uint32_t mature_time = *(uint32_t *)(buf + 4);
	if (mature_time == 0 || animal_id != 1270010) {
		ERROR_RETURN(("you have not cat %u", p->id), -1);
	}
	return db_set_sth_done(p, 160, 5, p->id);
}

/* @brief 如果引逗猫没有超过5次,查询相应的物品是否超过上限
 */
int tantalize_cat_set_sth_done_callback(sprite_t *p)
{
	uint32_t db_buf[] = {4, 180071, 180048, 1230025, 1230018};
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, sizeof(db_buf), db_buf, p->id);
}

/* @brief 如果物品没有达到对应的上限，则随机送物品
 */
int tantalize_cat_get_something_callbak(sprite_t *p, char *buf, int len)
{
	uint32_t item_count[8] = { };
	int j = 0, i = 0;
	uint32_t count = 0;
	UNPKG_H_UINT32(buf, count, j);
	for (i = 0; i < 4; i++) {
		UNPKG_H_UINT32(buf, item_count[2 * i], j);
		UNPKG_H_UINT32(buf, item_count[2 * i + 1], j);
	}
	uint32_t item_id = 0;
	if (rand() % 100 < 17) {
		item_t *itm = get_item_prop(180071);
		if (item_count[1] < itm->max) {/*180071 个数*/
			item_id = 180071;
		}
	} else if (rand() % 100 < 33) {
		item_t *itm = get_item_prop(180048);
		if (item_count[3] < itm->max) {/*180048 个数*/
			item_id = 180048;
		}
	} else if (rand() % 100 < 39) {
		item_t *itm = get_item_prop(1230025);
		if (item_count[5] < itm->max) {/*1230025 个数*/
			item_id = 1230025;
		}
	} else if (rand() % 100 < 45) {/*1230018 个数*/
		item_t *itm = get_item_prop(1230018);
		if (item_count[7] < itm->max) {
			item_id = 1230018;
		}
	}
	if (item_id != 0) {
		db_single_item_op(0, p->id, item_id, 1, 1);
	}
	response_proto_uint32(p, p->waitcmd, item_id, 0);
	return 0;
}


/* @brief 场景中得到美食，校验是否达到当天上限
 */
int get_delicous_food_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type = 0, i = 0;
	UNPKG_UINT32(body, type, i);
	if (type > 4) {
		ERROR_RETURN(("type id is wrong %u %u", p->id, type), -1);
	}
	*(uint32_t *)p->session = type;
	send_request_to_db(SVR_PROTO_GET_DELICOUS_FOOD, p, sizeof(type), &type, p->id);
	return 0;
}

/* @brief 如果没有达到上限，增加物品,每次增加三个物品，一天一次
 */
int get_delicous_food_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t index = *(uint32_t *)buf;
	uint32_t item_id = 180074 + index;
	if (item_id > 180083) {
		ERROR_RETURN(("valid itemid %u %u", p->id, item_id), -1);
	}
	db_single_item_op(0, p->id, item_id, 2, 1);
	response_proto_uint32(p, p->waitcmd, item_id, 0);
	return 0;
}

/* @brief 检查ID是否是分享的ID，如是得到对应增加的美誉度
 */
int check_share_id(uint32_t item_id, uint32_t * p_reputation)
{
	uint32_t share_id[][2] = {
								{180074,  1}, {180075,  1}, {180076,  1}, {180077,  1}, {180078,  1}, {180079,  1},
								{180080,  1}, {180081,  1}, {180082,  1}, {180083,  1}, {180084,  5}, {180085,  5},
								{180086,  5}, {180087,  5}, {180088,  5}, {180089,  5}, {180017, 10}, {180072, 10},
								{180062, 10}, {180065, 10}, {180063, 10}, {180061, 10}, {180059, 10}, {180057, 10},
								{180054, 10}, {180053, 10}, {180048, 10}, {180021, 10}, {180016, 10}, {180041, 10},
								{180025, 10}, {180043, 10}, {180044, 10}, {190659, 10}, {180045, 10}, {180046, 10},
								{180090, 10}
							};
	int i = 0;
	for (i = 0; i < (sizeof(share_id) / sizeof(share_id[0])); i++) {
		if (share_id[i][0] == item_id) {
			*p_reputation = share_id[i][1];
			return 0;
		}
	}
	return 1;
}

/* @brief  分享美食，并且增加美誉度
 */
int share_delicous_food_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 4);
	uint32_t count = 0, j = 0;
	UNPKG_UINT32(body, count, j);
	if (count > 20) {
		ERROR_RETURN(("share delicous food count error %u %u", p->id, count), -1);
	}
	CHECK_BODY_LEN(bodylen, 4 + count * 8);
	uint32_t itm_id, itm_count, reputation, total_reputation = 0;
	uint32_t i = 0;
	uint8_t db_buf[4096];
	int k = 0;
	PKG_H_UINT32(db_buf, count, k);
	PKG_H_UINT32(db_buf, 0, k);
	PKG_H_UINT32(db_buf, 0, k);
	PKG_H_UINT32(db_buf, 0, k);
	for (i = 0; i < count; i++) {
		UNPKG_UINT32(body, itm_id, j);
		pkg_item_kind(p, db_buf, itm_id, &k);
		PKG_H_UINT32(db_buf, itm_id, k);
		UNPKG_UINT32(body, itm_count, j);
		PKG_H_UINT32(db_buf, itm_count, k);
		if (check_share_id(itm_id, &reputation)) {
			ERROR_LOG("share id is wrong %u %u", p->id, itm_id);
			continue;
		}
		total_reputation += reputation * itm_count;
	}
	*(uint32_t *)p->session = total_reputation;
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, k, db_buf, p->id);
	return 0;
}

/* @brief 如果物品扣除成功，增加美誉度
 */
int share_delicoust_food_callback(sprite_t *p)
{
	uint32_t total_reputation = *(uint32_t *)p->session;
	send_request_to_db(SVR_PROTO_SHARE_DELICOUS_FOOD, NULL, sizeof(total_reputation), &total_reputation, p->id);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 检查ID是否是分享的ID，如是得到对应增加的美誉度
 */
int check_exchange_id(uint32_t item_id, uint32_t * p_reputation)
{
	uint32_t share_id[][2] = {
								/*itmeid, 兑换需要的美誉值*/
								{13401,	  40}, {13402,   40}, {160747,  20}, {160748,  20}, {1300012,200},
								{160253,  20}, {12505,   40}, {12508,   40}, {160256,  20}, {160251,  40},
							};
	int i = 0;
	for (i = 0; i < (sizeof(share_id) / sizeof(share_id[0])); i++) {
		if (share_id[i][0] == item_id) {
			*p_reputation = share_id[i][1];
			return 0;
		}
	}
	return 1;
}

/* @brief 用美誉度交换物品,先获取用户目前拥有的数目
 */
int exchange_something_with_reputation_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t itm_id, itm_count, i = 0;
	UNPKG_UINT32(body, itm_id, i);
	UNPKG_UINT32(body, itm_count, i);
	uint32_t reputation = 0;
	if (check_exchange_id(itm_id, &reputation)) {
		ERROR_RETURN(("wrong id %u %u", p->id, itm_id), -1);
	}
	*(uint32_t *)p->session = itm_id;
	*(uint32_t *)(p->session + 4) = itm_count;
	*(uint32_t *)(p->session + 8) = reputation;
	if (itm_id == 1300012) {
		struct db_buf {
			uint32_t attiretype;
			uint32_t start_id;
			uint32_t end_id;
			uint8_t flag;
		}__attribute__((packed)) db_data;
		db_data.attiretype = 2;
		db_data.start_id = 1220097;
		db_data.end_id= 1220098;
		db_data.flag = 2;

		return send_request_to_db(SVR_PROTO_GET_ITEM, p, sizeof(struct db_buf), &db_data, p->id);
	}
	int32_t sub_reputation = reputation * itm_count * (-1);
	return send_request_to_db(SVR_PROTO_SHARE_DELICOUS_FOOD, p, sizeof(sub_reputation), &sub_reputation, p->id);
}

/* @brief 如果用户兑换的是汽车，检查用户是否有车库
 */
int check_if_have_garge_callback(sprite_t *p, char *buf, int len)
{
	uint32_t count = *(uint32_t *)buf;
	if (count == 0) {
		send_to_self_error(p, p->waitcmd, -ERR_not_have_garage, 1);
	}
	uint32_t itm_count = *(uint32_t *)(p->session + 4);
	uint32_t reputation = *(uint32_t *)(p->session + 8);
	int32_t sub_reputation = reputation * itm_count * (-1);
	send_request_to_db(SVR_PROTO_SHARE_DELICOUS_FOOD, p, sizeof(sub_reputation), &sub_reputation, p->id);
	return 0;
}

/* @brief 扣除美誉值成功，加物品
 */
int sub_reputation_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itm_id = *(uint32_t *)p->session;
	uint32_t itm_count = *(uint32_t *)(p->session + 4);
	if (itm_id == 1300012) {
		db_add_car(NULL, 1300012, 0, p->id);
	} else {
		item_kind_t* kind = find_kind_of_item(itm_id);
		if (!kind) {
			ERROR_RETURN(("bad itmid\t"), -1);
		}
		uint32_t flag = 0;
		if (kind->kind == HOME_ITEM_KIND) {
			flag = 1;
		} else if (kind->kind == HOMELAND_ITEM_KIND) {
			flag = 2;
		} else if (kind->kind == HAPPY_CARD_KIND) {
			flag = 6;
		}
		item_t *itm = get_item_prop(itm_id);
		uint32_t db_buf[] = {0, 1, 0, 0, flag, itm_id, itm_count, itm->max};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
	}
	response_proto_uint32(p, p->waitcmd, itm_id, 0);
	return 0;
}

/* @brief 查询美誉度
 */
int get_reputation_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_REPUTATION, p, 0, NULL, p->id);
}

/* @brief 对传过来的物品填充相应的信息
 */
int pkg_itm_flag_count_max(sprite_t* p, uint32_t itemid, int count, uint8_t *db_buf, uint32_t *index)
{
	item_t* itm = get_item_prop(itemid);
	item_kind_t* kind = find_kind_of_item(itemid);
	if ((itm == NULL) || (kind == NULL)) {
		ERROR_RETURN(("wrong item id %u %u", p->id, itemid), -1);
	}
	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	}
	PKG_H_UINT32(db_buf, flag, *index);
	PKG_H_UINT32(db_buf, itm->id, *index);
	PKG_H_UINT32(db_buf, count, *index);
	PKG_H_UINT32(db_buf, itm->max, *index);
	return 0;
}


/* @brief 把要送的物品打包成DB的114D的格式要求
 */
int give_sth(sprite_t *p, struct itm_add_info itm_count[], uint32_t length, uint8_t *db_buf, uint32_t *index)
{
	uint32_t i = 0, j = 0;
	PKG_H_UINT32(db_buf, 0, j);
	PKG_H_UINT32(db_buf, length, j);
	PKG_H_UINT32(db_buf, 0, j);
	PKG_H_UINT32(db_buf, 0, j);
	for (i = 0; i < length; i++) {
		pkg_itm_flag_count_max(p, itm_count[i].itm_id, itm_count[i].count, db_buf, &j);
	}
	*index = j;
	return 0;
}


/* @brief 查询美誉度的回调函数
 */
int get_reputation_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t cur_reputation = *(uint32_t *)buf;
	uint32_t total_reputation = *(uint32_t *)(buf + 4);
	response_proto_uint32_uint32(p, p->waitcmd, total_reputation, cur_reputation, 0);
	return 0;
}

/* @brief 根据美誉度赠送物品
 */
int give_something_accord_reputation_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_REPUTATION, p, 0, NULL, p->id);
}
