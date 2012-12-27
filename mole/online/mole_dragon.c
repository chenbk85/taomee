/*
 * =====================================================================================
 *
 *       Filename:  mole_dragon.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/05/2010 10:18:02 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <statistic_agent/msglog.h>
#include<libtaomee/project/utilities.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include "proto.h"
#include "util.h"
#include "exclu_things.h"
#include "communicator.h"
#include "small_require.h"
#include "mole_dragon.h"

/* @brief 检查ID号是否合法
 * @param exist_id 保存合法的ID号
 * @param array_len exist_id数组的长度
 * @param id需要检查的ID号是否在exist_id数组中
 * @return 0表示ID不合法，1表示ID合法
 */
static int check_id(uint32_t exist_id[], int array_len, uint32_t id)
{
	int i = 0;
	for (i = 0; i < array_len; i++) {
		if (id == exist_id[i]) {
			break;
		}
	}

	if (i == array_len) {
		return 0;
	}

	return 1;
}

/* @brief 得到飞龙背包里龙的信息
 */
int get_dragon_bag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(0, bodylen);
	return send_request_to_db(SVR_PROTO_GET_DRAGON_BAG, p, 0, NULL, p->id);
}

/* @brief 得到飞龙背包的数据，返回给客户端
 */
int get_dragon_bag_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t count = 0;
	CHECK_BODY_LEN_GE(len, sizeof(count));

	unpkg_host_uint32((uint8_t *)buf, &count);
	CHECK_BODY_LEN(len, sizeof(count) + count * sizeof(mole_dragon_info_t));

	uint8_t cli_buf[PAGESIZE] = { };
	int i = 0;
	int index = 0;
	mole_dragon_info_t *info = (mole_dragon_info_t *)(buf + sizeof(count));
	/*检查用户是否消费了1万摩尔豆，扩展飞龙背包, 第15个bit位表示是否扩展飞龙背包*/
	uint32_t expand_flag = (p->only_one_flag[0] & (1 << (15 - 1)));
	PKG_UINT32(cli_buf, expand_flag, index);
	PKG_UINT32(cli_buf, count, index);
	for (i = 0; i < count; i++) {
		PKG_UINT32(cli_buf, info->id, index);
		PKG_STR(cli_buf, info->name, index, DRAGON_NICK_LEN);
		PKG_UINT32(cli_buf, info->growth, index);
		PKG_UINT32(cli_buf, info->state, index);
		info++;
	}
	response_proto_str(p, p->waitcmd, index, cli_buf, 0);
	return 0;
}

/* @召唤龙（把冻结的龙解封，把召唤的龙设置为未召唤，把未召唤的设置为召唤)
 */
int call_dragon_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, sizeof(struct call_dragon_in));
	struct call_dragon_in in_data;
	unpack(body, sizeof(struct call_dragon_in), "LL", &in_data.id, &in_data.set_state);
	if (in_data.set_state != NOT_CALL_STATE && in_data.set_state != CALL_STATE) {
		ERROR_RETURN(("STATE WRONG %u %u", p->id, in_data.set_state), -1);
	}
	memcpy(p->session, &in_data, sizeof(in_data));
	return send_request_to_db(SVR_PROTO_CALL_DRAGON, p, sizeof(in_data), &in_data, p->id);
}

/* @brief 召唤龙，返回数据给客户端，并且保存数据sprite_t结构体中
 */
int call_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t growth = 0;
	unpkg_host_uint32((uint8_t *)buf, &growth);
	struct call_dragon_in session_data;
	memcpy(&session_data, p->session, sizeof(session_data));
	int i = sizeof(protocol_t);
	PKG_UINT32 (msg, p->id, i);
	PKG_UINT32 (msg, session_data.id, i);
	PKG_UINT32 (msg, growth, i);
	PKG_UINT32 (msg, session_data.set_state, i);
	init_proto_head(msg, p->waitcmd, i);
	if (session_data.set_state == CALL_STATE) {
		/*重新召唤龙，更新缓存*/
		p->dragon_id = session_data.id;
		p->dragon_growth = growth;
	} else {
		if (session_data.id == p->dragon_id) {
			/*取消召唤龙，更新缓存*/
			p->dragon_id = 0;
			p->dragon_growth = 0;
			memset(p->dragon_name, 0, DRAGON_NICK_LEN);
		}
	}
	send_to_map (p, msg, i, 1);
	return 0;
}

