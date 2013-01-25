#include "battle.h"
#include "center.h"
#include "cli_login.h"
#include "pet.h"

#include "sns.h"
#include "code.h"
#include "items.h"
#include "homemap.h"
#include "mail.h"
#include "maze.h"
#include "skill.h"
#include "./proto/mole2_home.h"
#include "./proto/mole2_home_enum.h"

void chat_across_svr(sprite_t* p, uint8_t* body, uint32_t bodylen)
{
	if (send_to_switch(0, COCMD_chat_across_svr, bodylen, body, p->id) == -1) {
		//userid_t recvid = *(userid_t*)body; 
		//*(uint32_t*)body = bodylen;
		//send_request_to_db(0, recvid, dbproto_add_offline_msg, body, bodylen);
	}
}

void chat_msg_log(sprite_t* p, uint32_t recvid, const uint8_t* mesg, int msglen)
{
	uint8_t out[8912];
	int j = 0;
	PKG_H_UINT32(out, 7, j);
	PKG_H_UINT32(out, p->id, j);
	PKG_H_UINT32(out, recvid, j);
	PKG_H_UINT32(out, get_server_id(), j);
	if (p->tiles) {
		PKG_H_UINT32(out, high32_val(p->tiles->id), j);
		PKG_H_UINT32(out, low32_val(p->tiles->id), j);
	} else {
		PKG_H_UINT32(out, 0, j);
		PKG_H_UINT32(out, 0, j);
	}

	PKG_H_UINT32(out, get_now_tv()->tv_sec, j);
	PKG_H_UINT32(out, msglen, j);
	PKG_STR(out, mesg, j, msglen);
	send_udp_chat_msg_to_db(p, p->id, 0xF133, out, j);
}

int do_talk(sprite_t* p, const uint8_t mesg[], int msglen, uint32_t recvrid)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg + 4, p->id, i);
	PKG_STR(msg + 4, p->nick, i, sizeof(p->nick));
	PKG_UINT32(msg + 4, recvrid, i);
	PKG_UINT32(msg + 4, msglen, i);
	PKG_STR(msg + 4, mesg, i, msglen);
	init_proto_head(msg + 4, p->waitcmd, i);

	sprite_t* receiver;
	uint32_t itemid=0;
	if( recvrid == 2){
		if(p->tiles && !IS_TRADE_MAP(p->tiles->id)){
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1); 
		}
		//交易吆喝喇叭 需要使用物品
		if(cache_item_have_cnt(p, 350037, 1)){
			itemid=350037;
		}else if(cache_item_have_cnt(p, 360024, 1) ){
			itemid=360024;
		}
		if( itemid == 0 ){
			return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1); 
		}
	}
	if (recvrid == 0) {
		send_to_map(p, msg + 4, i, 1,1);
	} else if (recvrid == 1) {
		send_to_team(p, msg + 4, i, 1);
	} else if (recvrid == 2) {
		//交易吆喝喇叭
		*(uint32_t*)msg = recvrid;
		chat_across_svr(p, msg, i + 4);
		p->waitcmd=0;
		p->sess_len = 0;
		//send_to_self(p, msg + 4, i, 1);
	} else if ((receiver = get_sprite(recvrid))) {
		send_to_self(receiver, msg + 4, i, 0);
		send_to_self(p, msg + 4, i, 1);
	} else if (IS_NORMAL_ID(recvrid)) {
		*(uint32_t*)msg = recvrid;
		chat_across_svr(p, msg, i + 4);
		send_to_self(p, msg + 4, i, 1);
	} else {
		KERROR_LOG(p->id, "error id=%u", recvrid);
		return -1;
	}
	if( recvrid == 2){
		//交易吆喝喇叭
	   cache_reduce_kind_item(p, itemid, 1);   			
	   db_add_item(NULL, p->id, itemid, -1);
	   response_proto_uint32(p,proto_noti_user_consume_item,itemid,0,0);
	}
	chat_msg_log(p, recvrid, mesg, msglen);

	return 0;
}

int talk_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id, msglen;
	int j = 0;
	UNPKG_UINT32(body, id, j);
	UNPKG_UINT32(body, msglen, j);
	
	if (msglen < 2 || msglen > len - 8 || msglen > 4096)
		ERROR_RETURN(("talk len invalid id=%u %u %d", p->id, id, msglen), -1);

	uint8_t* ptmp = body;
	*(ptmp + len - 1) = '\0';
	
	CHECK_DIRTYWORD(p, body + 8);
	KDEBUG_LOG(p->id, "TALK\t\t[%u %d]", id, msglen);
	return do_talk(p, body + 8, msglen, id);
}

int show_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t show_type, direction;
	int j = 0;
	UNPKG_UINT32(body, show_type, j);
	UNPKG_UINT32(body, direction, j);

	p->base_action = show_type;
	p->direction = direction;
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, k);
	PKG_UINT32(msg, show_type, k);
	PKG_UINT32(msg, direction, k);
	init_proto_head(msg, p->waitcmd, k);
	
	send_to_map(p, msg, k, 1,1);
	return 0;
}

int text_notify_pkg_pic(uint8_t buf[], int cmd, uint32_t id, map_id_t map,
			const char nick[], int accept, uint32_t pic, int len, const char txt[])
{
	int i = sizeof(protocol_t);
	PKG_UINT32(buf, cmd, i);
    PKG_MAP_ID(buf, map, i);
	PKG_UINT32(buf, id, i);
	PKG_STR(buf, nick, i, 16);	
	PKG_UINT32(buf, accept, i);
	PKG_UINT32(buf, now.tv_sec, i);
	PKG_UINT32(buf, pic, i);
	PKG_UINT32(buf, len, i);
	PKG_STR(buf, txt, i, len);
	init_proto_head(buf, proto_cli_text_notify, i);
	return i;
}

void req_add_friend_notifier(sprite_t* p, userid_t frdid)
{
	char txt[256];
#ifdef TW_VER
	int j = sprintf(txt, "%s請求將您加為好友，您是否同意？", p->nick);
#else
	int j = sprintf(txt, "%s请求将您加为好友，您是否同意？", p->nick);
#endif

	sprite_t* s = get_sprite(frdid);
	j = text_notify_pkg(msg + 4, proto_cli_add_friend, p->id, 0, p->nick, 0, j, txt);
	if (s) {
		send_to_self(s, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = frdid;
		chat_across_svr(p, msg, j + 4);
	}
}

void req_del_friend_notifier(sprite_t* p, userid_t frdid)
{
	sprite_t* s = get_sprite(frdid);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg + 4, p->id, j);
	PKG_STR(msg + 4, p->nick, j, USER_NICK_LEN);
	init_proto_head(msg + 4, proto_cli_noti_friend_del, j);
	if (s) {
		send_to_self(s, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = frdid;
		chat_across_svr(p, msg, j + 4);
	}
}


void notify_add_friend_response(uint32_t id, sprite_t* rep, uint8_t accept)
{
	int j;
	char txt[256];
	sprite_t* initiator;

	switch (accept) {
	case 0:
		j = sprintf(txt, "%s拒绝了你的好友邀请.", rep->nick);
		break;
	case 1:
		j = sprintf(txt, "%s通过了你的好友邀请.", rep->nick);
		break;
	case 2:
		j = sprintf(txt, "%s设置为不接受好友邀请的状态.", rep->nick);
		break;
	case 3:
		j = sprintf(txt, "%s的好友已经达到了上限.", rep->nick);
		break;
	}

	KDEBUG_LOG (rep->id, "ADD FRIEND\t[%u]", id);
	j = text_notify_pkg(msg + 4, proto_cli_rsp_friend_add, rep->id, 0, rep->nick, accept, j, txt);
	if ((initiator = get_sprite(id))) {
		send_to_self(initiator, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = id;
		chat_across_svr(rep, msg, j + 4);
	}

}

static inline int
do_add_blacklist(sprite_t* p, uint32_t uid)
{
	send_request_to_db(0, uid, proto_db_del_friend, &p->id, 4);
	
	return db_add_bklist(p, uid);
}

static inline int
do_add_friend_req(sprite_t* p, uint32_t friend_id)
{
	req_add_friend_notifier(p, friend_id);
	response_proto_head(p, proto_cli_add_friend, 1, 0);

	KDEBUG_LOG(p->id, "FRIEND REQUEST\t[%u]", friend_id);
	return 0;
}

static inline int
do_add_friend_rsp(sprite_t* p, uint32_t initor, uint8_t accept)
{
	if (accept != 1) {
		response_proto_head(p, proto_cli_rsp_friend_add, 1, 0);
		notify_add_friend_response(initor, p, accept);
	} else {
		db_add_friend(0, p->id, initor);
		return db_add_friend(p, initor, p->id);
	}
	
	return 0;
}

int add_blacklist_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id;
	int i = 0;
	UNPKG_UINT32(body, id, i);

	CHECK_VALID_ID(id);

	KDEBUG_LOG (p->id, "ADD BKLIST\t[%u]", id);
	return do_add_blacklist(p, id);
}

int del_blacklist_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id;
	int i = 0;
	UNPKG_UINT32(body, id, i);

	CHECK_VALID_ID(id);
	KDEBUG_LOG (p->id, "DEL BKLIST\t[%u]", id);
	return db_del_bklist(p, id);
}

int get_blacklist_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	KDEBUG_LOG (p->id, "GET BLACKLIST");
	if (IS_GUEST_ID (p->id) || IS_BEAST_ID (p->id)) {
		int k = sizeof (protocol_t);
		PKG_UINT32 (msg, 0, k);
		init_proto_head (msg, proto_cli_get_bklist, k);
		return send_to_self (p, msg, k, 1);
	}
	return db_get_bklist(p);
}

int del_friend_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id;
	int i = 0;
	UNPKG_UINT32(body, id, i);
	CHECK_VALID_ID(id);
	
	KDEBUG_LOG (p->id, "DEL FRIEND\t[%u]", id);
	db_del_friend(NULL, id, p->id);
	req_del_friend_notifier(p, id);
	return db_del_friend(p, p->id, id);
}

int add_friend_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id;
	int i = 0;
	UNPKG_UINT32(body, id, i);
	CHECK_VALID_ID(id);

	KDEBUG_LOG(p->id, "ADD FRIND\t[%u]", id);
	return do_add_friend_req(p, id);
}

int reply_add_friend_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t initiator;
	uint8_t  accept;
	
	int i = 0;
	UNPKG_UINT32(body, initiator, i);
	UNPKG_UINT32(body, accept, i);
	
	CHECK_VALID_ID(initiator);

	KDEBUG_LOG(p->id, "REPLY ADD FRIEND\t[adder=%u accept=%d]", initiator, accept);
	return do_add_friend_rsp(p, initiator, accept);
}

struct stru_addrelation_md5 {
	uint32_t userid;
	uint32_t friendid;
	uint32_t time;
	uint32_t addtype;
}__attribute__((packed));

void do_add_relation_req(sprite_t* p, uint32_t uid, uint32_t type)
{	
    stru_addrelation_md5 add_friend_md5;
    uint8_t buf1[16],  buf2[40];
    add_friend_md5.userid = p->id;
    add_friend_md5.friendid = uid;
    add_friend_md5.time = time(NULL);
	add_friend_md5.addtype = type;
    des_encrypt_n(MDKEY, &add_friend_md5, (char *)buf1, 2);
    hex2str ((char *)buf1, 16, (char *)buf2);
    buf2[32] = '\0';
	
	sprite_t* s = get_sprite(uid);
	if (s) {
		cli_add_relation_out cli_out;
		cli_out.uid = p->id;
		memcpy(cli_out.nick, p->nick, sizeof(cli_out.nick));
		cli_out.type = type;
		memcpy(cli_out.session, buf2, sizeof(cli_out.session));
		send_msg_to_self(s, cli_add_relation_cmd, &cli_out, 0);
	} else {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg + 4, p->id, i);
		PKG_STR(msg + 4, p->nick, i, USER_NICK_LEN);
		PKG_UINT32(msg + 4, type, i);
		PKG_STR(msg + 4, buf2, i, 32);
		init_proto_head(msg + 4, cli_add_relation_cmd, i);
		*(uint32_t *)msg = uid;
		chat_across_svr(p, msg, i + 4);
	}
}

int cli_add_relation(sprite_t* p, Cmessage* c_in)
{
	cli_add_relation_in* p_in = P_IN;
	if (p->id == p_in->uid)
		return -1;

	sprite_t* s = get_sprite(p_in->uid);

	if (p->level >= 30) {
		if (p->relationship.count >= 1)
			return send_to_self_error(p, p->waitcmd, cli_err_pupil_cnt_max, 1);
		if (s && s->relationship.count >= 3)
			return send_to_self_error(p, p->waitcmd, cli_err_relation_cnt_max, 1);
		do_add_relation_req(p, p_in->uid, relation_pupil);
	} else if (p->level >= 10) {
		if (p->relationship.count >= 3)
			return send_to_self_error(p, p->waitcmd, cli_err_master_cnt_max, 1);
		if (s && s->relationship.count >= 1)
			return send_to_self_error(p, p->waitcmd, cli_err_relation_cnt_max, 1);
		do_add_relation_req(p, p_in->uid, relation_master);
	} else {
		KERROR_LOG(p->id, "lv not fit\t[%u]", p->level);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	cli_add_relation_out cli_out;
	cli_out.uid = p->id;
	return send_msg_to_self(p, cli_add_relation_cmd, &cli_out, 1);
}

int cli_reply_add_relation(sprite_t* p, Cmessage* c_in)
{
	cli_reply_add_relation_in* p_in = P_IN;

	char desbuf[16];
	str2hex(p_in->session, 32, desbuf);

	 struct stru_addrelation_md5 ak;
    des_decrypt_n(MDKEY, desbuf, (char *)&ak, 2);
    KDEBUG_LOG(p->id, "md5 decode userid:%u,friendid:%u,time:%u,tmp:%u",
            ak.userid,
            ak.friendid,
            ak.time,
            ak.addtype
            );

	if (p_in->uid != ak.userid || p->id != ak.friendid || p_in->type != ak.addtype) {
		KERROR_LOG(p->id, "verify fail\t[%u %u %u]", ak.userid, p_in->type, ak.addtype);
		return -1;
	}

	switch (p_in->type) {
	case relation_master:
		CHECK_LV_FIT(p, p, 30, MAX_SPRITE_LEVEL);
		if (p->relationship.count >= 1) {
			KERROR_LOG(p->id, "pupil cnt max\t[%u]", p->relationship.count);
			return send_to_self_error(p, p->waitcmd, cli_err_pupil_cnt_max, 1);
		}
		break;
	case relation_pupil:
		CHECK_LV_FIT(p, p, 10, 29);
		if (p->relationship.count >= 3) {
			KERROR_LOG(p->id, "pupil cnt max\t[%u]", p->relationship.count);
			return send_to_self_error(p, p->waitcmd, cli_err_pupil_cnt_max, 1);
		}
		break;
	default:
		KERROR_LOG(p->id, "type fail impossible\t[%u]", p_in->type);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	mole2_add_relation_uid_in db_in;
	db_in.flag = p_in->type % 2 + 1;
	db_in.uid = p->id;
	KDEBUG_LOG(p_in->uid, "ADD RELATION DB\t[%u %u]", p->level, db_in.flag);
	return send_msg_to_db(p, p_in->uid, mole2_add_relation_uid_cmd, &db_in);
}

int mole2_add_relation_uid(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	KDEBUG_LOG(p->id, "ADD RELATION CALLBACK\t[%u]", id);
	if (p->id != id) {
		CHECK_DBERR(p, ret);
		mole2_add_relation_uid_out* p_out = P_OUT;
		mole2_add_relation_uid_in db_in;
		db_in.flag = p_out->flag % 2 + 1;
		db_in.uid = id;
		*(uint32_t *)p->session = p_out->flag;
		*(uint32_t *)(p->session + 4) = id;
		return send_msg_to_db(p, p->id, mole2_add_relation_uid_cmd, &db_in);
	} else if (ret) {
		KERROR_LOG(p->id, "need rollback\t[%u %d]", *(uint32_t *)(p->session + 4), ret);
		mole2_del_relation_uid_in db_in;
		db_in.flag = *(uint32_t *)p->session;
		db_in.uid = p->id;
		db_in.op_uid = 0;
		send_msg_to_db(NULL, *(uint32_t *)(p->session + 4), mole2_del_relation_uid_cmd, &db_in);
		return send_to_self_error(p, p->waitcmd, \
			db_in.flag == relation_pupil ? cli_err_pupil_cnt_max : cli_err_master_cnt_max, 1);
	} else {
		uint32_t uid = *(uint32_t *)(p->session + 4);
		KDEBUG_LOG(p->id, "ADD RELATION SUCC\t[%u]", uid);
		mole2_add_relation_uid_out* p_out = P_OUT;
		p->relationship.flag = p_out->flag;
		p->relationship.users[p->relationship.count].uid = uid;
		p->relationship.users[p->relationship.count].day = get_now_tv()->tv_sec;
		p->relationship.users[p->relationship.count].intimacy = 0;
		p->relationship.count ++;
		response_proto_uint32_uint32(p, p->waitcmd, p->id, p_out->flag,1, 0);
		noti_other_relation_update(p, uid, relation_add);
		return 0;
	}
}

int cli_del_relation(sprite_t* p, Cmessage* c_in)
{
	cli_del_relation_in* p_in = P_IN;
	for (uint32_t loop = 0; loop < p->relationship.count; loop ++) {
		if (p_in->uid == p->relationship.users[loop].uid) {
			if (check_same_period(p->relationship.users[loop].day, repeat_day, 0))
				return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
			*(uint32_t *)p->session = p_in->uid;
			mole2_del_relation_uid_in db_in;
			db_in.flag = p->relationship.flag;
			db_in.uid = p_in->uid;
			db_in.op_uid = p->id;
			return send_msg_to_db(p, p->id, mole2_del_relation_uid_cmd, &db_in);
		}
	}

	KERROR_LOG(p->id, "uid not find\t[%u]", p_in->uid);
	return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
}

int mole2_del_relation_uid(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_del_relation_uid_out* p_out = P_OUT;
	p->relationship.flag = p_out->flag;
	p->relationship.exp = p_out->exp;
	p->relationship.graduation = p_out->graduation;
	p->relationship.intimacy = p_out->total_val;
	p->relationship.count = p_out->relations.size();
	for (uint32_t loop = 0; loop < p_out->relations.size(); loop ++) {
		p->relationship.users[loop].uid = p_out->relations[loop].uid;
		p->relationship.users[loop].day = p_out->relations[loop].day;
		p->relationship.users[loop].intimacy = p_out->relations[loop].val;
	}

	mole2_del_relation_uid_in db_in;
	db_in.flag = p->relationship.flag % 2 + 1;
	db_in.uid = p->id;
	db_in.op_uid = p->id;
	uint32_t uid = *(uint32_t *)p->session;
	send_msg_to_db(NULL, uid, mole2_del_relation_uid_cmd, &db_in);
	response_proto_uint32(p, p->waitcmd, uid, 1, 0);
	noti_other_relation_update(p, uid, relation_del);
	return 0;
}

int cli_get_relationship(sprite_t* p, Cmessage* c_in)
{
	cli_get_relationship_in* p_in = P_IN;
	return send_request_to_db(p, p_in->uid, mole2_get_relation_cmd, NULL, 0);
}

void noti_reply_add_relation(sprite_t* p)
{
	int i = sizeof(protocol_t);
	uint8_t out[64];
	memcpy(out + i, p->session, 24);
	i += 24;
	init_proto_head(out, p->waitcmd, i);
	send_to_self(p, out, i, 1);
}

int mole2_get_relation(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_relation_out* p_out = P_OUT;

	if (p->id == id) {
		p->relationship.flag = p_out->flag;
		p->relationship.exp = p_out->exp;
		p->relationship.graduation = p_out->graduation;
		p->relationship.intimacy = p_out->total_val;
		p->relationship.count = p_out->relations.size();
		for (uint32_t loop = 0; loop < p_out->relations.size(); loop ++) {
			p->relationship.users[loop].uid = p_out->relations[loop].uid;
			p->relationship.users[loop].day = p_out->relations[loop].day;
			p->relationship.users[loop].intimacy = p_out->relations[loop].val;
		}

		if (IS_MASTER_HONOR(p->honor)) {
			uint32_t masterlv = calc_relation_level(p->relationship.exp);
			if (p->honor < relation_level[masterlv - 1][1]) {
				set_user_honor(p, relation_level[masterlv - 1][1]);
				db_set_user_honor(NULL, p->id, p->honor);
				notify_user_honor_up(p, 0, 0, 1);
			}
		}
	}

	cli_get_relationship_out cli_out;

	switch (p->waitcmd) {
	case cli_get_relationship_cmd:
		cli_out.uid = id;
		cli_out.flag = p_out->flag;
		cli_out.exp = p_out->exp;
		cli_out.level = calc_relation_level(cli_out.exp);
		cli_out.title = relation_level[cli_out.level - 1][1];
		cli_out.graduation = p_out->graduation;
		cli_out.total_val = p_out->total_val;
		for (uint32_t loop = 0; loop < p_out->relations.size(); loop ++) {
			cli_out.relations.push_back(p_out->relations[loop]);
		}
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	case proto_cli_noti_relation_up:
		noti_reply_add_relation(p);
		return 0;
	default:
		return -1;
	}
}

int noti_relation_up_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	memcpy(p->session, body, len);
	return send_msg_to_db(p, p->id, mole2_get_relation_cmd, NULL);
}

int cli_get_relation_reward(sprite_t* p, Cmessage* c_in)
{
	cli_get_relation_reward_in* p_in = P_IN;
	uint32_t count = 0;
	switch (p_in->itemid) {
	case item_id_exp:
		if (p->experience >= MAX_TEMP_EXP_EX - 1)
			return send_to_self_error(p, p->waitcmd, cli_err_exp_too_much, 1);
		count = p->relationship.intimacy * 5;
		break;
	case item_id_xiaomee:
		count = p->relationship.intimacy * 5;
		break;
	case item_id_pet_expbox:
		count = p->relationship.intimacy * 50;
		break;
	case item_id_skill_expbox:
		count = p->relationship.intimacy;
		break;
	default:
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}

	*(uint32_t *)p->session = p_in->itemid;
	*(uint32_t *)(p->session + 4) = count;

	if (!count) {
		response_proto_uint32_uint32(p, p->waitcmd, p_in->itemid, count, 1, 0);
		return 0;
	}

	mole2_add_relation_val_in db_in;
	db_in.uid = 0;
	db_in.addval = -p->relationship.intimacy;
	return send_msg_to_db(p, p->id, mole2_add_relation_val_cmd, &db_in);
}

int mole2_add_relation_val(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_add_relation_val_out* p_out = P_OUT;
	if (p_out->uid) {
		KERROR_LOG(p->id, "unexpected uid\t[%u]", p_out->uid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	} else {
		p->relationship.intimacy = p_out->value;
		uint32_t itemid = *(uint32_t *)p->session;
		uint32_t count = *(uint32_t *)(p->session + 4);
		switch(itemid) {
		case item_id_exp:
			count = add_exp_to_sprite(p, count);
			break;
		case item_id_xiaomee:
			p->xiaomee += count;
			db_add_xiaomee(NULL, p->id, count);
			break;
		case item_id_pet_expbox:
			p->expbox += count;
			db_set_expbox(NULL, p->id, p->expbox);
			break;
		case item_id_skill_expbox:
			p->skill_expbox += count;
			db_add_skill_expbox(NULL, p->id, count);
			break;
		}
		response_proto_uint32_uint32(p, p->waitcmd, itemid, count, 1, 0);
	}
	return 0;
	
}

int get_bklist_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t count, bkuser;
	uint8_t out[1024];

	int i = 0;
	UNPKG_H_UINT32(buf, count, i);
	CHECK_BODY_LEN(len, 4 + count * 4);
	CHECK_INT_LE(count, 100);

	int k = sizeof (protocol_t);
	PKG_UINT32 (out, count, k);

	for (uint32_t j = 0; j < count; j++) {
		UNPKG_H_UINT32 (buf, bkuser, i);
		PKG_UINT32 (out, bkuser, k);
	}

	init_proto_head (out, p->waitcmd, k);
	return send_to_self (p, out, k, 1);
}

int add_friend_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	KDEBUG_LOG(p->id, "REPLY ADD FRIEND CALLBACK\t[%u]", id);
	response_proto_head(p, proto_cli_rsp_friend_add, 1, 0);
	notify_add_friend_response(id, p, 1);
	return 0;
}

