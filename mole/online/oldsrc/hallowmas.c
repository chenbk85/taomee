#include <stdlib.h>

#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee/utils.h>

#include "log.h"

#include "exclu_things.h"
#include "logic.h"
#include "login.h"
#include "message.h"
#include "proto.h"

#include "hallowmas.h"

typedef struct AddCandysReq {
    uint32_t isvip;
    uint32_t flag;       // flag to be `OR'ed into DB
    int32_t  increment;  // candy increment
} __attribute__((packed)) add_candys_req_t;

typedef struct AddCandysRet {
    uint32_t flag;
    int32_t  actual_increment;
    uint32_t total_candys;
} __attribute__((packed)) add_candys_ret_t;

typedef struct ChkCandysRet {
	uint32_t flag;
    uint32_t task;
    uint32_t total_candys;
} __attribute__((packed)) chk_candys_ret_t;

//-----------------------------------------------------------------------
// DB Operations
/**
 * db_add_candys - add candys
 * @flag: flag to be `OR'ed into DB
 *
 * return: 0 if succeeded, -1 otherwise
 */
int db_add_candys(sprite_t* p, userid_t uid, int increment, int flag, int ret_needed)
{
	add_candys_req_t req;

	req.isvip     = (p)?ISVIP(p->flag):0;
	req.flag      = flag;
	req.increment = increment;

	return send_request_to_db(SVR_PROTO_ADD_CANDYS, ret_needed ? p : 0, sizeof req, &req, uid);
}

//-----------------------------------------------------------------------
// Callbacks
/*int add_candys_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	CHECK_BODY_LEN(len, sizeof(add_candys_ret_t));

	const add_candys_ret_t* ret = (void*)buf;

	if (p->id == id) {
		p->tmpinfo.total_candys = ret->total_candys;
		p->tmpinfo.flag         = ret->flag;
	}

	switch (p->waitcmd) {
	case PROTO_GET_CANDYS_WEEKLY:
		response_proto_uint32(p, p->waitcmd, ret->actual_increment, 0);
		break;
	case PROTO_GAME_SCORE:
	{
		int i = sizeof(protocol_t);
		uint32_t* pInt = (void*)(p->session + i + 32);
		uint32_t  gid  = *(uint32_t*)(p->session + i);
		if (ret->actual_increment == 0) {
			pInt[5] = 0;
		}
		PKG_UINT32(p->session, gid, i);
		db_attr_op(p, pInt[4], pInt[0], pInt[1], pInt[2], pInt[3], ATTR_CHG_play_game, gid);
		i += 28;
		PKG_UINT32(p->session, pInt[0], i);
		PKG_UINT32(p->session, pInt[1], i);
		PKG_UINT32(p->session, pInt[2], i);
		PKG_UINT32(p->session, pInt[3], i);
		PKG_UINT32(p->session, pInt[4], i);
		break;
	}
	case PROTO_LAHM_GET_CANDYS:
		if (!((ret->flag >> 16) & p->tmpinfo.flag)) {
			int flag = 1;
			uint32_t candy_num = *(uint32_t*)(p->session);
			p->sess_len = sizeof(protocol_t);

			if (ret->total_candys == 0) {
				PKG_UINT32(p->session, 2, p->sess_len);
				PKG_UINT16(p->session, p->tmpinfo.flag, p->sess_len);
			} else {
				p->tmpinfo.flag |= (ret->flag >> 16);
				if (ret->total_candys < candy_num) {
					PKG_UINT32(p->session, 1, p->sess_len);
				} else {
					flag = 0;
					PKG_UINT32(p->session, 0, p->sess_len);
				}
				PKG_UINT16(p->session, p->tmpinfo.flag, p->sess_len);
				db_add_candys(p, p->id, 0, p->tmpinfo.flag, 0);
				db_add_candys(0, id, -candy_num, 0, 0);
			}
			init_proto_head(p->session, p->waitcmd, p->sess_len);
			if (flag) {
				send_postcard(p->nick, p->id, id, 1000038, "你没有准备好糖果哦！我家的拉姆生气了，给你一点小惩罚，你也不要生气啦！快摆出糖盒，多多准备糖果吧！", 0);
				return db_get_home_i(p, id);
			} else {
				char buf[192];
				snprintf(buf, sizeof buf, "我家的拉姆刚才来过了，吃了你的糖盒中%u块糖。小拉姆很高兴哦，说你家的糖果特别美味！真是太谢谢你啦！", candy_num);
				send_postcard(p->nick, p->id, id, 1000037, buf, 0);
				send_to_self(p, p->session, p->sess_len, 1);
				p->sess_len = 0;
				return 0;
			}
		}
		ERROR_RETURN(("%u had already got this candy: %u %u", p->id, p->tmpinfo.flag, ret->flag), -1);

	case PROTO_GET_CANDYS:
	{
		response_proto_uint32(p, p->waitcmd, ret->actual_increment, 0);
		break;
	}
	default:
		break;
	}
	
	return 0;
}

int chk_candys_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(chk_candys_ret_t));

	const chk_candys_ret_t* ret = (void*)buf;
	if (p->id == id) {
		p->tmpinfo.total_candys = ret->total_candys;
		p->tmpinfo.task         = ret->task;
		p->tmpinfo.flag         = ret->flag;
		//DEBUG_LOG("%u %u %u %u", p->id, ret->total_candys, ret->task, ret->flag);
	}

	switch (p->waitcmd) {
	case PROTO_LOGIN:
		return proc_final_login_step(p);
	case PROTO_CHK_CANDY_NUM:
	{
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, id, i);
		PKG_UINT16(msg, ((ret->flag) >> 16), i);
		PKG_UINT16(msg, ret->total_candys, i);
		init_proto_head(msg, p->waitcmd, i);
		send_to_self(p, msg, i, 1);
		break;
	}
	default:
		break;
	}

	return 0;
}*/