/* @brief 放生龙
 */
int release_dragon_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t dragon_id = 0;
	CHECK_BODY_LEN(bodylen, sizeof(dragon_id));
	unpack(body, sizeof(dragon_id), "L", &dragon_id);
	if (dragon_id == 1350002 || dragon_id == 1350004 || dragon_id == 1350009) {
		ERROR_RETURN(("THIS DRAGON CANNOT RELIEASE %u %u", p->id, dragon_id), 0);
	}
	*(uint32_t *)p->session = dragon_id;
	return send_request_to_db(SVR_PROTO_RELEASE_DRAGON, p, sizeof(dragon_id), &dragon_id, p->id);
}

/* @brief 龙放生的回调函数
 */
int release_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	CHECK_BODY_LEN(0, len);
	uint32_t dragon_id = *(uint32_t *)p->session;
	if (dragon_id == p->dragon_id) {
		p->dragon_id = 0;
		p->dragon_growth = 0;
	}
	int i = sizeof(protocol_t);
	PKG_UINT32 (msg, p->id, i);
	PKG_UINT32 (msg, dragon_id, i);
	PKG_UINT32 (msg, 0, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

/* @brief 给龙命名
 */
int set_dragon_name_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, sizeof(struct dragon_id_name));
	struct dragon_id_name in_data;
	unpack(buf, sizeof(in_data.dragon_id), "L", &in_data.dragon_id);
	memcpy(in_data.name, buf + sizeof(in_data.dragon_id), DRAGON_NICK_LEN);
	if(tm_dirty_check(7, (char*)(buf + sizeof(in_data.dragon_id))) > 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_dirty_word, 1);
	}
	memcpy(p->dragon_name, in_data.name, DRAGON_NICK_LEN);
	return send_request_to_db(SVR_PROTO_SET_DRAGON_NAME, p, sizeof(struct dragon_id_name), &in_data, p->id);
}

/* @brief 给龙起名字的回调函数
 */
int set_dragon_name_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	CHECK_BODY_LEN(0, len);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 给龙喂食
 */
int feed_dragon_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	struct feed_dragon_in feed_info;
	CHECK_BODY_LEN(bodylen, sizeof(feed_info.dragon_id) + sizeof(feed_info.item_id));
	unpack(buf, sizeof(struct feed_dragon_in), "LL", &feed_info.dragon_id, &feed_info.item_id);
	uint32_t feed_id[] = {190673, 190688};
	int array_len = (sizeof(feed_id) / sizeof(feed_id[0]));
	if (!check_id(feed_id, array_len, feed_info.item_id)) {
		ERROR_RETURN(("FEED ID IS WRONG %u %u", p->id, feed_info.item_id), -1);
	}
	return send_request_to_db(SVR_PROTO_FEED_DRAGON, p, sizeof(struct feed_dragon_in), &feed_info, p->id);
}

/* @brief 喂龙的回调函数
 */
int feed_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t dragon_id = 0;
	uint32_t dragon_growth = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &dragon_id, &dragon_growth);
	response_proto_uint32_uint32(p, p->waitcmd, dragon_id, dragon_growth, 0);
	if (dragon_id == p->dragon_id) {
		p->dragon_growth = dragon_growth;
	}
	return 0;
}

/* @brief 龙孵蛋
 */
int brood_dragon_egg_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	uint32_t dragon_egg_id = 0;
	unpack(buf, sizeof(dragon_egg_id), "L", &dragon_egg_id);
	uint32_t dragon_eggs[] = {190681, 190690, 190708, 190709, 190710, 190711, 
	                            190712, 190713, 190714, 190715, 190716};
	int array_len = (sizeof(dragon_eggs) / sizeof(dragon_eggs[0]));
	if (!check_id(dragon_eggs, array_len, dragon_egg_id)) {
		ERROR_RETURN(("WRONG DRAGON ID %u %u", p->id, dragon_egg_id), -1);
	}
	return send_request_to_db(SVR_PROTO_BROOD_DRAGON_EGG, p, sizeof(dragon_egg_id), &dragon_egg_id, p->id);
}

/* @brief  龙孵蛋的回调函数
 */
