
#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "login.h"
#include "communicator.h"
#include "pet_item.h"
#include "lamu_skill.h"
#include "lahm_sport.h"
#include <statistic_agent/msglog.h>
#include "tasks.h"
#include "street.h"
#include "small_require.h"

#define db_lahm_sbm_rate_score(p_, buf) \
		send_request_to_db(SVR_PROTO_LAHM_SBM_RACE_SCORE, p_, 44, buf, (p_)->id)
#define db_lahm_get_score_list(p_) \
		send_request_to_db(SVR_PROTO_LAHM_GET_SCORE_LIST, p_, 0, NULL, (p_)->id)
#define db_lahm_list_team_score(p_) \
		send_request_to_db(SVR_PROTO_LAHM_LIST_TEAM_SCORE, p_, 0, NULL, (p_)->id)
#define db_lahm_get_best_score(p_, buf) \
		send_request_to_db(SVR_PROTO_LAHM_GET_BEST_SCORE, p_, 4, buf, (p_)->id)
#define db_lahm_get_team_madels(p_, buf) \
		send_request_to_db(SVR_PROTO_LAHM_GET_TEAM_MADELS, p_, 4, buf, (p_)->id)


int db_lahm_sign_up(sprite_t* p, int team, uint32_t petid)
{
	uint32_t buf[2] = {petid, team};
	return send_request_to_db(SVR_PROTO_LAHM_SIGN_UP, p, 8, buf, p->id);
}

int sign_up_sport_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	uint32_t hats_itm[4] = {1200001, 1200002, 1200003, 1200004};
	uint32_t medals_itm[4] = {1210006, 1210007, 1210008, 1210009};

	if (!p->followed) {
		ERROR_RETURN(("no pet follow uid=%u", p->id), -1);
	}

	uint32_t team_type;
	int i = 0;
	UNPKG_UINT32(body, team_type, i);

	if (team_type > 3){
		ERROR_RETURN(("error team uid=%u petid=%u team=%u", p->id, p->followed->id, team_type), -1);
	}


	DEBUG_LOG("BUY PET ITEM\t[buyer=%u petid=%u itemid=%u count=%u]", p->id, p->followed->id, hats_itm[team_type], medals_itm[team_type]);
	db_single_pet_item_op(0, p->followed->id, hats_itm[team_type], 1, 1, p->id);
	db_single_pet_item_op(0, p->followed->id, medals_itm[team_type], 1, 1, p->id);

	return db_lahm_sign_up(p, hats_itm[team_type], p->followed->id);
}

int sign_up_sport_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	//len == 0
	DEBUG_LOG("SIGN UP CALLBACK\t[%u]", p->id);
	db_set_cnt(0, 3, 1);

	//p->tmpinfo.sign_up_tag = 1;
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_lahm_sign_up_tag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	//p->tmpinfo.sign_up_tag = *(uint32_t*)buf;

	//DEBUG_LOG("SIGN TAG\t[%u %u]", p->id, p->tmpinfo.sign_up_tag);
	return proc_final_login_step(p, 20);
}

int become_shovel_expeired(void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	if (p->action != 1210011)
		return 0;

	p->action = 0;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, PROTO_BECOME_SHOVEL_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("SHOVEL EXPIRED\t[uid=%u]", p->id);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int become_shovel_cmd (sprite_t *p, const uint8_t *body, int len)
{
	//CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	uint32_t itmid = 1210011; // only for sport
	p->action = itmid;
	DEBUG_LOG("BECOME SHOVEL \t[uid=%u]", p->id);

	add_event(&(p->timer_list), become_shovel_expeired, p, 0, get_now_tv()->tv_sec + 60 * 3, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, itmid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

int finish_shovel_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);

	p->action = 0;
	DEBUG_LOG("FINISH SHOVEL\t[%u]", p->id);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

int submit_race_score_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	CHECK_VALID_ID(p->id);

	if (!p->followed) {
		ERROR_RETURN(("no pet follow uid=%u", p->id), -1);
	}

	uint32_t event_type, score;
	int i = 0;
	UNPKG_UINT32(body, event_type, i);
	UNPKG_UINT32(body, score, i);

	if (event_type > 5 || event_type == 0){
		ERROR_RETURN(("error event uid=%u petid=%u event=%u", p->id, p->followed->id, event_type), -1);
	}

	if (event_type == 1 && score < 25)
		score = 50;
	if (event_type == 2 && score < 20)
		score = 40;
	if (event_type == 3 && score > 10000)
		score = 5000;

	*(uint32_t*)p->session = event_type;
	char buff[44];
	int j = 0;
	PKG_H_UINT32(buff, p->followed->id, j);
	PKG_H_UINT32(buff, event_type, j);
	PKG_H_UINT32(buff, score, j);
	PKG_STR(buff, p->nick, j, 16);
	PKG_STR(buff, p->followed->nick, j, 16);

	DEBUG_LOG("SBM RACE\t[uid=%u petid=%u eventType=%u score=%u]", p->id, p->followed->id, event_type, score);
	return db_lahm_sbm_rate_score(p, buff);
}

int submit_race_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t race_item[5] = {1210001, 1210002, 1210003, 1210010, 1210011};
	uint32_t rand_item[4] = {180009, 180014, 180020, 180021};

	if (!p->followed) {
		ERROR_RETURN(("no pet follow uid=%u", p->id), -1);
	}

	DEBUG_LOG("SUBMIT RACE CALLBACK\t[%u]", p->id);
	uint32_t is_first_set = *(uint32_t*)buf;
	uint32_t score = *(uint32_t*)(buf + 4);
	uint32_t event_type = *(uint32_t*)p->session;
	uint32_t itmid = 0;

	if (is_first_set) {
		itmid = race_item[event_type - 1];
		db_single_pet_item_op(0, p->followed->id, itmid, 1, 1, p->id);
	} else {
		if ((rand() % 100) < 20) {
			itmid = rand_item[rand() % 4];
			db_single_item_op(0, p->id, itmid, 1, 1);
		}
	}

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, score, j);
	PKG_UINT32(msg, itmid, j);
	init_proto_head(msg, p->waitcmd, j);

	return send_to_self(p, msg, j, 1);
}

