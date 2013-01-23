#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <arpa/inet.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/utilities.h>
}

#include "proto.hpp"
#include "pea_common.hpp"
#include "cli_proto.hpp"
#include "db_player.hpp"
#include "friends.hpp"
#include "db_friends.hpp"
#include "utils.hpp"
#include "login.hpp"

using namespace taomee;

static char dbpkgbuf[dbproto_max_len] = {0};

int db_check_user_exist(player_t *p, uint32_t checked_uid, uint32_t role_tm)
{
	int idx = 0;
	pack_h(dbpkgbuf, checked_uid, idx);
	pack_h(dbpkgbuf, role_tm, idx);
	pack_h(dbpkgbuf, p->server_id, idx);

	return send_to_db(p, p->id, p->role_tm, db_proto_check_user_exist_cmd, dbpkgbuf, idx);	
}

int db_proto_check_user_exist_callback(DEFAULT_ARG)
{
	db_proto_check_user_exist_out *p_in = P_IN;
	switch (p->waitcmd) {
	case cli_proto_add_friend_cmd :
		if (p_in->flag_exist == 0) {
			return send_error_to_player(p, ONLINE_ERR_FRIEND_UID_NOT_EXIST);
		} else {
			return do_add_friend(p, p_in->user.user_id, p_in->user.role_tm, p_in->forbid_friends_me);
		}
		break;
	case cli_proto_add_to_blacklist_cmd :
		if (p_in->flag_exist == 0) {
			return send_error_to_player(p, ONLINE_ERR_FRIEND_UID_NOT_EXIST);
		} else {
			return do_add_to_blacklist(p, p_in->user.user_id, p_in->user.role_tm);
		}
		break;
	default :
		break;
	}
	return 0;
}

int db_add_friend(player_t *p, uint32_t user_id, uint32_t role_tm)
{
	db_proto_add_friend_in out;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	out.ufriend.user_id = user_id;
	out.ufriend.role_tm = role_tm;
	out.server_id = p->server_id;
	return send_to_db(p, db_proto_add_friend_cmd, &out);
}

int db_proto_add_friend_callback(DEFAULT_ARG)
{
	db_proto_add_friend_out *p_in = P_IN;
	cli_proto_add_friend_out out;
	out.user.user_id = p_in->ufriend.user_id;
	out.user.role_tm = p_in->ufriend.role_tm;
	uint64_t key = gen_key_by_uid_role(p_in->ufriend.user_id, p_in->ufriend.role_tm);
	p->friends.insert(key);
	
	//notify_added_user
	player_t *p_dest = get_player(p_in->ufriend.user_id);
	if (p_dest != NULL) { //在线，即时通知
		cli_proto_friend_notify_added_out noti_out;	
		noti_out.user.user_id = p->id;
		noti_out.user.role_tm = p->role_tm;
		send_to_player(p_dest, &noti_out, cli_proto_friend_notify_added_cmd, 0);
	} else {//离线，发离线通知
		db_proto_send_offline_msg_in db_out;
		db_out.db_user_id.user_id = p_in->ufriend.user_id;
		db_out.db_user_id.role_tm = p_in->ufriend.role_tm;
		db_out.db_user_id.server_id = p->server_id;
		int idx = sizeof(cli_proto_t);
		pack(db_out.msg.msg, p->id, idx);
		pack(db_out.msg.msg, p->role_tm, idx);
		player_t tmp_player;
		tmp_player.id = db_out.db_user_id.user_id;
		init_cli_proto_head(db_out.msg.msg, &tmp_player, cli_proto_friend_notify_added_cmd, idx);
		db_out.msg._msg_len = idx;
		DEBUG_TLOG("send offline add friend msg to user");
		send_to_db(NULL, db_out.db_user_id.user_id, db_out.db_user_id.role_tm, db_proto_send_offline_msg_cmd, &db_out);
	}

	return send_to_player(p, &out, cli_proto_add_friend_cmd, 1);
}

int db_del_friend(player_t *p, uint32_t user_id, uint32_t role_tm)
{
	db_proto_del_friend_in out;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	out.ufriend.user_id = user_id;
	out.ufriend.role_tm = role_tm;
	out.server_id = p->server_id;
	return send_to_db(p, db_proto_del_friend_cmd, &out);
}

int db_proto_del_friend_callback(DEFAULT_ARG)
{
	db_proto_del_friend_out *p_in = P_IN;
	cli_proto_del_friend_out out;
	out.user.user_id = p_in->ufriend.user_id;
	out.user.role_tm = p_in->ufriend.role_tm;
	uint64_t key = gen_key_by_uid_role(p_in->ufriend.user_id, p_in->ufriend.role_tm);
	p->friends.erase(key);
	if (p->waitcmd == cli_proto_add_to_blacklist_cmd) {
		return db_add_to_blacklist(p, p_in->ufriend.user_id, p_in->ufriend.role_tm);
	}
	return send_to_player(p, &out, cli_proto_del_friend_cmd, 1);
}