int brood_dragon_egg_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	struct brood_egg {
		uint32_t egg_id;
		uint32_t remain_time;
	}__attribute__((packed)) info;

	CHECK_BODY_LEN(len, sizeof(info));
	unpkg_host_uint32_uint32((uint8_t *)buf, &info.egg_id, &info.remain_time);
	response_proto_uint32_uint32(p, p->waitcmd, info.egg_id, info.remain_time, 0);
	return 0;
}

/* @brief 加速孵蛋时间
 */
int accelerate_dragon_brood_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	struct item_id_time {
		uint32_t id;
		uint32_t add_time;
	}__attribute__((packed)) info;

	CHECK_BODY_LEN(bodylen, sizeof(info.id));
	unpack(buf, sizeof(info.id), "L", &info.id);

	switch(info.id) {
		case 190674:
			info.add_time = 20 * 60;
			break;

		case 190685:
			info.add_time = 90 * 60;
			break;

		default:
			ERROR_RETURN(("WRONG ID %u %u", p->id, info.id), -1);
	}
	memcpy(p->session, &info, sizeof(info));
	return send_request_to_db(SVR_PROTO_QUICK_BROOD_EGG, p, sizeof(info), &info, p->id);
}

/* @brief 加速孵蛋时间回调函数，返回已经孵化了多长时间
 */
int accelerate_dragon_brood_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t remain_time = 0;
	CHECK_BODY_LEN(len, sizeof(remain_time));
	unpkg_host_uint32((uint8_t *)buf, &remain_time);
	uint32_t itm_id = 0;
	uint32_t add_time = 0;
	unpkg_host_uint32_uint32(p->session, &itm_id, &add_time);
	response_proto_uint32_uint32_uint32(p, p->waitcmd, itm_id, remain_time, add_time, 0);
	return 0;
}

/* @brief 领取幼龙
 */
int get_baby_dragon_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(0, bodylen);
	return send_request_to_db(SVR_PROTO_TAKE_LITTLE_DRAGON, p, 0, NULL, p->id);
}

/* @brief 领取龙的回调函数
 */
int get_baby_dargon_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t dragon_id = 0;
	uint32_t egg_id = 0;
	CHECK_BODY_LEN(len, sizeof(dragon_id) + sizeof(egg_id));
	unpkg_host_uint32_uint32((uint8_t *)buf, &dragon_id, &egg_id);
	response_proto_uint32_uint32(p, p->waitcmd, dragon_id, egg_id, 0);
	return 0;
}

/* @brief 获取孵蛋的时间
 */
int get_brood_dragon_egg_time_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(0, bodylen);
	return send_request_to_db(SVR_PROTO_GET_BROOD_EGG_TIME, p, 0, NULL, p->id);
}

/* @brief 获取孵蛋时间的回调函数
 */
int get_brood_dragon_egg_time_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t dragon_egg_id = 0;
	uint32_t remain_time = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &dragon_egg_id, &remain_time);
	response_proto_uint32_uint32(p, p->waitcmd, dragon_egg_id, remain_time, 0);
	return 0;
}

/* @brief 得到召唤龙的信息
 */
int get_call_dragon_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(0, bodylen);
	return send_request_to_db(SVR_PROTO_GET_CALLED_DRAONG_INFO, p, 0, NULL, p->id);
}

/* @brief 得到召唤龙的信息回调函数,并且把信息保存到sprite_t结构体中
 */
int get_call_dragon_info_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	struct call_dragon_info {
		uint32_t dragon_id;
		uint8_t	dragon_name[DRAGON_NICK_LEN];
		uint32_t growth;
	}__attribute__((packed));

	uint8_t cli_buf[PAGESIZE];
	struct call_dragon_info *info = (struct call_dragon_info *)buf;
	int index = 0;
	PKG_UINT32(cli_buf, info->dragon_id, index);
	p->dragon_id = info->dragon_id;
	PKG_STR(cli_buf, info->dragon_name, index, DRAGON_NICK_LEN);
	memcpy(p->dragon_name, info->dragon_name, DRAGON_NICK_LEN);
	PKG_UINT32(cli_buf, info->growth, index);
	p->dragon_growth = info->growth;
	response_proto_str(p, p->waitcmd, index, cli_buf, 0);
	return 0;
}

