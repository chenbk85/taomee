/*
 * =====================================================================================
 *
 *       Filename:  pasture.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/11/2011 09:22:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <libtaomee/dataformatter/bin_str.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "small_require.h"
#include "central_online.h"
#include "mole_homeland.h"
#include "pasture.h"
#include "central_online.h"


//进入蒙牛牧场

int get_mongolia_cow_pasture_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_GET_MONGOLIA_COW_PASTURE, p, 0, NULL, p->id);
}
int get_mongolia_cow_pasture_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	typedef struct ret_pasture_info{
		uint32_t background;
		uint32_t milk_storage;
		uint32_t grass_count;
		uint32_t cow_count;
	} ret_pasture_t;

	typedef struct ret_grass_info{
		uint32_t grassid;
		uint32_t index;
	} ret_grass_t;

	typedef struct ret_cow_info{
		uint32_t id;
		uint32_t cowid;
		uint32_t hp;
		uint32_t mood;
		uint32_t clean;
		uint32_t is_milk;
		uint32_t eat_date;
	} ret_cow_t;
	DEBUG_LOG("=================================len:%u", len);

	CHECK_BODY_LEN_GE(len, sizeof(ret_pasture_t));

	ret_pasture_t *head = (ret_pasture_t *)buf;
	int offset = sizeof(protocol_t);
	PKG_UINT32(msg, head->background, offset);
	PKG_UINT32(msg, head->milk_storage, offset);
	PKG_UINT32(msg, head->grass_count, offset);
	uint32_t expect_len = sizeof(ret_grass_t) * head->grass_count + sizeof(ret_cow_t) *
		head->cow_count + sizeof(ret_pasture_t);
	CHECK_BODY_LEN(len, expect_len);
	char* pointer = buf + sizeof(ret_pasture_t);
	uint32_t k = 0, l = 0;
	ret_grass_t grass_info;
	for(; k < head->grass_count; ++k){
		UNPKG_H_UINT32(pointer, grass_info.grassid, l);
		UNPKG_H_UINT32(pointer, grass_info.index, l);

		PKG_UINT32(msg, grass_info.grassid, offset);
		PKG_UINT32(msg, grass_info.index, offset);
		DEBUG_LOG("grassid: %u   index:%u ==========", grass_info.grassid, grass_info.index);
	}

	PKG_UINT32(msg, head->cow_count, offset);
	ret_cow_t cow_info;
	for(k = 0; k < head->cow_count; ++k){
		UNPKG_H_UINT32(pointer, cow_info.id, l);
		UNPKG_H_UINT32(pointer, cow_info.cowid, l);
		UNPKG_H_UINT32(pointer, cow_info.hp, l);
		UNPKG_H_UINT32(pointer, cow_info.mood, l);
		UNPKG_H_UINT32(pointer, cow_info.clean, l);
		UNPKG_H_UINT32(pointer, cow_info.is_milk, l);
		UNPKG_H_UINT32(pointer, cow_info.eat_date, l);

		PKG_UINT32(msg, cow_info.id, offset);
		PKG_UINT32(msg, cow_info.cowid, offset);
		PKG_UINT32(msg, cow_info.hp, offset);
		PKG_UINT32(msg, cow_info.mood, offset);
		PKG_UINT32(msg, cow_info.clean, offset);
		PKG_UINT32(msg, cow_info.is_milk, offset);
	}
	 uint32_t msgbuff[2] = { id, 1 };
	 msglog(statistic_logfile, 0x0408B3F0, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);
}

//种植牧草
int plant_pasture_grass_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t grassid;
	uint32_t index;
	CHECK_BODY_LEN(len, sizeof(uint32_t) * 2);
	int offset = 0;
	UNPKG_UINT32(body, grassid, offset);
	UNPKG_UINT32(body, index, offset);
	uint32_t db_buf[] = { index, grassid };
	return send_request_to_db(SVR_PROTO_PLANT_PASTURE_GRASS, p, sizeof(db_buf), db_buf, p->id);

}
int plant_pasture_grass_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	typedef struct ret_grass_info{
		uint32_t grassid;
		uint32_t index;
	} ret_grass_t;

	CHECK_BODY_LEN_GE(len, sizeof(uint32_t));
	uint32_t grass_count = 0;
	uint32_t l = 0;
	int offset = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, grass_count, l);
	DEBUG_LOG("=================================count:%u", grass_count);
	PKG_UINT32(msg, grass_count, offset);

	uint32_t expect_len = grass_count * sizeof(ret_grass_t) + sizeof(uint32_t);
	CHECK_BODY_LEN(len, expect_len);

	uint32_t k = 0;
	ret_grass_t grass_info;
	for(; k < grass_count; ++k){

		UNPKG_H_UINT32(buf, grass_info.grassid, l);
		UNPKG_H_UINT32(buf, grass_info.index, l);
		PKG_UINT32(msg, grass_info.grassid, offset);
		PKG_UINT32(msg, grass_info.index, offset);
	}
	 uint32_t msgbuff[2] = { id, 1 };
	msglog(statistic_logfile, 0x0408B3D5, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);
}

//玩家与奶牛互动
int play_with_milk_cow_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);
	uint32_t id;
	uint32_t oper_id;
	CHECK_BODY_LEN(len, sizeof(uint32_t) * 2);

	uint32_t l = 0;
	UNPKG_UINT32(body, id, l);
	UNPKG_UINT32(body, oper_id, l);

	uint32_t db_buf[] = { id, oper_id };
	return send_request_to_db(SVR_PROTO_PLAY_WITH_MILK_COW, p, sizeof(db_buf), db_buf, p->id);
}
int play_with_milk_cow_callback(sprite_t *p, uint32_t id, char *buf, int len)
{

	typedef struct ret_cow_info{
		uint32_t id;
		uint32_t cowid;
		uint32_t hp;
		uint32_t mood;
		uint32_t clean;
		uint32_t is_milk;
		uint32_t eat_date;
		uint32_t milk_storage;
		uint32_t day_count;
		uint32_t oper_id;
	} ret_cow_t;

	CHECK_BODY_LEN(len, sizeof(ret_cow_t));
	ret_cow_t *cow_info = (ret_cow_t *)buf;
	uint32_t offset = sizeof(protocol_t);

	PKG_UINT32(msg, cow_info->id, offset);
	PKG_UINT32(msg, cow_info->cowid, offset);
	PKG_UINT32(msg, cow_info->hp, offset);
	PKG_UINT32(msg, cow_info->mood, offset);
	PKG_UINT32(msg, cow_info->clean, offset);
	PKG_UINT32(msg, cow_info->is_milk, offset);
	PKG_UINT32(msg, cow_info->milk_storage, offset);
	PKG_UINT32(msg, cow_info->day_count, offset);
	PKG_UINT32(msg, cow_info->oper_id, offset);


 	uint32_t msgbuff[2] = { id, 1 };
	if(cow_info->oper_id == 1){
		msglog(statistic_logfile, 0x0408B3D6, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if(cow_info->oper_id == 2){
		msglog(statistic_logfile, 0x0408B3D7, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if(cow_info->oper_id == 3){
		msglog(statistic_logfile, 0x0408B3D8, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);
}

//妙妙收购牛奶
int purchase_pasture_milk_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_PURCHASE_PASTURE_MILK, p, 0, NULL, p->id);

}
int purchase_pasture_milk_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(uint32_t) * 2);
	uint32_t milk_storage, day_count;
	uint32_t l = 0;
	uint32_t offset = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, milk_storage, l);
	PKG_UINT32(msg, milk_storage, offset);
	purchase_award_t awards;
	memset(&awards, 0, sizeof(purchase_award_t));
	UNPKG_H_UINT32(buf, day_count, l);
	if(day_count <= 3){
		awards.count = 2;
		awards.items[0].itemid = 1270044;
		awards.items[0].count = 1;
		awards.items[1].itemid = 190293;
		awards.items[1].count = 1;
		uint32_t msgbuff_1[2] = { id, 1 };
		msglog(statistic_logfile, 0x0408B3EF, get_now_tv()->tv_sec, msgbuff_1, sizeof(msgbuff_1));
	}
	else if(day_count > 3 && day_count <= 7){

		awards.count = 2;
		awards.items[0].itemid = 180073;
		awards.items[0].count = 1;
		awards.items[1].itemid = 0;
		awards.items[1].count = 500;
	}
	else{
		awards.count = 1;
		awards.items[0].itemid = 0;
		awards.items[0].count = 500;

	}
	PKG_UINT32(msg, awards.count, offset);
	uint32_t k =0;
	for(; k < awards.count; ++k){
		db_exchange_single_item_op(p, 202, awards.items[k].itemid, awards.items[k].count, 0);
		PKG_UINT32(msg, awards.items[k].itemid, offset);
		PKG_UINT32(msg, awards.items[k].count, offset);
	}
 	uint32_t msgbuff_2[2] = { id, 1 };
	msglog(statistic_logfile, 0x0408B3D9, get_now_tv()->tv_sec, msgbuff_2, sizeof(msgbuff_2));

	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);
}