int get_pet_score_list_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	// len == 0
	CHECK_VALID_ID(p->id);

	DEBUG_LOG("GET SCORE LIST\t[uid=%u]", p->id);
	return db_lahm_get_score_list(p);
}

int get_pet_score_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct score_list{
	uint32_t petid;
	uint32_t marathon_score;
	uint32_t swim_score;
	uint32_t high_jump_score;
	uint32_t cheer_show_score;
	uint32_t build_ground_score;
} __attribute__((packed)) score_list_t;

	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * 24);

	DEBUG_LOG("GET SCORE LIST CALLBACK\t[uid=%u cnt=%u]", p->id, count);

	int loop;
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	for (loop = 0; loop < count; loop++) {
		score_list_t* sl = (score_list_t*)(buf + 4 + loop * sizeof(score_list_t));
		PKG_UINT32(msg, sl->petid, j);
		pet_t* pet = get_pet(p->id, sl->petid);
		if (!pet) {
			ERROR_RETURN(("error petid\t[%u %u]", p->id, sl->petid), -1);
		}
		PKG_STR(msg, pet->nick, j, 16);
		PKG_UINT8(msg, get_pet_level(pet), j);
		PKG_UINT32(msg, pet->color, j);
		PKG_UINT32(msg, sl->marathon_score, j);
		PKG_UINT32(msg, sl->swim_score, j);
		PKG_UINT32(msg, sl->high_jump_score, j);
		PKG_UINT32(msg, sl->cheer_show_score, j);
		PKG_UINT32(msg, sl->build_ground_score, j);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int list_team_score_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	// len == 0

	DEBUG_LOG("LIST TEAM SCORE\t[uid=%u]", p->id);
	return db_lahm_list_team_score(p);
}

int list_team_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct team_score_list{
	uint32_t date;
	uint32_t red_team_madel_num;
	uint32_t yellow_team_madel_num;
	uint32_t pink_team_madel_num;
	uint32_t blue_team_madel_num;
} __attribute__((packed)) team_score_list_t;

	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * 20);

	DEBUG_LOG("LIST TEAM SCORE CALLBACK\t[uid=%u cnt=%u]", p->id, count);

	int loop;
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	for (loop = 0; loop < count; loop++){
		team_score_list_t* sl = (team_score_list_t*)(buf + 4 + loop * sizeof(team_score_list_t));
		PKG_UINT32(msg, sl->date, j);
		PKG_UINT32(msg, sl->red_team_madel_num, j);
		PKG_UINT32(msg, sl->yellow_team_madel_num, j);
		PKG_UINT32(msg, sl->pink_team_madel_num, j);
		PKG_UINT32(msg, sl->blue_team_madel_num, j);
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_lahm_best_score_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int i = 0;
	uint32_t event_type;
	UNPKG_UINT32(body, event_type, i);

	DEBUG_LOG("GET BEST SCORE\t[uid=%u event_type=%u]", p->id, event_type);
	return db_lahm_get_best_score(p, &event_type);
}

int get_lahm_best_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct best_score{
	uint32_t score;
	uint32_t team_id;
	char	 usernick[16];
	char	 petnick[16];
} __attribute__((packed)) best_score_t;

	CHECK_BODY_LEN(len, 40);

	DEBUG_LOG("GET BEST SCORE CALLBACK\t[uid=%u]", p->id);

	int j = sizeof(protocol_t);
	best_score_t* bs = (best_score_t*)buf;
	PKG_UINT32(msg, bs->score, j);
	PKG_UINT32(msg, bs->team_id, j);
	PKG_STR(msg, bs->usernick, j, 16);
	PKG_STR(msg, bs->petnick, j, 16);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_lahm_team_madels_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t team_type;
	int i = 0;
	UNPKG_UINT32(body, team_type, i);

	if (team_type > 3){
		ERROR_RETURN(("error team uid=%u team=%u", p->id, team_type), -1);
	}

	DEBUG_LOG("GET TEAM MADELS\t[uid=%u team=%u]", p->id, team_type);
	return db_lahm_get_team_madels(p, &team_type);
}

int get_lahm_team_madels_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	CHECK_BODY_LEN(len, 4);

	uint32_t count = *(uint32_t*)buf;

	DEBUG_LOG("GET TEAM MADELS CALLBACK\t[uid=%u cnt=%u]", p->id, count);

	response_proto_uint32(p, p->waitcmd, count, 0);
	return 0;
}

int get_lahm_madel_and_cup_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	if (!p->followed) {
		ERROR_RETURN(("no pet follow uid=%u", p->id), -1);
	}

	// init the data send to db
	uint32_t dbbuf[4];
	dbbuf[0] = p->followed->id;
	dbbuf[1] = 1210001;
	dbbuf[2] = 1210012;
	dbbuf[3] = 2;
	return send_request_to_db(SVR_PROTO_GET_PET_ITEM_LIST, p, 16, dbbuf, p->id);
}

