/*
 * =====================================================================================
 *
 *       Filename:  mole_ocean.c
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
#include "mole_ocean.h"

typedef struct{
	uint32_t shell;
	uint32_t coin;
}shell_coin_t;

shell_coin_t shell_coin[] = {
	{1633003, 1},
	{1633004, 3},
	{1633007, 10},
};

int get_user_ocean_info_cmd(sprite_t *p, uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);
	uint32_t index = 1, j = 0 ,userid = 0;

	UNPKG_UINT32(body, index, j);
	UNPKG_UINT32(body, userid, j);
	uint32_t dbbuf[] = {index, p->id};
	return send_request_to_db(SVR_PROTO_USER_GET_USER_OCEAN_INFO, p, 8, dbbuf, userid);
}
int get_user_ocean_info_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct ocean_info_head{
		uint32_t freshman;
		uint32_t coin;
		uint32_t cur_exp;
		uint32_t upgrade_need_exp;
		uint32_t level;
        uint32_t ground;
        uint32_t capacity;
		uint32_t fish_food[3];
		uint32_t shells;
        uint32_t count_1;
        uint32_t count_2;
	}ocean_info_head_t;

	typedef struct fish_info{
		uint32_t  index;
		uint32_t fishid;
		uint32_t breed;
		uint32_t birthday;
		uint32_t growth;
		int32_t state;
		uint32_t hungry;
		int32_t lifetime;
	}fish_info_t;

	typedef struct diy_info{
		uint32_t itemid;
		int32_t x;
		int32_t y;
		int32_t z;
	}diy_info_t;

	CHECK_BODY_LEN_GE(len, sizeof(ocean_info_head_t));
	
	ocean_info_head_t *phead = (ocean_info_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(ocean_info_head_t)+phead->count_1* sizeof(diy_info_t)
	    + phead->count_2*  sizeof(fish_info_t));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, phead->freshman, j);
	PKG_UINT32(msg, phead->coin, j);
	PKG_UINT32(msg, phead->cur_exp, j);
	PKG_UINT32(msg, phead->upgrade_need_exp, j);
	PKG_UINT32(msg, phead->level,j);
	PKG_UINT32(msg, phead->ground, j);
	PKG_UINT32(msg, phead->capacity, j);
	uint32_t i =0;
	for(; i < 3; ++i){
		PKG_UINT32(msg, phead->fish_food[i], j);
	}
	PKG_UINT32(msg, phead->shells, j);
	PKG_UINT32(msg, phead->count_1, j);
	PKG_UINT32(msg, phead->count_2, j);

	diy_info_t* pdiy= (diy_info_t*)(buf+sizeof(ocean_info_head_t));
	uint32_t k = 0;
	for(; k < phead->count_1;++k){
		PKG_UINT32(msg, pdiy->itemid, j);
		PKG_UINT32(msg, pdiy->x, j);
		PKG_UINT32(msg, pdiy->y, j);
		PKG_UINT32(msg, pdiy->z, j);
		++pdiy;
	}

	fish_info_t* pfish = (fish_info_t*)(buf+sizeof(ocean_info_head_t)+ phead->count_1* sizeof(diy_info_t));
	k = 0;
	for(; k < phead->count_2;++k){
		PKG_UINT32(msg, pfish->index, j);
		PKG_UINT32(msg, pfish->fishid, j);
		PKG_UINT32(msg, pfish->breed, j);
		PKG_UINT32(msg, pfish->birthday, j);
		PKG_UINT32(msg, pfish->growth, j);
		PKG_UINT32(msg, pfish->state, j);
		PKG_UINT32(msg, pfish->hungry, j);
		PKG_UINT32(msg, pfish->lifetime, j);
		++pfish;
	}
	pfish = (fish_info_t*)(buf+sizeof(ocean_info_head_t)+ phead->count_1* sizeof(diy_info_t));
	k = 0;
	for(; k < phead->count_2;++k){
		        DEBUG_LOG("index: %u, fishid: %u, breed: %u, birthday: %u, growth: %u, state: %u, hungry: %u,, lifetime: %u",
						pfish->index, pfish->fishid, pfish->breed, pfish->birthday, pfish->growth, pfish->state, pfish->hungry,
					pfish->lifetime);
		++pfish;
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
	
}

int put_user_ocean_animal_in_ocean_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t oceanid = 0;
	uint32_t ocean_anmialid = 0;
	UNPKG_UINT32(body,oceanid,j);
	UNPKG_UINT32(body,ocean_anmialid,j);
	item_t *itm = get_item_prop(ocean_anmialid);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t ocean_animal_type = itm->u.ocean_animal_tag.type;
    uint32_t dbbuf[] ={oceanid, ocean_anmialid, ocean_animal_type};
	return  send_request_to_db(SVR_PROTO_USER_PUT_OCEAN_ANIMAL_IN_OCEAN, p, sizeof(dbbuf), dbbuf, p->id);
}

int put_user_ocean_animal_in_ocean_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t index;
		uint32_t fishid;
		uint32_t breed;
		uint32_t birthday;
		uint32_t growth;
		int32_t fish_state; 
		uint32_t hungry;
		int32_t lifetime;
	}put_fish_t;
	
	CHECK_BODY_LEN(len, sizeof(put_fish_t));
	put_fish_t* head = (put_fish_t*)buf;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, head->state, i);
	PKG_UINT32(msg, head->index, i);
	PKG_UINT32(msg, head->fishid, i);
	PKG_UINT32(msg, head->breed, i);
	PKG_UINT32(msg, head->birthday, i);
	PKG_UINT32(msg, head->growth, i);
	PKG_UINT32(msg, head->fish_state, i);
	PKG_UINT32(msg, head->hungry, i);
	PKG_UINT32(msg, head->lifetime, i);

	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int user_feed_ocean_animal_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	int j = 0;
	uint32_t oceanid = 0;
	uint32_t index = 0;
	uint32_t itemid = 0;

	UNPKG_UINT32(body,oceanid,j);
	UNPKG_UINT32(body,index,j);
	UNPKG_UINT32(body,itemid,j);
	
	DEBUG_LOG("oceanid: %u, fish_index: %u, itemid: %u", oceanid, index, itemid);
	item_t *itm = get_item_prop(itemid);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if(itm->u.ocean_item_tag.type != 101){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	DEBUG_LOG("oceanid: %u, fish_index: %u, itemid: %u", oceanid, index, itemid);
	uint32_t value = itm->u.ocean_item_tag.value;
    uint32_t dbbuf[] ={oceanid, index, itemid, value};
	return  send_request_to_db(SVR_PROTO_USER_FEED_OCEAN_ANIMAL, p, sizeof(dbbuf), dbbuf, p->id);
}

int user_feed_ocean_animal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t state = 0;
	uint32_t hungry = 0;
	uint32_t is_upgrade = 0;
	uint32_t cur_exp = 0;
	uint32_t upgrade_need_exp =0;
	int j = 0;
	CHECK_BODY_LEN(len, 20);
	UNPKG_H_UINT32(buf,cur_exp,j);
	UNPKG_H_UINT32(buf,upgrade_need_exp,j);
	UNPKG_H_UINT32(buf,state,j);
	UNPKG_H_UINT32(buf,hungry,j);
	UNPKG_H_UINT32(buf,is_upgrade,j);

	DEBUG_LOG("cur_exp: %u, need_exp: %u, state: %u, hungry: %u", cur_exp, upgrade_need_exp, state, hungry);
	send_request_to_db(SVR_PROTO_OCEAN_SYNCHRO_EXP, NULL, 4, &cur_exp, p->id);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cur_exp, i);
	PKG_UINT32(msg, upgrade_need_exp, i);
	PKG_UINT32(msg, state, i);
	PKG_UINT32(msg, hungry, i);
	PKG_UINT32(msg, is_upgrade, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int user_use_ocean_tool_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	int j = 0;
	uint32_t oceanid = 0;
	uint32_t index = 0;
	uint32_t itemid = 0;
	UNPKG_UINT32(body,oceanid,j);
	UNPKG_UINT32(body,index,j);
	UNPKG_UINT32(body,itemid,j);
	
	item_t *itm = get_item_prop(itemid);
	if(!itm) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if(itm->u.ocean_item_tag.type != 2){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    uint32_t dbbuf[] ={oceanid, index, itemid};
	return  send_request_to_db(SVR_PROTO_USER_USE_OCEAN_TOOL, p, sizeof(dbbuf), dbbuf, p->id);
}

int user_use_ocean_tool_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t state = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,state,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, state, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int user_expand_ocean_capacity_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t oceanid = 0;
	UNPKG_UINT32(body,oceanid,j);
	return  send_request_to_db(SVR_PROTO_USER_EXPAND_OCEAN_CAPACITY, p, 4, &oceanid, p->id);
}

int user_expand_ocean_capacity_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_user_ocean_items_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_GET_OCEAN_ITEMS, p, 0, NULL, p->id);
}

int get_user_ocean_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 8);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t itemid = 0;
		uint32_t cnt = 0;
		UNPKG_H_UINT32 (buf, itemid, j);
		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, itemid, i);
		PKG_UINT32 (msg, cnt, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int user_diy_self_ocean_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	typedef struct{
		uint32_t itemid;
		int32_t x;
		int32_t y;
		int32_t z;
	}diy_ocean_t;

	typedef struct{
		uint32_t ocean_index;
		uint32_t groundback;
		uint32_t count;
		diy_ocean_t decorate[200];
	}decorate_t;
	
	CHECK_BODY_LEN_GE(bodylen, 12);
	uint32_t count = 0, ocean_index = 0, ground_back = 0;
	int i = 0;
	UNPKG_UINT32(body, ocean_index, i);
	UNPKG_UINT32(body, ground_back, i);
	UNPKG_UINT32(body, count, i);

	CHECK_BODY_LEN(bodylen, sizeof(diy_ocean_t)*count + 12);
	decorate_t diys;
	memset(&diys, 0, sizeof(decorate_t));
	diys.count = count;
	diys.ocean_index = ocean_index;
	diys.groundback = ground_back;

	int j = 0;
	for(; j < count; ++j){
		UNPKG_UINT32(body, diys.decorate[j].itemid, i);
		UNPKG_UINT32(body, diys.decorate[j].x, i);
		UNPKG_UINT32(body, diys.decorate[j].y, i);
		UNPKG_UINT32(body, diys.decorate[j].z, i);
	}
	
	return  send_request_to_db(SVR_PROTO_USER_DIY_SELF_OCEAN, p, sizeof(diy_ocean_t)*diys.count + 12, &diys, p->id);
}

int user_diy_self_ocean_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	int i = sizeof (protocol_t);
	PKG_UINT32(msg, *(uint32_t*)buf, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int user_get_offline_shells_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t oceanid = 0;
	int i = 0;
	UNPKG_UINT32(body, oceanid, i);

	return  send_request_to_db(SVR_PROTO_USER_GET_OFFLINE_SHELLS, p, 4, &oceanid, p->id);
}

int user_get_offline_shells_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 20);
	typedef struct{
		uint32_t cur_exp;
		uint32_t upgrade_need_exp;
		uint32_t oceanid;
		uint32_t shells;
		uint32_t is_upgrade;
	} offline_shell_t;

	offline_shell_t* head = (offline_shell_t*)buf;
	send_request_to_db(SVR_PROTO_OCEAN_SYNCHRO_EXP, NULL, 4, &head->cur_exp, p->id);
	int i = sizeof (protocol_t);
	PKG_UINT32(msg, head->cur_exp, i);
	PKG_UINT32(msg, head->upgrade_need_exp, i);
	PKG_UINT32(msg, head->oceanid, i);
	PKG_UINT32(msg, head->shells, i);
	PKG_UINT32(msg, head->is_upgrade, i);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);

}
int user_get_online_shells_cmd(sprite_t* p, uint8_t* body, int bodylen)
{

	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t oceanid = 0;
	UNPKG_UINT32(body, oceanid, i);

	return  send_request_to_db(SVR_PROTO_OCENA_GET_ONLINE_DROP_SHELLS, p, 4, &oceanid, p->id);
}
int user_get_online_shells_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t fish_index;
		uint32_t itemid;
		uint32_t count;
	}drop_item_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(drop_item_t)*count + 4);

	memset(&p->ocean_drop_item, 0, sizeof(uint32_t)*10);
	memset(&p->ocean_drop_count, 0, sizeof(uint32_t)*10);

	drop_item_t* head = (drop_item_t*)(buf+4);

	int i = sizeof (protocol_t);
	PKG_UINT32(msg, count, i);
	int k = 0, num = 0;
	for(; k < count; ++k){
		int m = 0;
		for(; m < 10; ++m){
			if(p->ocean_drop_item[m] == (head+k)->itemid){
				p->ocean_drop_count[m] += (head+k)->count;
				p->ocean_drop_item[m] = (head+k)->itemid;
				break;
			}
		}
		if(m >= 10){
			p->ocean_drop_count[num] = (head+k)->count;
			p->ocean_drop_item[num] = (head+k)->itemid;
			++num;
		}
		PKG_UINT32(msg, (head+k)->fish_index, i);
		PKG_UINT32(msg, (head+k)->itemid, i);
		PKG_UINT32(msg, (head+k)->count, i);
	}
	
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int user_pick_online_shells_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t oceanid = 0, itemid = 0;
	int j = 0;
	UNPKG_UINT32(body, oceanid, j);
	UNPKG_UINT32(body, itemid, j);
	DEBUG_LOG("itemid: %u ============", itemid);
	int n = 0;
	for(; n < 10; ++n){
		DEBUG_LOG("itemid; %u, count: %u", p->ocean_drop_item[n], p->ocean_drop_count[n]);
	}
	
	int k = 0;
	for(; k < 10; ++k){
		if(p->ocean_drop_item[k] == itemid && p->ocean_drop_count[k] != 0){
			 p->ocean_drop_count[k] -= 1;
			 break;
		}
	}
	if(k >= 10){
		itemid = 0;
	}
	DEBUG_LOG("itemid: %u ============", itemid);
	
	if(itemid != 0){
		//db_exchange_single_item_op(p, 202, itemid, 1, 0);
		int m = 0;
		for(;m < sizeof(shell_coin)/sizeof(shell_coin_t); ++m){
			if(shell_coin[m].shell == itemid){
				break;
			}
		}
		if(m < sizeof(shell_coin)/sizeof(shell_coin_t)){
			
			send_request_to_db(SVR_PROTO_ADD_OCEAN_COIN, NULL, 4, &shell_coin[m].coin, p->id);
		}
		
	}
	int i = sizeof (protocol_t);
	PKG_UINT32(msg, oceanid, i);
	PKG_UINT32(msg, itemid, i);
	

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int user_sale_ocean_fish_cmd(sprite_t *p, uint8_t* body, int bodylen)
{

	CHECK_BODY_LEN_GE(bodylen, 4);
	uint8_t db_buff[1024] = {0};
	uint32_t count = 0;
	int i = 0;
	UNPKG_UINT32(body, count, i);
	CHECK_BODY_LEN(bodylen, sizeof(uint32_t)*count + 4);
	int l = 0;
	PKG_H_UINT32(db_buff, count, l);

	uint32_t fish_index = 0;
	int k = 0;
	for(; k < count; ++k){
		UNPKG_UINT32(body, fish_index, i);
		PKG_H_UINT32(db_buff, fish_index, l);
	}

	return  send_request_to_db(SVR_PROTO_SALE_OCEAN_FISH, p, l, db_buff, p->id);
}

int user_sale_ocean_fish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len ,4);
	uint32_t shells = *(uint32_t*)buf;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, shells, i);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int user_buy_ocean_things_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t itemid = 0;
	int i = 0;
	UNPKG_UINT32(body, itemid, i);

	return  send_request_to_db(SVR_PROTO_BUY_OCEAN_THINGS, p, 4, &itemid, p->id);

}
int user_buy_ocean_things_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t leave_shell;
		uint32_t attireid;
		uint32_t count;
	}buy_ocean_t;
	CHECK_BODY_LEN(len, sizeof(buy_ocean_t));

	buy_ocean_t* cur = (buy_ocean_t*)buf;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cur->leave_shell, i);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int mole_get_ocean_adventure_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return send_request_to_db(SVR_PROTO_GET_OCEAN_ADVENTURE, p, 0, NULL, p->id);
}
int mole_get_ocean_adventure_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t already_pass;
		uint32_t count_1;
		uint32_t count_2;
	}pass_gate_t;
	typedef struct{
		uint32_t prime;
		uint32_t secondary;
	}gate_info_t;
	typedef struct{
		uint32_t prime;
		uint32_t day_count;
	}gate_play_t;

	CHECK_BODY_LEN_GE(len, sizeof(pass_gate_t));
	pass_gate_t* head = (pass_gate_t*)buf;
	DEBUG_LOG("count_1: %u, count_2: %u", head->count_1, head->count_2);
	CHECK_BODY_LEN(len, sizeof(gate_info_t)*head->count_1 + sizeof(gate_play_t)*head->count_2 + sizeof(pass_gate_t));

	gate_info_t* cur = (gate_info_t*)(buf+sizeof(pass_gate_t));

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, head->already_pass, i);
	PKG_UINT32(msg, head->count_1, i);
	//PKG_UINT32(msg, head->count_2, i);
	int k = 0;
	for(; k < head->count_1; ++k){
		PKG_UINT32(msg, (cur+k)->prime, i);
		PKG_UINT32(msg, (cur+k)->secondary, i);
	}
	PKG_UINT32(msg, head->count_2, i);
	gate_play_t* pointer = (gate_play_t*)(buf+sizeof(pass_gate_t)+ sizeof(gate_info_t)*head->count_1);
	int j = 0;
	for(; j < head->count_2; ++j){
		PKG_UINT32(msg, (pointer+j)->prime, i);
		PKG_UINT32(msg, (pointer+j)->day_count, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int mole_set_ocean_adventure_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	
	uint32_t now = time(0);
	if(now - p->adventure_interval < 1){
		p->adventure_interval = now;
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 3, i);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
		init_proto_head (msg, p->waitcmd, i);
		return send_to_self (p, msg, i, 1);
	}
	p->adventure_interval = now;
	int j = 0;
	UNPKG_UINT32(body, *(uint32_t*)p->session, j);
	UNPKG_UINT32(body, *(uint32_t*)(p->session+4), j);
	uint32_t db_type = 50027;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &db_type, p->id);
	
}
int mole_set_ocean_adventure_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t itemid;
		uint32_t count;
		uint32_t crystal_cnt;
	}set_adventure_t;

	CHECK_BODY_LEN(len, sizeof(set_adventure_t));
	set_adventure_t* cur = (set_adventure_t*)buf;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cur->state, i);
	PKG_UINT32(msg, cur->itemid, i);
	PKG_UINT32(msg, cur->count, i);
	
	uint32_t tear_count = 15;
	uint32_t flag = *(uint32_t*)(p->session+8);
	if(flag == 0){
		db_exchange_single_item_op(p, 99, 1351390, 30, 0);
		uint32_t day_buf[] = {50027, 20, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
	}
	else{
		tear_count = 0;
	}
	PKG_UINT32(msg, tear_count, i);
	PKG_UINT32(msg, cur->crystal_cnt, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);

}

int mole_get_ocean_handbook_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return send_request_to_db(SVR_PROTO_GET_OCEAN_HANDBOOK, p, 0, NULL, p->id);
}
int mole_get_ocean_handbook_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, count* 4 + 4);

	uint32_t *head = (uint32_t*)(buf+4);
	int i = sizeof(protocol_t);
	
	PKG_UINT32(msg, count, i);
	int k = 0;
	for(;k < count; ++k){
		PKG_UINT32(msg, *(head+k), i);
	}
	
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
	
}

int mole_get_friend_ocean_info_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	
	CHECK_BODY_LEN_GE(bodylen, 4);
	uint32_t count = 0;
	int  j = 0;
	UNPKG_UINT32( body, count, j );
	
	uint32_t friendid = 0;
	CHECK_BODY_LEN(bodylen, sizeof(friendid)*count + sizeof(count));
	
	uint8_t db_buf[1024];
	int sendlen = 0;
	PKG_H_UINT32( db_buf, count, sendlen );
	int k = 0;
	for(; k < count; ++k){
		UNPKG_UINT32( body, friendid, j);
		PKG_H_UINT32( db_buf, friendid, sendlen );
	}
	return send_request_to_db( SVR_PROTO_GET_OCEAN_FRIEND_LEVEL, p, sendlen, db_buf, p->id );
}
int mole_get_friend_ocean_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t friendid;
		uint32_t level;
	}friend_ocean_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = 0;
	count = *(uint32_t*)buf;

	CHECK_BODY_LEN(len, sizeof(friend_ocean_t)*count + 4);
	DEBUG_LOG("count: %u =================", count);
	friend_ocean_t *head = (friend_ocean_t*)(buf+4);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);

	int k = 0;
	for(; k < count; ++k){
		PKG_UINT32(msg, (head+k)->friendid, i);
		PKG_UINT32(msg, (head+k)->level, i);

	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}
