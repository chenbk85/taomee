#include <openssl/md5.h>
#include <libtaomee/project/constants.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "central_online.h"
#include "communicator.h"
#include "logic.h"
#include "proto.h"
#include "dbproxy.h"
#include "event.h"
#include "tasks.h"
#include "login.h"
#include "mole_question.h"
#include "mole_pasture.h"
#include "im_message.h"

typedef struct LoginSession {
	uint32_t uid, ip, tm1, tm2;
} __attribute__((packed)) login_session_t;

enum {
	default_login_maps_num	= 10
};

static const map_t* default_login_maps[default_login_maps_num];
static map_id_t default_maps[default_login_maps_num] = {1,3,2,8,10,37,9,41,6,7};

//------------------ Responses ------------------------------------
static void rsp_proto_login(sprite_t* p)
{
	int i = sizeof(protocol_t);

	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, p->nick, i, 16);
	PKG_UINT32(msg, p->inviter_id, i);
	PKG_UINT32(msg, p->old_invitee_cnt, i);
	PKG_UINT32(msg, p->invitee_cnt, i);
	PKG_UINT32(msg, p->color, i);

	PKG_UINT32(msg, p->flag, i);

    DEBUG_LOG("p->flag: %u ", p->flag);

	PKG_UINT32(msg, p->birth, i);
	PKG_UINT32(msg, p->exp, i);

	PKG_UINT32(msg, p->strong, i);
	PKG_UINT32(msg, p->iq, i);

	PKG_UINT32(msg, p->lovely, i);
	PKG_UINT32(msg, p->game_king, i);
	PKG_UINT32(msg, p->yxb, i);
	PKG_UINT32(msg, p->engineer, i);
	PKG_UINT32(msg, calculation_level_from_fashion(p->fashion), i);

	PKG_UINT32 (msg, calculation_level_from_skill(p->cultivate_skill), i);
	PKG_UINT32 (msg, calculation_level_from_skill(p->breed_skill), i);
	PKG_UINT32 (msg, p->dining_flag, i);
	PKG_UINT32 (msg, p->dining_level, i);
	PKG_MAP_ID(msg, (p->tiles ? p->tiles->id : 0), i);

	PKG_UINT8(msg, get_sprite_status(p), i);
	PKG_UINT32(msg, p->action, i);
	PKG_UINT8(msg, p->direction, i);

	PKG_UINT32(msg, p->posX, i);
	PKG_UINT32(msg, p->posY, i);
	p->sub_grid = 0;
	// pack login times
	PKG_UINT32(msg, p->olcount + 1, i);
	// for birthday airship
	PKG_UINT32(msg, p->birthday, i);
	PKG_UINT32(msg, p->pet_task, i);
	DEBUG_LOG("PET TASK %u %u", p->pet_task, p->id);
	PKG_UINT32(msg, p->magic_task, i);
	PKG_UINT32(msg, p->vip_level, i);
	PKG_UINT32(msg, p->vip_month, i);
	PKG_UINT32(msg, p->vip_value, i);
	PKG_UINT32(msg, p->vip_end, i);
	PKG_UINT32(msg, p->vip_autopay, i);
	PKG_UINT32(msg, p->dragon_id, i);
	PKG_STR (msg, p->dragon_name, i, DRAGON_NICK_LEN);
	PKG_UINT32(msg, p->dragon_growth, i);

	if (is_holiday(get_now_tm()->tm_wday))	{
		if (p->oltoday < FIVE_HOUR_SEC)
			PKG_UINT32(msg, FIVE_HOUR_SEC - p->oltoday, i);
		else
			PKG_UINT32(msg, 0, i);
	} else {
		if (p->oltoday < TWO_HOUR_SEC)
			PKG_UINT32(msg, TWO_HOUR_SEC - p->oltoday, i);
		else
			PKG_UINT32(msg, 0, i);
	}

	/*
	const event_t* ev_cloth = get_event(5);
	const event_t* ev_furni = get_event(10);
	if (ev_cloth->status) {
		PKG_UINT32(msg, 5, i);
	} else if(ev_furni->status) {
		PKG_UINT32(msg, 10, i);
	} else {
		PKG_UINT32(msg, 0, i);
	}
	*/
	//
	i += pack_activity_info(p, msg + i);

	i += pkg_items(p->item_cnt, p->items, msg + i, 0);

	DEBUG_LOG("LOGIN SUCC\t[uid=%u fd=%u use_time=%u,vip=(%u,%u,%u,%u)]",
		p->id, p->fd, p->oltoday,p->vip_level,p->vip_month,p->vip_value,p->vip_end);

	uint32_t msg_buff[2] = {p->id, p->vip_level};
	msglog(statistic_logfile, 0x0409B460, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	init_proto_head(msg, PROTO_LOGIN, i);
	send_to_map(p, msg, i, 1);
}
//----------------------------------------------------------------