/*
	SET TEAM
*/
int set_lahm_team_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	uint32_t team;

	CHECK_BODY_LEN(bodylen,0);

	if(p->followed == NULL) {
		return send_to_self_error(p,p->waitcmd,-ERR_the_lahm_not_followed,1);
	}

	team = rand() % 3 + 1;
	uint32_t count = 1;
	msglog(statistic_logfile, 0x020D0230 + (team - 1), get_now_tv()->tv_sec, &count, sizeof(uint32_t));
	p->sess_len = 0;
	PKG_H_UINT32(p->session,team,p->sess_len);

	DEBUG_LOG("set_lahm_team_cmd %u team = %d",p->id,team);

	return send_request_to_db(SVR_PROTO_SET_LAHM_TEAM, p, p->sess_len, p->session, p->id);
}

int set_lahm_team_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	p->sess_len = 0;

	DEBUG_LOG("set_lahm_team_callback %u team = %d",p->id,*(uint32_t *)p->session);

	response_proto_uint32(p, p->waitcmd, *(uint32_t *)p->session, 0);
	return 0;
}

int get_lahm_team_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,0);
	return send_request_to_db(SVR_PROTO_GET_LAHM_TEAM, p, 0, NULL, p->id);
}

int get_lahm_team_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	uint32_t team = 0, medal_num = 0;

	//CHECK_BODY_LEN(len,4);

	int i = 0;
	UNPKG_H_UINT32(buf,team,i);
	UNPKG_H_UINT32(buf,medal_num,i);

	DEBUG_LOG("get_lahm_team_callback %u team = %d",p->id,team);
	switch(p->waitcmd) {
	case PROTO_GET_LAHM_TEAM_CLOTH:
		return get_lahm_team_cloth_callback(p,team);
		break;
	case PROTO_ADD_TEAM_SPORT_SCORE:
		return ding_pigu_get_team_callbak(p, buf, len);
		break;
		/*case PROTO_ADD_STH_FOR_MAX_SCORE_TEAM:*/
		/*return give_sth_get_team_callback(p, buf, len);*/
		/*break;*/
	case PROTO_LAHM_SPORT_ADD_MEDAL:
		return get_team_id_before_add_medal(p, buf, len);
		break;
	}
	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,team,bytes);
	PKG_UINT32(msg,medal_num,bytes);
	init_proto_head(msg, p->waitcmd,bytes);
	return send_to_self(p, msg, bytes, 1);
}

int get_lahm_team_cloth_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,0);

	if(p->followed == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	if(!test_bit_on(p->tasks, sizeof p->tasks, 2 * 114 + 2)) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_not_fini_task, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_LAHM_TEAM, p, 0, NULL, p->id);
}

int get_lahm_team_cloth_callback(sprite_t * p,uint32_t team)
{
	if(team == 0 || team > 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_select_team, 1);
	}

	uint32_t itms[] = {1200046,1200044,1200045};
	uint32_t itm_id = itms[team - 1];

	p->sess_len = 0;
	PKG_H_UINT32(p->session,itm_id,p->sess_len);

	return db_buy_pet_items(p, p->followed->id, itm_id, 1, 1, p->id);
}

int super_lamu_party_get_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,4);

	uint32_t type, i = 0;
	UNPKG_UINT32(body, type, i);
	p->sess_len = 0;
	PKG_H_UINT32(p->session,type,p->sess_len);

	if (p->followed == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}

	if (!PET_IS_SUPER_LAHM(p->followed)) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}

	switch(type) {
		case 1:
		case 2:
		case 3:
			return db_set_sth_done(p, 165, 5,p->id);
		case 4:
			if (p->lamu_action == 0 || (p->lamu_action -1) % 3 != 0) {
				return send_to_self_error(p, p->waitcmd, -ERR_the_action_not_fit, 1);
			}
			if (!check_skill(p, p->lamu_action)) {
				return send_to_self_error(p, p->waitcmd, -ERR_lahm_have_not_this_skill, 1);
			}
			return db_set_sth_done(p, 166, 5,p->id);
		case 5:
			if (p->lamu_action == 0 || (p->lamu_action -1) % 3 != 2) {
				return send_to_self_error(p, p->waitcmd, -ERR_the_action_not_fit, 1);
			}
			if (!check_skill(p, p->lamu_action)) {
				return send_to_self_error(p, p->waitcmd, -ERR_lahm_have_not_this_skill, 1);
			}
			return db_set_sth_done(p, 167, 2,p->id);
//		case 6:
//		case 7:
//		case 8:
//			return db_set_sth_done(p, 168, 3,p->id);
		default:
			return send_to_self_error(p,p->waitcmd, -(100000 + p->waitcmd),1);
	}
}