int cli_get_user_simple(sprite_t* p, Cmessage* c_in)
{
	cli_get_user_simple_in* p_in = P_IN;
	sprite_t* s = get_sprite(p_in->uid);
	if (s) {
		cli_get_user_simple_out cli_out;
		cli_out.uid = p_in->uid;
		memcpy(cli_out.nick, s->nick, USER_NICK_LEN);
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	}
	return db_get_other_user_info(p, p_in->uid);
}

int get_user_info_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id;
	int i = 0;
	UNPKG_UINT32(body, id, i);
	CHECK_VALID_ID(id);
	KDEBUG_LOG(p->id, "GET USER INFO\t[%u]", id);

	sprite_t* lp = get_sprite(id);
	if (lp) {
		int i = sizeof(protocol_t);
		i += pkg_sprite_simply(lp, msg + i);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}
	return db_get_other_user_info(p, id);
}

int pkg_sprite_simply_temp(const sprite_t *p, body_cloth_t* pc, uint32_t count, uint8_t *buf)
{
	int i = 0;
	PKG_UINT32(buf, p->id, i);
	PKG_STR(buf, p->nick, i, 16);
	PKG_UINT32(buf, p->flag, i);
	PKG_UINT32(buf, p->viplv, i);
	PKG_UINT32(buf, p->vipexp, i);
	PKG_UINT32(buf, p->vip_begin, i);
	PKG_UINT32(buf,	p->vip_end, i);
	PKG_UINT32(buf, p->hero_team, i);
	PKG_UINT32(buf, p->color, i);
	PKG_UINT8(buf, p->race, i);
	PKG_UINT8(buf, p->prof, i);
	PKG_UINT32(buf, p->prof_lv, i);
	PKG_UINT32(buf, p->honor, i);
	PKG_UINT32(buf, p->pkpoint_total, i);
	PKG_UINT32(buf, p->level, i);
	PKG_UINT32(buf, p->hp, i);
	PKG_UINT32(buf, p->mp, i);
	PKG_UINT32(buf, p->hp_max, i);
	PKG_UINT32(buf, p->mp_max, i);
	PKG_UINT32(buf, p->injury_lv, i);
	PKG_UINT32(buf, p->shapeshifting.pettype, i);

	PKG_MAP_ID(buf, (p->tiles ? p->tiles->id : 0), i);
	PKG_UINT32(buf, p->posX, i);
	PKG_UINT32(buf, p->posY, i);
	PKG_UINT32(buf, p->base_action, i);
	PKG_UINT32(buf, p->advance_action, i);
	PKG_UINT8(buf, p->direction, i);
	PKG_UINT32(buf, (p->btr_team ? p->btr_team->teamid: 0), i);
	PKG_UINT32(buf, (p->btr_team ? get_user_in_bt(p->btr_team, p->id) : 0), i);
	PKG_UINT32(buf, (p->btr_team ? p->btr_team->players[0]->team_registerd : p->team_registerd), i);
	PKG_STR(buf, (p->btr_team ? p->btr_team->players[0]->team_nick : p->team_nick), i, TEAM_NICK_LEN);
	PKG_UINT8(buf, team_switch_state(p), i);
	PKG_UINT8(buf, p->pk_switch, i);
	PKG_UINT8(buf, p->busy_state, i);
	i += pack_activity_info(p, buf + i);
	PKG_UINT8(buf, count, i);
	for (uint32_t loop = 0; loop < count; loop ++) {
		PKG_UINT32(buf, pc[loop].clothid, i);
		PKG_UINT32(buf, pc[loop].clothtype, i);
		PKG_UINT16(buf, pc[loop].clothlv, i);
	}
	i += pkg_pet_follow(p->p_pet_follow, buf + i);
	
	return i;
}

int pkg_other_sprite_detail_temp(const sprite_t* p, body_cloth_t* pc, uint32_t count, uint8_t* buf)
{
	int i = 0;

	PKG_UINT32(buf, p->id, i);
	PKG_STR(buf, p->nick, i, 16);
	PKG_UINT32(buf, p->flag, i);
	PKG_UINT32(buf, p->viplv, i);
	PKG_UINT32(buf, p->vipexp, i);
	PKG_UINT32(buf, p->vip_begin, i);
	PKG_UINT32(buf,	p->vip_end, i);
	PKG_UINT32(buf, p->hero_team, i);
	PKG_UINT32(buf, p->color, i);
	PKG_UINT32(buf, p->register_time, i);
	PKG_UINT8(buf, p->race, i);
	PKG_UINT8(buf, p->prof, i);
	PKG_UINT32(buf, p->prof_lv, i);
	PKG_UINT32(buf, p->honor, i);
	PKG_UINT32(buf, p->xiaomee, i);
	PKG_UINT32(buf, p->pkpoint_total, i);
	PKG_UINT32(buf, p->energy, i);
	PKG_UINT32(buf, p->level, i);
	PKG_UINT32(buf, p->experience, i);
	PKG_UINT16(buf, p->physique, i);
	PKG_UINT16(buf, p->strength, i);
	PKG_UINT16(buf, p->endurance, i);
	PKG_UINT16(buf, p->quick, i);
	PKG_UINT16(buf, p->intelligence, i);
	PKG_UINT16(buf, p->attr_addition, i);
	PKG_UINT32(buf, p->hp, i);
	PKG_UINT32(buf, p->mp, i);

	PKG_UINT8(buf, 0, i);
	PKG_UINT8(buf, 0, i);
	PKG_UINT8(buf, 0, i);
	PKG_UINT8(buf, 0, i);
	PKG_UINT32(buf, p->injury_lv, i);
	PKG_UINT32(buf, p->shapeshifting.pettype, i);

	PKG_MAP_ID(buf, (p->tiles ? p->tiles->id : 0), i);
	PKG_UINT32(buf, p->posX, i);
	PKG_UINT32(buf, p->posY, i);
	PKG_UINT32(buf, p->base_action, i);
	PKG_UINT32(buf, p->advance_action, i);
	PKG_UINT8(buf, p->direction, i);
	PKG_UINT8(buf, p->in_front, i);
	PKG_UINT32(buf, (p->btr_team ? p->btr_team->teamid: 0), i);
	PKG_UINT32(buf, (p->btr_team ? get_user_in_bt(p->btr_team, p->id) : 0), i);

	PKG_UINT32(buf, p->hp_max, i);
	PKG_UINT32(buf, p->mp_max, i);

	PKG_UINT16(buf, p->attack, i);
	PKG_UINT16(buf, p->defense, i);
	PKG_UINT16(buf, p->mdefense, i);
	PKG_UINT16(buf, p->speed, i);
	PKG_UINT16(buf, p->spirit, i);
	PKG_UINT16(buf, p->resume, i);
	PKG_UINT16(buf, p->hit_rate < 0 ? 0 : p->hit_rate, i);
	PKG_UINT16(buf, p->avoid_rate < 0 ? 0 : p->avoid_rate, i);
	PKG_UINT16(buf, p->bisha < 0 ? 0 : p->bisha, i);
	PKG_UINT16(buf, p->fight_back < 0 ? 0 : p->fight_back, i);

	PKG_UINT16(buf, p->rpoison, i);
	PKG_UINT16(buf, p->rlithification, i);
	PKG_UINT16(buf, p->rlethargy, i);
	PKG_UINT16(buf, p->rinebriation, i);
	PKG_UINT16(buf, p->rconfusion, i);
	PKG_UINT16(buf, p->roblivion, i);
	
	i += pack_activity_info(p, buf + i);
	PKG_UINT8(buf, count, i);
	for (uint32_t loop = 0; loop < count; loop ++) {
		i += pkg_cloth(pc + loop, buf + i);
	}
	
	return i;
}


int get_other_user_info_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	KDEBUG_LOG(p->id, "GET USER INFO CALLBACK\t[%u]", id);
	sprite_t user_info;
	sprite_t* lp = &user_info;
	
	memset(lp, 0, sizeof(sprite_t));
	lp->id = id;

	int slen = sizeof(get_sprite_rsp_t);
	get_sprite_rsp_t* rsp = (get_sprite_rsp_t *)buf;
	memcpy(&lp->nick, rsp, slen);
	lp->busy_state = be_idle;

	UNPKG_H_UINT32(buf, lp->vip_auto, slen);
	UNPKG_H_UINT32(buf, lp->vip_exp_ex, slen);
	UNPKG_H_UINT32(buf, lp->vip_base_exp, slen);
	UNPKG_H_UINT32(buf, lp->vip_end, slen);
	UNPKG_H_UINT32(buf, lp->vip_begin, slen);
	UNPKG_H_UINT32(buf, lp->vip_time_1, slen);
	UNPKG_H_UINT32(buf, lp->vip_time_2, slen);
	UNPKG_H_UINT32(buf, lp->draw_cnt_gift, slen);
	lp->draw_cnt_gift = lp->draw_cnt_gift / 86400 / 2;

	UNPKG_H_UINT32(buf, lp->vip_time_3, slen);

	UNPKG_H_UINT32(buf, lp->pkpoint_total, slen);

	recalc_vip_exp_level(lp);

	uint32_t cloth_cnt;
	UNPKG_H_UINT32(buf, cloth_cnt, slen);
	CHECK_BODY_LEN(len, slen + cloth_cnt * sizeof(body_cloth_t));
	cache_set_body_attr_temp(lp, (body_cloth_t *)(buf + slen), cloth_cnt);
	modify_sprite_second_attr(lp);
	int i = sizeof(protocol_t);
	switch(p->waitcmd) {
		case proto_cli_get_user_info:
			i += pkg_sprite_simply_temp(lp, (body_cloth_t *)(buf + slen), cloth_cnt, msg + i);
			break;
		case proto_cli_get_user_detail:
			i += pkg_other_sprite_detail_temp(lp, (body_cloth_t *)(buf + slen), cloth_cnt, msg + i);
			break;
		case cli_get_user_simple_cmd:
			cli_get_user_simple_out cli_out;
			cli_out.uid = lp->id;
			memcpy(cli_out.nick, lp->nick, USER_NICK_LEN);
			return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}
int get_user_detail_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t id;
	int i = 0;
	UNPKG_UINT32(body, id, i);
	CHECK_VALID_ID(id);
	KDEBUG_LOG(p->id, "GET USER DETAIL\t[%u]", id);

	sprite_t* lp = get_sprite(id);
	if (lp) {
		i = sizeof(protocol_t);
		i += pkg_sprite_detail(lp, msg + i, (p->id == id));
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}
	return db_get_other_user_info(p, id);
}

int set_user_nick_cmd(sprite_t *p, uint8_t *body, uint32_t bodylen)
{
	char nick[USER_NICK_LEN];
	memcpy(nick, body, USER_NICK_LEN);
	nick[USER_NICK_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, nick);
	if (strlen(nick) == 0) {
		ERROR_RETURN(("set user nick null %u", p->id), -1);
	}
	return send_request_to_db(p, p->id, proto_db_set_user_nick, nick, USER_NICK_LEN);
}

int set_user_nick_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	int i = sizeof(protocol_t);
	memcpy(p->nick, buf, USER_NICK_LEN);
	p->nick[USER_NICK_LEN - 1] = '\0';
	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, p->nick, i, USER_NICK_LEN);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1,1);
	return 0;
}

int set_user_color_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t itemid;
	int j = 0;
	UNPKG_UINT32(body, itemid, j);
	KDEBUG_LOG(p->id, "USER SET COLOR\t[%u]", itemid);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_LV_FIT(p, p, pni->minlv, pni->maxlv);
	CHECK_ITEM_FUNC(p, pni, item_for_set_color);

	if(!ISVIP(p->flag)) {
		CHECK_ITEM_EXIST(p, itemid, 1);
		*(uint32_t *)p->session = itemid;
	}
	
	return send_request_to_db(p, p->id, proto_db_set_user_color, &pni->color, 4);
}

int set_user_color_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	p->color = *(uint32_t *)buf;

	if(ISVIP(p->flag)) {
		response_proto_uint32_uint32(p, p->waitcmd, p->id, p->color, 1, 1);
		return 0;
	}

	uint32_t itemid = *(uint32_t *)p->session;
	db_add_item(NULL, p->id, itemid, -1);
	cache_reduce_kind_item(p, itemid, 1);
	response_proto_uint32_uint32(p, p->waitcmd, p->id, p->color, 1, 1);
	return 0;
}

static inline int
unpkg_users_online(const uint8_t body[], int bodylen, int* count, uint32_t* uin)
{
	int i = 0;
	//UNPKG_UINT8(body, *return_all, i);
	UNPKG_UINT32(body, *count, i);
	if (*count > SPRITE_FRIENDS_MAX) {
		ERROR_RETURN(("too many users, count=%d", *count), -1);
	}
	CHECK_BODY_LEN(*count * 4 + 4, bodylen);

	int j = 0;
	for (; j != *count; ++j) {
		UNPKG_UINT32(body, uin[j], i);
		CHECK_VALID_ID(uin[j]);
	}	
	return 0;
}

static inline void
pack_online_pkg(sprite_t* p, uint32_t friend_cnt)
{
	int idx = sizeof(protocol_t);
	if (friend_cnt) {
		//PKG_UINT16(p->session, 1, idx); // online cnt
		//PKG_UINT16(p->session, 0, idx); // online id == 0 --> same svr as p
		PKG_UINT32(p->session, friend_cnt, idx);
	} else {
		//PKG_UINT16(p->session, 0, idx); // online cnt
		PKG_UINT32(p->session, friend_cnt, idx);
		p->sess_len = idx;
	}
}

int do_users_online(sprite_t* p, int count, uint32_t* uin)
{
	int i, k = 4;
	uint8_t buf[(4 * SPRITE_FRIENDS_MAX) + 4];
	uint32_t friend_cnt = 0;
	p->sess_len = sizeof(protocol_t) + 4;
	for (i = 0; i != count; ++i) {
		sprite_t* s = get_sprite(uin[i]);
		if (s) {
			PKG_UINT32(p->session, uin[i], p->sess_len);
			PKG_UINT32(p->session, get_server_id(), p->sess_len);
			PKG_MAP_ID(p->session, s->tiles ? s->tiles->id : 0, p->sess_len);
			++friend_cnt;
		} else {
			PKG_H_UINT32(buf, uin[i], k);
		}
	}

	pack_online_pkg(p, friend_cnt);
	if ( k > 4 ) {		
		*((uint32_t*)buf) = (k - 4) / 4;
		chk_online_across_svr(p, buf, k);
	} else {
		chk_online_across_svr_callback(p, p->id, 0, 0, 0);
	}

	return 0;
}

int users_online_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int count;
	uint32_t uin[SPRITE_FRIENDS_MAX];
	if (unpkg_users_online(body, len, &count, uin) == -1)
		return -1;

	KDEBUG_LOG(p->id, "CHK IF USERS ONLINE\t[cnt=%d]", count);
	return do_users_online(p, count, uin);
}

uint32_t get_cure_injury_cost(sprite_t* p)
{
	return 0;
	uint32_t cost = p->level < 10 ? 0 : p->level;
	switch (p->injury_lv) {
		case INJURY_NONE:
			return 0;
		case INJURY_WHITE:
			return 4 * cost;
		case INJURY_YELLOW:
			return 6 * cost;
		case INJURY_RED:
			return 10 * cost;
	}
	return 0;
}

uint32_t cure_all_and_calc_xiaomee(sprite_t* p)
{
	uint32_t hpmax, mpmax, count = 0, need_cure = 0, xiaomee = 0;;
	if (p->injury_lv) {
		uint32_t old_ratio = get_injury_effect(p->injury_lv);
		uint32_t new_ratio = get_injury_effect(INJURY_NONE);
		uint32_t old_hpmax = (8 * p->physique + 2 * p->strength + 3 * p->endurance + 3 * p->quick + p->intelligence) * old_ratio / 10 + 20;
		uint32_t old_mpmax = (p->physique + 2 * p->strength + 2 * p->endurance + 2 * p->quick + 10* p->intelligence) * old_ratio / 10 + 20;
		hpmax = (8 * p->physique + 2 * p->strength + 3 * p->endurance + 3 * p->quick + p->intelligence) * new_ratio / 10 + 20;
		mpmax = (p->physique + 2 * p->strength + 2 * p->endurance + 2 * p->quick + 10* p->intelligence) * new_ratio / 10 + 20;
		hpmax = p->hp_max + (hpmax - old_hpmax);
		mpmax = p->mp_max + (mpmax - old_mpmax);
		xiaomee += get_cure_injury_cost(p);
		need_cure = 1;
	} else {
		hpmax = p->hp_max;
		mpmax = p->mp_max;
	}

	if (p->hp < hpmax) {
		//xiaomee += p->level < 10 ? 0 : (hpmax - p->hp) / 2;
		xiaomee +=  (hpmax - p->hp) / 2;
		need_cure = 1;
	}

	if (p->mp < mpmax) {
		//xiaomee += p->level < 10 ? 0 : (IS_WIZARD(p) ? (mpmax - p->mp) / 2 : (mpmax - p->mp));
		xiaomee += (IS_WIZARD(p) ? (mpmax - p->mp) / 2 : (mpmax - p->mp));
		need_cure = 1;
	}

	int len = 8;
	uint8_t out[256];
	if (need_cure) {
		count ++;
		PKG_H_UINT32(out, 0, len);
		PKG_H_UINT32(out, hpmax, len);
		PKG_H_UINT32(out, mpmax, len);
		PKG_H_UINT32(out, INJURY_NONE, len);
	}

	int loop;
	for (loop = 0; loop < p->pet_cnt; loop ++) {
		need_cure = 0;
		pet_t* pp = &p->pets_inbag[loop];
		if (pp->injury_lv) {
			uint32_t new_ratio = get_injury_effect(INJURY_NONE);
			hpmax = ROUND_1000((8 * pp->physi100 + 2 * pp->stren100 + 3 * pp->endur100 + 3 * pp->quick100 + pp->intel100) * new_ratio) + 20;
			mpmax = ROUND_1000((pp->physi100 + 2 * pp->stren100 + 2 * pp->endur100 + 2 * pp->quick100 + 10* pp->intel100) * new_ratio) + 20;
			need_cure = 1;
		} else {
			hpmax = pp->hp_max;
			mpmax = pp->mp_max;
		}

		if (pp->hp < hpmax || pp->mp < mpmax) {
			xiaomee += pp->level < 10 ? ((100 - pp->level) / 4 + pp->level - 20) : 0;
			//xiaomee += (pp->hp_max-pp->hp)/2;
			//xiaomee += (pp->mp_max-pp->mp)/2;
			need_cure = 1;
		}

		if (need_cure) {
			count ++;
			PKG_H_UINT32(out, pp->petid, len);
			PKG_H_UINT32(out, hpmax, len);
			PKG_H_UINT32(out, mpmax, len);
			PKG_H_UINT32(out, INJURY_NONE, len);
		}
	}
    xiaomee=0;//free now 
	if (p->xiaomee < xiaomee)
		return cli_err_xiaomee_not_enough;

	if (count) {
		int i = 0;
		PKG_H_UINT32(out, count, i);
		PKG_H_UINT32(out, -xiaomee, i);
		return send_request_to_db(p, p->id, proto_db_cure_all, out, len);
	}

	return cli_err_not_need_cure;

}

int user_cure_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t type;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	CHECK_USER_IN_MAP(p, 0);

	/*
	if (!ISVIP(p->flag) && p->level > 15 && MAP_HAS_BEAST(p->tiles)) {
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	*/

	uint32_t xiaomee = 0, hp = 0, mp = 0, injurylv = p->injury_lv;
	int ret = 0;
	switch(type) {
		case 1 :
			if (p->hp >= p->hp_max) {
				KERROR_LOG(p->id, "user cure err: not need cure [%u %u]", p->hp, p->hp_max);
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
			}
			
			hp = p->hp_max - p->hp;
			//xiaomee = p->level < 10 ? 0 : hp / 2;
			xiaomee = hp;
			break;
		case 2:
			if (p->mp >= p->mp_max)
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
			
			mp = p->mp_max - p->mp;
			//xiaomee = p->level < 10 ? 0 : (IS_WIZARD(p) ? mp/ 2 : mp);
			//剑士、弓术士、忍者每恢复1点MP，扣除1星豆
			//魔法师、传教士每恢复2点MP，扣除1星豆
			xiaomee = IS_WIZARD(p)? mp/2:mp;
			break;
		case 3:
			if (!p->injury_lv)
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);
			xiaomee = get_cure_injury_cost(p);
			injurylv = INJURY_NONE;
			break;
        case 4:
            if (!p->injury_lv && p->hp >= p->hp_max && p->mp >= p->mp_max)
				return send_to_self_error(p, p->waitcmd, cli_err_not_need_cure, 1);

			if (injurylv) {
				injurylv = INJURY_NONE;
				uint32_t old_ratio = get_injury_effect(p->injury_lv);
				uint32_t new_ratio = get_injury_effect(injurylv);
				uint32_t old_hpmax = (8 * p->physique + 2 * p->strength + 3 * p->endurance + 3 * p->quick + p->intelligence) * old_ratio / 10 + 20;
				uint32_t old_mpmax = (p->physique + 2 * p->strength + 2 * p->endurance + 2 * p->quick + 10* p->intelligence) * old_ratio / 10 + 20;
				uint32_t new_hpmax = (8 * p->physique + 2 * p->strength + 3 * p->endurance + 3 * p->quick + p->intelligence) * new_ratio / 10 + 20;
				uint32_t new_mpmax = (p->physique + 2 * p->strength + 2 * p->endurance + 2 * p->quick + 10* p->intelligence) * new_ratio / 10 + 20;
				hp = p->hp_max + (new_hpmax - old_hpmax) - p->hp;
				mp = p->mp_max + (new_mpmax - old_mpmax) - p->mp;
				xiaomee = get_cure_injury_cost(p);	
			} else {
				hp = p->hp_max > p->hp ? (p->hp_max - p->hp) : 0;
				mp = p->mp_max > p->mp ? (p->mp_max - p->mp) : 0;
			}

			//xiaomee += p->level < 10 ? 0 : (hp / 2 + IS_WIZARD(p) ? mp / 2 : mp);
			xiaomee += hp / 2 + IS_WIZARD(p) ? mp / 2 : mp;
            break;
		case 5:
			ret = cure_all_and_calc_xiaomee(p);
			if (ret)
				return send_to_self_error(p, p->waitcmd, ret, 1);
			return 0;
		//case 6:
			//ret = cure_all_and_calc_xiaomee(p);
			//if (ret)
				//p->waitcmd=0;
			//return 0;
		default:
			ERROR_RETURN(("cli user cure type err: id=%u type=%u", p->id, type), -1);
	}

	KDEBUG_LOG(p->id, "CURE USER\t[%u %u %u %u %u %u %u %u %u]", \
		type, p->level, p->xiaomee, xiaomee, p->hp, hp, p->mp, mp, injurylv);

	CHECK_XIAOMEE_ENOUGH(p, xiaomee);
	if( ISVIP(p->flag) ){
		xiaomee=0;	
	}	
	*(uint32_t *)p->session = -xiaomee;
	return db_add_hp_mp(p, p->id, hp, mp, injurylv);
}

int rsponse_user_cure(sprite_t* p, uint32_t cost)
{
	uint8_t out[128];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->xiaomee, i);
	PKG_UINT32(out, cost, i);
	PKG_UINT32(out, 1, i);
	PKG_UINT32(out, 0, i);
	PKG_UINT32(out, p->hp, i);
	PKG_UINT32(out, p->hp_max, i);
	PKG_UINT32(out, p->mp, i);
	PKG_UINT32(out, p->mp_max, i);
	PKG_UINT32(out, p->injury_lv, i);
	if (p->btr_team) {
		notify_team_state(p->btr_team);
	}

	init_proto_head(out, p->waitcmd, i);
	return send_to_self(p, out, i, 1);
}