//-------------------------- Utils ---------------------------------
static inline int
long_time_min45_in_game(void* owner, void* data)
{
	sprite_t* p = owner;

	int too_long = 1;
	response_uint32_not_complete(p, PROTO_TOO_LONG_IN_GAME, too_long, 0);
	ADD_TIMER_EVENT(p, long_time_min45_in_game, 0, now.tv_sec + 45*60);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

//-------------------------- Utils ---------------------------------
static inline int
long_time_min1_in_game(void* owner, void* data)
{
	sprite_t* p = owner;
	response_proto_uint32_uint32_not_complete(p, PROTO_TIME_SYN_ONE_MINUTE, now.tv_sec, now.tv_usec, 0);
	ADD_TIMER_EVENT(p, long_time_min1_in_game, 0, now.tv_sec + 60);
	return 0; // returns 0 means the `timer scanner` should remove this event
}


static inline int
long_time_min10_in_game(void* owner, void* data)
{
	sprite_t* p = owner;
	int too_long = 3;
	if (get_now_tv()->tv_sec - p->stamp > 45*60)
		too_long = 4;
	response_uint32_not_complete(p, PROTO_TOO_LONG_IN_GAME, too_long, 0);
	ADD_TIMER_EVENT(p, long_time_min10_in_game, 0, now.tv_sec + 10*60);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

static inline int
long_time_min300_in_game(void* owner, void* data)
{
	sprite_t* p = owner;
	if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) < FIVE_HOUR_SEC) {
		uint32_t remain_time = FIVE_HOUR_SEC - p->oltoday - (now.tv_sec - p->stamp);
		response_uint32_not_complete(p, PROTO_NOTIFY_REMAIN_TIME, remain_time, 0);
		ADD_TIMER_EVENT(p, long_time_min300_in_game, 0, now.tv_sec + 120);
		return 0;
	}
	int too_long = 6;
	response_uint32_not_complete(p, PROTO_TOO_LONG_IN_GAME, too_long, 0);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

static inline int
long_time_min120_in_game(void* owner, void* data)
{
	sprite_t* p = owner;
	//DEBUG_LOG("LONG TIME\t[%u %u %u %u]", p->id, now.tv_sec, p->stamp, p->oltoday);
	if ((now.tv_sec - p->stamp + p->oltoday) > FIVE_HOUR_SEC) {
		int too_long = 6;
		response_uint32_not_complete(p, PROTO_TOO_LONG_IN_GAME, too_long, 0);
		return 0;
	}
	if ((now.tv_sec - p->stamp + p->oltoday) < TWO_HOUR_SEC) {
		int remain_time;
		if (is_holiday(get_now_tm()->tm_wday)) {
			remain_time = FIVE_HOUR_SEC - p->oltoday - (now.tv_sec - p->stamp);
			response_uint32_not_complete(p, PROTO_NOTIFY_ONLINE_TIME, FIVE_HOUR_SEC-remain_time, 0);
			response_uint32_not_complete(p, PROTO_NOTIFY_REMAIN_TIME, remain_time, 0);
		} else {
			remain_time = TWO_HOUR_SEC - p->oltoday - (now.tv_sec - p->stamp);
			response_uint32_not_complete(p, PROTO_NOTIFY_ONLINE_TIME, TWO_HOUR_SEC-remain_time, 0);
			response_uint32_not_complete(p, PROTO_NOTIFY_REMAIN_TIME, remain_time, 0);
		}

//		DEBUG_LOG("long_time_min120_in_game remain_time %d flag %d ", remain_time, is_holiday(get_now_tm()->tm_wday));

		ADD_TIMER_EVENT(p, long_time_min120_in_game, 0, now.tv_sec + 120);
		return 0;
	}



	int too_long = 5;
	response_uint32_not_complete(p, PROTO_TOO_LONG_IN_GAME, too_long, 0);

	if (is_holiday(get_now_tm()->tm_wday))
		ADD_TIMER_EVENT(p, long_time_min300_in_game, 0, now.tv_sec + 120);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

static inline int
long_time_min15_in_game(void* owner, void* data)
{
    sprite_t* p = owner;
    int idx = p->ques_id;
    p->ques_ans_flag = 1;
    uint8_t msg[1024];
    int len = 0;
    PKG_UINT32(msg, ques[idx].id, len);
    PKG_UINT32(msg, p->ques_correct_cnt, len);
    PKG_STR(msg, ques[idx].content, len, sizeof(ques[idx].content));
    PKG_STR(msg, ques[idx].option_a, len, sizeof(ques[idx].option_a));
    PKG_STR(msg, ques[idx].option_b, len, sizeof(ques[idx].option_b));
    PKG_STR(msg, ques[idx].option_c, len, sizeof(ques[idx].option_c));
    PKG_STR(msg, ques[idx].option_d, len, sizeof(ques[idx].option_d));

	response_str_not_complete(p, PROTO_SEND_QUESTION, len, msg, 0);
	ADD_TIMER_EVENT(p, long_time_min15_in_game, 0, now.tv_sec + 10*60);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

static inline int
unpkg_auth(const uint8_t body[], int len, login_session_t* sess)
{
	int l, i = 0;
	char outbuf[32];

	CHECK_BODY_LEN(len, 20);
	UNPKG_UINT32 (body, l, i);
	CHECK_BODY_LEN(l, sizeof *sess);

	//session: ip + time + userid + time
	des_decrypt(LOGIN_DES_KEY, (char*)(body + 4), outbuf);
	des_decrypt(LOGIN_DES_KEY, (char*)(body + 12), outbuf + 8);
	sess->ip  = *(uint32_t *)outbuf;
	sess->uid = *(uint32_t *)(outbuf + 8);
	sess->tm1 = *(uint32_t*)(outbuf + 4);
	sess->tm2 = *(uint32_t*)(outbuf + 12);

	return 0;
}

/**
 * @brief verify if the md5 code from the visitor is correct
 * @param const char*, md5 code from the visitor
 *
 * @return int, 0 on success, otherwise failure
 */
static inline int
verify_visitor_md5(const char* md5)
{
	static unsigned char md5key[] = "bAbY mOlE Go tO heLl!";

	unsigned char md_out[16], md[32];
	MD5((unsigned char*)md5key, sizeof(md5key) - 1, md_out);

	int i;
	for (i = 0; i != 16; ++i) {
		sprintf((char*)md + i * 2, "%.2x", md_out[i]);
	}

	return memcmp(md + 8, md5, 16);
}

/**
 * @brief verify if the md5 code from the user is correct
 * @param const char*, md5 code from the user
 * @param const char*, DESed session from the user, which in turn acquired from LoginServer
 *
 * @return int, 0 on success, otherwise failure
 */
static inline int
verify_user_md5(const char* md5, const char* sess)
{
	static char md5key[256];

	unsigned char md_out[16], md[32];
	const char* str = "fREd hAo crAzy BAby in Our ProgRAm?";
	int len = snprintf(md5key, sizeof md5key, "%u%.11s%u", ntohl(*(uint32_t*)(sess + 10)), str + 5, ntohl(*(uint32_t*)(sess + 3)));
	MD5((unsigned char*)md5key, len, md_out);

	int i;
	for (i = 0; i != 16; ++i) {
		sprintf((char*)md + i * 2, "%.2x", md_out[i]);
	}

	return memcmp(md + 6, md5, 16);
}

/**
 * @brief verify if the DESed session from the user is correct
 * @param const sprite_t*, the user that is currently logging in
 * @param const login_session_t*, UnDESed session
 *
 * @return int, 0 on success, otherwise failure
 */
static inline int
verify_session(const sprite_t* p, const login_session_t* sess)
{
	//sprite id
	if (sess->uid != p->id) {
		ERROR_RETURN(("verify userid failed: id=%u %u", p->id, sess->uid), -1);
	}

	if (sess->tm1 != sess->tm2) {
		ERROR_RETURN(("verify time failed: tm=%u %u", sess->tm1, sess->tm2), -1);
	}
	int diff = now.tv_sec - sess->tm1;
	if ( (diff > 3600) || (diff < -3600) ) {
		ERROR_RETURN(("verify time failed: tm=%u %ld tmdiff=%d uid=%u", sess->tm1, now.tv_sec, diff, p->id), -1);
	}
	return 0;
}

static inline int
//do_auth(sprite_t* v)
do_auth(sprite_t* p)
{
	notify_user_login(p, 1);

    send_request_to_db(SVR_PROTO_USER_SPRINT_FESTIVAL_LOGIN, NULL, 0, NULL, p->id);
	send_request_to_db(SVR_PROTO_USER_VIP_CONTINUE_WEEK_LOGIN, NULL, 0, NULL, p->id);
	//in self's ocean, clear last drop shells time
	send_request_to_db(SVR_PROTO_OCENA_CLEAR_DROP_SHELLS_TIME, NULL, 0, NULL, p->id);
	p->adventure_interval = time(0);
	if (get_now_tm()->tm_wday == 0 || get_now_tm()->tm_wday == 5 || get_now_tm()->tm_wday == 6)
	{
	    send_request_to_db(SVR_PROTO_USER_WEEKEND_LOGIN, NULL, 0, NULL, p->id);
	}
	uint32_t buf[3]={1000000036, 1000,1};
	send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, 12, &buf, p->id);
	

	ADD_TIMER_EVENT(p, long_time_min45_in_game, 0, now.tv_sec + 45*60);
	ADD_TIMER_EVENT(p, long_time_min10_in_game, 0, now.tv_sec + 10*60);
	ADD_TIMER_EVENT(p, long_time_min120_in_game, 0, now.tv_sec + 10);
	ADD_TIMER_EVENT(p, long_time_min1_in_game, 0, now.tv_sec + 60);
#ifndef TW_VER
	ADD_TIMER_EVENT(p, long_time_min15_in_game, 0, now.tv_sec + 3*60);
#endif
	if (IS_GUEST_ID(p->id)) {
		p->ver = FIRST_VERSION;
		enter_map(p, 1, 0,0);
		rsp_proto_login(p);
		DEBUG_LOG("VISITOR LOGIN\t[%u]", p->id);
		return 0;
	} else {
		return db_get_sprite_with_mail(p);
	}
}


/**
 * @brief init `default_login_maps`
 *
 * @return int, 0,success; -1 false
 */
int init_default_login_maps()
{
	int loop;
	for(loop = 0; loop < default_login_maps_num; loop++){
		default_login_maps[loop] = get_map(default_maps[loop]);
		if(default_login_maps[loop] == NULL)
			return -1;
	}
	return 0;
}

/**
 * @brief select a map with appropriate number of users from `default_login_maps` to login to
 *
 * @param sprite_t, the user that is logging in
 * @return map_id_t, the selected map id
 */
static inline map_id_t
select_default_login_map(sprite_t* p, uint32_t step)
{
	int loop;
	p->sub_grid = 0;
	if (check_recv_not_fini_task(p, 300)) {
        return 137;
	}
	if(!p->olcount)
		return 15;  // in castle
	if(p->exp < 15)
		return default_login_maps[0]->id;
	for(loop = 0; loop < default_login_maps_num; loop++){
		if(default_login_maps[loop]->sprite_num < 20)
			return default_login_maps[loop]->id;
	}
	return default_login_maps[rand()%default_login_maps_num]->id;
}

//------------------------ Callbacks -------------------------------
/**
 * @brief final login step, response login pkg and send offline msg to the user `p`
 *
 * @param sprite_t, the user that is logging in
 * @return int, 0 on success, otherwise -1
 */
int proc_final_login_step(sprite_t* p, uint32_t step)
{
	int i, j, k, count;

	p->login = 1;
	p->ver = FIRST_VERSION;
	enter_map(p, select_default_login_map(p, step), 0, 0);
	rsp_proto_login(p);

	i = 0;
	UNPKG_H_UINT32(p->session, k, i);
	UNPKG_H_UINT32(p->session, count, i);

	if (k > 1000 && count > 1000)
	{
		ERROR_LOG("k:%u count:%u uid: %u", k, count, p->id);
		return 0;
	}

	DEBUG_LOG("k:%u count:%u uid: %u", k, count, p->id);
	for (j = 0; j < count && i < k; j++) {
		int l;
		UNPKG_H_UINT32(p->session, l, i);
		if (l + i - 4 > k || l < 4) {
			ERROR_RETURN(("error len, id=%u, l=%d, i=%d, k=%d", p->id, l, i, k), -1);
		}
		send_to_self(p, p->session + i, l - 4, 0);
		//DEBUG_LOG ("OFFLINE MSG\t[%u %d %d %d]", p->id, l, j, count);
		i += l - 4;
	}
	p->sess_len = 0;

	return 0;
}

int check_session(sprite_t *p)
{
	login_session_t sess;
	if (unpkg_auth(p->session, p->sess_len, &sess) == -1) {
		return -1;
	}
	if ( verify_session(p, &sess) == -1 ) {
		notify_user_exit(p, -ERR_invalid_session, 1);
		return 0;
	}
	send_login_exit_info(p->id, 1, config_cache.bc_elem->online_id, get_now_tv()->tv_sec);
	return do_auth(p);
}

//------------------------ CMDs for proto.c ------------------------------------

int auth_cmd_old(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 22);

	const char* md5 = (void*)(body + 2);

	if (p->id > 0) {
		CHECK_BODY_LEN(bodylen, 38);
		if (verify_user_md5(md5, (void*)(body + 22)) != 0) {
			ERROR_RETURN(("Failed to Verify User Md5 Code: uid=%u", p->id), -1);
		}

		login_session_t sess;
		int fd = *(int*)p->session;
		if ( (fd < 0) || (unpkg_auth(body + 18, bodylen - 18, &sess) == -1) ) {
			return -1;
		}
		if ( verify_session(p, &sess) == -1 ) {
			notify_user_exit(p, -ERR_invalid_session, 1);
			return 0;
		}
	} else if (verify_visitor_md5(md5) != 0) {
		ERROR_RETURN(("Failed to Verify User Md5 Code: uid=%u", p->id), -1);
	}
	send_login_exit_info(p->id, 1, config_cache.bc_elem->online_id, get_now_tv()->tv_sec);
	DEBUG_LOG("USER LOGIN\t[%u]", p->id);
	return do_auth(p);
}


int auth_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 22);

	const char* md5 = (void*)(body + 2);
	sprite_t *v = add_sprite(p);
	if (p->id > 0) {
		CHECK_BODY_LEN(bodylen, 38);
		if (verify_user_md5(md5, (void*)(body + 22)) != 0) {
			ERROR_RETURN(("Failed to Verify User Md5 Code: uid=%u", p->id), -1);
		}
		memcpy(v->session, body + 18, bodylen - 18);
		v->sess_len = bodylen - 18;
		return check_session_by_session_server(p, 1, body + 22, 1);
	} else if (verify_visitor_md5(md5) != 0) {
		ERROR_RETURN(("Failed to Verify User Md5 Code: uid=%u", p->id), -1);
	}
	send_login_exit_info(p->id, 1, config_cache.bc_elem->online_id, get_now_tv()->tv_sec);
	DEBUG_LOG("USER LOGIN\t[%u]", p->id);
	return do_auth(v);
}