int super_lamu_party_get_callback(sprite_t * p)
{
	uint32_t type;
	uint32_t item_id;
	int rand_val = rand() % 100;
	int pkg_size = sizeof(protocol_t);

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session,type,p->sess_len);
	PKG_UINT32(msg, type, pkg_size);

	switch(type) {
		case 1:
		case 2:
		case 3:
		{
			uint32_t itms[] = {180089,180088,180087};
			item_id = itms[type - 1];
			PKG_UINT32(msg,item_id,pkg_size);
			db_buy_itm_freely(NULL, p->id, item_id, 1);
			if(rand_val < 10) {
				PKG_UINT32(msg,1230046,pkg_size);
				db_buy_itm_freely(NULL, p->id, 1230046, 1);
			} else {
				PKG_UINT32(msg,0,pkg_size);
			}
			break;
		}
		case 4:
		case 5:
		{
			uint32_t itms[] = {180086,1230045};
			item_id = itms[type - 4];
			PKG_UINT32(msg,item_id,pkg_size);
			PKG_UINT32(msg,0,pkg_size);
			db_buy_itm_freely(NULL, p->id, item_id, 1);
			break;
		}
		case 6:
		{
			uint32_t itms[] = {
				180074,180075,180076,180077,180078,180079,180080,180081,180082,180083};
			item_id = itms[rand_val % 10];
			PKG_UINT32(msg,item_id,pkg_size);
			PKG_UINT32(msg,0, pkg_size);
			db_buy_itm_freely(NULL, p->id, item_id, 2);
			break;
		}
		case 7:
		{
			int count = 3;
			uint32_t itms[] = {
				180074,180075,180076,180077,180078,180079,180080,180081,180082,180083,
				180084,180084,180084,180085,180085,180085,180086,180087,180088,180089};
			item_id = itms[rand_val % 20];
			PKG_UINT32(msg,item_id,pkg_size);
			PKG_UINT32(msg,0, pkg_size);
			if(rand_val % 20 >= 10) count = 2;
			db_buy_itm_freely(NULL, p->id, item_id, count);
			break;
		}
		case 8:
		{
			int count = 3;
			uint32_t itms[] = {0,180086,180087,180088,180089};
			item_id = itms[rand_val % 5];
			if(item_id == 0) {
				count = 1;
				itms[0] = 1200037;
				itms[1] = 1200041;
				itms[2] = 1270012;
				itms[3] = 1270012;
				item_id = itms[rand_val % 4];
			}
			PKG_UINT32(msg,item_id,pkg_size);
			PKG_UINT32(msg,0, pkg_size);
			if(item_id == 1200037 || item_id == 1200041) {
				db_buy_pet_items(NULL, p->followed->id, item_id, count, 1, p->id);
			} else {
				db_buy_itm_freely(NULL, p->id, item_id, count);
			}
			break;
		}
		default:
			PKG_UINT32(msg,0,pkg_size);
			PKG_UINT32(msg,0,pkg_size);
			ERROR_LOG("bug cmd=%u,arg=%u",p->waitcmd,type);
			break;
	}

	DEBUG_LOG("super_lamu_party_get_callback(iud=%u,type=%d,item=%u)",p->id,type,item_id);

	init_proto_head(msg,p->waitcmd,pkg_size);
	return send_to_self(p, msg, pkg_size, 1);
}


/* @brief 顶屁股增加分数，先查询用户是哪个队的
 */
int ding_pigu_add_score_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	DEBUG_LOG("team %u %u", p->id, p->waitcmd);
	uint32_t score = 0;
	unpack(body, bodylen, "L", &score);
	if (score > 780) { /*顶屁股的最大分数*/
		ERROR_RETURN(("OUT OF MAX SCORE %u %u", p->id, score), -1);
	}

	uint32_t msglog_buf[] = {1, p->id};
	msglog(statistic_logfile, 0x020D1001, get_now_tv()->tv_sec, &msglog_buf, sizeof(msglog_buf));

	*(uint32_t *)p->session = score;
	return send_request_to_db(SVR_PROTO_GET_LAHM_TEAM, p, 0, NULL, p->id);
}

/* @brief 如果用户已经选择了对，则给相应的对加分数
 */
int  ding_pigu_get_team_callbak(sprite_t *p, char *buf, int len)
{
	struct team_and_score {
		uint32_t team;
		uint32_t score;
	}__attribute__((packed)) db_buf;

	db_buf.team = *(uint32_t *)buf;
	if (db_buf.team == 0) {/*没选择队*/
		return send_to_self_error(p, p->waitcmd, -ERR_have_not_choose_team, 1);
	}
	db_buf.score = *(uint32_t *)p->session;
	DEBUG_LOG("team1 %u %u", p->id, p->waitcmd);
	return send_request_to_db(SVR_PROTO_ADD_TEAM_SPORT_SCORE, p, sizeof(db_buf), &db_buf, p->id);
}

/* @brief 为队伍加分数的DB返回值处理
 */
int ding_pigu_add_score_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t team1 = *(uint32_t *)buf;
	uint32_t team2 = *(uint32_t *)(buf + 4);
	uint32_t team3 = *(uint32_t *)(buf + 8);
	uint32_t which_team = *(uint32_t *)(buf + 12);/*哪个队达到最大值*/
	uint32_t flag = *(uint32_t *)(buf + 15);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, team1, j);
	PKG_UINT32(msg, team2, j);
	PKG_UINT32(msg, team3, j);
	PKG_UINT32(msg, flag, j);
	init_proto_head(msg, p->waitcmd, j);
	send_to_self(p, msg, j, 1);

	if (which_team != 0) {/*如果有队达到最大值，全场景广播*/
		DEBUG_LOG("whichteam %u %u", p->id, which_team);
		send_to_broadcaster(MCAST_sport_team_score, sizeof(which_team), &which_team, 0, 47, PROTO_TEAM_SCORE_GET_MAX);
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, which_team, i);
		init_proto_head(msg, PROTO_TEAM_SCORE_GET_MAX, i);
		send_to_map3(47, msg, i);
	}
	return 0;
}


/* @brief 对某个队达到最大分数的组播包的处理
 */
