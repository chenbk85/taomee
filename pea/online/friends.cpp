extern "C" {
#include <glib.h>
#include <libtaomee/log.h>
}

#include <libtaomee++/inet/pdumanip.hpp>

#include "fwd_decl.hpp" //脏词库头文件
#include "proto.hpp"    //协议头文件，chat.hpp已经包含
#include "cli_proto.hpp"//send_to_player相关
#include "pea_common.hpp"
#include "player.hpp"   //玩家相关
//#include "union.hpp"  //工会相关
#include "db_friends.hpp"
#include "utils.hpp"
#include "friends.hpp"

enum {
	MAX_FRIENDS				= 200,
	MAX_BLACKLIST 			= 200,
	NO						= 0,
	YES						= 1,
	RECOMMEND_FRIENDS_LIMIT	= 10,
};


int do_add_friend(player_t *p, uint32_t user_id, uint32_t role_tm, uint32_t forbid_friends_me)
{
	if (p->id == user_id) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_SELF_ADD_DENIED);
	}
	
	uint64_t key = gen_key_by_uid_role(user_id, role_tm);
	if (p->friends.find(key) != p->friends.end()) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_ALREADY_FRIENDS);
	}
	if (p->blacklist.find(key) != p->blacklist.end()) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_ALREADY_BLACKLIST);
	}
	if (forbid_friends_me == YES) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_FORBID_ADD);
	} 
	if (user_id == 0) {//user_id do not exist
		return send_error_to_player(p, ONLINE_ERR_FRIEND_UID_NOT_EXIST);
	}
	if (p->friends.size() >= MAX_FRIENDS) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_TOO_MANY_FRIENDS);
	}
	
	return db_add_friend(p, user_id, role_tm);
}

int cli_proto_add_friend(DEFAULT_ARG)
{
	cli_proto_add_friend_in *p_in = P_IN;

	player_t *p_dest = get_player(p_in->user.user_id);
	if (p_dest != NULL && p_dest->role_tm == p_in->user.role_tm) {//用户在本online
		return do_add_friend(p, p_dest->id, p_dest->role_tm, p_dest->forbid_friends_me);
	} else {//用户不在线,去DB查
		return db_check_user_exist(p, p_in->user.user_id, p_in->user.role_tm);
	}
}

int cli_proto_del_friend(DEFAULT_ARG)
{
	cli_proto_del_friend_in *p_in = P_IN;
	uint64_t key = gen_key_by_uid_role(p_in->user.user_id, p_in->user.role_tm);
	if (p->friends.find(key) == p->friends.end()) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_NOT_FRIEND);
	}

	return db_del_friend(p, p_in->user.user_id, p_in->user.role_tm);
}

int do_add_to_blacklist(player_t *p, uint32_t user_id, uint32_t role_tm)
{
	if (user_id == 0) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_UID_NOT_EXIST);
	}	
	if (p->id == user_id) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_SELF_ADD_DENIED);
	}
	
	uint64_t key = gen_key_by_uid_role(user_id, role_tm);
	if (p->blacklist.find(key) != p->blacklist.end()) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_ALREADY_BLACKLIST);
	}
	if (p->blacklist.size() >= MAX_BLACKLIST) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_TOO_MANY_BLACK);
	}
	if (p->friends.find(key) != p->blacklist.end()) {
		return db_del_friend(p, user_id, role_tm);
	}
	return db_add_to_blacklist(p, user_id, role_tm);
}

int cli_proto_add_to_blacklist(DEFAULT_ARG)
{
	cli_proto_add_to_blacklist_in *p_in = P_IN;

	player_t *p_dest = get_player(p_in->user.user_id);
	if (p_dest != NULL && p_dest->role_tm == p_in->user.role_tm) {//用户在本online
		return do_add_to_blacklist(p, p_dest->id, p_dest->role_tm);
	} else {//用户不在线,去DB查
		return db_check_user_exist(p, p_in->user.user_id, p_in->user.role_tm);
	}
}

int cli_proto_del_from_blacklist(DEFAULT_ARG)
{
	cli_proto_del_from_blacklist_in *p_in = P_IN;
	uint64_t key = gen_key_by_uid_role(p_in->user.user_id, p_in->user.role_tm);
	if (p->blacklist.find(key) == p->blacklist.end()) {
		return send_error_to_player(p, ONLINE_ERR_FRIEND_NOT_IN_BLACKLIST);
	}

	return db_del_from_blacklist(p, p_in->user.user_id, p_in->user.role_tm);
}