int add_hp_mp_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t hp, mp, injury_lv;
	int j = 0;
	UNPKG_H_UINT32(buf, hp, j);
	UNPKG_H_UINT32(buf, mp, j);
	UNPKG_H_UINT32(buf, injury_lv, j);

	p->hp += hp;
	p->mp += mp;
	//if (injury_lv != p->injury_lv) {
		//update_user_second_attr(p, (base_5_attr_t *)&p->physique, injury_lv);
	//}

	KDEBUG_LOG(p->id, "HP MP BACK\t[%u %u %u]", p->hp, p->mp, injury_lv);

	switch (p->waitcmd) {
		case proto_cli_user_cure:
			if (*(uint32_t *)p->session) {
				return db_add_xiaomee(p, p->id, *(uint32_t *)p->session);
			} else {
				return rsponse_user_cure(p, 0);
			}
		default:
			return -1;
	}

	return 0;
}

int cure_all_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	uint32_t count, petid, hp, mp, injurylv;
	int i = 0, cost;
	UNPKG_H_UINT32(buf, count, i);
	UNPKG_H_UINT32(buf, p->xiaomee, i);
	UNPKG_H_UINT32(buf, cost, i);
	CHECK_BODY_LEN(len, 12 + count * 16);

	monitor_sprite(p, "CURE ALL");

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->xiaomee, j);
	PKG_UINT32(msg, -cost, j);
	int retlen = j + 4, usercnt = 0;
	uint32_t loop;
	for (loop = 0; loop < count; loop ++) {
		UNPKG_H_UINT32(buf, petid, i);
		UNPKG_H_UINT32(buf, hp, i);
		UNPKG_H_UINT32(buf, mp, i);
		UNPKG_H_UINT32(buf, injurylv, i);

		if (!petid) {
			//if (injurylv != p->injury_lv) {
				//update_user_second_attr(p, (base_5_attr_t *)&p->physique, injurylv);
			//}
			p->hp = hp > p->hp_max ? p->hp_max : hp;
			p->mp = mp > p->mp_max ? p->mp_max : mp;
			usercnt ++;
			PKG_UINT32(msg, 0, retlen);
			PKG_UINT32(msg, p->hp, retlen);
			PKG_UINT32(msg, p->hp_max, retlen);
			PKG_UINT32(msg, p->mp, retlen);
			PKG_UINT32(msg, p->mp_max, retlen);
			PKG_UINT32(msg, p->injury_lv, retlen);

			KDEBUG_LOG(p->id, "CURE ALL USER\t[%u %u %u %u %u]", p->hp, p->hp_max, p->mp, p->mp_max, p->injury_lv);

			if (p->btr_team)
				notify_team_state(p->btr_team);
		} else {
			pet_t* pp = get_pet_inbag(p, petid);
			if (!pp)
				continue;

			if (injurylv != pp->injury_lv) {
				pp->injury_lv = injurylv;
				calc_pet_second_level_attr(pp);
			}
			pp->hp = hp > pp->hp_max ? pp->hp_max : hp;
			pp->mp = mp > pp->mp_max ? pp->mp_max : mp;
			usercnt ++;
			PKG_UINT32(msg, pp->petid, retlen);
			PKG_UINT32(msg, pp->hp, retlen);
			PKG_UINT32(msg, pp->hp_max, retlen);
			PKG_UINT32(msg, pp->mp, retlen);
			PKG_UINT32(msg, pp->mp_max, retlen);
			PKG_UINT32(msg, pp->injury_lv, retlen);
		}
	}

	PKG_UINT32(msg, usercnt, j);
	init_proto_head(msg, p->waitcmd, retlen);
	return send_to_self(p, msg, retlen, 1);
}

int cli_set_injury(sprite_t *p,  Cmessage * c_in)
{
	cli_set_injury_in* p_in = P_IN;	
	if (p_in->injurylv <= p->injury_lv || p_in->injurylv >= INJURY_MAX) {
		KERROR_LOG(p->id, "set injury invalid\t[%u %u]", p_in->injurylv, p->injury_lv);
		return -1;
	}
	*(uint32_t *)p->session = p_in->injurylv;
	return db_set_user_injury(p, p->id, p_in->injurylv);
}

int set_injury_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	//uint32_t injurylv = *(uint32_t *)p->session;
	//update_user_second_attr(p, (base_5_attr_t *)&p->physique, injurylv);
	cli_set_injury_out out;
	out.userid = p->id;
	out.hp = p->hp;
	out.mp = p->mp;
	out.hpmax = p->hp_max;
	out.mpmax = p->mp_max;
	out.injurylv = p->injury_lv;

	if (p->btr_team)
		notify_team_state(p->btr_team);

	return send_msg_to_self(p, p->waitcmd, &out, 1);
}

int add_xiaomee_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int32_t add_value;
	int j = 0;
;
	UNPKG_H_UINT32(buf, p->xiaomee, j);
	UNPKG_H_UINT32(buf, add_value, j);

	switch (p->waitcmd) {
		case proto_cli_user_cure:
			monitor_sprite(p, "CURE USER");
			return rsponse_user_cure(p, -add_value);
		default:
			KERROR_LOG(p->id, "invalid cmd\t[%u]", p->waitcmd);
			return -1;
	}

}

int set_energy_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	p->energy = *(uint32_t *)buf;
	uint32_t itemid = *(uint32_t *)p->session;
	KDEBUG_LOG(p->id, "SET ENERGY BACK\t[%u %u]", p->energy, itemid);
	return db_add_item(p, p->id, itemid, -1);
}


int set_position_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint8_t pos;
	int j = 0;
	UNPKG_UINT32(body, pos, j);
	CHECK_TRUE_FALSE_VALID(p, pos);

	if (p->in_front == pos) {
		response_proto_uint8(p, p->waitcmd, pos, 1, 0);
		return 0;
	}

	pos = pos ? 1 : 0;
	KDEBUG_LOG(p->id, "SET POS\t[%u]", pos);
	return db_set_user_position(p, p->id, pos);
}

int set_position_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	int i = sizeof(protocol_t);
	uint8_t pos = *(uint8_t *)buf;
	p->in_front = pos;

	PKG_UINT8(msg, pos, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int user_init_info(sprite_t* p, Cmessage* c_in)
{	
	if (BE_INITED(p->flag)) {
		return send_to_self_error(p, p->waitcmd, cli_err_user_registerd, 1);
	}

	user_init_info_in* p_in = P_IN;
	CHECK_DIRTYWORD(p, p_in->nick);

	init_prof_info_t* pipi = get_init_prof_info(p_in->prof);
	if (!pipi) {
		KERROR_LOG(p->id, "invalid prof info\t[%u]", p_in->prof);
		return -1;
	}
	memcpy(p->session, pipi, sizeof(init_prof_info_t));

	uint32_t hp = 8 * pipi->physique + 2 * pipi->strength + 3 * pipi->endurance + 3 * pipi->quick + pipi->endurance + 20;
	uint32_t mp = pipi->physique + 2 * pipi->strength + 2 * pipi->endurance + 2 * pipi->quick + 10 * pipi->endurance + 20;
	hp *= 5;

	cloth_t* pc = get_cloth(p_in->crystal_id);
	if (!pc || p_in->crystal_id < 120001 || p_in->crystal_id > 120040 || !pc->clothlvs[0].valid) {
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}

	KDEBUG_LOG(p->id, "INIT SPRITE\t[%u %u %u]", p_in->prof, p_in->color, p_in->crystal_id);
	uint8_t out[4096] = {};
	int i = 0;
	PKG_STR(out, p_in->nick, i, USER_NICK_LEN);
	PKG_H_UINT32(out, p_in->prof, i);
	PKG_H_UINT32(out, p_in->color, i);
	PKG_H_UINT16(out, pipi->physique, i);
	PKG_H_UINT16(out, pipi->strength, i);
	PKG_H_UINT16(out, pipi->endurance, i);
	PKG_H_UINT16(out, pipi->quick, i);
	PKG_H_UINT16(out, pipi->intelligence, i);
	PKG_H_UINT32(out, hp, i);
	PKG_H_UINT32(out, mp, i);

	body_cloth_t* pbc = (body_cloth_t *)(out + i + 4);
	gen_cloth_attr(pc, 0, &pc->clothlvs[0], (cloth_attr_t*)&pbc->clothtype, 0);
	if (!cache_get_body_cloth(p, pc->equip_part + BASE_BODY_TYPE))
		pbc->grid = pc->equip_part + BASE_BODY_TYPE;
	uint32_t cloth_cnt = 1;
	for (uint32_t loop = 0; loop < pipi->cloth_cnt; loop ++) {
		cloth_t* pc = get_cloth(pipi->cloths[loop]);
		if (pc && pc->clothlvs[0].valid) {
			gen_cloth_attr(pc, 0, &pc->clothlvs[0], (cloth_attr_t*)&pbc[cloth_cnt].clothtype, 0);
			KDEBUG_LOG(p->id, "INIT PROF CLOTH\t[%u %u]", pc->id, pbc[cloth_cnt].bless_type);
			if (!cache_get_body_cloth(p, pc->equip_part + BASE_BODY_TYPE)) {
				pbc[cloth_cnt].grid = pc->equip_part + BASE_BODY_TYPE;
			}
			cloth_cnt ++;
		} else {
			KERROR_LOG(p->id, "invalid cloth id\t[%d %u]", loop, pipi->cloths[loop]);
		}
	}

	PKG_H_UINT32(out, cloth_cnt, i);
	i += cloth_cnt * sizeof(body_cloth_t);

	return send_request_to_db(p, p->id, proto_db_update_sprite, out, i);
}

int check_id_in_range(sprite_t *p)
{
	typedef struct minmax {
		uint32_t		min;
		uint32_t		max;
	} minmax_t;
	if (p->id == 232609985)
		return 1;

	minmax_t rangid[3] = {{240001000, 240976689}, \
						{241000100, 241201966}, \
						{241400010, 241601554}};
	for (int loop = 0; loop < 3; loop ++) {
		if (p->id < rangid[loop].min)
			return 0;
		if (p->id <= rangid[loop].max)
			return 1;
	}
	return 0;
}
int update_clothes_when_init_user(sprite_t* p, body_cloth_t* apc, uint32_t cloth_cnt)
{
	uint32_t loop;
	for (loop = 0; loop < cloth_cnt; loop++) {
		cloth_t* p_sc = get_cloth(apc[loop].clothtype);
		if (!p_sc || apc[loop].clothlv >= MAX_CLOTH_LEVL_CNT || !p_sc->clothlvs[apc[loop].clothlv].valid) {
			KERROR_LOG(p->id, "Invalid cloth id\t[%u %u]", apc[loop].clothtype, apc[loop].clothlv);
			continue;
		}

		if (!cache_get_bag_cloth(p, apc[loop].clothid)) {
			body_cloth_t* pc = (body_cloth_t *)g_slice_alloc(sizeof(body_cloth_t));
			memcpy(pc, apc + loop, sizeof(body_cloth_t));
			KDEBUG_LOG(p->id, "SET CLOTH AFTER\t[%u %u %u]", pc->clothid, pc->clothtype, pc->grid);
			g_hash_table_insert(p->all_cloths, &pc->clothid, pc);
			if (pc->grid > BASE_BODY_TYPE && !cache_get_body_cloth(p, pc->grid))
				g_hash_table_insert(p->body_cloths, &pc->grid, pc);
			if (pc->validday)
				g_hash_table_insert(p->cloth_timelist, &pc->clothid, pc);
		}
	}

	return cloth_cnt * sizeof(body_cloth_t);
}

void give_honor_when_init(sprite_t* p)
{
	init_prof_info_t* pipi = (init_prof_info_t *)p->session;
	honor_attr_t* pha = get_honor_attr(pipi->honorid);
	if (pha) {
		update_one_honor(p, pha);
		set_user_honor(p, pha->honor_id);
		db_set_user_honor(0, p->id, p->honor);
		db_add_user_honor(NULL, p->id, p->prof_lv, 0, pha->honor_id);
	}
}

int update_sprite_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	memcpy(&p->nick, buf, sizeof(get_sprite_rsp_t));
	int i = sizeof(get_sprite_rsp_t);
	uint32_t count;
	UNPKG_H_UINT32(buf, count, i);
	CHECK_BODY_LEN(len, i + sizeof(body_cloth_t) * count);
	update_clothes_when_init_user(p, (body_cloth_t*)(buf + i), count);
	give_honor_when_init(p);
	calc_sprite_second_level_attr(p);
	cache_set_cloth_attr(p);
	p->pkpoint_total = 100;
	modify_sprite_second_attr(p);
	notify_user_clothes(p);

	cache_add_kind_item(p, ITEM_ID_PRENSET, 1);
	db_add_item(NULL, p->id, ITEM_ID_PRENSET, 1);
	if (check_id_in_range(p)) {
		cache_add_kind_item(p, 300001, 3);
		db_add_item(NULL, p->id, 300001, 3);
	}

	log_sprite(p);

	msg_log_mole_reg(p->id);
	msg_log_bus_mole2_reg(p->id, p->nick);
	msg_log_profession(p->prof, 0, 0, 0);
	msg_log_user_level(p->id, p->register_time, p->level);

	int k = sizeof (protocol_t);
	k += pkg_sprite_attr(p, msg + k);
	
	init_proto_head (msg, p->waitcmd, k);
	return send_to_self (p, msg, k, 1);
}

int mole2_set_flag_bits(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_set_flag_bits_out* p_out = P_OUT;
	if (p_out->pos_id/32==0 ){
		p->flag = p_out->flag;

		switch (*(uint32_t *)p->session) {
			case 1:
				response_user_vip_info(p);
				break;
			case 2:
				response_proto_uint32_uint32(p, p->waitcmd, p->id, p->flag, 1, 1);
				return 0;
		}		
	}else{
		p->flag2= p_out->flag;
	}

	response_proto_uint32_uint32(p, p->waitcmd, p->id, p->flag, 1, 0);
	return 0;
}

int cli_add_base_attr(sprite_t* p, Cmessage* c_in)
{
	cli_add_base_attr_in* p_in = P_IN;
	KDEBUG_LOG(p->id,"cli_add_base_attr:%u %u %u %u %u %u %u",p->level,p->attr_addition,
			p_in->physique , p_in->strength , p_in->endurance , p_in->quick , p_in->intelligence);
	//uint32_t single_max = 2 * p->level + 13;
	uint32_t single_max = ((p->level-1)*4 + 30+(IS_NEW_PROF(p->prof)?30:0))/2;
	uint32_t attr_all = p_in->physique + p_in->strength + p_in->endurance + p_in->quick + p_in->intelligence;
	if (p->physique + p_in->physique > single_max \
		|| p->strength + p_in->strength > single_max \
		|| p->endurance + p_in->endurance > single_max \
		|| p->quick + p_in->quick > single_max \
		|| p->intelligence + p_in->intelligence > single_max \
		|| attr_all > p->attr_addition) {
		KERROR_LOG(p->id, "attr_all: %u", attr_all);
		return send_to_self_error(p, p->waitcmd, cli_err_attr_invalid, 1);
	}

	int j = 0;
	int8_t out[256];
	
	PKG_H_UINT16(out, p->physique + p_in->physique, j);
	PKG_H_UINT16(out, p->strength + p_in->strength, j);
	PKG_H_UINT16(out, p->endurance + p_in->endurance, j);
	PKG_H_UINT16(out, p->quick + p_in->quick, j);
	PKG_H_UINT16(out, p->intelligence + p_in->intelligence, j);
	PKG_H_UINT16(out, p->attr_addition - attr_all, j);

	return send_request_to_db(p, p->id, proto_db_add_base_attr, out, j);
}