int sport_team_get_max_score(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN(len, 16);
	communicator_body_t *cbody = buf;
	uint32_t team = *(uint32_t *)cbody->body;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, team, i);
	init_proto_head(msg, PROTO_TEAM_SCORE_GET_MAX, i);
	DEBUG_LOG("broad cast %u", team);
	send_to_map3(cbody->mapid, msg, i);
	return 0;
}


/* @brief 得到三个队的分数和上轮哪个队获胜的
 */
int get_team_score_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_SPORT_TEAM_SCORE, p, 0, NULL, p->id);
}

/* @brief 得到分数的回调函数
 */
int get_team_score_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t team1 = *(uint32_t *)buf;
	uint32_t team2 = *(uint32_t *)(buf + 4);
	uint32_t team3 = *(uint32_t *)(buf + 8);
	uint32_t win_team = *(uint32_t *)(buf + 12);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, team1, i);
	PKG_UINT32(msg, team2, i);
	PKG_UINT32(msg, team3, i);
	PKG_UINT32(msg, win_team, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/* @brief 得到三个队的奖牌数
 */
int get_team_medal_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_SPORT_TEAM_MEDAL, p, 0, NULL, p->id);
}

/* @brief 得到奖牌的回调函数
 */
int get_team_medal_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t team1_medal = *(uint32_t *)buf;
	uint32_t team2_medal = *(uint32_t *)(buf + 4);
	uint32_t team3_medal = *(uint32_t *)(buf + 8);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, team1_medal, i);
	PKG_UINT32(msg, team2_medal, i);
	PKG_UINT32(msg, team3_medal, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/* @brief  获胜的队领取物品,先查询用户属于哪个队的
 */
int give_sth_accord_max_score_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_GET_LAHM_TEAM, p, 0, NULL, p->id);
}

/* @brief  得到用户属于的队，发包确认用户是否能够拿物品
 */
int give_sth_get_team_callback(sprite_t *p, char *buf, int len)
{
	uint32_t team = *(uint32_t *)buf;
	*(uint32_t *)p->session = team;
	if (team == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_not_choose_team, 1);
	}
	return db_chk_itm_cnt(p, 13453, 13456);
}

/* @brief  如果用户符合获取物品的条件，给用户增加物品
 */
int add_sh_for_wim_tem_member_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t team = *(uint32_t *)p->session;
	uint32_t count = *(uint32_t *)(p->session + 4);
	uint32_t add_change_itm = 0;
	uint32_t add_mole_itm = 0;
	switch (team) {
	case 1:/*水*/
		if (count == 0) {
			add_mole_itm = 13453;
		}
		add_change_itm = 150006;
		break;
	case 2:/*火*/
		if (count == 0) {
			add_mole_itm = 13455;
		}
		add_change_itm = 150005;
		break;
	case 3:/*木*/
		if (count == 0) {
			add_mole_itm = 13454;
		}
		add_change_itm = 150007;
		break;
	default:
		ERROR_RETURN(("wrong team %u %u", p->id, team), -1);
	}
	db_buy_itm_freely(NULL, p->id, add_change_itm, 5);
	if (add_mole_itm != 0) {
		db_buy_itm_freely(NULL, p->id, add_mole_itm, 1);
	}
	response_proto_uint32_uint32(p, p->waitcmd, add_change_itm, add_mole_itm, 0);
	return 0;
}

/* @brief 用户玩游戏获取奖牌
 */
int add_medal_by_play_game_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	struct game_id_and_score {
		uint32_t game_id;
		uint32_t game_score;
		uint32_t game_level;
		uint32_t game_xiaomee;
		uint32_t pet_id;
	}__attribute__((packed)) game_info;

	CHECK_BODY_LEN(bodylen, 16);

	if (p->followed == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
	}
	game_info.pet_id = p->followed->id;

	unpack(body, bodylen, "LLLL", &game_info.game_id, &game_info.game_score,
	&game_info.game_level, &game_info.game_xiaomee);
	if (game_info.game_id < GAME_START_ID  || game_info.game_id > GAME_END_ID || game_info.game_level <= 0) {
		ERROR_RETURN(("GAME INFO WRONG [%u %u %u %u]", p->id, game_info.game_id,
		game_info.game_score, game_info.game_level), -1);
	}
	memcpy(p->session, &game_info, sizeof(game_info));

	return send_request_to_db(SVR_PROTO_GET_LAHM_TEAM, p, 0, NULL, p->id);
}


/* @brief 根据游戏的分数得到奖牌数
 */
int cal_medal_num(sprite_t *p, uint32_t game_id, uint32_t game_xiaomee,
		uint32_t game_score, uint32_t game_level, uint32_t *medal_num)
{
	#define CHECK_GAME_AND_GET_MEDAL(game_xiaomee_max, game_score_max, array_name)\
	do{\
		if (game_xiaomee > (game_xiaomee_max)\
		   || (game_score > (game_score_max))\
		   || (game_level > ((sizeof(array_name) / sizeof(array_name[0]))))) {\
				ERROR_RETURN((" DATA ERROR %u %u %u %u %u", p->id, game_id, game_xiaomee, game_score, game_level), -1);\
		}\
		*medal_num = array_name[game_level - 1];\
	}while(0)

	/*障碍跳跳*/
	uint32_t zhan_ai_tiao[]  = {1, 3, 7};
	/*小投手*/
	uint32_t xiao_tou_shou[] = {1, 2, 5, 9, 15, 20};
	/*鸭鸭快跑*/
	uint32_t ya_ya_kuai_pao[] = {1, 2, 5, 9, 15, 20};
	/*攀岩*/
	uint32_t pan_yan[] = {1, 2, 3, 5, 6, 7};
	/*跳高*/
	uint32_t tiao_gao[] = {1, 2, 3, 5, 8};

	switch(game_id) {
		case 1:
			CHECK_GAME_AND_GET_MEDAL(1000, 10000, zhan_ai_tiao);
			break;

		case 2:
			CHECK_GAME_AND_GET_MEDAL(1500, 10000, xiao_tou_shou);
			break;

		case 3:
			CHECK_GAME_AND_GET_MEDAL(1500, 10000, ya_ya_kuai_pao);
			break;

		case 4:
			CHECK_GAME_AND_GET_MEDAL(1000, 10000, pan_yan);
			break;

		case 5:
			CHECK_GAME_AND_GET_MEDAL(1000, 10000, tiao_gao);
			break;

		default:
			ERROR_RETURN(("WRONG GAME ID %u %u", p->id, game_id), -1);
	}
	return 0;
}