/* @brief 判断用户带在身边的龙是否成年
 * @reutrn 1表示成年，0表示未成年
 */
int is_dragon_adult(sprite_t *p)
{
    uint32_t dragon_id = p->dragon_id;
    switch (dragon_id)
    {
        case 1350001:
        {
            return (p->dragon_growth >= 1000) ? 1 : 0;
        }
        case 1350002:
        {
            return (p->dragon_growth >= 10000) ? 1 : 0;
        }
        case 1350003:
        {
            return (p->dragon_growth >= 7000) ? 1 : 0;
        }
        case 1350004:
        case 1350005:
        case 1350006:
        {
            return (p->dragon_growth >= 10000) ? 1 : 0;
        }
        case 1350007:
        case 1350008:
        case 1350043:
        {
            return (p->dragon_growth >= 12000) ? 1 : 0;
        }
        case 1350009:
        case 1350010:
        {
            return (p->dragon_growth >= 15000) ? 1 : 0;
        }
        default:
        {
            return 0;
        }
    }
}

/* @brief 扩充飞龙背包
 */
int expand_dragon_bag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	/*是否有足够的摩尔豆*/
	if (p->yxb < 10000) {
		return send_to_self_error(p, p->waitcmd, -ERR_mibi_not_enough_money, 1);
	}

	/*第15bit位表示是否扩充了飞龙背包*/
	if (!set_only_one_bit(p, 15)) {
		ERROR_RETURN(("YOU HAVE EXPAND DRAGON BAG %u", p->id), 0);
	}
	/*扣除10000摩尔豆*/
	uint32_t db_buf[] = {1, 0, 0, 0, 0, 0, 10000};
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
	/*更新缓存*/
	p->yxb -= 10000;
	uint32_t add_count = 1;
   	msglog(statistic_logfile, 0x02100100, get_now_tv()->tv_sec, &add_count, sizeof(add_count));
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int train_kaku_nian_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(0, bodylen);

	return send_request_to_db(SVR_PROTO_GET_TRAIN_KAKU_NIAN_INFO, p, 0, NULL, p->id);
}

int get_train_kaku_nian_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(0, bodylen);
    return send_request_to_db(SVR_PROTO_GET_TRAIN_KAKU_NIAN_INFO, p, 0, NULL, p->id);
}


int get_train_kaku_nian_info_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
    uint32_t train_count = 0;

	unpkg_host_uint32((uint8_t *)buf, &train_count);

	if (p->waitcmd == PROTO_GET_TRAIN_KAKU_NIAN_INFO)
	{
	    response_proto_uint32(p, p->waitcmd, train_count, 0);
	    return 0;
	}

	int rand_value = rand()%10;
    if (rand_value >= 3)
    {
        uint32_t flag = 0;
        *(uint32_t *)p->session = flag;

        if (!train_count)
        {
            response_proto_uint32(p, p->waitcmd, 0, 0);

            send_request_to_db(SVR_PROTO_ADD_TRAIN_KAKU_NIAN_COUNT, NULL, sizeof(flag), &flag, p->id);
	        return 0;
        }
        else
        {
            send_request_to_db(SVR_PROTO_ADD_TRAIN_KAKU_NIAN_COUNT, NULL, sizeof(flag), &flag, p->id);
            uint32_t db_buf[7] = {1, 0, 0, 0, 0, 16012, 5};
	        return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
	    }
    }
    else
    {
        uint32_t flag = 1;
        *(uint32_t *)p->session = flag;

	    if (!train_count)
	    {
	        uint32_t db_buf[11] = {1, 1, 0, 0, 0, 190836, 1, 27, 1350012, 1, 1};
	        return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
	    }
	    else
	    {
	        uint32_t db_buf[14] = {2, 1, 0, 0, 0, 190836, 1, 0, 16012, 5, 27, 1350012, 1, 1};
	        return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
	    }
	}


}

int change_ice_dragon_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(0, bodylen);
	return send_request_to_db(SVR_PROTO_USER_CHANGE_ICE_DRAGON, p, 0, NULL, p->id);
}

int change_ice_dragon_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	p->dragon_id = 0;
	p->dragon_growth = 0;
	memset(p->dragon_name, 0, DRAGON_NICK_LEN);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}