//-----------------------------------------------------------------------
// Cmds
int chk_candy_num_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	userid_t uid;
	if (unpkg_uint32(body, bodylen, &uid) == 0) {
		CHECK_VALID_ID(uid);

		return db_chk_candys(p, uid);
	}

	return -1;
}

int get_candys_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t num;
	if (unpkg_uint32(body, bodylen, &num) == 0) {
		return db_add_candys(p, p->id, 1, 0, 1);
	}
	return -1;
}


int get_candy_can_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	static int pos[]
				= { 27, 28, 29, 30, 31, 32 };
	static uint32_t itms[]
				= { 160095, 160096, 160097, 160098, 160099, 160100 };

	if ( !((p->tmpinfo.task) & 0x3) ) {
		// start the task of collecting candys
		p->tmpinfo.task = set_bit_on32(p->tmpinfo.task, 1);		
		db_set_hallowmas_task(p, &(p->tmpinfo.task));

		int idx = rand() % array_elem_num(pos);
		// clear mole cup's flag
		p->flag = p->flag & ~0xF8000000;
		// set color of can selected
		p->flag = set_bit_on32(p->flag, pos[idx]);
		db_set_user_flag(p, set_bit_on32(p->flag, 1));
		do_buy_item(p, itms[idx], 1, 1, 0);
		// set color of candy collected
		//p->tmpinfo.flag = set_bit_on32(p->tmpinfo.flag, idx + 1);
		//p->tmpinfo.flag = set_bit_on32(p->tmpinfo.flag, idx + 16 + 1);
		//db_add_candys(p, p->id, 0, p->tmpinfo.flag, 0);

		DEBUG_LOG("GET CANDY CAN\t[uid=%u itm=%u]", p->id, itms[idx]);
		response_proto_uint32(p, p->waitcmd, itms[idx], 0);
		return 0;
	}

	ERROR_RETURN( ("Already Got A Candy Can: uid=%u", p->id), -1);
}

int get_candys_weekly_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t done[] = { 1100000001,
						1100000002, 1100000003,	1100000004,
						1100000005, 1100000006, 1100000007, 1100000008,
						1100000009, 1100000010, 1100000011, 1100000012, 1100000013 };

	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 1);

	uint8_t type = *body;
	CHECK_INT_LE(type, 12);