int db_add_to_blacklist(player_t *p, uint32_t user_id, uint32_t role_tm)
{
	db_proto_add_to_blacklist_in out;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	out.blacked_user.user_id = user_id;
	out.blacked_user.role_tm = role_tm;
	out.server_id = p->server_id;
	return send_to_db(p, db_proto_add_to_blacklist_cmd, &out);
}

int db_proto_add_to_blacklist_callback(DEFAULT_ARG)
{
	db_proto_add_to_blacklist_out *p_in = P_IN;
	cli_proto_add_to_blacklist_out out;
	out.user.user_id = p_in->blacked_user.user_id;
	out.user.role_tm = p_in->blacked_user.role_tm;
	uint64_t key = gen_key_by_uid_role(p_in->blacked_user.user_id, p_in->blacked_user.role_tm);
	p->blacklist.insert(key);
	return send_to_player(p, &out, cli_proto_add_to_blacklist_cmd, 1);
}

int db_del_from_blacklist(player_t *p, uint32_t user_id, uint32_t role_tm)
{
	db_proto_del_from_blacklist_in out;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	out.unblacked_user.user_id = user_id;
	out.unblacked_user.role_tm = role_tm;
	out.server_id = p->server_id;
	return send_to_db(p, db_proto_del_from_blacklist_cmd, &out);
}

int db_proto_del_from_blacklist_callback(DEFAULT_ARG)
{
	db_proto_del_from_blacklist_out *p_in = P_IN;
	cli_proto_del_from_blacklist_out out;
	out.user.user_id = p_in->unblacked_user.user_id;
	out.user.role_tm = p_in->unblacked_user.role_tm;
	uint64_t key = gen_key_by_uid_role(p_in->unblacked_user.user_id, p_in->unblacked_user.role_tm);
	p->blacklist.erase(key);
	return send_to_player(p, &out, cli_proto_del_from_blacklist_cmd, 1);
}

int db_forbid_friends_me(player_t *p, uint32_t flag_forbid)
{
	db_proto_forbid_friends_me_in out;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	out.server_id = p->server_id;
	out.flag_forbid = flag_forbid;
	return send_to_db(p, db_proto_forbid_friends_me_cmd, &out);
}

int db_proto_forbid_friends_me_callback(DEFAULT_ARG)
{
	db_proto_forbid_friends_me_out *p_in = P_IN;
	cli_proto_forbid_friends_me_out out;
	out.flag_forbid = p_in->flag_forbid;
	p->forbid_friends_me = p_in->flag_forbid;
	return send_to_player(p, &out, cli_proto_forbid_friends_me_cmd, 1);
}


int db_get_user_info(player_t *p, uint32_t user_id, uint32_t role_tm)
{
	int idx = 0;
	pack_h(dbpkgbuf, user_id, idx);
	pack_h(dbpkgbuf, role_tm, idx);
	pack_h(dbpkgbuf, p->server_id, idx);
	return send_to_db(p, user_id, p->role_tm, db_proto_get_user_info_cmd, dbpkgbuf, idx);	
}

int db_proto_get_user_info_callback(DEFAULT_ARG)
{
	db_proto_get_user_info_out *p_in = P_IN;
	cli_proto_get_user_info_out out;
	out.user.user_id = p_in->user.user_id;
	out.user.role_tm = p_in->user.role_tm;
	memcpy(out.nick, p_in->nick, sizeof(out.nick));
	out.level = p_in->level;
	out.exp = p_in->exp;
	return send_to_player(p, &out, cli_proto_get_user_info_cmd, 1);
}

int db_get_friends_list(player_t *p)
{
	db_proto_get_friends_list_in out;
	out.server_id = p->server_id;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	return send_to_db(p, db_proto_get_friends_list_cmd, &out);
}

int db_proto_get_friends_list_callback(DEFAULT_ARG)
{
	db_proto_get_friends_list_out *p_in = P_IN;
	std::vector<uid_role_t>::iterator it;
	for(it = p_in->user_list.begin(); it != p_in->user_list.end(); it++) {
		uint64_t key = gen_key_by_uid_role(it->user_id, it->role_tm);
		p->friends.insert(key);
	}
    return db_get_blacklist(p);
}

int db_get_blacklist(player_t *p)
{
	db_proto_get_blacklist_in out;
	out.server_id = p->server_id;
	out.user.user_id = p->id;
	out.user.role_tm = p->role_tm;
	return send_to_db(p, db_proto_get_blacklist_cmd, &out);
}

int db_proto_get_blacklist_callback(DEFAULT_ARG)
{
	db_proto_get_blacklist_out *p_in = P_IN;
	cli_proto_get_blacklist_out out;
	std::vector<uid_role_t>::iterator it;
	for(it = p_in->blacklist.begin(); it != p_in->blacklist.end(); it++) {
		uint64_t key = gen_key_by_uid_role(it->user_id, it->role_tm);
		p->blacklist.insert(key);
	}
	p->set_module(MODULE_FRIEND);
	return process_login(p);
}

int db_get_union_list(player_t *p)
{
	return 0;
}

int db_proto_get_union_list_callback(DEFAULT_ARG)
{
	return 0;
}