int recommend_add_attr(sprite_t* p, Cmessage* c_in)
{
	if (!p->attr_addition) {
		KERROR_LOG(p->id, "attr add 0");
		//return -1;
		return send_to_self_error(p, p->waitcmd, cli_err_attr_invalid, 1);
	}
	KDEBUG_LOG(p->id,"recommend_add_attr:%u %u",p->level,p->attr_addition);
	KDEBUG_LOG(p->id,"recommend_add_attr:%u %u",p->level,p->attr_addition);
	attr_info_t attr;
	memcpy(&attr, &p->physique, sizeof(attr_info_t));
	//int single_max = 2 * p->level + 13;
	int single_max = ((p->level-1)*4 + 30+(IS_NEW_PROF(p->prof)?30:0))/2;
	switch (p->prof) {
		case prof_swordman:
			if (attr.strength < single_max) {
				int max_add = single_max - attr.strength;
				if (max_add >= attr.attr_add) {
					attr.strength += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.strength += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					int quick_add = attr.attr_add * 2 / 3 + 1;
					attr.quick += quick_add;
					attr.physique += attr.attr_add - quick_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_ancher:
			if (attr.quick < single_max) {
				int max_add = single_max - attr.quick;
				if (max_add >= attr.attr_add) {
					attr.quick += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.quick += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					attr.strength += attr.attr_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_magician:
			if (attr.intelligence < single_max) {
				int max_add = single_max - attr.intelligence;
				if (max_add >= attr.attr_add) {
					attr.intelligence += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.intelligence += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					int endurance_add = attr.attr_add * 2 / 3 + 1;
					attr.endurance += endurance_add;
					attr.physique += attr.attr_add - endurance_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_churchman:
			if (attr.physique < single_max) {
				int max_add = single_max - attr.physique;
				if (max_add >= attr.attr_add) {
					attr.physique += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.physique += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					int intelligence_add = attr.attr_add * 3 / 4 + 1;
					attr.intelligence += intelligence_add;
					attr.quick += attr.attr_add - intelligence_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_ninja:
			if (attr.quick < single_max) {
				int max_add = single_max - attr.quick;
				if (max_add >= attr.attr_add) {
					attr.quick += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.quick += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					int strength_add = attr.attr_add * 2 / 3 + 1;
					attr.strength += strength_add;
					attr.physique += attr.attr_add - strength_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_gedoushi:
			if (attr.strength < single_max) {
				int max_add = single_max - attr.strength;
				if (max_add >= attr.attr_add) {
					attr.strength += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.strength += max_add;
					attr.attr_add -= max_add;
				}
				if (attr.attr_add) {
					int physique_add = attr.attr_add   / 2 + 1;
					attr.physique += physique_add;
					attr.quick += attr.attr_add - physique_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_heimodaoshi:
			if (attr.intelligence < single_max) {
				int max_add = single_max - attr.intelligence;
				if (max_add >= attr.attr_add) {
					attr.intelligence += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.intelligence += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					int physique_add = attr.attr_add  / 2 + 1;
					attr.physique += physique_add ;
					attr.endurance += attr.attr_add - physique_add ;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_shengyanshi:
			if (attr.physique < single_max) {
				int max_add = single_max - attr.physique;
				if (max_add >= attr.attr_add) {
					attr.physique += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.physique += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					int endurance_add = attr.attr_add * 1 / 2 + 1;
					attr.endurance += endurance_add;
					attr.intelligence += attr.attr_add - endurance_add;
					attr.attr_add = 0;
				}
			}
			break;
		case prof_wushushi:
			if (attr.strength < single_max){
				int max_add = single_max - attr.strength;
				if (max_add >= attr.attr_add) {
					attr.strength += attr.attr_add;
					attr.attr_add = 0;
				} else {
					attr.strength += max_add;
					attr.attr_add -= max_add;
				}

				if (attr.attr_add) {
					attr.quick += attr.attr_add ;
					attr.attr_add = 0;
				}
			}
			break;
		default:
			KERROR_LOG(p->id, "invalid prof\t[%u]", p->prof);
			return -1;
	}
	return send_request_to_db(p, p->id, proto_db_add_base_attr, &attr, sizeof(attr_info_t));
}

int add_base_attr_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	int i, j = 0;
	uint16_t attr[5], attr_addition;
	for (i = 0; i < 5; i ++) {
		UNPKG_H_UINT16(buf, attr[i], j);
	}
	UNPKG_H_UINT16(buf, attr_addition, j);
	p->attr_addition = attr_addition;
	
	update_user_second_attr(p, (base_5_attr_t *)attr, p->injury_lv);
	modify_sprite_second_attr(p);
	log_sprite(p);

	if (p->btr_team) {
		notify_team_state(p->btr_team);
	}

	switch(p->waitcmd) {
		case proto_cli_re_birth:
			j = sizeof(protocol_t);
			if (!cache_reduce_kind_item(p, *(uint32_t *)p->session, 1)) {
				db_add_item(0, p->id, *(uint32_t *)p->session, -1);
			}

			PKG_UINT32(msg, 0, j);
			PKG_UINT16(msg, attr_addition, j);
			PKG_UINT32(msg, p->hp, j);
			PKG_UINT32(msg, p->mp, j);
			PKG_UINT32(msg, p->hp_max, j);
			PKG_UINT32(msg, p->mp_max, j);
			
			init_proto_head(msg, p->waitcmd, j);
			return send_to_self(p, msg, j, 1);
		case cli_add_base_attr_cmd:
		case recommend_add_attr_cmd:
			j = sizeof(protocol_t);
			for (i = 0; i < 5; i ++) {
				PKG_UINT16(msg, attr[i], j);
			}
			PKG_UINT16(msg, attr_addition, j);

			PKG_UINT32(msg, p->hp_max, j);
			PKG_UINT32(msg, p->mp_max, j);
			PKG_UINT16(msg, p->attack, j);
			PKG_UINT16(msg, p->defense, j);
			PKG_UINT16(msg, p->speed, j);
			PKG_UINT16(msg, p->spirit, j);
			PKG_UINT16(msg, p->resume, j);
			PKG_UINT16(msg, p->hit_rate < 0 ? 0 : p->hit_rate, j);
			PKG_UINT16(msg, p->avoid_rate < 0 ? 0 : p->avoid_rate, j);
			PKG_UINT16(msg, p->bisha < 0 ? 0 : p->bisha, j);
			PKG_UINT16(msg, p->fight_back < 0 ? 0 : p->fight_back, j);
			init_proto_head(msg, p->waitcmd, j);
			return send_to_self(p, msg, j, 1);
		case proto_cli_reset_attr:
			j = sizeof(protocol_t);
			PKG_UINT32(msg, 0, j);
			PKG_UINT16(msg, attr_addition, j);
			PKG_UINT32(msg, p->hp, j);
			PKG_UINT32(msg, p->mp, j);
			PKG_UINT32(msg, p->hp_max, j);
			PKG_UINT32(msg, p->mp_max, j);
			init_proto_head(msg, p->waitcmd, j);
			return send_to_self(p, msg, j, 1);
		default:
			ERROR_RETURN(("INVALID CMD WHEN ADD BASE ATTR CALLBACK %u %u", p->id, p->waitcmd), -1);
	}
}

int check_online_id_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	response_proto_uint32(p, p->waitcmd, get_server_id(), 1, 0);
	return 0;
}

int db_add_exp_level(sprite_t* p)
{
	uint8_t out[128];
	int j = 0;
	PKG_H_UINT32(out, p->experience, j);
	PKG_H_UINT32(out, p->level, j);
	PKG_H_UINT32(out, p->attr_addition, j);
	PKG_H_UINT32(out, p->hp, j);
	PKG_H_UINT32(out, p->mp, j);

	return send_request_to_db(0, p->id, proto_db_user_levelup, out, j);
}

int chk_str_valid_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	char buff[MAX_STR_LEN] = {0};
	memcpy(buff, body, MAX_STR_LEN);
	buff[MAX_STR_LEN - 1] = '\0';
	KDEBUG_LOG(p->id, "CHK STR DIRTY\t[%s]", buff);
	CHECK_DIRTYWORD(p, buff);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int prof_compensation(sprite_t* p, Cmessage * c_in)
{
	if (p->prof || !BE_INITED(p->flag)) {
		KERROR_LOG(p->id, "user has pro\t[%u %u]", p->prof, p->flag);
		return -1;
	}

	prof_compensation_in* p_in = P_IN;
	KDEBUG_LOG(p->id, "PROF COMPEANSATION\t[%u]", p_in->prof);
	init_prof_info_t* pipi = get_init_prof_info(p_in->prof);
	if (!pipi) {
		KERROR_LOG(p->id, "invalid prof info\t[%u]", p_in->prof);
		return -1;
	}
	memcpy(p->session, pipi, sizeof(init_prof_info_t));
	uint8_t prof = p_in->prof;
	return send_request_to_db(p, p->id, proto_db_set_prof, &prof, sizeof(prof));
}

int set_prof_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	p->prof = *(uint8_t *)buf;
	
	init_prof_info_t* pipi = (init_prof_info_t *)p->session;
	for (uint32_t loop = 0; loop < pipi->cloth_cnt; loop ++) {
		cloth_t* pc = get_cloth(pipi->cloths[loop]);
		if (pc && pc->clothlvs[0].valid) {
			db_add_cloth(NULL, p->id, 0, pc, 0, &pc->clothlvs[0]);
		}
	}
	cloth_t* pc = get_cloth(pipi->clothpatch);
	if (pc && pc->clothlvs[0].valid)
		db_add_cloth(NULL, p->id, 0, pc, 0, &pc->clothlvs[0]);
	msg_log_profession(p->prof, 0, 0, 0);
	return db_get_packet_cloth_list(p);
}

int reset_attr_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t petid;
	int i = 0, j = 0;
	uint8_t out[64];
	UNPKG_UINT32(body, petid, j);
	CHECK_LV_FIT(p,p,40,MAX_SPRITE_LEVEL);
	if( !petid ){
		item_t* pday = cache_get_day_limits(p, ssid_reset_user_cnt);
		if (pday->count >= MAX_USER_RESET_CNT ) {
			return send_to_self_error(p,p->waitcmd,cli_err_rand_day_max,1);
		}
		pday->count++;
		db_day_add_ssid_cnt(0,p->id,ssid_reset_user_cnt,1,-1);
		if (!p->physique && !p->strength && !p->endurance && !p->quick && !p->intelligence)
			return send_to_self_error(p, p->waitcmd, cli_err_not_need_reset, 1);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		int16_t attr_add =((p->level - 1) * 4 + 30);
		if( IS_NEW_PROF(p->prof) ){
			attr_add += 30 ;
		}
		PKG_H_UINT16(out, attr_add, i); 
		KDEBUG_LOG(p->id, "USER RESET ATTR SUCC\t[%u %u]", p->level, attr_add);
		return send_request_to_db(p, p->id, proto_db_add_base_attr, out, i);
	}else{
		item_t* pday = cache_get_day_limits(p, ssid_reset_pet_cnt);
		if (pday->count >= MAX_PET_RESET_CNT ) {
			return send_to_self_error(p,p->waitcmd,cli_err_rand_day_max,1);
		}
		pday->count++;
		db_day_add_ssid_cnt(0,p->id,ssid_reset_pet_cnt,1,-1);
		pet_t* pp = get_pet_inbag(p, petid);
		CHECK_PET_INBAG(p, pp, petid);
		PKG_H_UINT32(out, pp->petid, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		PKG_H_UINT16(out, 0, i);
		uint16_t attr_addition = pp->base_attr_db.physi_add
								 + pp->base_attr_db.endur_add
								 + pp->base_attr_db.stren_add
								 + pp->base_attr_db.quick_add
								 + pp->base_attr_db.intel_add
								 + pp->attr_addition;

		KDEBUG_LOG(p->id, "PET RESET ATTR SUCC\t[%u %u]", petid, attr_addition);
		if (attr_addition == pp->attr_addition) 
			return send_to_self_error(p, p->waitcmd, cli_err_not_need_reset, 1);
		PKG_H_UINT16(out, attr_addition, i);
		return send_request_to_db(p, p->id, proto_db_pet_add_base_attr, out, i);
	}
}

int user_re_birth_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t petid, itemid;
	int i = 0, j = 0;
	uint8_t out[64];
	UNPKG_UINT32(body, petid, j);
	UNPKG_UINT32(body, itemid, j);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_FUNC(p, pni, item_for_reset_attr);
	CHECK_LV_FIT(p, p, pni->minlv, pni->maxlv);
	CHECK_ITEM_EXIST(p, itemid, 1);

	if (pni->target != item_target_self) {
		KERROR_LOG(p->id, "item target err\t[%u]", pni->target);
		return -1;
	}

	*(uint32_t *)p->session = itemid;

	int all = !(pni->attrinfo.physique || pni->attrinfo.strength \
		|| pni->attrinfo.endurance || pni->attrinfo.quick || pni->attrinfo.intelligence);

	if (all && !p->physique && !p->strength && !p->endurance && !p->quick && !p->intelligence)
		return send_to_self_error(p, p->waitcmd, cli_err_not_need_reset, 1);

	int physique = all ? 0 : p->physique + pni->attrinfo.physique;
	int strength = all ? 0 : p->strength + pni->attrinfo.strength;
	int endurance = all ? 0 : p->endurance + pni->attrinfo.endurance;
	int quick = all ? 0 : p->quick + pni->attrinfo.quick;
	int intel = all ? 0 : p->intelligence + pni->attrinfo.intelligence;

	int attr_max = p->level * 2 + 13;

	if (physique < 0 || strength < 0 || endurance < 0 || quick < 0 || intel < 0)
		return send_to_self_error(p, p->waitcmd, cli_err_attr_not_enough, 1);

	if (attr_max < physique || attr_max < strength || attr_max < endurance || attr_max < quick || attr_max < intel)
		return send_to_self_error(p, p->waitcmd, cli_err_attr_over_max, 1);
	
	PKG_H_UINT16(out, physique, i);
	PKG_H_UINT16(out, strength, i);
	PKG_H_UINT16(out, endurance, i);
	PKG_H_UINT16(out, quick, i);
	PKG_H_UINT16(out, intel, i);
	int16_t attr_add = (pni->attrinfo.attr_add == -1) \
		? ((p->level - 1) * 4 + 30+(IS_NEW_PROF(p->prof)?30:0)) : (pni->attrinfo.attr_add + p->attr_addition);

	PKG_H_UINT16(out, attr_add, i); 
	KDEBUG_LOG(p->id, "USER RE BIRTH SUCC\t[%u %u]", p->level, attr_add);
	return send_request_to_db(p, p->id, proto_db_add_base_attr, out, i);
}

int register_bird_map_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	CHECK_USER_IN_MAP(p, 0);
	KDEBUG_LOG(p->id, "REGISTER BIRD MAP\t[%lu]", p->tiles->id);

	if (!IS_NORMAL_MAP(p->tiles->id) || !p->tiles->chocobo) {
		return send_to_self_error(p, p->waitcmd, cli_err_not_birdmap, 1);
	}

	uint32_t mapid = p->tiles->id;
	return send_request_to_db(p, p->id, proto_db_register_bird_map, &mapid, 4);
}

int register_bird_map_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	cache_add_bird_map(p, p->tiles->id);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int check_team_bird_map_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t mapid;
	int j = 0;
	UNPKG_UINT32(body, mapid, j);
	map_t* pm = get_map(mapid);
	if (!pm) {
		KERROR_LOG(p->id, "mapid invalid\t[%u]", mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_mapid_invalid, 1);
	}

	CHECK_SELF_TEAM_LEADER(p, mapid, 1);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, mapid, i);
	if (!p->btr_team) {
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, p->id, i);
		PKG_UINT32(msg, cache_bird_map_registerd(p, pm), i);
	} else {
		PKG_UINT32(msg, p->btr_team->count, i);
		int loop;
		for (loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (s) {
				PKG_UINT32(msg, s->id, i);
				PKG_UINT32(msg, cache_bird_map_registerd(s, pm), i);
			}
		}
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);

}

static void pkg_bird_map(gpointer key, gpointer value, gpointer data)
{
	map_info_t* pm = (map_info_t *)value;
	if ((pm->state & MAP_INFO_BIRD_MAP_BIT) == 0)
		return;
	PKG_UINT32(msg, pm->mapid, (*(int *)data));
}


int get_bird_maps_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int k = sizeof(protocol_t) + 4;
	g_hash_table_foreach(p->map_infos, pkg_bird_map, &k);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, ((k - sizeof(protocol_t) - 4) / 4), i);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int get_maps_state_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t map_cnt;
	int j = 0;
	UNPKG_H_UINT32(buf, map_cnt, j);
	CHECK_BODY_LEN(len, sizeof(map_info_t) * map_cnt + 4);

	map_info_t* mapinfos = (map_info_t*)(buf + 4);

	for (uint32_t loop = 0; loop < map_cnt; loop ++) {
		map_info_t* pmi = cache_get_map_info(p, mapinfos[loop].mapid);
		if (!pmi) {
			pmi = (map_info_t *)g_slice_alloc(sizeof(map_info_t));
			pmi->mapid = mapinfos[loop].mapid;
			pmi->state = mapinfos[loop].state;
			g_hash_table_insert(p->map_infos, &pmi->mapid, pmi);
		}
	}

	return send_request_to_db(p, p->id, proto_db_get_item_day, NULL, 0);
}

int get_handbook_info_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	return send_request_to_db(p, p->id, proto_db_get_handbook_info, NULL, 0);
}

int get_handbook_info_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t item_hb_cnt, beast_hb_cnt;
	uint32_t hb_id, count, hb_cnt;
	int i = sizeof(protocol_t), j = 0;
	UNPKG_H_UINT32(buf, item_hb_cnt, j);
	UNPKG_H_UINT32(buf, beast_hb_cnt, j);

	CHECK_BODY_LEN(len, (item_hb_cnt + beast_hb_cnt) * 12 + 8);
	KDEBUG_LOG(p->id, "HANDBOOK INFO\t[%u %u]", item_hb_cnt, beast_hb_cnt);

	PKG_UINT32(msg, item_hb_cnt, i);
	uint32_t loop;
	for (loop = 0; loop < item_hb_cnt; loop ++) {
		UNPKG_H_UINT32(buf, hb_id, j);
		UNPKG_H_UINT32(buf, hb_cnt, j);
		UNPKG_H_UINT32(buf, count, j);
		PKG_UINT32(msg, hb_id, i);
		PKG_UINT32(msg, hb_cnt, i);
		PKG_UINT32(msg, count, i);
		KDEBUG_LOG(p->id, "ITEM HB\t[%u %u %u]", hb_id, hb_cnt, count);
	}

	uint32_t beast_hb_real_cnt = 0;
	int k = i;

	i += 4;
	for (loop = 0; loop < beast_hb_cnt; loop ++) {
		UNPKG_H_UINT32(buf, hb_id, j);
		UNPKG_H_UINT32(buf, hb_cnt, j);
		UNPKG_H_UINT32(buf, count, j);
		if (!(hb_cnt & 0x1) && !count)
			continue;
		PKG_UINT32(msg, hb_id, i);
		PKG_UINT32(msg, hb_cnt, i);
		PKG_UINT32(msg, count, i);
		beast_hb_real_cnt ++;
	}

	PKG_UINT32(msg, beast_hb_real_cnt, k);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int set_busy_state_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t type, state;
	UNPKG_UINT32(body, type, j);
	UNPKG_UINT32(body, state, j);
	switch (type) {
		case team_switch:
			CHECK_TRUE_FALSE_VALID(p, state);
			update_user_flag(p, flag_team_sw, state);
			db_set_flag(NULL, p->id, flag_team_sw, state);
			break;
		case sns_state:
			p->busy_state = state;
			break;
		case pk_switch:
			CHECK_TRUE_FALSE_VALID(p, state);
			p->pk_switch = state;
			break;
	}
	notify_busy_state_update(p, type, state);
	p->waitcmd = 0;
	return 0;
}

int allocate_exp_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	CHECK_USER_APPLY_PK(p, p);
	uint32_t petid, exp;
	int i = 0;
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, exp, i);

	KDEBUG_LOG(p->id, "ALLOCATE EXP BOX\t[%u %u %u %u]", petid, exp, p->experience, p->expbox);

	//if (!exp) {
	if (!exp || !petid) {
		KERROR_LOG(p->id, "should not 0");
		return -1;
	}
	
	if (p->expbox < exp) {
		KERROR_LOG(p->id, "exp box not enough\t[%u %u]", p->expbox, exp);
		return send_to_self_error(p, p->waitcmd, cli_err_exp_not_enough, 1);
	}
	uint32_t level, exper, attr_addition, hp, mp;
	if (petid) {
		pet_t* p_p = get_pet_inbag(p, petid);
		CHECK_PET_INBAG(p, p_p, petid);

		pet_t tmp_pet;
		memcpy(&tmp_pet, p_p, sizeof(pet_t));

		tmp_pet.experience += exp;
		LIMIT_MAX_EXP(tmp_pet.experience, p->id);
		LIMIT_PET_MAX_EXP(tmp_pet.experience, p->id);
		level = tmp_pet.level = calc_pet_level_from_exp(tmp_pet.experience, tmp_pet.level);
		exper = tmp_pet.experience;

		if (level > p->level + 5) {
			KERROR_LOG(p->id, "level max\t[%u %u]", petid, level);
			return send_to_self_error(p, p->waitcmd, cli_err_exp_too_much, 1);
		}

		if (p_p->level < tmp_pet.level) {
			tmp_pet.attr_addition += tmp_pet.level - p_p->level;
			attr_addition = tmp_pet.attr_addition;
			calc_pet_5attr(&tmp_pet);
			calc_pet_second_level_attr(&tmp_pet);

			hp = tmp_pet.hp_max;
			mp = tmp_pet.mp_max;
		} else {
			attr_addition = p_p->attr_addition;
			hp = p_p->hp;
			mp = p_p->mp;
		}
	}else {
		exper = p->experience + exp;
		LIMIT_MAX_EXP(exper, p->id);
		LIMIT_USER_MAX_EXP(exper, p->id);
		level = calc_level_from_exp(exper, p->level);
		if (p->level < level) {
			attr_addition = p->attr_addition + 4 * (level - p->level);
			hp = p->hp_max;
			mp = p->mp_max;
		} else {
			attr_addition = p->attr_addition;
			hp = p->hp;
			mp = p->mp;
		}
	}
	
	uint8_t out[64];
	int j = 0;
	KDEBUG_LOG(p->id, "ALLOCATE EXP REAL\t[%u %u %u %u %u %u %u]", petid, exper, level, attr_addition, hp, mp, p->expbox - exp);
	PKG_H_UINT32(out, petid, j);
	PKG_H_UINT32(out, exper, j);
	PKG_H_UINT32(out, level, j);
	PKG_H_UINT32(out, attr_addition, j);
	PKG_H_UINT32(out, hp, j);
	PKG_H_UINT32(out, mp, j);
	PKG_H_UINT32(out, p->expbox - exp, j);
	return send_request_to_db(p, p->id, proto_db_allocate_exp, out, j);
}

void noti_expbox_change(sprite_t* p)
{
	uint8_t out[32];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->id, i);
	PKG_UINT32(out, p->expbox, i);
	init_proto_head(out, proto_cli_expbox_change, i);
	send_to_map(p, out, i, 0,1);
}

int allocate_exp_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t petid, level, exp, attr_add, hp, mp, hpmax, mpmax;
	int j = 0;
	UNPKG_H_UINT32(buf, petid, j);
	UNPKG_H_UINT32(buf, exp, j);
	UNPKG_H_UINT32(buf, level, j);
	UNPKG_H_UINT32(buf, attr_add, j);
	UNPKG_H_UINT32(buf, hp, j);
	UNPKG_H_UINT32(buf, mp, j);
	UNPKG_H_UINT32(buf, p->expbox, j);

	if (petid) {
		pet_t* p_p = get_pet_inbag(p, petid);
		if (p_p) {
			if (level > p_p->level) {
				msg_log_pet_level(level, p_p->level);
				update_pet_skill_level(p, p_p);
			}
			if (level > p->level + 5) {
				p_p->action_state = just_in_bag;
				db_set_pet_state(0, p->id, p_p->petid, just_in_bag);
			}
			p_p->level = level;
			calc_pet_5attr(p_p);
			calc_pet_second_level_attr(p_p);
			p_p->experience = exp;
			p_p->attr_addition = attr_add;
			p_p->hp = hp;
			p_p->mp = mp;
			hpmax = p_p->hp_max;
			mpmax = p_p->mp_max;
		}
	} else {
		
		
		p->experience = exp;
		p->attr_addition = attr_add;
		p->hp = hp;
		p->mp = mp;
		hpmax = p->hp_max;
		mpmax = p->mp_max;
		monitor_sprite(p, "ALLOCE EXP");
		
		update_when_level_change(p, p->level, level);
	}

	KDEBUG_LOG(p->id, "ALLOCATE EXP BOX CALLBACK\t[%u %u %u %u %u %u]", \
		petid, p->level, p->experience, p->expbox, p->hp, p->mp);

	noti_expbox_change(p);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, petid, i);
	PKG_UINT32(msg, level, i);
	PKG_UINT32(msg, exp, i);
	PKG_UINT32(msg, attr_add, i);
	PKG_UINT32(msg, hp, i);
	PKG_UINT32(msg, hpmax, i);
	PKG_UINT32(msg, mp, i);
	PKG_UINT32(msg, mpmax, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int get_offline_msg_callback (sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t maxlen, count, msglen, curlen = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, maxlen, j);
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, maxlen + 8);
	KDEBUG_LOG(p->id, "OFFLINE MSG\t[%u %u]", maxlen, count);
	rsp_proto_login(p);
	
	for (uint32_t loop = 0; loop < count; loop ++) {
		UNPKG_H_UINT32(buf, msglen, j);
		if (!msglen || msglen > maxlen || curlen + msglen > maxlen || curlen + msglen > OFFLINE_MSG_LEN)
			break;
		curlen += msglen;
		UNPKG_STR(buf, msg, j, msglen);
		send_to_self(p, msg, msglen, 0);
	}
	
	response_proto_enter_map(p, 1,1);
	notify_next_fight_step(p);
	return 0;
}

int parent_add_invited(sprite_t* p)
{
	if(IS_NORMAL_ID(p->parent_id)) {
		return send_request_to_db(NULL, p->parent_id, proto_db_add_invited, NULL, 0);
	}
	return 0;
}

int get_invited_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	return send_request_to_db(p, p->id, proto_db_get_invited, NULL, 0);
}

int get_invited_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t invited_count;
	uint32_t already_times;

	CHECK_DBERR(p, ret);
	
	int j = 0;
	UNPKG_H_UINT32(buf, invited_count, j);
	UNPKG_H_UINT32(buf, already_times, j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, invited_count, i);
	PKG_UINT32(msg, already_times, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int read_news_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t type;
	int i = 0;
	UNPKG_UINT32(body, type, i);
	switch (type) {
	case 1:
		msg_log_read_news(p->id);
		break;
	case 2:
		msg_log_read_vip(p->id);
		break;
	case 3:
		msg_log_read_calendar(p->id);
		break;
	case 4:
		msg_log_plant_active(1);
		break;
	case 5:
		msg_log_plant_active(0);
		break;
	default:
		if (type < 15)
			msg_log_game(p->id, type-6);
		break;
	}
	
	p->waitcmd = 0;
	return 0;
}

int set_flag_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t bit, flag;
	int j = 0;
	UNPKG_UINT32(body, bit, j);
	UNPKG_UINT32(body, flag, j);

	CHECK_TRUE_FALSE_VALID(p, flag);

	*(uint32_t *)p->session = 0;

	switch (bit) {
		case flag_20lv_bit:
		case flag_vip_1st_on:
		case flag_vip_item_tip:
		case flag_vip_item_tip_2:
		case flag_cli_1:
		case flag_cli_2:
		case flag_cli_3:
		case flag_cli_4:
		case flag_cli_5:
		case flag_cli_16:
		case flag_cli_31:
		case flag_auto_fight:
		case flag_fire_acitivity:
		case flag_reset_attr:
		case flag_cli_35:
		case flag_try_vip:
		case flag_try_vip2:
		case flag_try_vip3:
			break;
		case flag_hero_cup_bit:
			if (flag || !(p->flag & (1 << bit)) || !p->hero_team)
				return -1;
			break;
		case flag_chaola_bit:
			*(uint32_t *)p->session = 1;
		case flag_fashion_bit:
			*(uint32_t *)p->session = 2;
			break;
		default:
			KDEBUG_LOG(p->id,"33333 opt flag_cli_31: id :%u",flag_cli_31 );
			return -1;
	}

	return db_set_flag(p, p->id, bit, flag);
}

int post_msg_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint8_t title[64] = {0}, msg[4096] = {0}, out[4096] = {0};
	int i = 0, type;
	uint32_t msglen;
	UNPKG_UINT32(body, type, i);
	UNPKG_STR(body, title, i, 60);
	UNPKG_UINT32(body, msglen, i);

	CHECK_INT_LE(msglen, 3072);
	CHECK_INT_GE(msglen, 4);
	CHECK_BODY_LEN(len, msglen + 68);
	UNPKG_STR(body, msg, i, msglen);

	KDEBUG_LOG(p->id, "POST MSG\t[%u %s %u %s %u]", type, title, msglen, msg, len);

	int j = 0;
	PKG_H_UINT32(out, 7, j);
	PKG_H_UINT32(out, 0, j);
	PKG_H_UINT32(out, type, j);
	PKG_STR(out, p->nick, j, USER_NICK_LEN);
	PKG_STR(out, title, j, 60);
	PKG_H_UINT32(out, msglen, j);
	PKG_STR(out, msg, j, msglen);
	send_udp_post_msg_to_db(NULL, p->id, proto_db_post_msg, out, j);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int start_double_exp_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	if (p->double_exptime != 0xFFFFFFFF) {
		KERROR_LOG(p->id, "no need");
		return -1;
	}
	
	p->double_exptime = 1;
	p->start_dexp_time = get_now_tv()->tv_sec;
	db_day_add_ssid_cnt(NULL, p->id, item_id_double_exp, 1, holidy_factor[get_now_tm()->tm_wday].timetype ? 3600*3 : 1800);
	int i = sizeof(protocol_t);
	i += pkg_ol_time(p, msg + i);
	init_proto_head(msg, p->waitcmd,  i);
	return send_to_self(p, msg, i, 1);
}