/* @得到用户是属于哪个队的,并且加到个人的奖牌数里
 */
int get_team_id_before_add_medal(sprite_t *p, const char *buf, int lne)
{
	struct game_id_and_score {
		uint32_t game_id;
		uint32_t game_score;
		uint32_t game_level;
		uint32_t game_xiaomee;
		uint32_t pet_id;
		uint32_t team_id;
		uint32_t medal_num;
	}__attribute((packed)) game_info;

	memcpy(&game_info, p->session, sizeof(game_info));
	unpkg_host_uint32((uint8_t *)buf, &game_info.team_id);
	CHECK_TEAM_VALID(game_info.team_id);

	if (cal_medal_num(p, game_info.game_id, game_info.game_xiaomee,
	game_info.game_score, game_info.game_level, &game_info.medal_num) == -1) {
		ERROR_RETURN(("WRONG PARAM %u", p->id), -1);
	}

	memcpy(p->session, &game_info, sizeof(game_info));

	return send_request_to_db(SVR_PROTO_ADD_MEDAL_FOR_SELF, p, sizeof(game_info.medal_num), &game_info.medal_num, p->id);
}

/* @brief 个人奖牌数加成功，为对加奖牌数并记录每个游戏的总分数和拉姆的最高分
 */
int add_medal_by_play_game_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	struct game_all_info {
		uint32_t game_id;
		uint32_t game_score;
		uint32_t game_level;
		uint32_t game_xiaomee;
		uint32_t pet_id;
		uint32_t team_id;
		uint32_t medal_num;
	}__attribute((packed)) game_info;

	struct game_id_and_score {
		uint32_t game_id;
		uint32_t team_id;
		uint32_t pet_id;
		uint32_t medal_num;
		uint32_t game_score;
		char user_nick[USER_NICK_LEN];
		char pet_nick[USER_NICK_LEN];
	}__attribute((packed)) db_game_info;

	memcpy(&game_info, p->session, sizeof(game_info));
	db_game_info.game_id = game_info.game_id;
	db_game_info.team_id = game_info.team_id;
	db_game_info.pet_id = game_info.pet_id;
	db_game_info.medal_num = game_info.medal_num;
	db_game_info.game_score = game_info.game_score;
	memcpy(db_game_info.user_nick, p->nick, USER_NICK_LEN);
	memcpy(db_game_info.pet_nick, p->followed->nick, PET_NICK_LEN);

	uint32_t msglog_buf[] = {1, p->id};
	msglog(statistic_logfile, 0x020D1002 + game_info.game_id - 1, get_now_tv()->tv_sec, &msglog_buf, sizeof(msglog_buf));


	db_change_xiaome(NULL, game_info.game_xiaomee, 0, 0, p->id);
	send_request_to_db(SVR_PROTO_ADD_MEDAL_FOR_TEAM, NULL, sizeof(db_game_info), &db_game_info, p->id);
	response_proto_uint32_uint32(p, p->waitcmd, game_info.medal_num, game_info.game_xiaomee, 0);
	return 0;
}

/* @brief 得到游戏的前十名和拉姆在这个游戏中的最高分数
 */
int get_top_10_pet_score_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t game_id = 0;
	unpack(buf, len, "L", &game_id);
	return send_request_to_db(SVR_PROTO_GET_TOP_PET_SCORE, p, sizeof(game_id), &game_id, p->id);
}

/* @brief 得到游戏前十名和拉姆分数的回调函数
 */
int get_top_10_pet_score_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	struct id_score {
		uint32_t id;
		uint32_t game_score;
		char nick[USER_NICK_LEN];
	}__attribute__((packed));

	uint32_t game_top_count = 0, pet_score_count = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &game_top_count, &pet_score_count);
	int index = sizeof(protocol_t);
	PKG_UINT32(msg, game_top_count, index);
	PKG_UINT32(msg, pet_score_count, index);

	struct id_score *p_id_score = (struct id_score *)(buf
			+ sizeof(game_top_count) + sizeof(pet_score_count));
	int i = 0;
	for (i = 0; i < game_top_count + pet_score_count; i++) {
		PKG_UINT32(msg, p_id_score->id, index);
		PKG_UINT32(msg, p_id_score->game_score, index);
		PKG_STR(msg, p_id_score->nick, index, USER_NICK_LEN);
		p_id_score++;
	}
	init_proto_head(msg, p->waitcmd, index);
	return send_to_self(p, msg, index, 1);
}

/* @brief 得到个人的奖牌数
 */
int get_myself_medal_num_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_GET_SELF_MEDAL_NUM, p, 0, NULL, p->id);
}