//	if ((type > 1) && !ISVIP(p->flag)) {
//		ERROR_RETURN(("%u is not a vip!", p->id), -1);
//	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session, type, p->sess_len);
	DEBUG_LOG("GET CANDYS WEEKLY\t[uid=%u type=%d]", p->id, type);
	return db_set_sth_done(p, done[type], 1, p->id);
}
/*
int get_golden_key_weekly_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t done[] = { 1200000001,	1200000002};

	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 1);

	uint8_t type = *body;
	CHECK_INT_LE(type, 1);
	DEBUG_LOG("GET %s KEY WEEKLY\t[uid=%u type=%d]", type?"SILVER":"GOLDEN", p->id, type);
	return db_set_sth_done(p, done[type], 1);
}
*/
int upgrade_candy_can_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	int i = 0;

	PKG_H_UINT32(msg, 1, i); // del cnt
	PKG_H_UINT32(msg, 1, i); // add cnt

	if (p->flag & 0xFC000000) {
		if (test_bit_on32(p->flag, 27)) {
			PKG_H_UINT32(msg, 160095, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 160101, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 1, i);
		} else if (test_bit_on32(p->flag, 28)) {
			PKG_H_UINT32(msg, 160096, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 160102, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 1, i);
		} else if (test_bit_on32(p->flag, 29)) {		
			PKG_H_UINT32(msg, 160097, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 160103, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 1, i);
		} else if (test_bit_on32(p->flag, 30)) {
			PKG_H_UINT32(msg, 160098, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 160104, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 1, i);
		} else if (test_bit_on32(p->flag, 31)) {
			PKG_H_UINT32(msg, 160099, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 160105, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 1, i);
		} else if (test_bit_on32(p->flag, 32)) {
			PKG_H_UINT32(msg, 160100, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 160106, i);
			PKG_H_UINT32(msg, 1, i);
			PKG_H_UINT32(msg, 1, i);
		}

		// update candy can
		db_swap_home_itm(0, p->id, msg, i);
		// set task finished
		p->tmpinfo.task = set_bit_off32(p->tmpinfo.task, 1);
		p->tmpinfo.task = set_bit_on32(p->tmpinfo.task, 2);
		db_set_hallowmas_task(p, &(p->tmpinfo.task));

		response_proto_head(p, p->waitcmd, 0);
	}else {
		ERROR_RETURN(("You Have No Candy Can! uid=%u", p->id), -1);
	}

	return 0;
}

int get_pumpkin_lamp_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	if ( !((p->tmpinfo.task) & 0xC) ) {
		// start the task of collecting 6 different-colored candys
		p->tmpinfo.task = set_bit_on32(p->tmpinfo.task, 3);
		db_set_hallowmas_task(p, &(p->tmpinfo.task));
		do_buy_item(p, 12252, 1, 1, 0);

		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}

	ERROR_RETURN( ("Already Got A Pumpkin Lamp: uid=%u", p->id), -1);
}

int lahm_get_candys_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t uid, cnt;
	if (unpkg_uint32_uint32(body, bodylen, &uid, &cnt) == 0) {
		CHECK_VALID_ID(uid);
		if (cnt < 11 && cnt > 0) {
			p->sess_len = 0;
			PKG_H_UINT32(p->session, cnt, p->sess_len);
			return db_add_candys(p, uid, 0, 0, 1);
		}
		ERROR_RETURN(("Candy Num Too Large: uid=%u num=%u", p->id, cnt), -1);
	}

	return -1;
}

int fetch_candy_dress_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	/*
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t sex;
	int i = 0;
	UNPKG_UINT32(body, sex, i);
	if ((p->tmpinfo.flag & 0xFFFF) == 0x3F) {
		// ends the task of collecting 6 different-colored candys
		p->tmpinfo.task = set_bit_off32(p->tmpinfo.task, 3); // no task 
		p->tmpinfo.task = set_bit_on32(p->tmpinfo.task, 4);  // task was done
		db_set_hallowmas_task(p, &(p->tmpinfo.task));
		if(sex){
			do_exchange_item(p, 1801, 0);
		}
		else{
			do_exchange_item(p, 1800, 0);
		}

		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	
	ERROR_RETURN(("%u not yet collects all the 6 color of candys", p->id), -1);
	*/
	return 0;
}

int become_corpse_bat_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;
	uint32_t corpse_bat = 0;
	
	if(p->followed){
		corpse_bat = 1;
		PET_RESET_FORM_BAT(p->followed);
	}
	p->action = 0;
	
	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, corpse_bat, i);
	init_proto_head(msg, PROTO_CORPSE_BAT_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("CORPSE EXPIRED\t[%u %d]", p->id, p->action);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int become_corpse_bat_cmd (sprite_t *p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t corpse_bat;
	int j = 0;
	UNPKG_UINT32(body, corpse_bat, j);

	DEBUG_LOG("BECOME CORPSE\tpid=%u corpse=%u", corpse_bat);
	if(corpse_bat){ // 1 , change lamu to bat, and change moer to corpse
		if(!p->followed)
			ERROR_RETURN(("no pet follow %u", p->id), -1);
		PET_SET_FORM_BAT(p->followed);
		db_single_item_op(p, p->id, 190052, 1, 0);
	}
	
	uint32_t itmid = (rand()%2)?18000:18001; // only for hallowmas
	p->action = itmid;     //change moer to corpse

	add_event(&(p->timer_list), become_corpse_bat_expeired, p, 0, get_now_tv()->tv_sec + 300, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, corpse_bat, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	
	return 0;
}