int cli_get_survey_info(sprite_t* p, Cmessage* c_in)
{
	item_t* pday = cache_get_day_limits(p, survey_id_201108);
	cli_get_survey_info_out cli_out;
	cli_out.surveyed = pday->count != 0;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_engage_survey(sprite_t* p, Cmessage* c_in)
{
	item_t* pday = cache_get_day_limits(p, survey_id_201108);
	if (pday->count) 
		return send_to_self_error(p, p->waitcmd, cli_err_ever_surveyed, 1);
	cli_engage_survey_in* p_in = P_IN;
	mole2_reply_question_in db_in;
	db_in.surveyid = survey_id_201108;
	for (uint32_t loop = 0; loop < p_in->answerlist.size(); loop ++) {
		stru_survey_reply tmpreply;
		tmpreply.option_id = p_in->answerlist[loop].id;
		tmpreply.value_id = p_in->answerlist[loop].count;
		db_in.replys.push_back(tmpreply);
	}

	return send_msg_to_db(p, p->id, mole2_reply_question_cmd, &db_in);
}

int cli_engage_skill_survey(sprite_t* p, Cmessage* c_in)
{
	cli_engage_skill_survey_in* p_in = P_IN;
	CHECK_ITEM_EXIST(p, ITEM_ID_TICKET, 1);
	CHECK_INT_LE(p_in->option, 2);
	mole2_reply_question_in db_in;
	db_in.surveyid = survey_id_pet_skill;
	stru_survey_reply tmpreply;
	tmpreply.option_id = 1;
	tmpreply.value_id = p_in->option;
	db_in.replys.push_back(tmpreply);
	send_msg_to_db(NULL, p->id, mole2_reply_question_cmd, &db_in);
	cache_reduce_kind_item(p, ITEM_ID_TICKET, 1);
	db_add_item(NULL, p->id, ITEM_ID_TICKET, -1);
	cache_add_kind_item(p, ITEM_ID_XXXX, 1);
	db_add_item(NULL, p->id, ITEM_ID_XXXX, 1);
	cli_engage_skill_survey_out cli_out;
	cli_out.item.itemid = ITEM_ID_XXXX;
	cli_out.item.count = 1;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int mole2_reply_question(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	item_t* pday = cache_get_day_limits(p, survey_id_201108);
	pday->count ++;
	db_day_add_ssid_cnt(NULL, p->id, survey_id_201108, 1, 1);
	p->xiaomee += 2000;
	db_add_xiaomee(NULL, p->id, 2000);
	cli_engage_survey_out cli_out;
	cli_out.item.itemid = item_id_xiaomee;
	cli_out.item.count = 2000;
	
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_skill_survey_info(sprite_t* p, Cmessage* c_in)
{
	mole2_get_survey_in db_in;
	db_in.surveyid = survey_id_pet_skill;
	return send_msg_to_db(p, p->id, mole2_get_survey_cmd, &db_in);
}

int mole2_get_survey(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_survey_out* p_out = P_OUT;
	cli_get_skill_survey_info_out cli_out;
	for (uint32_t loop = 0; loop < p_out->replys.size(); loop ++) {
		stru_id_cnt tmp;
		if (p_out->replys[loop].option_id == 1) {
			tmp.id = p_out->replys[loop].answer_id;
			tmp.count = p_out->replys[loop].total;
			cli_out.answerlist.push_back(tmp);
		}
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_item_valid_cnt(sprite_t* p, Cmessage * c_in)
{
	cli_get_item_valid_cnt_in *p_in=P_IN;
	cli_get_item_valid_cnt_out cli_out;
	for(uint32_t loop=0; loop < p_in->eids.size(); loop++){
		exchange_info_t * pei=get_exchange_info(p_in->eids[loop]);
		//KDEBUG_LOG(p->id, "get valid item cnt ---exchangeid [%u]",p_in->eids[loop]);
		if(pei){
			stru_ex_item tmp;
			tmp.eid=pei->id;
			tmp.itemid=pei->getitems[0].itemid;
			//KDEBUG_LOG(p->id, "get valid item cnt ---itemid [%u]",pei->getitems[0].itemid);
			cli_out.vals.push_back(tmp);
		}
	}
	return send_msg_to_db(p, p->id, mole2_get_val_by_id_cmd, &cli_out);
}
int mole2_get_val_by_id(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	mole2_get_val_by_id_out* p_out = P_OUT;
	mole2_get_val_by_id_out cli_out;
	CHECK_DBERR(p, ret);
	for(uint32_t loop = 0; loop < p_out->vals.size(); loop ++) {
		exchange_info_t * pei=get_exchange_info(p_out->vals[loop].id);
		stru_id_cnt tmp;
		memset(&tmp, 0, sizeof(tmp));
		tmp.id = p_out->vals[loop].id;
		//KDEBUG_LOG(p->id, "get val by id itemid1   [%u] count[%u]",tmp.id,p_out->vals[loop].val); 
		if(pei && pei->limit_cnt!=0){
			tmp.count = abs(pei->limit_cnt-p_out->vals[loop].count);
			cli_out.vals.push_back(tmp);
		}
		//KDEBUG_LOG(p->id, "get val by id itemid2  [%u] count[%u]",tmp.id,tmp.count); 
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_pet_skill_reward(sprite_t* p, Cmessage* c_in)
{
	cli_get_pet_skill_reward_in* p_in = P_IN;
	if (p_in->index != 3)
		return -1;
	CHECK_INT_LE(p_in->type, 2);
	uint32_t ssid = ssid_pet_skill_reward_1 + p_in->type;

	p->game_start_time = 0;
	p->game_type = -1;

	item_t* pday = cache_get_day_limits(p, ssid);
	if (pday->count) {
		KERROR_LOG(p->id, "count max");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	uint32_t itemid = rewards[p_in->type][p_in->index].itemid;

	if (get_cloth(itemid)) {
		if (get_bag_cloth_cnt(p) >= get_max_grid(p)) {
			KERROR_LOG(p->id, "cloth bag full");
			return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
		}
		*(uint32_t *)(p->session + 4) = 0;
	} else {
		normal_item_t* pni = get_item(itemid);
		CHECK_ITEM_VALID(p, pni, itemid);
		CHECK_CAN_HAVE_MORE_ITEM(p, pni, 1);
		*(uint32_t *)(p->session + 4) = 1;
	}

	*(uint32_t *)(p->session + 8) = p_in->type;
	*(uint32_t *)(p->session + 12) = p_in->index;
	*(uint32_t *)(p->session + 16) = ssid;

	if (p_in->index == 3) {
		db_day_add_ssid_cnt(NULL, p->id, ssid, 1, 1);
		pday->count ++;
		cache_add_kind_item(p, itemid, 1);
		db_add_item(NULL, p->id, itemid, 1);
		cli_get_pet_skill_reward_out cli_out;
		stru_item tmpitem;
		tmpitem.itemid = itemid;
		tmpitem.count = 1;
		cli_out.itemlist.push_back(tmpitem);
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	} else {
		*(uint32_t *)p->session = itemid;

		mole2_add_val_in db_in;
		db_in.userid = p_in->type * 3 + p_in->index;
		db_in.limit = rewards[p_in->type][p_in->index].count;
		return send_msg_to_db(p, p->id, mole2_add_val_cmd, &db_in);
	}
}

int mole2_add_val(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_add_val_out* p_out = P_OUT;
	switch(p->waitcmd){
		case cli_exchange_cmd:{
			uint32_t id=*((uint32_t*)p->session);
			exchange_info_t* pei =get_exchange_info(id);
			if(!pei || pei->limit_cnt == 0){
				KERROR_LOG(p->id,"exchange info error[%u]",id);
				return send_to_self_error(p, p->waitcmd, cli_err_exchange_id_invalid, 1);
			}
			KDEBUG_LOG(p->id, "START EXCHANG LIMITED ITEM\t[%u]", pei->id);
			item_t add_items[100];
			uint32_t loop;
			memset(add_items, 0, sizeof(add_items));
			for (loop = 0; loop < pei->getcnt; loop ++) {
				add_items[loop].itemid = pei->getitems[loop].itemid;
				add_items[loop].count = cache_add_kind_item(p, pei->getitems[loop].itemid, pei->getitems[loop].count);
				if (add_items[loop].count != pei->getitems[loop].count) {
					for (int i = 0; i < 100; i ++) {
						if (add_items[loop].itemid == 0) {
							break;
						}
						cache_reduce_kind_item(p, add_items[i].itemid, add_items[i].count);
					}
					return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
				}
			}
			for (loop = 0; loop < pei->getcnt; loop ++) {
				KDEBUG_LOG(p->id, "EXCHANGE ADD LIMITED ITEM\t[%u %u]", pei->getitems[loop].itemid, pei->getitems[loop].count );
				db_add_item(NULL, p->id, pei->getitems[loop].itemid, pei->getitems[loop].count );
				sysinfo_t* psys = get_sys_info(sys_info_get_item, pei->getitems[loop].itemid);
				if (psys)
					noti_all_get_item(p, psys);
			}
			for (loop = 0; loop < pei->paycnt; loop ++) {
				item_enough_del(p, pei->payitems[loop].itemid, pei->payitems[loop].count );
			}
			msg_log_pkpt_exchange(p->id, pei->id);
			if (pei->type == item_cloth)
				return db_get_packet_cloth_list(p);
			KDEBUG_LOG(p->id, "RESPONSE PROTO\t[%u %u]", p->pkpoint_total, pei->id);
			response_proto_uint32(p, p->waitcmd, p->pkpoint_total, 1, 0);
			return 0;
			}
		case cli_get_pet_reward_info_cmd:{
			uint32_t ssid = *(uint32_t *)(p->session + 16);
			db_day_add_ssid_cnt(NULL, p->id, ssid, 1, 1);
			item_t* pday = cache_get_day_limits(p, ssid);
			pday->count ++;
			cli_get_pet_skill_reward_out cli_out;
			stru_item tmpitem;
			tmpitem.itemid = *(uint32_t *)p->session;
			tmpitem.count = 1;
			cli_out.itemlist.push_back(tmpitem);
			if (*(uint32_t *)(p->session + 4)) {
				cache_add_kind_item(p, tmpitem.itemid, tmpitem.count);
				db_add_item(NULL, p->id, tmpitem.itemid, tmpitem.count);
				return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
			} else {
				cloth_t* pcloth = get_cloth(tmpitem.itemid);
				if (!pcloth || !pcloth->clothlvs[0].valid) {
					KERROR_LOG(p->id, "fail to get cloth\t[%u %u]", tmpitem.itemid, tmpitem.count);
					return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
				}
				return db_add_cloth(p, p->id, 0, pcloth, 0, &pcloth->clothlvs[0]);
			}
	 	}
	}
	return 0;
}

int cli_get_pet_reward_info(sprite_t* p, Cmessage* c_in)
{
	return send_msg_to_db(p, p->id, mole2_get_val_cmd, NULL);
}

int mole2_get_val(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_val_out* p_out = P_OUT;
	cli_get_pet_reward_info_out cli_out;
	uint32_t count[3][3];
	memset(count, 0, sizeof(count));
	for (uint32_t loop = 0; loop < p_out->vals.size(); loop ++) {
		if (p_out->vals[loop].userid < 9) {
			int type = p_out->vals[loop].userid / 3;
			int index = p_out->vals[loop].userid % 3;
			count[type][index] = p_out->vals[loop].val;
		}
	}

	for (uint32_t loop = 0; loop < 3; loop ++) {
		for (uint32_t loopi = 0; loopi < 3; loopi ++) {
			stru_game_info tmp;
			tmp.type = loop;
			tmp.index = loopi;
			tmp.itemid = rewards[loop][loopi].itemid;
			tmp.count = rewards[loop][loopi].count > count[loop][loopi] \
				? rewards[loop][loopi].count - count[loop][loopi] : 0;
			cli_out.glist.push_back(tmp);
		}
	}

	item_t* pday = cache_get_day_limits(p, ssid_pet_skill_reward_1);
	stru_id_cnt tmp;
	tmp.id = pday->itemid - ssid_pet_skill_reward_1;
	tmp.count = pday->count;
	cli_out.ulist.push_back(tmp);

	pday = cache_get_day_limits(p, ssid_pet_skill_reward_2);
	tmp.id = pday->itemid - ssid_pet_skill_reward_1;
	tmp.count = pday->count;
	cli_out.ulist.push_back(tmp);

	pday = cache_get_day_limits(p, ssid_pet_skill_reward_3);
	tmp.id = pday->itemid - ssid_pet_skill_reward_1;
	tmp.count = pday->count;
	cli_out.ulist.push_back(tmp);

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int get_bus_friend_uplist(sprite_t* p, Cmessage* c_in)
{
	p->waitcmd = 0;
	return 0;
	get_bus_friend_uplist_in* p_in = P_IN;
	bus_get_friend_uplist_in bus_in;
	for (uint32_t loop = 0; loop < p_in->friendlist.size(); loop ++) {
		bus_in.friend_list.push_back(p_in->friendlist[loop]);
	}
	return send_msg_to_db(p, p->id, bus_get_friend_uplist_cmd, &bus_in);
}

int bus_get_friend_uplist(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_bus_friend_uplist_out* p_out = (get_bus_friend_uplist_out *)c_out;
	return send_msg_to_self(p, p->waitcmd, p_out, 1);
}

int get_bus_friend_base_info(sprite_t* p, Cmessage* c_in)
{
	p->waitcmd = 0;
	return 0;
	get_bus_friend_base_info_in* p_in = P_IN;
	bus_get_friend_base_info_in bus_in;
	bus_in.friendid = p_in->userid;
	return send_msg_to_db(p, p->id, bus_get_friend_base_info_cmd, &bus_in);
}

int bus_get_friend_base_info(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_bus_friend_base_info_out* p_out = (get_bus_friend_base_info_out *)c_out;
	return send_msg_to_self(p, p->waitcmd, p_out, 1);
}

#define HANG_ROUND_TIME		30
#define MAX_HANG_OL_TIME	(18000 - HANG_ROUND_TIME)
#define MAX_SKILL_HANG_TIME 3600

void clean_hangup_info(sprite_t* p)
{
	hangup_uids[p->seatid - 1] = 0;
	p->team_state = 0;
	REMOVE_TIMER(p->p_hangup_timer);
	p->p_hangup_timer = NULL;
	p->seatid = 0;
}

void rsp_stop_hangup(sprite_t* p, uint32_t reason, int completed)
{
	cli_stop_hangup_out cli_out;
	cli_out.uid = p->id;
	cli_out.reason = reason;
	send_msg_to_map(p, cli_stop_hangup_cmd, &cli_out, completed, 1);
}

int hangup_get_exp(void* owner, void* data)
{
	sprite_t* p = (sprite_t *)owner;

	uint32_t itemid, count;

	switch (p->team_state) {
		case hang_for_exp:
			itemid = item_id_exp;
			count = exp_hang_base[p->level];
			//count *= 2;
			count = add_exp_to_sprite(p, count);
			break;
		case hang_for_pet_exp:
			itemid = item_id_exp_box;
			count = exp_hang_base[p->level];
			//count *= 2;
			if (ISVIP(p->flag))
				count = count * 2;
			p->expbox += count;
			db_set_expbox(NULL, p->id, p->expbox);
			break;
		case hang_for_skill_exp:
			itemid = item_id_skill_expbox;
			count = ISVIP(p->flag) ? 20 : 10;
			//count *= 2;
			p->skill_expbox += count;
			db_add_skill_expbox(NULL, p->id, count);
			p->hang_skill_time += HANG_ROUND_TIME;
			db_day_add_ssid_cnt(NULL, p->id, item_id_hang_skill, HANG_ROUND_TIME, -1);
			break;
	}

	noti_cli_hang_get_exp_out cli_out;
	cli_out.uid = p->id;
	cli_out.item.itemid = itemid;
	cli_out.item.count = count;
	cli_out.hang_skill_time = p->hang_skill_time;
	send_msg_to_map(p, noti_cli_hang_get_exp_cmd, &cli_out, 0, 1);
	
	if (get_ol_time(p) > MAX_HANG_OL_TIME || (p->team_state == hang_for_skill_exp && p->hang_skill_time > MAX_SKILL_HANG_TIME)) {
		clean_hangup_info(p);
		rsp_stop_hangup(p, 1, 0);
	} else {
		p->p_hangup_timer = ADD_ONLINE_TIMER(p, n_hangup_get_exp, NULL, HANG_ROUND_TIME);
	}

	return 0;
}

int cli_start_hangup(sprite_t* p, Cmessage* c_in)
{
	CHECK_SELF_JOIN_TEAM(p);
	cli_start_hangup_in* p_in = P_IN;
	if (!p_in->seatid || p_in->seatid > SEAT_NUM) {
		KERROR_LOG(p->id, "invalid seat\t[%u]", p_in->seatid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	if (p->seatid) {
		KERROR_LOG(p->id, "already sit\t[%u]", p->seatid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	if (!p_in->seatid || p_in->seatid > SEAT_NUM || !p_in->type || p_in->type > SEAT_TYPE) {
		KERROR_LOG(p->id, "invalid seat\t[%u %u]", p_in->seatid, p_in->type);
	}

	uint32_t seatid = p_in->seatid + (p_in->type - 1) * SEAT_NUM;

	if (hangup_uids[seatid - 1]) {
		KERROR_LOG(p->id, "not null\t[%u %u]", p_in->seatid, hangup_uids[seatid - 1]);
		return send_to_self_error(p, p->waitcmd, cli_err_seat_has_mole, 1);
	}

	uint32_t oltime = get_ol_time(p);
	if (oltime > MAX_HANG_OL_TIME) {
		KERROR_LOG(p->id, "time not enough\t[%u]", oltime);
		return send_to_self_error(p, p->waitcmd, cli_err_user_tired, 1);
	}

	switch (p_in->type) {
	case 1:
		CHECK_USER_IN_MAP(p, 11309);
		p->team_state = hang_for_exp;
		msg_log_exp_hang(p->id);
		break;
	case 2:
		CHECK_USER_IN_MAP(p, 11311);
		p->team_state = hang_for_pet_exp;
		msg_log_pet_hang(p->id);
		break;
	case 3:
		CHECK_USER_IN_MAP(p, 11310);
		if (p->hang_skill_time > MAX_SKILL_HANG_TIME) {
			KERROR_LOG(p->id, "hang skill max\t[%u]", p->hang_skill_time);
			return send_to_self_error(p, p->waitcmd, cli_err_skill_hang_max, 1);
		}
		p->team_state = hang_for_skill_exp;
		msg_log_skill_hang(p->id);
		break;
	}

	p->seatid = seatid;
	hangup_uids[seatid - 1] = p->id;
	p->p_hangup_timer = ADD_ONLINE_TIMER(p, n_hangup_get_exp, NULL, HANG_ROUND_TIME);

	cli_start_hangup_out cli_out;
	cli_out.uid = p->id;
	cli_out.seatid = p_in->seatid;
	cli_out.type = p_in->type;
	
	send_msg_to_map(p, p->waitcmd, &cli_out, 1, 1);
	return 0;
}

int cli_stop_hangup(sprite_t* p, Cmessage* c_in)
{
	if (!p->seatid) {
		KERROR_LOG(p->id, "not in seat");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	clean_hangup_info(p);	
	rsp_stop_hangup(p, 0, 1);
	return 0;
}

int cli_get_seat_info(sprite_t* p, Cmessage* c_in)
{
	cli_get_seat_info_in* p_in = P_IN;
	if (!p_in->type || p_in->type > SEAT_TYPE) {
		KERROR_LOG(p->id, "invalid seat type\t[%u]", p_in->type);
		return -1;
	}
	cli_get_seat_info_out cli_out;
	cli_out.hang_skill_time = p->hang_skill_time;
	cli_out.type = p_in->type;
	for (uint32_t loop = 0; loop < SEAT_NUM; loop ++) {
		if (hangup_uids[loop]) {
			stru_pos_uid tmp;
			tmp.pos = loop + 1;
			tmp.userid = hangup_uids[loop + (p_in->type - 1) * SEAT_NUM];
			cli_out.seatuids.push_back(tmp);
		}
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

/**
 * @brief 每日签到，赠送物品
 * 2012-5-7 
 * 3天赠送：追加 350009 Name="精灵1.5倍经验丸" 
 * 4天赠送：追加 350010 Name="精灵2倍经验丸" 
 * 5天赠送：追加 350021 Name="红豆月饼" "
 * @return 返回获得的id count 
 */
int cli_signin_day(sprite_t* p, Cmessage* c_in)
{
	if (p->flag & (1 << flag_signin_bit)) {
		KERROR_LOG(p->id, "signed");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	if (p->login_day == 5 && p->chemical_grid >= get_max_grid(p)) {
		KERROR_LOG(p->id, "bag full\t[%u]", p->chemical_grid);
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}

	CHECK_LV_FIT(p, p, 10, MAX_SPRITE_LEVEL);
	double ratio = pow(p->login_day, 0.5);
	cli_signin_day_out cli_out;
	stru_item tmp_item;
	tmp_item.itemid = item_id_exp;
	tmp_item.count = add_exp_to_sprite(p, 3000 * ratio);
	cli_out.itemlist.push_back(tmp_item);
	tmp_item.itemid = item_id_xiaomee;
	tmp_item.count = 1000 * ratio;
	p->xiaomee += tmp_item.count;
	db_add_xiaomee(NULL, p->id, tmp_item.count);
	cli_out.itemlist.push_back(tmp_item);
	memset(&tmp_item,0,sizeof(stru_item));
	if( p->login_day == 3){
		tmp_item.itemid = 350003;
		tmp_item.count = 1;
	}else if(p->login_day == 4){
		tmp_item.itemid = 350010;
		tmp_item.count = 1;
	}else if (p->login_day == 5) {
		uint32_t items[8] = {310003, 310005, 310007, 350003, 350009, 350013, 350014,350021};
		uint32_t percents[8] = {150, 130, 120, 130, 120, 100, 100,150};
		int idx = rand_type_idx(8, percents, RAND_COMMON_RAND_BASE);
		tmp_item.itemid = items[idx];
		tmp_item.count = 1;
		//cache_add_kind_item(p, tmp_item.itemid, 1);
		//db_add_item(NULL, p->id, tmp_item.itemid, 1);
		//cli_out.itemlist.push_back(tmp_item);
	}
	if( tmp_item.itemid && tmp_item.count){
		cache_add_kind_item(p, tmp_item.itemid, 1);
		db_add_item(NULL, p->id, tmp_item.itemid, 1);
		cli_out.itemlist.push_back(tmp_item);
		msg_log_login_reward(p->login_day, p->id);
	}
	//临时赠送精灵经验笔记
	tmp_item.itemid = 300046;
	tmp_item.count = 2;
	cache_add_kind_item(p, tmp_item.itemid, 2);
	db_add_item(NULL, p->id, tmp_item.itemid, 2);
	cli_out.itemlist.push_back(tmp_item);

	p->flag |= 1<< flag_signin_bit;
	db_set_flag(NULL, p->id, flag_signin_bit, 1);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int is_in(uint32_t *arr, uint32_t val, uint32_t cnt)
{
   int ret=-1;
   for(uint32_t i=0; i<cnt; i++){
	//KDEBUG_LOG(1111,"111[%u]",arr[i]);
	if(arr[i]==val){
	   ret=i;
	   break;
   	}
   }
 	return ret;
}

inline int clear_raceskill_cache(sprite_t *p)
{
	p->skill_id_cache_cnt=0;
	memset(p->skill_id_cache,0,MAX_SKILL_CACHE);
	p->petid_in_use=0;
	p->item_in_use=0;
	p->stone_in_use=0;
	p->stone_cnt=0;
    return 0;
}
//种族技能历练
int cli_train_race_skill(sprite_t *p, Cmessage* c_in)
{
	cli_train_race_skill_in *p_in=P_IN;
	cli_train_race_skill_out cli_out;

	KDEBUG_LOG(p->id, "TRAIN RACE SKILL\t[%u]", p_in->petid);

    if(p->item_in_use !=0){
		if(p->petid_in_use==p_in->petid){
			for(uint32_t i=0; i < p->skill_id_cache_cnt; i++)
				cli_out.skill_list.push_back(p->skill_id_cache[i]);
			return send_msg_to_self(p,p->waitcmd, &cli_out, 1);
		}
		else {//different petid then delete the item and clear cache
			KDEBUG_LOG(p->id,"petid is diffrent  and delete item");
			del_item(p,p->item_in_use,1);
			del_item(p,p->stone_in_use,p->stone_cnt);
			clear_raceskill_cache(p);
			return send_msg_to_self(p,p->waitcmd, &cli_out, 1);
		}
	}
	normal_item_t *pnis  = get_item(p_in->itemid);
	CHECK_ITEM_EXIST(p, p_in->itemid, 1);
	CHECK_ITEM_VALID(p, pnis, p_in->itemid);
	CHECK_ITEM_FUNC(p, pnis, item_for_race_skill);
	normal_item_t *pnis2  = get_item(p_in->extra_itemid);
    if(p_in->count>0){
		CHECK_ITEM_EXIST(p, p_in->extra_itemid, p_in->count);
		CHECK_ITEM_VALID(p, pnis2, p_in->extra_itemid);
		CHECK_ITEM_FUNC(p, pnis2, item_for_precious_stone);
    }
	pet_t* pet_p=get_pet_inbag(p, p_in->petid);
	if(!pet_p){
		KERROR_LOG(p->id, "pet not in bag[%u]",p_in->petid);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_not_inbag, 1);
	}
	uint8_t idx=MAX_RACE_SKILL_TYPE;
	for(uint8_t i=0; i<MAX_RACE_SKILL_TYPE; i++){
		if(pnis->race_skill_info.type_info[i].raceid == pet_p->race)
			idx=i;
	}
	if(idx>=MAX_RACE_SKILL_TYPE){
		KERROR_LOG(p->id, "races mismatch[%u]",  pet_p->race);
		return send_to_self_error(p, p->waitcmd, cli_err_attr_mismatch, 1);
	}
	if(p_in->count >10){
		KERROR_LOG(p->id, "stone > 10[%u]", p_in->count);
		return send_to_self_error(p, p->waitcmd, cli_err_attr_mismatch, 1);
	}
	for(uint32_t i=0; i<MAX_GET_SKILL; i++){
    	if(pnis->race_skill_info.order[i] !=0){
			uint32_t share_rand = rand() % RAND_COMMON_RAND_BASE;
			uint32_t add_rate = pnis2 ? p_in->count*pnis2->rate : 0;
			if((pnis->race_skill_info.order[i]+add_rate) >= share_rand){
				 int j=rand_type_idx(3, pnis->race_skill_info.type_info[idx].percent, RAND_COMMON_RAND_BASE);
				 p->skill_id_cache[p->skill_id_cache_cnt++]=pnis->race_skill_info.type_info[idx].skillid[j];
				 cli_out.skill_list.push_back(pnis->race_skill_info.type_info[idx].skillid[j]);
				 KDEBUG_LOG(p->id, "TRAIN RACE SKILL GEN\t[%u %u]", p_in->petid,\
				 		pnis->race_skill_info.type_info[idx].skillid[j]);
			}
        }
	}
	if(p->skill_id_cache_cnt==0){
		del_item(p,p_in->itemid,1);
		if(p_in->count>0)
		 	del_item(p,p_in->extra_itemid,p_in->count);
	}
	else{
		p->item_in_use=p_in->itemid;
		p->petid_in_use=p_in->petid;
		if(p_in->count>0){
			p->stone_in_use=p_in->extra_itemid;
			p->stone_cnt=p_in->count;
		}
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

//cli_train_race_skill callback
int cli_change_race_skill(sprite_t* p, Cmessage* c_in)
{
	cli_change_race_skill_in *p_in=P_IN;
	KDEBUG_LOG(p->id, "CHANGE RACE SKILL\t[%u]", p_in->petid);
	Cmessage cli_out;
	skill_t *skill=NULL;
	pet_t* pet_p=get_pet_inbag(p, p_in->petid);
	if(!pet_p){
		KERROR_LOG(p->id, "pet not in bag[%u]",	p_in->petid);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_not_inbag, 1);
	}
	if(p_in->petid != p->petid_in_use){
		KERROR_LOG(p->id, "petid mismatch[%u   %u]",p_in->petid,p->petid_in_use);
		return send_to_self_error(p, p->waitcmd, cli_err_petid_mismatch, 1);
	}

	uint32_t tmpcnt = 0;
	skill_t* tmpskill[MAX_GET_SKILL]={};
	uint32_t desskill[MAX_GET_SKILL]={};
	if(p_in->exchange_list.size()>MAX_GET_SKILL){
		KERROR_LOG(p->id, "too long list  len>[%u]",MAX_GET_SKILL);
		return send_to_self_error(p, p->waitcmd, cli_err_list_full, 1);
	}

	for(uint32_t i=0; i< p_in->exchange_list.size(); i++){
		KDEBUG_LOG(p->id, "CHANGE RACE SKILL LIST\t[%u %u %u]", p_in->petid, p_in->exchange_list[i].src_id, p_in->exchange_list[i].des_id);
		if (p_in->exchange_list[i].src_id ==p_in->exchange_list[i].des_id)
			continue;
		skill=cache_get_pet_skill(pet_p, p_in->exchange_list[i].src_id);
		if((is_in(p->skill_id_cache, p_in->exchange_list[i].des_id, p->skill_id_cache_cnt) < 0) || 
			!skill ||skill->skill_exp !=1){
			KERROR_LOG(p->id, "skill not in skill_id_cache or not have[%u %u]", 
				p_in->exchange_list[i].src_id, p_in->exchange_list[i].des_id);			
			return send_to_self_error(p, p->waitcmd, cli_err_skill_not_have, 1);
		}
		tmpskill[tmpcnt]=skill;
		desskill[tmpcnt]=p_in->exchange_list[i].des_id;
		tmpcnt++;
	}

	if (tmpcnt == 2 && (tmpskill[0] == tmpskill[1] || desskill[0] == desskill[1])) {
		KERROR_LOG(p->id, "skill duplicate[%u %u %u %u]", tmpskill[0]->skill_id, tmpskill[1]->skill_id, desskill[0], desskill[1]);	
		return send_to_self_error(p, p->waitcmd, cli_err_skill_not_have, 1);
	}

	if (tmpcnt == 2 && tmpskill[1]->skill_id == desskill[0]) {
		tmpcnt --;
		desskill[0] = desskill[1];
	}
	del_item(p,p->item_in_use,1);
 	if(p->stone_cnt>0 && p->stone_in_use !=0)
		del_item(p,p->stone_in_use,p->stone_cnt);
	//cache_reduce_kind_item(p, p->item_in_use, 1);
	//db_add_item(NULL, p->id, p->item_in_use, -1);
	for(uint32_t i=0; i< tmpcnt; i++){
		db_del_pet_skill(NULL, p->id, pet_p->petid, tmpskill[i]->skill_id);
		tmpskill[i]->skill_id=desskill[i];
	}
	for(uint32_t i=0; i< tmpcnt; i++){
		db_buy_pet_skill(NULL, p->id, pet_p, desskill[i], 0xFF, 0);
	}
//clear cache	
	clear_raceskill_cache(p); 
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}
/*
//向switch发出pk申请 等待匹配
void apply_gvg_to_switch(sprite_t *p)
{
	uint32_t count = 1;
	uint32_t level = p->level;
	level = get_avg_level(p);
	p->team_state = apply_for_pk;
	uint8_t buf[32];
	int i = 0;
	PKG_H_UINT32(buf, 3, i);
	PKG_H_UINT32(buf, level, i);
	PKG_H_UINT32(buf, count, i);
	KDEBUG_LOG(p->id,"apply gvg pk to switch [fightpower=%u]",count);
	send_to_switch(NULL, COCMD_onli_apply_pk, i, buf, p->id);
}
//通知所有队友队伍的信息和战斗力
int notice_team_teaminfo(sprite_t *p)
{
	cli_get_btrteam_info_out cli_out;
    stru_id_cnt tmp;
    if(p->btr_team)
	{
        for(uint32_t loop=0 ; loop < p->btr_team->count ; loop++ )
		{
            tmp.id=p->btr_team->players[loop]->id;
            tmp.count=get_fight_power(p->btr_team->players[loop]);
            cli_out.info_list.push_back(tmp);
        }
        for(uint32_t loop=0 ; loop < p->btr_team->count ; loop++ )
			send_msg_to_self(p,proto_cli_notice_team_info,&cli_out,0);
	}
    else{
        tmp.id=p->id;
        tmp.count=get_fight_power(p);
        cli_out.info_list.push_back(tmp);
		send_msg_to_self(p,proto_cli_notice_team_info,&cli_out,0);
    }
	return 0;
}
//清理用户的gvg信息
int rm_from_gvg_group(sprite_t * p)
{
	KDEBUG_LOG(p->id,"cancel gvg pk");
	if(!p->btr_team)
	{
		KDEBUG_LOG(p->id,"cancel gvg pk succ");
	 	if(p->gvg_wait_pos >0 )			
			gvg_groups[p->gvg_wait_pos]=0;
	}
	else
	{
		sprite_t *s=p->btr_team->players[1];
		if(TEAM_LEADER(p))
		{
			p->btr_team->players[0]=s;
			p->btr_team->players[1]=p;
			change_batter_team_id(p->btr_team,s->id);
			s->gvg_wait_pos=p->gvg_wait_pos;	
			s->gvg_team_win_cnt=0;
			KDEBUG_LOG(p->id,"new team leader [%u]",s->id);
		}
		del_mbr_from_bt(s->btr_team,p->id);
		notice_team_teaminfo(s);
	}
	p->gvg_team_win_cnt=0;
	p->gvg_wait_pos=-1;
	return 0;
}
//遍历gvg等待序列  满足条件则加入队伍 强制组队
uint32_t join_gvg_group(sprite_t *p)
{
	for( uint32_t loop=0 ; loop < MAX_GVG_WAIT ; loop++ )
	{
		sprite_t *leader=NULL;
		if( gvg_groups[loop] && (leader=get_sprite(gvg_groups[loop])) && leader->id != p->id)
		{	
			map_copy_instance_t* pmci = get_map_copy_with_mapid(leader->portal); 
			if( p->btr_team)
			{//检查队伍人数是否满足  满足将其强制加入gvg队列中的队伍  
				if((!leader->btr_team && p->btr_team->count <= MAX_GVG_USER_CNT-1 ) ||
						(leader->btr_team && p->btr_team->count+leader->btr_team->count <=  MAX_GVG_USER_CNT))
				{
					if(abs((int32_t)leader->level-(int32_t)p->level)<=3)
					{
						for(uint8_t loop=1; loop<p->btr_team->count; loop++)
						{
							pmci->uids[pmci->usercnt]=p->btr_team->players[loop]->id;
							p->btr_team->players[loop]->portal=leader->portal;
							pmci->ustate[pmci->usercnt]=1;
							pmci->usercnt++;
							del_mbr_from_bt(p->btr_team,p->btr_team->players[loop]->id);
							add_mbr_to_bt(leader, p->btr_team->players[loop]);
						}
						pmci->uids[pmci->usercnt]=p->id;
						pmci->ustate[pmci->usercnt]=1;
						pmci->usercnt++;
						p->portal=leader->portal;
						add_mbr_to_bt(leader,p);
						KDEBUG_LOG(p->id,"join a gvg group [teamid=%u]",leader->id);
						return leader->id;
					}
				}
			}
			else
			{
				if((!leader->btr_team || leader->btr_team->count < MAX_GVG_USER_CNT) &&  abs((int32_t)leader->level-(int32_t)p->level)<=3 )
				{
					p->portal=leader->portal;
					add_mbr_to_bt(leader,p);
					KDEBUG_LOG(p->id,"join a gvg group [teamid=%u]",leader->id);
					return leader->id;
				}
			}
		}
	}
    return 0;	
} 
//将用户插入gvg等待队伍
int insert_gvg_group(uint32_t uid)
{
	for( uint32_t loop=0 ; loop < MAX_GVG_WAIT; loop++ )
		if( gvg_groups[loop] == 0 )
		{
			gvg_groups[loop]=uid;
			return loop;
		}
	return -1;
}
*/
int release_copy_as_noenter(void*owner, void *data)
{
	map_copy_instance_t* pmci = (map_copy_instance_t *)owner;
	free_map_copy(pmci->p_copy_config, pmci->mapid >> 32);
	g_hash_table_remove(map_copys, &pmci->mapid);
	g_slice_free1(sizeof(map_copy_instance_t), pmci);
	return 0;
}

int init_map_copy(sprite_t* p, sprite_t* s, map_id_t mapid, map_copy_config_t* pmcc)
{
	map_copy_instance_t* pmci = get_map_copy(mapid);
	if (pmci) {
		KERROR_LOG(p->id, "duplicate copy id\t[%lx]", mapid);
		return -1;
	}
	pmci = (map_copy_instance_t *)g_slice_alloc0(sizeof(map_copy_instance_t));
	pmci->mapid = mapid;
	pmci->p_copy_config = pmcc;
	INIT_LIST_HEAD(&pmci->timer_list);
	if (!p->btr_team) {
		if (s) {
			pmci->usercnt = 2;
			pmci->uids[0] = p->id;
			pmci->uids[1] = s->id;
			pmci->ustate[0] = 1;
			pmci->ustate[1] = 1;
			p->portal = mapid;
			s->portal = mapid;
			p->team_state = apply_for_copy;
			s->team_state = apply_for_copy;
			msg_log_copy_engage(p->id, p->level);
			msg_log_copy_engage(s->id, s->level);
		} else {
			pmci->usercnt = 1;
			pmci->uids[0] = p->id;
			pmci->ustate[0] = 1;
			p->portal = mapid;
			p->team_state = apply_for_copy;
			msg_log_copy_engage(p->id, p->level);
		}
	} else {
		pmci->usercnt = p->btr_team->count;
		for (uint32_t loop = 0; loop < pmci->usercnt; loop ++) {
			pmci->uids[loop] = p->btr_team->players[loop]->id;
			p->btr_team->players[loop]->portal = mapid;
			pmci->ustate[loop] = 1;
			p->btr_team->players[loop]->team_state = apply_for_copy;
			msg_log_copy_engage(p->btr_team->players[loop]->id, p->btr_team->players[loop]->level);
		}
	}
	ADD_ONLINE_TIMER(pmci, n_release_copy_as_noenter,0, 120);
	g_hash_table_insert(map_copys, pmci, &pmci->mapid);
	return 0;
}
//gvg副本
/*
int apply_gvg_pk(sprite_t *p ,map_copy_config_t* pmcc)
{
	cli_get_map_copy_out cli_out;
	uint32_t leaderid=0;
	sprite_t* leader=NULL;
	do
	{
		if(p->btr_team)
		{
			if(p->gvg_wait_pos >= 0)
				break;
			CHECK_SELF_TEAM_LEADER(p,p->id,1);
			for (uint32_t loop = 0; loop < p->btr_team->count; loop ++)
			{
				sprite_t* s = p->btr_team->players[loop];
				item_t* pday = cache_get_day_limits(s, ssid_gvg_challenge_times);
				if (pday->count >= MAX_GVG_CHALLENGE ) 
				{
					KERROR_LOG(p->id,"gvg pk day limits[now count=%u]",pday->count);
					cli_out.uidlist.push_back(s->id);
				//	return send_to_self_error(p, p->waitcmd, cli_err_fight_too_often, 1);
				}
			}
			if (cli_out.uidlist.size())
		   	{
				cli_out.mapid = 0;
				return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
			}
		}
		else
		{
			if(p->gvg_wait_pos >= 0)
				break;
			item_t* pday = cache_get_day_limits(p, ssid_gvg_challenge_times);
			if (pday->count >= MAX_GVG_CHALLENGE) 
			{
				KERROR_LOG(p->id,"gvg pk day limits[now count=%u]",pday->count);
				return send_to_self_error(p, p->waitcmd, cli_err_fight_too_often, 1);
			}
		}
		leaderid=join_gvg_group(p);
		KDEBUG_LOG(0,"join return leaderid [%u]",leaderid);
		if(leaderid == 0)
		{
			p->gvg_wait_pos=insert_gvg_group(p->id);
			cli_out.mapid=alloc_map_copy(pmcc);
			if (init_map_copy(p, 0, ((map_id_t)cli_out.mapid)<<32, pmcc) || p->gvg_wait_pos < 0)
				return send_to_self_error(p, p->waitcmd, cli_err_mapcopy_not_enough, 1);
			KDEBUG_LOG(p->id,"join a gvg group oneself [pos=%u]",p->gvg_wait_pos);
			leaderid = p->id;
		}	

	}while(0);
	leader=get_sprite(leaderid);
	if(leader && leader->btr_team && leader->btr_team->count == MAX_GVG_USER_CNT)
		apply_gvg_to_switch(leader);
	notice_team_teaminfo(p);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}
*/

//月宫副本 定时器 
int check_users_again(void*owner, void *data)
{
	user_wait_info_t* pwait = (user_wait_info_t*)owner;
	sprite_t* p =get_sprite(pwait->userid);
	
	if (!p)
		return 0;

	map_copy_config_t* pmcc = (map_copy_config_t*)data;
	cli_get_map_copy_out cli_out;
	cli_out.mapid=alloc_map_copy(pmcc);
	if(cli_out.mapid==0){
		KERROR_LOG(p->id, "mapid is not enough");	
		ADD_ONLINE_TIMER(pwait, n_check_users_again, pmcc, 30);
		return 0;
	}

	p->portal=((map_id_t)cli_out.mapid)<<32;
	if (init_map_copy(p, NULL, ((map_id_t)cli_out.mapid)<<32, pmcc)) {
		KERROR_LOG(p->id, "mapid is not enough");	
		ADD_ONLINE_TIMER(pwait, n_check_users_again, pmcc, 30);
		return 0;
	}
	memset(pwait,0,sizeof(user_wait_info_t));
	INIT_LIST_HEAD(&pwait->timer_list);
	p->pos_in_line=-1;
	send_msg_to_self(p, cli_get_map_copy_cmd, &cli_out, 0);
	return 0;
}
//月宫副本
#define MAX_ENTER_MAP_COPY_CNT		2
int get_moon_palace_copy(sprite_t* p, map_copy_config_t* pmcc) 
{
	cli_get_map_copy_out cli_out;
	if(p->btr_team){
		CHECK_SELF_TEAM_LEADER(p,p->id,1);
		for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			item_t* pday = cache_get_day_limits(s, ssid_enter_copy_map_cnt);
			if (pday->count >= MAX_ENTER_MAP_COPY_CNT || s->level < MAP_COPY_MINI_LV) {
				cli_out.uidlist.push_back(s->id);
			}
		}
		if (cli_out.uidlist.size()) {
			cli_out.mapid = 0;
			return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
		}
		cli_out.mapid=alloc_map_copy(pmcc);
		if(cli_out.mapid==0){
			KERROR_LOG(p->id, "mapid is not enough");	
			return send_to_self_error(p, p->waitcmd, cli_err_mapcopy_not_enough, 1);
		}
		if (init_map_copy(p,NULL , ((map_id_t)cli_out.mapid)<<32, pmcc)) {
			return send_to_self_error(p, p->waitcmd, cli_err_mapcopy_not_enough, 1);
		}
	}

	item_t* pday = cache_get_day_limits(p, ssid_enter_copy_map_cnt);
	if (pday->count >= MAX_ENTER_MAP_COPY_CNT) {
		cli_out.mapid = 0;
		cli_out.uidlist.push_back(p->id);
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	}
	CHECK_LV_FIT(p, p, MAP_COPY_MINI_LV, MAX_SPRITE_LEVEL);
	uint8_t pos=(p->level-1)/MAX_SKILL_DIFF;
	uint32_t userid=0;
//check whether others are available
	for(uint32_t i=0; i<3; i++){
		uint32_t j=pos-1+i;
		if(j>=0 && j<MAX_USER_WAIT&& user_for_team[j].userid!=0){
			if(abs_uint(p->level,user_for_team[j].lv)<=MAX_SKILL_DIFF){
				userid=user_for_team[j].userid;
				break;
			}
		}
	}
	sprite_t* s = userid ? get_sprite(userid) : NULL;
	if(userid==0 || !s || s->pos_in_line == -1){
		p->team_state = apply_for_copy;
		user_for_team[pos].userid=p->id;
		user_for_team[pos].lv=p->level;
		ADD_ONLINE_TIMER(&(user_for_team[pos]), n_check_users_again, pmcc, 30);
		p->pos_in_line=pos;
		cli_out.mapid=0xFFFFFFFF;
		return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
	}
	else{
		cli_out.mapid = alloc_map_copy(pmcc);
		if (init_map_copy(p, s, ((map_id_t)cli_out.mapid)<<32, pmcc))
			return send_to_self_error(p, p->waitcmd, cli_err_mapcopy_not_enough, 1);

		send_msg_to_self(p, p->waitcmd, &cli_out, 1);
		REMOVE_TIMERS(&(user_for_team[s->pos_in_line]));
		memset(&(user_for_team[s->pos_in_line]),0,sizeof(user_wait_info_t));
		INIT_LIST_HEAD(&user_for_team[s->pos_in_line].timer_list);
		KDEBUG_LOG(p->id, "team userid[%u %u]",p->id, s->id);
		return send_msg_to_self(s, cli_get_map_copy_cmd, &cli_out, 0);
	}
			
}
//获得地图副本
int cli_get_map_copy(sprite_t* p, Cmessage* c_in)
{/*
	uint32_t nowtime = get_now_sec();
	int day_idx = get_now_tm()->tm_wday;
	switch (day_idx) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		if (!((nowtime >= 19 * 3600 && nowtime <= 21 * 3600) \
			|| (nowtime >= 12 * 3600 && nowtime <= 13 * 3600))){
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
		}
		break;
	case 6:
	case 0:
		if (!((nowtime >= 20 * 3600 && nowtime <= 21 * 3600) \
			|| (nowtime >= 13 * 3600 && nowtime <= 15 * 3600)))
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
		break;
	}
	*/
	cli_get_map_copy_in* p_in=P_IN;
	map_copy_config_t* pmcc = get_map_copy_config(p_in->copyid);
	if (!pmcc || !pmcc->copyid) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	CHECK_USER_APPLY_PK(p, p);
	switch ( 1 ){
		case 1 :
			get_moon_palace_copy(p,pmcc);	
			break;
		case 2 :
			//apply_gvg_pk(p,pmcc);	
			break;
		default :
		break;
	}
	return 0;	
}
//用户取消进入副本
int cli_not_entry(sprite_t* p, Cmessage* c_in)
{
	Cmessage cli_out;
	cli_not_entry_in* p_in=P_IN;
	switch(p_in->copyid)
	{
		case 1:
			{
				if(p->pos_in_line!=-1){
					REMOVE_TIMERS(&(user_for_team[p->pos_in_line]));
					memset(&(user_for_team[p->pos_in_line]),0,sizeof(user_wait_info_t));
					INIT_LIST_HEAD(&user_for_team[p->pos_in_line].timer_list);
					p->pos_in_line=-1;
					p->team_state = 0;
				}
				break;
			}
	/*	case 2:
			{
				CHECK_SELF_BATTLE_INFO(p, p->id);
				if(p->gvg_challenge_tag)
				{
					item_t* pday = cache_get_day_limits(p, ssid_gvg_challenge_times);
					pday->count+=pday->count<MAX_GVG_CHALLENGE?1:0;
					db_day_add_ssid_cnt(0, p->id, ssid_gvg_challenge_times, 1, MAX_GVG_CHALLENGE);
					KDEBUG_LOG(p->id,"gvg pk times++ [count=%u]",pday->count);
				}
				if(p->btr_team && p->btr_team->count == MAX_GVG_USER_CNT && p->gvg_challenge_tag == 0)
					send_to_switch(NULL, COCMD_onli_cancel_pk_apply, 0, NULL, p->btr_team->teamid);
				rm_from_gvg_group(p);
				p->team_state=0;
				break;
			}
			*/
		default:
			break;
	}
	return send_msg_to_self(p, p->waitcmd,&cli_out, 1);	
}
//拉取地图副本信息
int cli_get_copy_info(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_COPY_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "not copy map");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	cli_get_copy_info_out cli_out;
	map_copy_instance_t* pmci = get_map_copy_with_mapid(p->tiles->id);
	if (pmci) {
		map_copy_config_t* pmcc = pmci->p_copy_config;
		cli_out.copyid = pmcc->copyid;
		for (int loop = 0; loop < pmcc->layer_cnt; loop ++) {
			copy_layer_t* layer = &pmcc->layers[loop];
			stru_copy_layer tmplayer;
			tmplayer.layerid = layer->layerid;
			tmplayer.mapid = layer->templatemap;
			tmplayer.needitem = layer->needitem;
			tmplayer.needcnt = layer->needitemcnt;
			tmplayer.parentlayer = layer->parentlayer;
			tmplayer.itemdel = pmci->itemdel[loop];
			for (uint32_t loopi = 0; loopi < layer->beast_orders; loopi ++) {
				copy_beast_t* pbeast = &layer->beasts[loopi];
				stru_copy_beast tmpbeast;
				tmpbeast.order = pbeast->order;
				for (uint32_t loopj = 0; loopj < pbeast->beast_type; loopj ++) {
					stru_copy_beastgrp tmpgrp;
					tmpgrp.grpid = pbeast->grpids[loopj];
					tmpgrp.validcnt = pbeast->validcnts[loopj];
					tmpgrp.needkillcnt = pbeast->needkillcnts[loopj];
					tmpgrp.killedcnt = pmci->killedcnts[loop][loopi][loopj];
					tmpbeast.grps.push_back(tmpgrp);
				}
				tmplayer.beast.push_back(tmpbeast);
			}
			cli_out.layers.push_back(tmplayer);
		}
	}
	
	return send_msg_to_self(p, p->waitcmd,&cli_out, 1);
}
//在线礼包 拉取在线时间
int cli_get_hangup_time(sprite_t* p, Cmessage* c_in)
{
	cli_get_hangup_time_out cli_out;
	uint32_t hangup_time=get_hangup_activity_time(p);
	cli_out.online_time=(hangup_time >= MAX_HANGUP_TIME)?MAX_HANGUP_TIME : hangup_time;
	//KDEBUG_LOG(p->id,"get hangup time [%u]",cli_out.online_time);
	return send_msg_to_self(p, p->waitcmd,&cli_out, 1);
}
//在线礼包 领取奖励
int cli_get_hangup_rewards(sprite_t* p, Cmessage* c_in)
{
	cli_get_hangup_rewards_in *p_in=P_IN;
	cli_get_hangup_rewards_out cli_out;
 	if(p_in->order > 3 || ((p->hangup_rewards>>(p_in->order+4))&0x1)==1 ||
			((p->hangup_rewards>>p_in->order)&0x1)==0){
		KERROR_LOG(p->id,"can not get hangup rewards \t[%u] [%x]",p_in->order,p->hangup_rewards);
		return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1);
	}
	//KDEBUG_LOG(p->id,"get hangup reward1 [%x]",p->hangup_rewards);
	p->hangup_rewards |= (1<<(p_in->order+4));
	db_day_add_ssid_cnt(NULL, p->id, ssid_hangup_reward, 1<<(p_in->order+4),0xFF);
	//KDEBUG_LOG(p->id,"get hangup reward2 [%u] [%x]",p_in->order,p->hangup_rewards);
	cli_out.itemid=hangup_rewards[p_in->order];
	cache_add_kind_item(p, cli_out.itemid, 1);
	db_add_item(0, p->id, cli_out.itemid, 1);
	msg_log_online_gift(p_in->order, p->id);
	return send_msg_to_self(p, p->waitcmd,&cli_out, 1);
}
//在线礼包 拉取可以领取的奖励信息    
int cli_get_rewards_info(sprite_t* p, Cmessage* c_in)
{
	cli_get_rewards_info_out cli_out;
	//KDEBUG_LOG(p->id,"rewards list total1:[%x]",p->hangup_rewards);
	uint8_t order=4;
	uint32 hangup_time=get_hangup_activity_time(p);
	//KDEBUG_LOG(p->id,"rewards list hangup time:[%u]",hangup_time);
	if(hangup_time >= GOLD_REWARD_BOX )
		order=3;
	else if(hangup_time >= SILVER_REWARD_BOX )
		order=2;
	else if(hangup_time >= COPPER_REWARD_BOX)
		order=1;
	else if(hangup_time >= IRON_REWARD_BOX)
		order=0;
	if(order<4){
		for(uint8_t i; i<=order; i++){
			if(((p->hangup_rewards>>i)&0x1)==0){
				p->hangup_rewards|=(1<<i);
				db_day_add_ssid_cnt(NULL, p->id, ssid_hangup_reward, 1<<i,0xFF);
			}
		}
	}
	//KDEBUG_LOG(p->id,"rewards list total2:[%x][%u]",p->hangup_rewards,order);
	for(uint8_t i=0; i< 4 ; i++){
		if(((p->hangup_rewards>>i)&0x1)^((p->hangup_rewards>>(i+4))&0x1)){
			cli_out.itemlist.push_back(i);
			//KDEBUG_LOG(p->id,"rewards list:[%u]",i);
		}
	}
	return send_msg_to_self(p, p->waitcmd,&cli_out, 1);
}
//计算战斗力
uint32_t get_fight_power(sprite_t *p)
{
	body_cloth_t* pc = cache_get_body_cloth(p, BASE_BODY_TYPE + part_weapon);
	cloth_t* psc = pc ? get_cloth(pc->clothtype) : NULL;
	uint32_t weapon_power=psc?psc->minlv:0;
	uint32_t max_skill_lv=0;
	for( uint32_t loop=0 ; loop<p->skill_cnt ; loop++ )
	{
		if(max_skill_lv < p->skills[loop].skill_level)
			max_skill_lv=p->skills[loop].skill_level;
	}
	for( uint32_t loop=0 ; loop<p->life_skill_cnt ; loop++ )
	{
		if(max_skill_lv < p->life_skills[loop].skill_level)
			max_skill_lv=p->life_skills[loop].skill_level;
	}
	return (p->level+max_skill_lv+weapon_power*10);
}
//拉取队友的id和队友战斗力 gvg 
int cli_get_btrteam_info(sprite_t* p, Cmessage* c_in)
{
	cli_get_btrteam_info_out cli_out;
	stru_id_cnt tmp;
	if(p->btr_team)
		for(uint32_t loop=0 ; loop < p->btr_team->count ; loop++ ){
			tmp.id=p->btr_team->players[loop]->id;
			tmp.count=get_fight_power(p->btr_team->players[loop]);
			cli_out.info_list.push_back(tmp);
		}
	else{
		tmp.id=p->id;
		tmp.count=get_fight_power(p);
		cli_out.info_list.push_back(tmp);
	}	
	return send_msg_to_self(p,p->waitcmd,&cli_out,1);	
}

//pumpkin lantern activity,get a activity type 0,1,2,3
#define  MAX_OPEN_CNT  10
int cli_open_pumpkin_lantern(sprite_t *p, Cmessage* c_in)
{
	cli_open_pumpkin_lantern_out cli_out;
	uint32_t pro_rate[4]={100,300,300,300};
	uint32_t nowtime = get_now_sec();
	int day_idx = get_now_tm()->tm_wday;
	switch (day_idx) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
			break;
		case 6:
		case 0:
			if (!(nowtime >= 14 * 3600 && nowtime <= 16 * 3600)) 
				return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
			break;
	}
	if(p->pumpkin_lantern_cnt >= MAX_OPEN_CNT)
		return send_to_self_error(p,p->waitcmd,cli_err_times_limit,1);
	CHECK_SELF_BATTLE_INFO(p, p->id);
	CHECK_USER_TIRED(p);
	CHECK_USER_IN_MAP(p,0);
	if(p->btr_team)
		return send_to_self_error(p,p->waitcmd,cli_err_alrdy_join_team,1);
	p->pumpkin_lantern_cnt ++;
	db_day_add_ssid_cnt(NULL, p->id, ssid_pumpkin_lantern_cnt, 1, MAX_OPEN_CNT);
	p->pumpkin_lantern_activity_type=cli_out.type=rand_type_idx(4,pro_rate,1000);
	KDEBUG_LOG(p->id,"pumpkin_lantern_activity_type=%u",cli_out.type);
	msg_log_pumpkin_activity(p->id);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

/**
 * @brief  设置宠物标记位
 */
int cli_set_pet_flag(sprite_t *p, Cmessage* c_in)
{
	cli_set_pet_flag_in *p_in=P_IN;
	if(p_in->bit>31 || p_in->state!=1)
		return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
	pet_t* p_p = get_pet_inbag(p, p_in->petid);
	CHECK_PET_INBAG(p, p_p,p_in->petid);
	beast_t* pb = get_beast(p_p->pettype);
	if (!pb) {
		KERROR_LOG(p->id, "no such pet in xml\t[%u]", p_p->pettype);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	p_p->flag |= 1<<p_in->bit; 
	KDEBUG_LOG(p->id, "SET PET FLAG\t[%u %u %u 0x=%o]",p_in->petid,p_in->bit,p_in->state,p_p->flag);
	send_msg_to_db(0,p->id,mole2_set_pet_flag_cmd,p_in);
	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}

#define ACCELER_FACTOR  2
/**
 * @brief  检测用户是否使用变速器
 */
int cli_sync_user_time(sprite_t *p, Cmessage* c_in)
{
	cli_sync_user_time_in *p_in=P_IN;
	DEBUG_LOG("cli_sync_user_time %u %u",p_in->time-p->user_timestamp,get_now_sec()-p->sys_timestamp);
	if(p->user_timestamp && p->sys_timestamp){
		double utime=p_in->time-p->user_timestamp,stime=get_now_sec()-p->sys_timestamp;
		if(utime>30 && stime>30 && utime >= stime*ACCELER_FACTOR){
			p->user_timestamp=p_in->time;
			p->sys_timestamp=get_now_sec();
			KDEBUG_LOG(p->id,"!!!!!user maybe use something illegal %u %u",(uint32_t)utime,(uint32_t)stime);
			//return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
		}
	}
	p->user_timestamp=p_in->time;
	p->sys_timestamp=get_now_sec();
	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}

/*---------------------- Trade --------------------------------*/


/* ----------------------------------------------------------------------------*/
/**
 * @brief  进入交易房间前用此协议，服务器检查房间人数并来获得地图id 
 * 第一次进入地图roomid发过来0xFFFFFFFF
 */
/* ----------------------------------------------------------------------------*/
int cli_trade_get_mapid(sprite_t *p, Cmessage* c_in)
{
	cli_trade_get_mapid_in *p_in=P_IN;
	bool check_time_flag=false;
	KDEBUG_LOG(p->id," trade enter map:xiaomee=%u roomid=%u",p->xiaomee,p_in->roomid);
	if (check_time_flag) {
		//检查时间对不对
		time_t nowtime = get_now_tv()->tv_sec;
		struct tm tm_tmp;
		localtime_r(&nowtime, &tm_tmp);
		if (!(tm_tmp.tm_hour>=8 && tm_tmp.tm_hour<23)){
			KDEBUG_LOG(p->id, "tm_tmp.tm_wday=%u tm_tmp.tm_hour:%u", tm_tmp.tm_wday,tm_tmp.tm_hour );
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
		}

	}
	CHECK_SELF_BATTLE_INFO(p,p->id);
	CHECK_SELF_JOIN_TEAM(p);
	CHECK_USER_APPLY_PK(p,p);
	//CHECK_LV_FIT(p,p,20,MAX_SPRITE_LEVEL);
	home_trade_get_mapid_in  h_in;
	if(p_in->roomid == 0xFFFFFFFF ){//进入交易大厅
		if (!p->tiles || !IS_NORMAL_MAP(p->tiles->id)){
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
	}else{//切换房间
		CHECK_SELF_IN_TRADE(p);
		if( p_in->roomid<0 || p_in->roomid >= MAX_TRADE_ROOM_CNT || p_in->roomid==p->roomid){
			KERROR_LOG(p->id,"roomid error %u %u",p_in->roomid,p->roomid);
			return send_to_self_error(p, p->waitcmd, cli_err_roomid_invalid, 1);
		}
		CHECK_SHOP_STATE(p, shop_init);
	}
	h_in.roomid=p_in->roomid;
	return send_msg_to_homeserv(p->id, p->tiles->id, home_trade_get_mapid_cmd ,&h_in);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  修改店铺状态  cmd：0 抢占摊铺 1 中止店铺 2 开店铺  3 关闭店铺
 * @return cmd :1 2 3时 返回参数 cmd；shopid；uid；state
 *         cmd ：0 时   增加两个参数 prof；nick（店铺名）
 */
/* ----------------------------------------------------------------------------*/
int cli_trade_change_shop(sprite_t *p, Cmessage* c_in)
{
	cli_trade_change_shop_in *p_in=P_IN;
	home_trade_change_shop_in h_in;
	cli_trade_change_shop_out cli_out;

	CHECK_SELF_IN_TRADE(p);

	cli_out.cmd=p_in->cmd;
	cli_out.uid=p->id;
	cli_out.sid=p->shop_id;

	h_in.cmd=p_in->cmd;
	h_in.roomid=p->roomid;
	KF_LOG("trade_change_shop",p->id,"cmd:%u sid=%u",p_in->cmd,p_in->sid);
	KDEBUG_LOG(p->id,"change shop state:cmd=%u,sid=%u,shop_state=%u",p_in->cmd,p_in->sid,p->shop_state);	
	switch ( p_in->cmd ){
		case 0 ://register
			CHECK_SHOP_STATE(p, shop_init);	
			if(p_in->sid >= MAX_TRADE_SHOP_CNT ){
				return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
			}
			if(p_in->sid==p->shop_id &&  get_now_sec()-p->register_shop_time < 2100 ){//同一个摊铺位置要相隔35分钟
				return send_to_self_error(p, p->waitcmd, cli_err_open_too_often, 1);
			}
			if(p->xiaomee<500){
				return send_to_self_error(p, p->waitcmd, cli_err_xiaomee_not_enough, 1);
			}
			p->shop_id=p_in->sid;
			h_in.sid=p_in->sid;
			memcpy(h_in.nick, p->shopname,sizeof(h_in.nick));
			break;
		case 1 ://hault
			CHECK_SHOP_STATE(p, shop_open);	
			p->shop_state = shop_register;
			h_in.sid=p->shop_id;
			//send to user
			cli_out.state=p->shop_state;
			send_msg_to_self(p,p->waitcmd,&cli_out,1);
			break;

		case 2 ://open
			CHECK_SHOP_STATE(p, shop_register);	
			if(p->trade_cloth_info->size() + p->trade_item_info->size() == 0){
				return send_to_self_error(p, p->waitcmd, cli_err_not_add_item, 1);
			}
			h_in.sid=p->shop_id;
			p->shop_state = shop_open;
			p->add_item_timestamp=time(NULL);

			//send to user
			cli_out.state=p->shop_state;
			send_msg_to_self(p,p->waitcmd,&cli_out,1);
			break;
		case 3 ://close
			if(p->shop_state == shop_init){
				return send_to_self_error(p, p->waitcmd, cli_err_shop_state, 1);
			}
			h_in.sid=p->shop_id;
			p->shop_state = shop_init;
			//DEBUG_LOG("xxxxx%u %u",p->trade_cloth_info!=NULL,p->trade_item_info!=NULL);	
			p->trade_cloth_info->clear();
			p->trade_item_info->clear();
			//send to user
			cli_out.state=p->shop_state;
			send_msg_to_self(p,p->waitcmd,&cli_out,1);
			break;
		case 5:
			if(p->shop_state == shop_init){
				return send_to_self_error(p, p->waitcmd, cli_err_shop_state, 1);
			}
			h_in.sid=p->shop_id;
			memcpy(p->shopname, p_in->nick,sizeof(p->shopname));
			memcpy(h_in.nick, p_in->nick,sizeof(h_in.nick));
			p->waitcmd = 0;
			p->sess_len = 0;
			break;
		default :
			return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
			break;
	}
	//send to home
	return send_msg_to_homeserv(p->id, p->tiles->id, home_trade_change_shop_cmd ,&h_in);
}
/* ----------------------------------------------------------------------------*/
/**
 * @brief  计算税
	店铺和物品栏中商品的“税收”默认为“单价”的5%；
	当“单价”大于默认值的5倍时，“税收”增加为“单价”的15%；
	当“单价”大于默认值的10倍时，“税收”增加为“单价”的25%。
 */
/* ----------------------------------------------------------------------------*/
uint32_t cal_item_tax(uint32_t itemid,uint32_t price)
{
	//return 0;
	normal_item_t *pni=get_item(itemid);
	uint32_t rate =price/pni->price;
	if(rate<5)
		return 5*price/100;
	else if(rate<10)
		return 15*price/100;
	else
		return 25*price/100;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  计算装备税
	店铺和物品栏中商品的“税收”默认为“单价”的5%；
	当“单价”大于默认值的5倍时，“税收”增加为“单价”的15%；
	当“单价”大于默认值的10倍时，“税收”增加为“单价”的25%。
 */
/* ----------------------------------------------------------------------------*/
uint32_t cal_cloth_tax(sprite_t *p,uint32_t clothid,uint32_t price)
{
	//return 0;
	body_cloth_t* pbc = cache_get_bag_cloth(p, clothid);
	cloth_t* pcloth = get_cloth(pbc->clothtype);
	cloth_lvinfo_t* pclv = &pcloth->clothlvs[pbc->clothlv];
	uint32_t rate =pclv->sell_price?price/pclv->sell_price:10;
	if(rate<5)
		return 5*price/100;
	else if(rate<10)
		return 15*price/100;
	else
		return 25*price/100;
}

int trade_add_item_ex(sprite_t *p, uint32_t itemid, uint32_t itemcnt, uint32_t price,uint32_t loop)
{
	trade_item_t tmp;
	tmp.tradeid=loop;	
	tmp.itemid=itemid;
	tmp.itemcnt=itemcnt;
	tmp.price=price;
	tmp.tax=cal_item_tax(itemid,price);
	p->trade_item_info->insert(std::make_pair(itemid,tmp));
	return 0;
}

int trade_add_cloth_ex(sprite_t *p, uint32_t clothid, uint32_t clothtype, uint32_t price,uint32_t loop)
{
	trade_cloth_t tmp;
	tmp.tradeid=loop;	
	tmp.clothid=clothid;
	tmp.clothtype=clothtype;
	tmp.price=price;
	tmp.tax=cal_cloth_tax(p,clothid,price);
	p->trade_cloth_info->insert(std::make_pair(clothid,tmp));
	return 0;
}


int check_trade_add_item_valid(sprite_t *p, uint32_t itemid ,uint32_t itemcnt)
{
	std::multimap<uint32_t,trade_item_t>::iterator it=p->trade_item_info->find(itemid);
	uint32_t cnt=0;
	while(it != p->trade_item_info->end()){
		cnt+=it->second.itemcnt;
		it++;
	}
	return cache_item_have_cnt(p, itemid, (cnt+itemcnt));
}

int check_trade_add_cloth_valid(sprite_t *p, uint32_t clothid ,uint32_t clothtype)
{
	std::multimap<uint32_t,trade_cloth_t>::iterator it=p->trade_cloth_info->find(clothid);
	while(it != p->trade_cloth_info->end()){
		return 0;
	}
	return 1;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  上架物品
 */
/* ----------------------------------------------------------------------------*/
int cli_trade_add_item(sprite_t *p, Cmessage* c_in)
{
	cli_trade_add_item_in *p_in=P_IN;
	cli_trade_add_item_out cli_out;
	KDEBUG_LOG(p->id,"cli_trade_add_item:");
	CHECK_SELF_IN_TRADE(p);
	CHECK_SHOP_STATE(p, shop_register);	
	p->trade_cloth_info->clear();
	p->trade_item_info->clear();
	for( uint32_t loop=0 ; loop<p_in->items.size() ; loop++){
		stru_add_item tmp;
		tmp.itemid=p_in->items[loop].itemid;
		tmp.itemcnt=p_in->items[loop].itemcnt;
		tmp.price=p_in->items[loop].price;
		KDEBUG_LOG(p->id,"trade_add_item:%u %u %u",p_in->items[loop].itemid,p_in->items[loop].itemcnt,p_in->items[loop].price);
		if(p_in->items[loop].price && p_in->items[loop].price > MAX_PRICE_VALUE){
			KERROR_LOG(p->id,"price too high %u %u",p_in->items[loop].itemid,p_in->items[loop].price);
			continue;
		}
		if(p_in->items[loop].itemcnt > 99){
			KERROR_LOG(p->id,"cnt too much %u",p_in->items[loop].itemcnt);
			continue;
		}
		if(IS_ITEM(p_in->items[loop].itemid)){
			if (!cache_item_have_cnt(p, p_in->items[loop].itemid, p_in->items[loop].itemcnt)) { 
				KERROR_LOG(p->id, "item no exist\t[%u %u]", p_in->items[loop].itemid, p_in->items[loop].itemcnt); 
				continue;
			}
			//CHECK_ITEM_EXIST(p,p_in->items[loop].itemid,p_in->items[loop].itemcnt);
			normal_item_t* pni = get_item(p_in->items[loop].itemid);	
			if (!pni || !pni->vendibility) {
				KERROR_LOG(p->id,"can't sell %u",p_in->items[loop].itemid);
				continue;
			}
			if(! check_trade_add_item_valid(p,p_in->items[loop].itemid,p_in->items[loop].itemcnt)){
				continue;
			}
			trade_add_item_ex(p,p_in->items[loop].itemid,p_in->items[loop].itemcnt,p_in->items[loop].price,loop);
			cli_out.count++;
		}else{
			body_cloth_t* pbc = cache_get_bag_cloth(p, p_in->items[loop].itemid);
			if (!pbc) {
				KERROR_LOG(p->id,"not exist %u",p_in->items[loop].itemid);
				continue;
			}
			cloth_t* pcloth = get_cloth(pbc->clothtype);
			//CHECK_CLOTH_LV_VALID(p, pcloth, pbc->clothtype, pbc->clothlv);
			if (!pcloth || pbc->clothlv >= MAX_CLOTH_LEVL_CNT || !pcloth->clothlvs[pbc->clothlv].valid) { 
				KERROR_LOG(p->id, "cloth lv invalid\t[%u %u]", pbc->clothid, pbc->clothlv); 
				continue;
			} 
			cloth_lvinfo_t* pclv = &pcloth->clothlvs[pbc->clothlv];
			if ((pbc->quality>=3 && pbc->quality <=3) || !pclv->vendibility) {
				KERROR_LOG(p->id,"can't sell %u %u %u",p_in->items[loop].itemid,pclv->vendibility,pbc->quality);
				continue;
			}
			if(!check_trade_add_cloth_valid(p,p_in->items[loop].itemid,p_in->items[loop].itemcnt)){
				KERROR_LOG(p->id,"can't add %u",p_in->items[loop].itemid);
				continue;
			}
			trade_add_cloth_ex(p,p_in->items[loop].itemid,pbc->clothtype,p_in->items[loop].price,loop);
			cli_out.count++;
		}
	}
	KDEBUG_LOG(p->id,"%lu %lu",p->trade_item_info->size(),p->trade_cloth_info->size());
	return send_msg_to_self(p,p->waitcmd,&cli_out,1);
}

trade_cloth_t trade_del_cloth_ex(sprite_t *p, uint32_t clothid, uint32_t tradeid)
{
	std::multimap<uint32_t,trade_cloth_t>::iterator it=p->trade_cloth_info->find(clothid);
	trade_cloth_t tmp={};
	while( it != p->trade_cloth_info->end() ){
		if( it->second.tradeid == tradeid ){
			memcpy(&tmp, &(it->second), sizeof(trade_cloth_t));
			p->trade_cloth_info->erase(it);
			KDEBUG_LOG(p->id,"del trade cloth [ %u %u %u %u %u]",it->second.clothid,
					it->second.clothtype,it->second.price,it->second.tax,it->second.tradeid);
			return tmp;
		}
		it++;
	}
	return tmp;
}

trade_item_t trade_del_item_ex(sprite_t *p, uint32_t itemid,int32_t itemcnt, uint32_t tradeid)
{
	trade_item_t  tmp={};
	std::multimap<uint32_t,trade_item_t>::iterator it=p->trade_item_info->find(itemid);
	while( it != p->trade_item_info->end() ){
		if( it->second.tradeid == tradeid ){
			memcpy(&tmp, &(it->second), sizeof(trade_cloth_t));
			if(itemcnt >= it->second.itemcnt){
				tmp.itemcnt=it->second.itemcnt;
				p->trade_item_info->erase(it);
			}else{
				tmp.itemcnt=itemcnt;
				it->second.itemcnt -= itemcnt;
				KDEBUG_LOG(p->id,"del trade item [ %u %u %u %u %u %u]",it->second.itemid,
						itemcnt,it->second.itemcnt,it->second.price,it->second.tax,it->second.tradeid);
			}
			return tmp;
		}
		it++;
	}	
	return tmp;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  拉取摊位信息
 */
/* ----------------------------------------------------------------------------*/
int cli_trade_get_shop_info(sprite_t *p, Cmessage* c_in)
{
	CHECK_SELF_IN_TRADE(p);
	home_trade_get_shop_info_in h_in;  
	KDEBUG_LOG(p->id,"home_trade_get_shop_info roomid=%u",p->roomid);
	h_in.roomid=p->roomid;
	return send_msg_to_homeserv(p->id, p->tiles->id , home_trade_get_shop_info_cmd ,&h_in);
}

void trade_get_records(sprite_t *p, cli_trade_get_records_out &out)
{
	std::map<uint32_t, trade_record_t>::iterator it=p->trade_record_info->begin();
	for(  ; it !=  p->trade_record_info->end(); it++ ){
		stru_trade_record tmp;
		tmp.itemid=it->second.itemid;
		tmp.itemcnt=it->second.itemcnt;
		tmp.price=it->second.price;
		tmp.tax=it->second.tax;
		tmp.opt_time=it->second.trade_time;
		out.info.push_back(tmp);
		//KDEBUG_LOG(p->id,"rrrrrrrrrr itemid=%u itemcnt=%u",tmp.itemid,tmp.itemcnt);
	}
}

/* ----------------------------------------------------------------------------*/
/**
  @brief  拉取交易记录
 */
/* ----------------------------------------------------------------------------*/
int cli_trade_get_records(sprite_t *p, Cmessage* c_in)
{
	cli_trade_get_records_in *p_in=P_IN;
	cli_trade_get_records_out cli_out;
	KDEBUG_LOG(p->id,"cli_trade_get_records:uid=%u",p_in->uid);
	CHECK_SELF_IN_TRADE(p);
	trade_get_records(p, cli_out);
	return send_msg_to_self(p,p->waitcmd,&cli_out,1);
}

void trade_get_items(sprite_t *p, cli_trade_get_items_out &out)
{
	std::multimap<uint32_t, trade_item_t>::iterator	it=p->trade_item_info->begin();
	for(  ; it !=  p->trade_item_info->end(); it++ ){
		stru_trade_item tmp;
		tmp.tradeid=it->second.tradeid;
		tmp.itemid=it->second.itemid;
		tmp.itemcnt=it->second.itemcnt;
		tmp.price=it->second.price;
		tmp.tax=it->second.tax;
		out.items.push_back(tmp);
	}
	std::multimap<uint32_t, trade_cloth_t>::iterator	ic=p->trade_cloth_info->begin();
	for(  ; ic !=  p->trade_cloth_info->end(); ic++ ){
		stru_trade_cloth tmp;
		body_cloth_t *pbc=cache_get_bag_cloth(p,ic->second.clothid);
		//DEBUG_LOG("1111 %u %u",pbc!=NULL,tmp.clothid);
		if( pbc ){
			//DEBUG_LOG("2222");
			tmp.tradeid=ic->second.tradeid;
			tmp.clothid=ic->second.clothid;
			tmp.clothtype=ic->second.clothtype;
			tmp.price=ic->second.price;
			tmp.tax=ic->second.tax;

			tmp.attr.clothid=pbc->clothid;
			tmp.attr.clothtype=pbc->clothtype;
			tmp.attr.grid=pbc->grid;
			tmp.attr.clothlv=pbc->clothlv;
			tmp.attr.duration_max=pbc->duration_max;
			tmp.attr.duration=pbc->duration;
			tmp.attr.hpmax=pbc->hp_max;
			tmp.attr.mpmax=pbc->mp_max;
			tmp.attr.attack=pbc->attack;
			tmp.attr.mattack=pbc->mattack;
			tmp.attr.defense=pbc->defense;
			tmp.attr.mdef=pbc->mdefense;
			tmp.attr.speed=pbc->speed;
			tmp.attr.spirit=pbc->spirit;
			tmp.attr.resume=pbc->resume;
			tmp.attr.hit=pbc->hit;
			tmp.attr.dodge=pbc->dodge;
			tmp.attr.crit=pbc->crit;
			tmp.attr.fightback=pbc->fightback;
			tmp.attr.rpoison=pbc->rpoison;
			tmp.attr.rlithification=pbc->rlithification;
			tmp.attr.rlethargy=pbc->rlethargy;
			tmp.attr.rinebriation=pbc->rinebriation;
			tmp.attr.rconfusion=pbc->rconfusion;
			tmp.attr.roblivion=pbc->roblivion;
			tmp.attr.quality=pbc->quality;
			tmp.attr.validday=pbc->validday;
			tmp.attr.crystal_attr=pbc->crystal_attr;
			tmp.attr.blesstype=pbc->bless_type;
			out.cloths.push_back(tmp);
		}
	}
	//DEBUG_LOG("333 %u",out.cloths.size());
}

/* ---------------------------------------------------------------------------*/
/*
 * @brief  拉去商品物品列表 
 */
/* ----------------------------------------------------------------------------*/
int cli_trade_get_items(sprite_t *p, Cmessage* c_in)
{
	cli_trade_get_items_in *p_in=P_IN;
	cli_trade_get_items_out cli_out;
	KDEBUG_LOG(p->id,"cli_trade_get_items:uid=%u",p_in->uid);
	CHECK_SELF_IN_TRADE(p);
	sprite_t *lp=get_sprite(p_in->uid);
	if(lp){
		if( p->id != p_in->uid ){
			CHECK_SHOP_STATE(lp, shop_open);	
		}

		//KDEBUG_LOG(p->id,"item %lu cloth %lu",lp->trade_item_info->size(),lp->trade_cloth_info->size());
		trade_get_items(lp, cli_out);
		cli_out.uid=lp->id;
		int lefttime=lp->register_shop_time+1800-get_now_sec();
		cli_out.lefttime=lefttime<0?0:lefttime;
		KDEBUG_LOG(p->id,"get item size:%lu %lu",cli_out.items.size(),cli_out.cloths.size());
		return send_msg_to_self(p,p->waitcmd,&cli_out,1);
	}else{
		KDEBUG_LOG(p->id,"cli_trade_get_items:other online");
		home_trade_get_items_in h_in;
		h_in.uid=p_in->uid;
		return send_msg_to_homeserv(p->id, p->tiles->id, home_trade_get_items_cmd  ,&h_in);
	}
}

bool check_trade_item_exist(sprite_t *p, uint32_t itemid ,uint32_t tradeid, int32_t itemcnt)
{
	std::multimap<uint32_t,trade_item_t>::iterator it=p->trade_item_info->find(itemid);
	while(it != p->trade_item_info->end()){
		//if(  it->second.tradeid == tradeid){
			//KDEBUG_LOG(p->id,"llllllllll %u %u",itemcnt,it->second.itemcnt);
		//}
		if( it->second.tradeid == tradeid && itemcnt <= it->second.itemcnt){
			return true;
		}
		it++;
	}
	return false;
}

bool check_trade_cloth_exist(sprite_t *p, uint32_t clothid ,uint32_t tradeid, int32_t itemcnt)
{
	if( itemcnt > 1 ){
		return false;
	}
	std::multimap<uint32_t,trade_cloth_t>::iterator it=p->trade_cloth_info->find(clothid);
	while(it != p->trade_cloth_info->end()){
		if( it->second.tradeid == tradeid ){
			return true;
		}
		it++;
	}
	return false;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  检测xiaomee是否足够
 * @return 足够则返回true 
 */
/* ----------------------------------------------------------------------------*/
bool check_trade_xiaomee_enough(uint32_t xiaomee, sprite_t *p, uint32_t itemid,int32_t itemcnt,uint32_t tradeid)
{
	if(IS_ITEM(itemid)){
		std::multimap<uint32_t,trade_item_t>::iterator it=p->trade_item_info->find(itemid);
		while(it != p->trade_item_info->end()){
			if( it->second.tradeid == tradeid && (uint64_t)xiaomee >= it->second.price*(uint64_t)itemcnt ){	
				//DEBUG_LOG("jjjjj %lu %lu %u %u",(uint64_t)xiaomee ,it->second.price*(uint64_t)itemcnt,itemid,tradeid);
				return true;
			}
			it++;
		}
	}else{
		std::multimap<uint32_t,trade_cloth_t>::iterator it=p->trade_cloth_info->find(itemid);
		while(it != p->trade_cloth_info->end()){
			if( it->second.tradeid == tradeid && xiaomee >= it->second.price){
				return true;
			}
			it++;
		}
	}
	return false;
}

/**
 * @brief  交易大厅里买东西
 */
int cli_trade_buy_item(sprite_t *p, Cmessage* c_in)
{
	cli_trade_buy_item_in *p_in=P_IN;
	cli_trade_buy_item_out cli_out;
	sprite_t *seller=get_sprite(p_in->uid);
	uint32_t money=0,tax=0;
	KDEBUG_LOG(p->id,"cli_trade_buy_item[seller=%u roomid =%u  itemid=%u itemcnt=%u tradeid=%u xiaomee%u]",
			p_in->uid,p->roomid, p_in->itemid, p_in->itemcnt ,p_in->tradeid, p->xiaomee);
	KF_LOG("trade_buy_begin",p->id,"seller:%u roomid:%u itemid:%u itemcnt:%u tradeid:%u xiaomee:%u",
			p_in->uid, p->roomid,p_in->itemid, p_in->itemcnt ,p_in->tradeid, p->xiaomee);
	CHECK_SELF_IN_TRADE(p);
	CHECK_SHOP_STATE(p, shop_init);	
	if(seller){//in the same online
		if(seller->shop_state != shop_open){
			return send_to_self_error(p,p->waitcmd, cli_err_shop_state, 1);
		}
		if( seller->add_item_timestamp > time(NULL) ){
			return send_to_self_error(p,p->waitcmd, cli_err_shop_state, 1);
		}
		CHECK_IN_SAME_MAP(p, seller);
		
		KF_LOG("trade_seller_info",seller->id,"xiaomee:%u shopid:%u roomid:%u",
				p->xiaomee,p->shop_id,p->roomid);
		if(IS_ITEM(p_in->itemid)){//if buy item
			CHECK_ITEM_EXIST_NEW(p,seller,p_in->itemid,p_in->itemcnt);//背包里物品是否存在
			//商店里物品是否存在
			if(!check_trade_item_exist(seller, p_in->itemid, p_in->tradeid, p_in->itemcnt))
				return send_to_self_error(p,p->waitcmd,cli_err_item_cannot_buy,1);

			if( !check_trade_xiaomee_enough(p->xiaomee,seller, p_in->itemid, p_in->itemcnt, p_in->tradeid) ){
				return send_to_self_error(p,p->waitcmd,cli_err_xiaomee_not_enough,1);
			}
			p->xiaomee-=money;//buyer cache del money
			trade_item_t tmp=trade_del_item_ex(seller, p_in->itemid, p_in->itemcnt, p_in->tradeid);
			money=tmp.price*tmp.itemcnt; 
			tax=tmp.tax*tmp.itemcnt; 
			KDEBUG_LOG(p->id,"trade start item:seller=%u buyer=%u roomid=%u shopid=%u itemid=%u itemcnt=%u money=%u tax=%u",
					seller->id, p->id,seller->roomid,seller->shop_id, tmp.itemid, tmp.itemcnt, money , tax);
			/*卖家加钱 扣物品*/
			seller->xiaomee += money-tax;//seller cache add xiaomee
			db_add_xiaomee(NULL,seller->id,  money-tax);//seller db add xiaomee
			cache_reduce_kind_item(p, tmp.itemid, tmp.itemcnt);
			//cache_add_kind_item(seller,tmp.itemid,-1*tmp.itemcnt);//seller cache del item
			db_add_item(NULL, seller->id,tmp.itemid, -1*tmp.itemcnt);//seller db del item

			/*买家扣钱 加物品*/
			//p->xiaomee -= money;//buyer cache del xiaomee
			p->xiaomee =(p->xiaomee>money)?(p->xiaomee-money):0;
			db_add_xiaomee(NULL,p->id,  money*(-1));//buyer db add xiaomee
			cache_add_kind_item(p,tmp.itemid,tmp.itemcnt);//buyer cache add item
			db_add_item(NULL, p->id,tmp.itemid, tmp.itemcnt);//buyer db add item

			cli_out.itemid=p_in->itemid;
			add_to_trade_record(seller,p->id,&tmp);
			KF_LOG("trade_buy_item_succ",p->id,"seller:%u buyer:%u roomid:%u shopid:%u itemid:%u itemcnt:%u money:%u tax:%u",
					seller->id, p->id,seller->roomid,seller->shop_id, tmp.itemid, tmp.itemcnt, money , tax);
		}else{//buy cloth
			//背包里物品是否存在
			CHECK_CLOTH_EXIST_NEW(p,seller, p_in->itemid);	
			//商店里物品是否存在
			if(!check_trade_cloth_exist(seller, p_in->itemid, p_in->tradeid, p_in->itemcnt)){
				return send_to_self_error(p,p->waitcmd,cli_err_item_cannot_buy,1);
			}
			if( !check_trade_xiaomee_enough(p->xiaomee,seller, p_in->itemid, p_in->itemcnt, p_in->tradeid) ){
				return send_to_self_error(p,p->waitcmd,cli_err_xiaomee_not_enough,1);
			}
			trade_cloth_t tmp={};
			KDEBUG_LOG(p->id,"trade start cloth:seller=%u buyer=%u clothid=%u clothtype=%u money=%u tax=%u",
					seller->id, p->id, tmp.clothid, tmp.clothtype, money , tax);
			tmp=trade_del_cloth_ex(seller, p_in->itemid, p_in->tradeid);
			money=tmp.price; 
			tax=tmp.tax; 

			/*加钱 扣物品 扣钱 加物品*/
			seller->xiaomee+=money-tax;//seller cache add xiaomee
			db_add_xiaomee(NULL,seller->id,  money-tax);//seller db add xiaomee

			body_cloth_t *pbc=cache_get_bag_cloth(seller, tmp.clothid);
			cli_out.itemid=pbc->clothtype;
			db_add_cloth_new(NULL,p->id,money,pbc);//buyer db add cloth and del money
			p->xiaomee =(p->xiaomee>money)?(p->xiaomee-money):0;
			log_cloth_new(seller, pbc, "trade_cloth_info");

			cache_del_bag_cloth(seller, tmp.clothid);//seller cache del cloth			
			db_del_cloth(NULL, seller->id ,tmp.clothid);//sller db del cloth

			add_to_trade_record(seller,p->id,&tmp);
			KF_LOG("trade_buy_cloth_succ",p->id,"seller:%u buyer:%u roomid:%u shopid:%u clothid:%u clothtype:%u money:%u tax:%u"
					,seller->id, p->id,seller->roomid,seller->shop_id,tmp.clothid, tmp.clothtype, money , tax);
		}
		cli_out.uid=p_in->uid;
		cli_out.itemcnt=p_in->itemcnt;
		cli_out.money=money;
		cli_out.tax=tax;
		//send to buyer
		send_msg_to_self(p,p->waitcmd,&cli_out,1);
		//send to seller
		return send_msg_to_self(seller, cli_trade_buy_item_cmd, &cli_out, 0);
	}else{//在其他的online上
		home_trade_buy_item_in h_in;
		h_in.uid=p_in->uid;
		h_in.itemid=p_in->itemid;
		h_in.itemcnt=p_in->itemcnt;
		h_in.tradeid=p_in->tradeid;
		h_in.roomid=p->roomid;
		h_in.xiaomee=p->xiaomee;
		KDEBUG_LOG(p->id,"seller is on other online [sellerid=%u]",p_in->uid);
		return send_msg_to_homeserv(p->id, p->tiles->id, home_trade_buy_item_cmd  ,&h_in);
	}
}
// 去除不符合职业的称号
void rm_user_prof_honor(gpointer key, gpointer value, gpointer data)
{
	sprite_t *p=(sprite_t*)data;
	uint32_t prof=p->prof;
	uint32_t hid=*(uint32_t*)key;
	honor_attr_t *pha=(honor_attr_t*)value;
	KDEBUG_LOG(p->id,"travers honor %u %u %u",hid,prof,pha->prof);
	if( ((pha->prof>>p->prof)&1) != 1 ){
		KDEBUG_LOG(p->id,"remove honor %u",hid);
		g_hash_table_remove(p->user_honors,&hid);
		mole2_user_del_title_in p_in;
		p_in.titleid=hid;
		send_msg_to_db(NULL,p->id,mole2_user_del_title_cmd,&p_in);
	}
}

#define     IS_SPEED_CLOTH(id) ((id)<=89004&&(id)>=89001)//坐骑
void put_cloth_to_home(gpointer key, gpointer value, gpointer data)
{
	sprite_t *p=(sprite_t*)data;
	uint32_t grid=*(uint32_t*)key;
	body_cloth_t *pbc=(body_cloth_t*)value;
	//cloth_t *pc=get_cloth(pbc->clothtype);
	DEBUG_LOG("put cloth %u %u %u",grid,pbc->clothid,pbc->clothtype);
	if(!IS_SPEED_CLOTH(pbc->clothtype)){
	//if( pc && IS_PROF_MATCH(p,pc->prof)){
		int i = 0;
		uint8_t out[16];
		PKG_H_UINT32(out, pbc->clothid, i);
		PKG_H_UINT32(out, START_GRID_CLOTH_WAREHOUSE, i);
		send_request_to_db(NULL, p->id, proto_db_cloth_in_out, out, i);
	}
	//	}
}

void foreach_get_cloth(gpointer key, gpointer value, gpointer data)
{
	std::vector<uint32_t>* arr=(std::vector<uint32_t>*)data;
	body_cloth_t *pbc=(body_cloth_t*)value;
//	cloth_t *pc=get_cloth(pbc->clothtype);
	if(!IS_SPEED_CLOTH(pbc->clothtype)){
	//if( pc && IS_PROF_MATCH(p,pc->prof)){
		arr->push_back(pbc->clothid);
	}
}

//角色进阶时清除原有技能、职业称号，重新分配属性点并赋予职称
int cli_change_prof(sprite_t *p, Cmessage* c_in)
{
	cli_change_prof_in *p_in=P_IN;
	if(!IS_OLD_PROF(p->prof) || !IS_NEW_PROF(p_in->prof)){
		return send_to_self_error(p,p->waitcmd,cli_err_not_right_prof,1);
	}
	CHECK_USER_IN_MAP(p,11307);//国王呈见厅
	CHECK_ITEM_EXIST(p, 290024,1);//职业进阶凭证

	//del skill
	for( uint32_t loop=0 ; loop < p->skill_cnt ;){
		skill_t* pst=&p->skills[loop];
		if(IS_BATTLE_SKILL(pst->skill_id)){
			send_request_to_db(NULL, p->id, proto_db_del_skill, &pst->skill_id, 4);
			cache_del_skill(p,pst->skill_id);
		}else{
			loop++;
		}
	}
	//reset attr
	uint8_t out[64];
	int i=0;
	PKG_H_UINT16(out, 0, i);
	PKG_H_UINT16(out, 0, i);
	PKG_H_UINT16(out, 0, i);
	PKG_H_UINT16(out, 0, i);
	PKG_H_UINT16(out, 0, i);
	int16_t attr_add =((p->level - 1) * 4 + 60);
	PKG_H_UINT16(out, attr_add, i); 
	send_request_to_db(NULL, p->id, proto_db_add_base_attr, out, i);
	p->attr_addition = attr_add;
	uint16_t attr[5]={0,0,0,0,0};
	update_user_second_attr(p, (base_5_attr_t *)attr, p->injury_lv);
	//log_sprite(p);
	//putoff cloth
	//uint32_t buff[MAX_ITEMS_WITH_BODY * 8 + 1] = {0};
	//send_request_to_db(NULL, p->id, proto_db_set_clothes_where, buff, buff[0] * 8 + 4);
	DEBUG_LOG("body_cloth:%u",g_hash_table_size(p->body_cloths));
	std::vector<uint32_t> body_cloths;
	g_hash_table_foreach(p->body_cloths,put_cloth_to_home,p);
	g_hash_table_foreach(p->body_cloths,foreach_get_cloth,&body_cloths);
	for( uint32_t loop=0 ; loop< body_cloths.size() ; loop++ ){
		cache_del_bag_cloth(p, body_cloths[loop]);
	}

	cache_set_cloth_attr(p);
	modify_sprite_second_attr(p);

	//change prof
	uint8_t prof = p_in->prof;
	p->prof=p_in->prof;
	send_request_to_db(NULL, p->id, proto_db_set_prof, &prof, sizeof(prof));


	//del old title
	db_set_user_honor(NULL,p->id,0);
	set_user_honor(p, 0);
	g_hash_table_foreach(p->user_honors,rm_user_prof_honor,p);
	//add titile
	uint32_t title[]={33,39,45,51};
	uint32_t idx=p->prof-6;
	honor_attr_t* pha = get_honor_attr(title[idx]);
	if (pha) {
		//DEBUG_LOG( "ADD HONOR id=%u",new_honor );
		update_one_honor(p, pha);
		set_user_honor(p, title[idx]);
		notify_user_honor_up(p, 1, 0, 1);
		db_add_user_honor(NULL, p->id, 2, 0, title[idx]);
	}
	//add skill
	add_skill_for_new_prof(p,p->level);
	cache_reduce_kind_item(p,290024,1);
	db_add_item(NULL,p->id,290024,-1);
	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}



/**
 * @brief 赠送vip活动
 *
 * @param p_in->type 0: 3 day ; 1: 7 day ;  2: 14 day  3: 5 day
 *
 * @return 
 */
int cli_give_vip_activity(sprite_t *p, Cmessage* c_in)
{
	//return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
	cli_give_vip_activity_in *p_in=P_IN;
	uint32_t count=3;

	bool check_time_flag=uid_need_check(p->id);
	
	item_t* pday = cache_get_day_limits(p, ssid_try_vip_activity);
	if(pday && pday->count>2){
		return send_to_self_error(p,p->waitcmd,cli_err_rand_day_max,1);
	}
	if( p_in->type > 3 ){
		return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
	}
	DEBUG_LOG("try vip %u",p_in->type);
	switch (p_in->type ){
		case 0 :
			return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
			count=3;
			break;
		case 1 :
			return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
			{
				if(!IS_OLD_PROF(p->prof)){
					return send_to_self_error(p,p->waitcmd,cli_err_not_right_prof,1);
				}
				count=7;
				uint32_t nowtime = get_now_sec();
				int day_idx = get_now_tm()->tm_wday;
				if (check_time_flag && 
						((nowtime < 14 * 3600 || nowtime > 14*3600+1800) || (day_idx==4 || day_idx==3))){
					return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
				}
				break;
			}
		case 2 :
			return send_to_self_error(p,p->waitcmd,cli_err_client_not_proc,1);
			{
				if(!IS_NEW_PROF(p->prof)){
					return send_to_self_error(p,p->waitcmd,cli_err_not_right_prof,1);
				}
				uint32_t nowtime = get_now_sec();
				int day_idx = get_now_tm()->tm_wday;
				//if (day_idx != 6 || !((nowtime >= 14 * 3600 && nowtime <= 14*3600+1800))){
				if (check_time_flag && 
						((nowtime < 14 * 3600 || nowtime > 14*3600+1800) || (day_idx==4 || day_idx==3 ))){
					return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
				}
				count=14;
				break;
			}
		case 3:
			if( p->level >= 10){
				return send_to_self_error(p, p->waitcmd, cli_err_level_not_fit, 1);
			}
			count=5;
		default :
			break;
	}
	*(uint32_t*)p->session=p_in->type;
	typedef struct _add_vip {
	   uint16_t channel_id;
	   char verify_info[32];
	   uint8_t count;
   	} __attribute__ ((packed))try_vip_t;
    try_vip_t session_req;
	session_req.count=count;
	unsigned char src[100];
    uint16_t channel_id = config_get_intval("channel_id_2", 0);
    uint32_t security_code = config_get_intval("security_code_2", 0);
    DEBUG_LOG("userid %d channel_id %d security_code %d", p->id, channel_id, security_code);

    session_req.channel_id = channel_id;
	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%u&data=", channel_id, security_code);
    *(uint8_t*)(src + len) = session_req.count; 
	utils::MD5 md5;
	md5.reset();
	md5.update ((void*)src, len + 1 ); 
    memcpy(session_req.verify_info, md5.toString().c_str() , 32);
	return send_request_to_vipserv(p,proto_db_try_vip,&session_req,sizeof(try_vip_t));
}

/**
 * @brief 赠送vip活动 DB返回 
 */
int try_vip_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{

	DEBUG_LOG("add_vip_callback:ret=%u",ret);
	if(ret){ 
		return send_to_self_error(p, p->waitcmd,cli_err_system_error, 1); 
	}
	item_t* pday = cache_get_day_limits(p, ssid_try_vip_activity);
	pday->count++;
	db_day_add_ssid_cnt(0, p->id, ssid_try_vip_activity, 1, -1);
	uint32_t count=*(uint32_t*)p->session;
	response_proto_uint32(p,p->waitcmd,count,1,0);
	return 0;
}