/* @brief 得到个人奖牌数的回调函数
 */
int get_myself_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t medal_num = 0;
	unpkg_host_uint32((uint8_t *)buf, &medal_num);
	response_proto_uint32(p, p->waitcmd, medal_num, 0);
	return 0;
}

static uint32_t start_time_map_8 = 0;

static uint32_t start_time_map_4 = 0;

static uint32_t start_time_map_41 = 0;

/* @brief 用户站上舞台去发广播包
 */
int sport_stage_start_cmd(sprite_t *p, uint32_t id, const uint8_t *buf, int len)
{
	#define CHECK_START_SET_START_TIME(type)\
		do{\
			if ((type) != 0) {\
				return 0;\
			}\
			(type) = time(NULL);\
		}while(0)


	if ((p->tiles == NULL)) {
		ERROR_RETURN(("WRONG POINTER %u", p->id), -1);
	}
	switch(p->tiles->id) {
	case MAP_ID_4:
		CHECK_START_SET_START_TIME(start_time_map_4);
		break;

	case MAP_ID_8:
		CHECK_START_SET_START_TIME(start_time_map_8);
		break;

	case MAP_ID_41:
		CHECK_START_SET_START_TIME(start_time_map_41);
		break;

	default:
		ERROR_RETURN(("WRONG MAP ID %u %lu", p->id, p->tiles->id), -1);
	}

	map_t *map = get_map(p->tiles->id);
	ADD_TIMER_EVENT(map, expire_sport_stage_end, &p->tiles->id, now.tv_sec + 60);

	int index = sizeof(protocol_t);
	PKG_UINT32(msg, 1, index);
	init_proto_head(msg, p->waitcmd, index);
	p->waitcmd = 0;
	send_to_map2(map, msg, index);
	return 0;
}

/* @brief 时间到发广播包
 */
int expire_sport_stage_end(void *owner, void *data)
{
	map_t *map = owner;

	switch(map->id) {
	case MAP_ID_4:
		start_time_map_4 = 0;
		break;

	case MAP_ID_8:
		start_time_map_8 = 0;
		break;

	case MAP_ID_41:
		start_time_map_41 = 0;
		break;

	default:
		ERROR_RETURN(("WRONG MAP ID %lu", map->id), -1);
	}

	uint32_t index = sizeof(protocol_t);
	PKG_UINT32(msg, 0, index);
	init_proto_head(msg, PROTO_LAHM_SPORT_START_AND_END, index);
	send_to_map2(map, msg, index);
	return 0;
}

/* @brief 得到已经开始时长
 */
int get_diff_time_start_to_now_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);

    uint32_t map_id = 0, if_start =0, diff_time = 0;
	unpack(buf, len, "L", &map_id);

	switch (map_id) {
	case MAP_ID_4:
		if (start_time_map_4 != 0) {
			if_start = 1;
			diff_time = time(NULL) - start_time_map_4;
		}
		break;

	case MAP_ID_8:
		if (start_time_map_8 != 0) {
			if_start = 1;
			diff_time = time(NULL) - start_time_map_8;
		}
		break;

	case MAP_ID_41:
		if (start_time_map_41 != 0) {
			if_start = 1;
			diff_time = time(NULL) - start_time_map_41;
		}
		break;

	default:
		ERROR_RETURN(("WRONG MAPID %u %u", p->id, map_id), -1);
	}

	response_proto_uint32_uint32(p, p->waitcmd, if_start, diff_time, 0);
	return 0;
}

int give_npc_shop_dish_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,8);

	int j = 0;
	uint32_t count;
	uint32_t dish_id;
	UNPKG_UINT32(body,dish_id,j);
	UNPKG_UINT32(body,count,j);

	if(p->dining_flag == 0) {
		ERROR_RETURN(("ERR_have_no_shop %u %u %d",p->id,dish_id,count),-1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session,0,p->sess_len);
	PKG_H_UINT32(p->session,dish_id,p->sess_len);
	PKG_H_UINT32(p->session,count,p->sess_len);

	return send_request_to_db(SVR_PROTO_GIVE_DISH_NPC, p, 12, p->session, p->id);
}

int give_npc_shop_dish_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	int j = 4;
	uint32_t count;
	uint32_t dish_id;

	if(p->waitcmd == PROTO_SET_MAGIC_MAN_EAT_DISH) {
		return set_magic_man_eat_dish_callback( p, id, buf, len);
	}

	UNPKG_H_UINT32(p->session,dish_id,j);
	UNPKG_H_UINT32(p->session,count,j);

	response_proto_uint32_uint32(p,p->waitcmd,dish_id,count,0);
	return 0;
}

typedef struct {
	uint32_t pet_id;
	uint32_t team_id;
	uint32_t team_No;
	uint32_t top10_Cnt;
	uint32_t Medal_Cnt;
	uint32_t Mole_Prize;
	uint32_t Lahm_Prize;
} lahm_sport_prize_t;

int get_lahm_team_rank_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,4);

	int i = 0;
	uint32_t team_id;
	UNPKG_UINT32(body,team_id,i);

	if(team_id==0 || team_id > 3) {
		ERROR_RETURN(("get_lahm_team_rank_cmd uid=%u team=%d",p->id,team_id),-1);
	}

	return send_request_to_db(SVR_PROTO_GET_TEAM_PRIZE_2,p,4,&team_id, p->id);
}