int cli_proto_get_recommended_friends_list(DEFAULT_ARG)
{
	cli_proto_get_recommended_friends_list_out *p_out = P_OUT;
	int count = 0;
	GHashTableIter iter;
    gpointer key, val;
    g_hash_table_iter_init(&iter, all_players);
    while (g_hash_table_iter_next(&iter, &key, &val)) {
        player_t *p_dest = reinterpret_cast<player_t*>(val);
		if (p_dest != p) {
        	int level_diff = p_dest->get_player_attr_value(OBJ_ATTR_LEVEL)	- p->get_player_attr_value(OBJ_ATTR_LEVEL);
			if (level_diff >= -5 && level_diff <= 5) {
				count ++;
				uid_role_t user;
				user.user_id = p_dest->id;
				user.role_tm = p_dest->role_tm;
				p_out->user_list.push_back(user);
			}
			if (count == RECOMMEND_FRIENDS_LIMIT)
				break;
		}
    }
	return send_to_player(p, p_out, cli_proto_get_recommended_friends_list_cmd, 1);
}

int cli_proto_forbid_friends_me(DEFAULT_ARG)
{
	cli_proto_forbid_friends_me_in *p_in = P_IN;
	return db_forbid_friends_me(p, p_in->flag_forbid);
}

int cli_proto_get_user_info(DEFAULT_ARG)
{
	cli_proto_get_user_info_in *p_in = P_IN;
	cli_proto_get_user_info_out out;
	player_t *p_dest = get_player(p_in->user.user_id);
	if (p_dest && p_dest->role_tm == p_in->user.role_tm) {
		out.user.user_id = p_dest->id;
		out.user.role_tm = p_dest->role_tm;
		memcpy(out.nick, p_dest->nick, sizeof(out.nick));
		out.level = p_dest->get_player_attr_value(OBJ_ATTR_LEVEL); 
		out.exp = p_dest->get_player_attr_value(OBJ_ATTR_EXP);
		return send_to_player(p, &out, cli_proto_get_user_info_cmd, 1);	
	}
	return db_get_user_info(p, p_in->user.user_id, p_in->user.role_tm);
}

int cli_proto_get_friends_list(DEFAULT_ARG)
{
    cli_proto_get_friends_list_out out;
    std::set<uint64_t>::iterator it;
    for (it = p->friends.begin(); it != p->friends.end(); it++) {
		uint32_t user_id = (uint32_t)((*it) >> 32);
		uint32_t role_tm = (uint32_t)(*it);
		uid_role_t user;
		user.user_id = user_id;
		user.role_tm = role_tm;
        out.user_list.push_back(user);
    }
	return send_to_player(p, &out, cli_proto_get_friends_list_cmd, 1);
}

int cli_proto_get_blacklist(DEFAULT_ARG)
{
    cli_proto_get_blacklist_out out;
    std::set<uint64_t>::iterator it;
    for (it = p->blacklist.begin(); it != p->blacklist.end(); it++) {
        uint32_t user_id = (uint32_t)((*it) >> 32);
		uint32_t role_tm = (uint32_t)(*it);
		uid_role_t user;
		user.user_id = user_id;
		user.role_tm = role_tm;
		out.blacklist.push_back(user);
    }
	return send_to_player(p, &out, cli_proto_get_blacklist_cmd, 1);
}

int cli_proto_check_online_users(DEFAULT_ARG)
{
	cli_proto_check_online_users_in *p_in = P_IN;
	cli_proto_check_online_users_out *p_out = P_OUT;

	std::vector<uid_role_t>::iterator it;
	for (it = p_in->id_list.begin(); it != p_in->id_list.end(); it++) {
		player_t *p_dest = get_player(it->user_id);
		if (p_dest != NULL && p_dest->role_tm == it->role_tm) {
			p_out->online_list.push_back(*it);
		}
	}
	return send_to_player(p, p_out, cli_proto_check_online_users_cmd, 1);
}

int cli_proto_get_union_list(DEFAULT_ARG)
{
	return db_get_union_list(p);
}