int check_session_by_session_server(sprite_t *p, int game_id, const uint8_t *body, int flag)
{
	struct session_server_data {
		uint32_t game_id;
		uint8_t	session[SESS_LEN];
		uint32_t flag;
	}__attribute__((packed)) data;

	data.game_id = game_id;
	memcpy(data.session, body, SESS_LEN);
	data.flag = flag;

#ifdef TW_VER
    return send_request_to_db(SVR_PROTO_CHECK_SESSION_TW, p, sizeof(struct session_server_data), &data, p->id);
#else
    return send_request_to_db(SVR_PROTO_CHECK_SESSION, p, sizeof(struct session_server_data), &data, p->id);
#endif


}

int check_session_by_session_server_tw_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	return send_request_to_db(SVR_PROTO_CHECK_USERID_EXSIT, p, 0, NULL, p->id);
}

int check_session_by_session_server_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	return send_request_to_db(SVR_PROTO_CHECK_USERID_EXSIT, p, 0, NULL, p->id);
}

int check_userid_exsit_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	send_login_exit_info(p->id, 1, config_cache.bc_elem->online_id, get_now_tv()->tv_sec);
	DEBUG_LOG("USER LOGIN\t[%u]", p->id);
	return do_auth(p);
}

static void notify_friend_user_login(uint32_t opid, const void* body, int len)
{
	CHECK_BODY_LEN_GE_VOID(len, 8);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)body, j);  //userid
	init_proto_head(msg, opid, j);

	uint32_t count = *(uint32_t*)(body + 4);
	CHECK_BODY_LEN_VOID(len, 8 + 4 * count);
	int loop;
	for (loop = 0; loop < count; loop++) {
		userid_t* uid = (uint32_t*)(body + 8 + loop * 4);
		sprite_t* p = get_sprite( *uid );
		if (p) {
			send_to_self(p, msg, j, 0);
		}
	}
}

int login_server_op(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);
	communicator_body_t* cbody = buf;

#define LOGIN_OP(op_, func_) \
		case (op_): (func_)(op_, cbody->body, len - 12); break

	switch (cbody->opid) {

		LOGIN_OP(PROTO_NOTIFY_FRIEND_USER_LOGIN, notify_friend_user_login);
	default:
		break;
	}
#undef LOGIN_OP

	return 0;
}

int get_online_time_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	int online_time;
	if (is_holiday(get_now_tm()->tm_wday)) {
		online_time = p->oltoday + (now.tv_sec - p->stamp);
		response_proto_uint32(p, PROTO_NOTIFY_ONLINE_TIME, online_time, 0);
	} else {
		online_time = p->oltoday + (now.tv_sec - p->stamp);
		response_proto_uint32(p, PROTO_NOTIFY_ONLINE_TIME, online_time, 0);
	}
	return 0;
}