int get_lahm_team_prize_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,0);

	if(p->followed == NULL) {
		return send_to_self_error(p,p->waitcmd,-ERR_the_lahm_not_followed, 1);
	}

	lahm_sport_prize_t *priz = (void*)p->session;

	priz->pet_id = p->followed->id;

	return send_request_to_db(SVR_PROTO_GET_TEAM_PRIZE_1,p,4,&priz->pet_id,p->id);
}

int db_set_team_prize(sprite_t *p,lahm_sport_prize_t *priz)
{
	uint32_t cnt1=0;
	uint32_t itm1=0;
	uint32_t cnt2=0;
	uint32_t itm2=0;

	if(priz->Lahm_Prize == 0) {
		switch(priz->team_No) {
			case 1:
				cnt1 = 1;
				itm1 = 1200047;
				cnt2 = 0;
				itm2 = 160766;
				if((p->only_one_flag[0] & (1<<7)) == 0) {
					cnt2 = 1;
				}
				break;
			case 2:
				cnt1 = 1;
				itm1 = 1200048;
				cnt2 = 0;
				itm2 = 160767;
				if((p->only_one_flag[0] & (1<<7)) == 0) {
					cnt2 = 1;
				}
				break;
			case 3:
				cnt1 = 1;
				itm1 = 1200049;
				cnt2 = 0;
				itm2 = 160768;
				if((p->only_one_flag[0] & (1<<7)) == 0) {
					cnt2 = 1;
				}
				break;
			default:
				break;
		}
		send_request_to_db(SVR_PROTO_SET_LAHM_PRIZE_F,NULL,4,&priz->pet_id, p->id);

		if(cnt1 == 1) {
			db_buy_pet_items(NULL,p->followed->id,itm1,cnt1,1,p->id);
		}

		if(cnt2 == 1) {
			int index = 8;
			p->only_one_flag[0] |= (1<<7);
			send_request_to_db(SVR_PROTO_SET_ONLY_ONE_FLAG, NULL, 4, &index, p->id);
			db_buy_items(p,get_item_prop(itm2),cnt2,find_kind_of_item(itm2),1,0);
		}

		int bytes = sizeof(protocol_t);
		PKG_UINT32(msg,cnt1,bytes);
		PKG_UINT32(msg,itm1,bytes);
		PKG_UINT32(msg,cnt2,bytes);
		PKG_UINT32(msg,itm2,bytes);
		init_proto_head(msg,p->waitcmd,bytes);
		return send_to_self(p,msg,bytes,1);
	}

	return send_to_self_error(p, p->waitcmd, -ERR_already_get_the_prize, 1);
}

int db_set_personnal_prize(sprite_t *p,lahm_sport_prize_t *priz)
{
	int bytes = sizeof(protocol_t);

	if(priz->Mole_Prize == 0) {
		if(priz->top10_Cnt) {
			int addmoney = 10000 * priz->top10_Cnt;
			p->yxb += addmoney;
			db_change_xiaome_not_limit(NULL,addmoney,p->id);
			PKG_UINT32(msg,addmoney,bytes);
			db_buy_items(p,get_item_prop(13466),1,find_kind_of_item(13466),1,0);
			PKG_UINT32(msg,13466,bytes);
		} else if(priz->Medal_Cnt > 50) {
			db_buy_items(p,get_item_prop(13466),1,find_kind_of_item(13466),1,0);
			PKG_UINT32(msg,0,bytes);
			PKG_UINT32(msg,13466,bytes);
		} else {
			p->yxb += 2000;
			db_change_xiaome_not_limit(NULL,2000,p->id);
			PKG_UINT32(msg,2000,bytes);
			PKG_UINT32(msg,0,bytes);
		}

		send_request_to_db(SVR_PROTO_SET_MOLE_PRIZE_F,NULL,0,NULL, p->id);
		init_proto_head(msg,p->waitcmd,bytes);
		return send_to_self(p,msg,bytes,1);
	}

	return send_to_self_error(p, p->waitcmd, -ERR_already_get_the_prize, 1);
}

int get_lahm_team_prize_1_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	CHECK_BODY_LEN(len,16);

	int i = 0;
	lahm_sport_prize_t *priz = (void*)p->session;

	UNPKG_H_UINT32(buf,priz->Mole_Prize,i);
	UNPKG_H_UINT32(buf,priz->Lahm_Prize,i);
	UNPKG_H_UINT32(buf,priz->Medal_Cnt,i);
	UNPKG_H_UINT32(buf,priz->team_id,i);

	return send_request_to_db(SVR_PROTO_GET_TEAM_PRIZE_2,p,4,&priz->team_id, p->id);
}

int get_lahm_team_prize_2_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	CHECK_BODY_LEN(len,8);

	int i = 0;
	lahm_sport_prize_t *priz = (void*)p->session;

	UNPKG_H_UINT32(buf,priz->top10_Cnt,i);
	UNPKG_H_UINT32(buf,priz->team_No,i);

	DEBUG_LOG("%u,pet=%u team=%d,flag=%d,%d,team_No,top,cnt=%d,%d,%d",p->id,priz->pet_id,priz->team_id,priz->Lahm_Prize,priz->Mole_Prize,priz->team_No, priz->top10_Cnt,priz->Medal_Cnt);

	if(p->waitcmd == PROTO_GET_SPORT_TEAM_PRIZE) {
		return db_set_team_prize(p,priz);
	} else if(p->waitcmd == PROTO_GET_SPORT_PERSON_PRIZE) {
		return db_set_personnal_prize(p, priz);
	}

	response_proto_uint32(p,p->waitcmd,priz->team_No,0);
	return 0;
}





