#include <libtaomee++/inet/byteswap.hpp>

using namespace taomee;

extern "C" {
#include <assert.h>
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/utils.h>

#include <async_serv/net_if.h>
#include "svc_ac_api.hpp"
}

#include "fwd_decl.hpp"

#include "battle.hpp"
#include "common_op.hpp"
#include "login.hpp"
#include "map.hpp"
#include "player.hpp"
#include "item.hpp"
#include "message.hpp"
#include "task.hpp"
#include "skill.hpp"
#include "user_feedback.hpp"
#include "cli_proto.hpp"
#include "item.hpp"
#include "restriction.hpp"
#include "warehouse.hpp"
#include "summon_monster.hpp"
#include "ambassador.hpp"
#include "apothecary.hpp"
#include "mail.hpp"
#include "magic_number.hpp"
#include "gf_cryptogram.hpp"
#include "store_sales.hpp"
#include "trade.hpp"
#include "global_data.hpp"
#include "switch.hpp"
#include "indoor_test.hpp"
#include "contest.hpp"
#include "limit.hpp"
#include "card.hpp"
#include "fight_team.hpp"
#include "rank_top.hpp"
#include "other_active.hpp"
#include "chat.hpp"
#include "temporary_team.hpp"

/**
  * @brief data type for handlers that handle protocol packages from client
  */
//typedef int (*cli_proto_hdlr_t)(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief data type for handles that handle protocol packages from client
  */
/*struct cli_proto_handle_t {
	cli_proto_handle_t(cli_proto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0, uint8_t noguest = 0)
		{ hdlr = h; len = l; cmp_method = cmp; no_guest = noguest; }

	cli_proto_hdlr_t	hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
	uint8_t				no_guest;
}; */

/*! save handles for handling protocol packages from client */
//static cli_proto_handle_t cli_handles[cli_proto_cmd_max];
/*! head node to link all players whose waitcmd is not 0 together */
//static list_head_t awaiting_playerlist;

/*! for packing protocol data and send to client. 2M */  
//uint8_t pkgbuf[1 << 21];

//-----------------------------------------------------------------

/**
  * @brief set client protocol handle
  */
#define SET_CLI_HANDLE(op_, len_, cmp_, novisitor_, time_limited_) \
		do { \
			if (cli_handles[cli_proto_ ## op_].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", cli_proto_ ## op_, #op_); \
				return false; \
			} \
			cli_handles[cli_proto_ ## op_] = cli_proto_handle_t(op_ ## _cmd, len_, cmp_, novisitor_, time_limited_); \
		} while (0)

//-----------------------------------------------------------------

/**
  * @brief determine if a msg should be sent to 'p'
  * @param p
  * @return true if the msg should be sent, false otherwise
  */
inline bool msg_receivable(const player_t* p)
{
	return !p->invisible;
}

//-----------------------------------------------------------------

/**
  * @brief package type for send_to_all
  * @see send_to_all
  */
struct to_all_pkg_t {
	uint32_t	len;
	void*		pkg;
};

/**
  * @brief the function that do the real work for send_to_all
  * @param key
  * @param player
  * @param userdata
  * @see send_to_all
  */
static void do_send_to_player(void* key, void* player, void* userdata)
{
	player_t* p = reinterpret_cast<player_t*>(player);
	to_all_pkg_t* pkg = reinterpret_cast<to_all_pkg_t*>(userdata);

	send_to_player(p, pkg->pkg, pkg->len, 0);
}


/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkg package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(player_t* p, void* pkg, uint32_t len, int completed)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(pkg);

	if (send_pkg_to_client(p->fdsess, pkg, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", p->id, bswap(proto->cmd));
		return -1;
	}


	if (completed) {
		p->waitcmd = 0;
		p->sesslen = 0;
	}

	p->last_act_tm = get_now_tv()->tv_sec;
	TRACE_LOG("send pkg to client: uid=%u == %u, seqno=%u, cmd=%u, pkglen=%u == %u, ret=%u, cp:%u", p->id, bswap(proto->id), bswap(proto->seqno), bswap(proto->cmd), len, bswap(proto->len), proto->ret, completed);
	return 0;
}

int send_to_player_by_uid(uint32_t uid, fdsession_t* fdsess, void* pkgbuf, uint32_t len, int completed)
{
	//assert(len <= max_pkg_size);

	tr_proto_t* proto = reinterpret_cast<tr_proto_t*>(pkgbuf);

	proto->id	= uid;
	proto->seq	= uid;
	if (send_pkg_to_client(fdsess, pkgbuf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", uid, proto->cmd);
		return -1;
	}

	TRACE_LOG("send to player %u cmd: %u len:%u err:%u", uid, proto->cmd, len, proto->ret);
	return 0;
}

int send_header_to_player_2(uint32_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t err, int completed)
{
	tr_proto_t pkg;

	init_cli_proto_head(&pkg, 0, cmd, sizeof(pkg));
    return send_to_player_by_uid(uid, fdsess, &pkg, sizeof(pkg), completed);
}

/**
  * @brief send a package to all the players
  * @param pkgbuf the pkg to send
  * @param len length of the package
  * @see do_send_to_player
  */
void send_to_all(void* pkgbuf, uint32_t len)
{
	to_all_pkg_t pkg = { len, pkgbuf };
	traverse_players(do_send_to_player, &pkg);
}


/**
  * @brief send a package header to player p
  * @param p player to send a package to
  * @param cmd command id of the package
  * @param err errno to be set into the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_header_to_player(player_t* p, uint16_t cmd, uint32_t err, int completed)
{
	cli_proto_t pkg;

	init_cli_proto_head_full(&pkg, p->id, p->seqno, cmd, sizeof(pkg), err);
	return send_to_player(p, &pkg, sizeof(pkg), completed);
}

/**
  * @brief send a package to all players that are in the map 'm'
  */
void do_send_to_map(map_t* m, void* pkg, uint32_t len)
{
	list_head_t* l;
	list_for_each (l, &m->playerlist) {
		player_t* player = list_entry(l, player_t, maphook);
		if (msg_receivable(player)) {
			send_to_player(player, pkg, len, 0);
		}
	}
}

/**
  * @brief send a package to all players that are in the same map as 'p' is
  */
void send_to_map(player_t* p, void* pkg, uint32_t len, int completed)
{
	map_t* m = p->cur_map;
	if (!m) {
		send_to_player(p, pkg, len, 0);
	} else {
		do_send_to_map(m, pkg, len);
	}

	if (completed) {
		p->waitcmd = 0;
		p->sesslen = 0;
	}
}

/**
  * @brief send a package to all players that are in the same map as 'p' is
  */
void send_to_map_except_self(player_t* p, void* pkg, uint32_t len)
{
	map_t* m = p->cur_map;
	if (m) {
		list_head_t* l;
		list_for_each (l, &m->playerlist) {
			player_t* player = list_entry(l, player_t, maphook);
			if ((player != p) && msg_receivable(player)) {
				send_to_player(player, pkg, len, 0);
			}
		}
	}
}

inline uint32_t calc_seqno(uint32_t pkglen, uint32_t seqno, uint16_t cmd, uint8_t crc8_val)
{
	return seqno - seqno / 7 + 147 + pkglen % 21 + cmd % 13 + crc8_val;
}

inline uint8_t calc_crc8(uint8_t* body, uint32_t body_len)
{
	uint8_t crc8_val = 0;
	for (uint32_t i = 0; i < body_len; i++) {
		crc8_val ^= (body[i] & 0xff);
	}
	return crc8_val;
}

inline bool is_invaild_seqno(player_t* p, uint32_t head_len, uint32_t head_seqno, uint16_t head_cmd, uint8_t* body, uint32_t body_len)
{
	uint8_t crc8_val = calc_crc8(body, body_len);
	uint32_t seqno_val = calc_seqno(head_len, p->seqno, head_cmd, crc8_val);//p->seqno - p->seqno / 7 + 147 + head_len % 21 + head_cmd % 13;
	p->real_seqno = seqno_val;

	if (seqno_val != head_seqno) {
		return true;
	}
	/*
	++(p->real_seqno);
	if (calc_seqno(head_len, p->real_seqno, head_cmd) != head_seqno) {
		return true;
	} */
	return false;
}

bool is_time_limit_1(player_t *p)
{
//	if (p->adult_flg) {
//		return false;
//	}
	if (p->oltoday >= battle_time_limit) {
		return true;
	}
	return false;
}

bool is_time_limit_2(player_t *p)
{
	return false;
#ifndef VERSION_TAIWAN
	if (p->adult_flg) {
		return false;
	}
	uint32_t lg_tm = get_now_tv()->tv_sec - p->login_tm;
	if ((p->oltoday_db +  lg_tm) >= 3 * 60 * 60) {
		return true;
	}
#endif
	return false;

}

bool is_time_limit_3(player_t *p)
{
//	if (p->adult_flg) {
//		return false;
//	}
	uint32_t lg_tm = get_now_tv()->tv_sec - p->login_tm;
	if ((p->oltoday_db +  lg_tm) >= 5 * 60 * 60) {
		return true;
	}
	return false;

}

void watch_hex_printf(player_t* p, uint32_t cmd, uint8_t *buf,int len)
{
        
    char tmpbuf[100 * 10] = ""; 
    char *pcur = tmpbuf;
    if (len > 100)
        len = 100;
    for (int i=0;i<len;i++)
    {   
        sprintf(pcur,"%02x ",buf[i]&0xff);
        pcur += 3;
    }   
    KDEBUG_LOG(p->id, "watch cmd %u hex: %s", cmd, tmpbuf);
        
}


//---------------------------------------------------------------------

/**
  * @brief dispatches protocol packages from client
  * @param data package from client
  * @param fdsess fd session
  * @return value that was returned by the protocol handling function
  */
int dispatch(void* data, fdsession_t* fdsess, bool first_tm)
{
	cli_proto_t  pkg_head;
	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);
	if (g_server_config[3].value) {
		send_cpkg_head_to_svr_ex(pkg->len, 0, pkg->cmd, pkg->id, pkg->ret, 0);
	}

	pkg_head.len   = bswap(pkg->len);
	pkg_head.cmd   = bswap(pkg->cmd);
	pkg_head.id    = bswap(pkg->id);
	pkg_head.seqno = bswap(pkg->seqno);
	pkg_head.ret   = bswap(pkg->ret);

	// protocol for testing if this server is OK
	if (pkg->cmd == cli_proto_probe) {
		const char *tmp_str = "2012-01-12";
		strcpy((char*)(pkg->body), tmp_str); 
		return send_pkg_to_client(fdsess, pkg, sizeof(cli_proto_t) + strlen(tmp_str));
	}

	if ((pkg_head.cmd < cli_proto_cmd_start) || (pkg_head.cmd >= cli_proto_cmd_end) || (cli_handles[pkg_head.cmd].hdlr == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg_head.cmd, fdsess->fd);
		return -1;
	}

	player_t* p = get_player_by_fd(fdsess->fd);
	if ( ((pkg_head.cmd != cli_proto_login) && !p) 
			|| ((pkg_head.cmd == cli_proto_login) && p)
			|| (p && (p->id != pkg_head.id)) ) {
		ERROR_LOG("pkg error: fd=%d uid=%u %u cmd=%u p=%p",
					fdsess->fd, (p ? p->id : 0), pkg_head.id, pkg_head.cmd, p);
		return -1;
	}
	
	if (first_tm && p) {
		// verify sequence number
        if (pkg_head.cmd != cli_proto_login) {

			if (is_invaild_seqno(p, pkg_head.len, pkg_head.seqno, pkg_head.cmd, pkg->body, pkg_head.len - sizeof(cli_proto_t))) {
				ERROR_LOG("wrong seqno: uid=%u seq=%u %u ret=%u cmd=%u",
						p->id, p->real_seqno, pkg_head.seqno, pkg_head.ret, pkg_head.cmd);
				return -1;
			}
        }
		TRACE_LOG("get a pkg from %u cmd=%u", p->id, pkg_head.cmd);

		p->seqno = pkg_head.seqno;

		if (p->waitcmd != 0) {
			if (g_queue_get_length(p->pkg_queue) < 100) {
				KDEBUG_LOG(p->id, "CACHE A PACKAGE\t[uid=%u len=%u cmd=%u waitcmd=%u]",
							p->id, pkg_head.len, pkg_head.cmd, p->waitcmd);
				cache_a_pkg(p, (char*)pkg, pkg_head.len);
				return 0;
			} else {
				WARN_LOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, pkg_head.cmd);
				return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 0);
			}
		}
	}

	// TODO:
	if ( p && (p->cur_map == 0)	&& (pkg_head.cmd != cli_proto_enter_map) ) {
		assert(!"impossible error! player not in map");
		//WARN_LOG("not in map: id=%u cmd=%d", pkg_head.id, pkg_head.cmd);
		// TODO:
		// respond a error code 
		return 0;
	}

	// login protocol
	player_t player;
	if (pkg_head.cmd == cli_proto_login) {
		player_t* old = get_player(pkg_head.id);
		// multiple login
		if (old) {
			TRACE_LOG("MULTIPLE LOGIN TO SAME ONLINE, UID=%u", old->id);
			// send multiple login notification
			send_header_to_player(old, cli_proto_login, cli_err_multi_login, 0);
			// kick the previous login instance offline
			close_client_conn(old->fd);
		}
		p          = &player;
		p->id      = pkg_head.id;
		p->fdsess  = fdsess;
		p->waitcmd = 0;
		p->seqno = pkg_head.seqno;

	}/* else {
		battle_grp_t* grp = p->battle_grp;
		if (grp && is_battle_started(grp) && grp->id == 0) {
			WARN_LOG("restart invalid cmd[%u %u]",p->id, pkg_head.cmd);
			return 0;
		}
	}*/

	if (cli_handles[pkg_head.cmd].no_guest && is_guest_id(p->id)) {
		ERROR_LOG("guest not allowed: uid=%u", p->id);
		return -1;
	}
	

	uint32_t bodylen = pkg_head.len - sizeof(cli_proto_t);
	if ( ((cli_handles[pkg_head.cmd].cmp_method == cmp_must_eq) && (bodylen != cli_handles[pkg_head.cmd].len))
			|| ((cli_handles[pkg_head.cmd].cmp_method == cmp_must_ge) && (bodylen < cli_handles[pkg_head.cmd].len)) ) {
		ERROR_LOG("invalid package cmd=%u len=%u %u cmpmethod=%d",
					pkg_head.cmd, bodylen, cli_handles[pkg_head.cmd].len, cli_handles[pkg_head.cmd].cmp_method);
		return -1;
	}
#ifndef VERSION_KAIXIN
	if (p && cli_handles[pkg_head.cmd].is_time_limit(p)) {
		//uint32_t tmp_ol_time = p->oltoday_db + (get_now_tv()->tv_sec - p->login_tm);
		//if (tmp_ol_time > 3600 * 5) {
		return send_header_to_player(p, p->waitcmd, cli_err_action_time_limited, 1);
	}
#endif
	p->waitcmd = pkg_head.cmd;
	if (p->id == 150051250) {
		watch_hex_printf(p, pkg_head.cmd, pkg->body, bodylen);
	}
	return cli_handles[pkg_head.cmd].hdlr(p, pkg->body, bodylen);
}

//----------------------------------------------------------------------
/**
  * @brief init handlers to handle protocol packages from client
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_cli_proto_handles()
{
	memset(cli_handles, 0, sizeof(cli_handles));
	INIT_LIST_HEAD(&awaiting_playerlist);	
	/* operation, bodylen, compare method, no guest */
	SET_CLI_HANDLE(login, 32, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(verify_pay_passwd, 32, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(is_set_pay_passwd, 0, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(enter_map, 16, cmp_must_eq, 0, 0);
//    SET_CLI_HANDLE(leave_map, 0, cmp_must_eq, 0);
	SET_CLI_HANDLE(list_user, 0, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(walk, 12, cmp_must_eq, 0, 0);
//	SET_CLI_HANDLE(talk, 10, cmp_must_ge, 0, 0);
	SET_CLI_HANDLE(walk_keyboard ,10, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(stand, 17, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(jump, 16, cmp_must_eq, 0, 0);
	SET_CLI_HANDLE(up_power_user, 0, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(get_usr_simple_info, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_usr_detail_info, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_hunter_top_info, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_hunter_self_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(change_usr_nickname, max_nick_size, cmp_must_eq, 1, 0);
//	SET_CLI_HANDLE(get_timestamp, 8, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(set_amb_task_status, 1, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_amb_task_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_amb_reward, 1, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_daily_hatch_summon_item, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(daily_hatch_summon, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(get_max_times_chapter, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_max_times_chapter, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(finger_guessing, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_finger_guessing_data, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_client_buf, 4 + max_client_buf_len, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_client_buf, 0, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(add_friend, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(reply_add_friend, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(del_friend, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(add_blacklist, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(check_users_onoff, 8, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(forbiden_add_friend, 4, cmp_must_eq, 1, 0);


	SET_CLI_HANDLE(move_item_warehouse_to_bag, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(move_item_bag_to_warehouse, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_warehouse_item_list,  0,  cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(get_warehouse_clothes_item_list, 0,  cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(move_clothes_item_bag_to_warehouse, 8,  cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(move_clothes_item_warehouse_to_bag, 8,  cmp_must_eq, 1, 0);	
	SET_CLI_HANDLE(learn_secondary_pro, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(secondary_pro_fuse, 8, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(mail_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(mail_body, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(delete_mail, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(take_mail_enclosure, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(send_mail, 8+ MAX_MAIL_TITLE_LEN + MAX_MAIL_CONTENT_LEN, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(undertake_task, 4, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(finish_task, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_task_buf, 4, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(set_task_buf, 44, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(cancel_task, 4, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(undertake_daily_task, 0, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(restrict_action, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(restrict_action_ex, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(show_restrict_reward, 4, cmp_must_eq, 1, 3);
	SET_CLI_HANDLE(recv_restrict_reward, 0, cmp_must_eq, 1, 3);
//	SET_CLI_HANDLE(double_exp_action, 0, cmp_must_eq, 1, 3);
	SET_CLI_HANDLE(double_exp_action_use_item, 4, cmp_must_eq, 1, 3);
	SET_CLI_HANDLE(get_killed_boss, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_restrict_action_times, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_buff_skill, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_buff_skill_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(coupons_exchange, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_home_list_pet, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(auto_join_contest, 0, cmp_must_eq, 1, 0); 
	SET_CLI_HANDLE(list_contest_group_info, 0, cmp_must_eq, 1, 0);
//	SET_CLI_HANDLE(contest_group_btl, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(leave_contest_group, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(contest_guess_champion, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_all_contest_player, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_all_passed_contest, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(re_join_contest, 0, cmp_must_eq, 1, 0);
#ifdef DEV_SUMMON
	/* summon monster cmds*/
	SET_CLI_HANDLE(hatch_summon_mon, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_summon_nick, max_nick_size, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(feed_summon_mon, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_fight_summon, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_summons_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_summons_attr_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(use_summon_skills_scroll, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(renew_summon_skill, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(fresh_summon_attr, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(call_summon, 1, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(evolve_summon, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(disable_summon_skill, 5, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(summon_unique_skill, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(allocate_exp_to_summon, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_allocator_exp, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(summon_ai_change, 4, cmp_must_eq, 1, 0);
#endif

    /* ! for god */
	SET_CLI_HANDLE(get_numen_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(invite_numen, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(change_numen_status, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(god_use_skills_scroll, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(make_numen_sex, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(change_numen_nick, 20, cmp_must_eq, 1, 0);

	/* battle cmds */
	
	SET_CLI_HANDLE(cancel_battle, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_ready, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(lv_matching_btl, 13, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(pve_btl, 13, cmp_must_eq, 1, 1);
	
	SET_CLI_HANDLE(create_btl_team, 16, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(join_btl_team, 8, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(hot_join_btl_team, 8, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(leave_btl_team, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(invite_friend_btl_team, 20, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(reply_friend_btl_team, 12, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(ready_btl_team, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(start_btl_team, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(cancel_ready_btl_team, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_btl_team_hot_join, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(kick_user_from_room, 4, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(pve_btl_restart, 0, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(sync_process, 4, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(player_move, 34, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_attack, 24, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_rsp_lag, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_enter_map, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_leave_btl, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_open_treasure_box, 2, cmp_must_eq, 1, 0);

	/* item cmds */
	SET_CLI_HANDLE(buy_item, 12, cmp_must_eq, 1, 3);
	SET_CLI_HANDLE(sell_item, 8, cmp_must_eq, 1, 3);
	SET_CLI_HANDLE(batch_sell_item, 8, cmp_must_ge, 1, 3);

	SET_CLI_HANDLE(buy_clothes, 9, cmp_must_ge, 1, 3);

	SET_CLI_HANDLE(wear_clothes, 4, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(get_clothes_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(sell_clothes, 8, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(repair_all_clothes, 4, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(decompose_attire, 8, cmp_must_ge, 1, 1);
	SET_CLI_HANDLE(compose_attire, 8, cmp_must_ge, 1, 1);
	SET_CLI_HANDLE(get_strengthen_material_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(strengthen_attire, sizeof(strengthen_attire_t) - 4, cmp_must_eq, 1, 3);
	
	
	SET_CLI_HANDLE(get_item_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_itembind, 40, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(use_item, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_pick_item, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(use_skill_book, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(discard_item, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(discard_clothes, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_clothes_duration, 0, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(skill_bind, 4, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(get_skills, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(skill_upgrade, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(learn_new_skill, 4, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(ts_view_magic_gift, 32, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(ts_consume_magic_gift, 34, cmp_must_ge, 1, 3);
	SET_CLI_HANDLE(ts_view_cryptogram_gift, 32, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(ts_consume_cryptogram_gift, 32, cmp_must_eq, 1, 3);
	SET_CLI_HANDLE(query_gcoin_balance, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(query_mb_balance, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(mb_trade, 40, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(query_user_store_item, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(gcoin_trade, 12, cmp_must_ge, 1, 1);
	SET_CLI_HANDLE(getback_outdated_attire, 4, cmp_must_ge, 1, 3);

	SET_CLI_HANDLE(submit_msg, 68,cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(report_msg, 28,cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_questionnaire, 4,cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(market_user_info, 300, cmp_must_ge, 1, 0);

	SET_CLI_HANDLE(get_encryption_session, 4, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(get_ranged_svr_list, 8, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(login_new_svr, 4, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(roll_item, 12, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(set_player_show_state, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_donate_count, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_buy_item_limit_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_player_hero_top, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_get_mechanism_drop, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_change_mechanism, 8, cmp_must_eq, 1, 0);
#ifdef ENABLE_TEST_FUN
	//for test
	SET_CLI_HANDLE(get_player_attr, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_mon_attr, 4, cmp_must_eq, 1, 0);
#endif
	//for home	
	SET_CLI_HANDLE(enter_own_home, 0, cmp_must_eq, 1, 0);	
	SET_CLI_HANDLE(enter_other_home, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(enter_other_home_random, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_friend_summon_list, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(leave_home, 24, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(pet_move, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(pet_home_call, 8, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(get_plants_list, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(set_plant_status, 20, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(sow_plant_seed, 12, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(pick_fruit, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_home_log, 8, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(pve_syn_home_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(home_kick_off_guest, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_home_attr, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(home_level_up, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(insert_vitality_2_home, 4, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(get_decorate_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_decorate, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(add_decorate_lv, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_decorate_reward, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(query_home_player_count, 0, cmp_must_eq, 1, 0);

	//for trade	
	SET_CLI_HANDLE(trade_enter_market, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_leave_market, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_update_shop, 12, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(trade_buy_goods, sizeof(trade_action_t), cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(trade_setup_shop, 4, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(trade_list_shop, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_shop_detail, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_decorate_shop, 8, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(trade_start_shop, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(trade_pause_shop, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_drop_shop, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_change_shop_name, 36, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(trade_get_sell_log, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(trade_enter_aim_market, 4, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(trade_mcast_message, sizeof(trade_mcast_info_t), cmp_must_eq, 1);
	SET_CLI_HANDLE(player_mcast_message, sizeof(player_mcast_msg_t), cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(stat_log, 12, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(invite_safe_trade, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(reply_safe_trade, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(cancel_safe_trade, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(safe_trade_set_item, 12, cmp_must_ge, 1, 0);
	SET_CLI_HANDLE(safe_trade_action_agree, 4, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(get_achievement_data_list, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_achieve_title, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_player_achieve_title, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_ap_toplist, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(achievement_stat_info, 0, cmp_must_eq, 1, 0);  

#ifndef RELEASE_VER
	SET_CLI_HANDLE(change_game_battle_value, 1, cmp_must_ge, 1, 0);
#endif

	//for contest 天下第一比武大会
	//SET_CLI_HANDLE(join_contest_team, 4, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(get_contest_team_info, 0, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(get_contest_donate, 0, cmp_must_eq, 1, 0);
	//SET_CLI_HANDLE(contest_donate_plant, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_self_contest, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_taotai_game_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_advance_game_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_gain_item, 0, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(get_final_game_info, 0, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(get_final_game_rank_list, 4, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(team_member_reward, 4, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(get_swap_action_detail_info, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_app_buff_end, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(captain_team_contest, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_team_contest_room, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(contest_cancel_yazhu, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(get_contest_pvp_list, 0, cmp_must_eq, 1, 0);
//	SET_CLI_HANDLE(player_watch_room, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(contest_pvp_yazhu, 12, cmp_must_eq, 1, 0);
	//拉取环任务的当天完成次数
	SET_CLI_HANDLE(get_master_ring_task_count, 0, cmp_must_eq, 1, 0);	
	SET_CLI_HANDLE(upgrade_item, 8, cmp_must_eq, 1, 1);
	SET_CLI_HANDLE(get_swap_action, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(swap_action, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(swap_action_ex, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_global_limit_data, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(summon_dragon, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_summon_dragon_list, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(check_task_extra_logic, 8, cmp_must_eq, 1, 0);
	//for challenge stage 954
	SET_CLI_HANDLE(get_challenge_status, 4, cmp_must_eq, 1, 0);


	//for card
	SET_CLI_HANDLE(player_get_card_list, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_insert_card, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_erase_card, 12, cmp_must_eq, 1, 0);
//	SET_CLI_HANDLE(card_set_reward, 4, cmp_must_eq, 1, 0);
	//
	//for offline mode
	SET_CLI_HANDLE(insert_offline_fumo_coins, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(start_offline_mode, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_offline_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(exchange_coins_exploit, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(end_player_offline, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(end_summon_offline, 0, cmp_must_eq, 1, 0);


	SET_CLI_HANDLE(create_fight_team, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(invite_team_member, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(reply_invite_team_member, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_team_info, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(del_team_member, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(del_team, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_team_top10, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(contribute_team, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(fetch_team_coin, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(team_active_enter, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_team_active_top100, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_team_member_lv, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(change_team_name, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(change_team_mcast, 240, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(search_team_info, 8, cmp_must_eq, 1, 0);  
	//for city battle 
	SET_CLI_HANDLE(get_city_battle_info, 0, cmp_must_eq, 1, 0);
//	SET_CLI_HANDLE(list_wuseng_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_other_active_info, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_player_other_active_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_player_single_other_active, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_zhanbo_fate, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_select_fate, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(upgrade_god_guard, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_god_guard_position, 40, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_god_guard_position, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_bet, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_fatch_bet, 0, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(list_test_room_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(set_book_for_fault, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(try_enter_test_room, 4, cmp_must_eq, 1, 0); 

	SET_CLI_HANDLE(player_list_ranker_info, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_get_other_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(player_item_2_score, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_player_community, 8, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(list_simple_role_info, 4, cmp_must_eq, 1, 0);

    //master - prentice
    SET_CLI_HANDLE(invite_prentice, 8, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(reply_invite_prentice, 12, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(kick_prentice, 8, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(get_master_info, 0, cmp_must_eq, 1, 0);
    SET_CLI_HANDLE(get_prentice_info, 0, cmp_must_eq, 1, 0);

    SET_CLI_HANDLE(get_reward_player, 0, cmp_must_eq, 1, 0);

	//for xunlu acticve
	SET_CLI_HANDLE(get_xunlu_active_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(get_xunlu_reward, 4, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(random_master, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(change_master, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(protect_banner, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(rob_banner, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_all_banner_info, 0, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(cancel_protect_banner, 0, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(invite_player_2_tmp_team, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(reply_tmp_team_invite, 16, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(tmp_team_member_leave, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_team_active_ranker, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(distract_clothes_strength, 12, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_single_active_ranker, 12, cmp_must_eq, 1, 0);

	SET_CLI_HANDLE(tmp_team_invite_pvp, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(tmp_team_reply_pvp_invite, 8, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(tmp_team_pvp_start, 0, cmp_must_eq, 1, 0);
	
	SET_CLI_HANDLE(chat, 12, cmp_must_ge, 1, 0);

	SET_CLI_HANDLE(generate_magic_number, 4, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(bind_magic_number, 10, cmp_must_eq, 1, 0);
	SET_CLI_HANDLE(list_magic_invite_info, 0, cmp_must_eq, 1, 0); 
	return true;
}



/**
  * @brief scan the 'awaiting_playerlist' and process cached packages of those players whose waitcmd is 0
  */
void proc_cached_pkgs()
{
	list_head_t* cur;
	list_head_t* next;
	list_for_each_safe (cur, next, &awaiting_playerlist) {
		player_t* p = list_entry(cur, player_t, wait_cmd_player_hook);
		while (p->waitcmd == 0) {
			cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(p->pkg_queue));
			KDEBUG_LOG(p->id, "PROC A CACHED PACKAGE\t[uid=%u len=%lu]", p->id, pkg->len - sizeof(cached_pkg_t));

			int err = dispatch(pkg->pkg, p->fdsess,false);
			g_slice_free1(pkg->len, pkg);
			if (!err) {
				if (g_queue_is_empty(p->pkg_queue)) {
					list_del_init(&(p->wait_cmd_player_hook));
					break;
				}
			} else {
				close_client_conn(p->fd);
				break;
			}
		}
	}
}

void cache_a_pkg(player_t *p, char* buf, uint32_t buflen)
{
	// alloc space to hold the incoming package
	uint16_t len	  = sizeof(cached_pkg_t) + buflen;
	cached_pkg_t* cachebuf = reinterpret_cast<cached_pkg_t*>(g_slice_alloc(len));
	cachebuf->len = len;
	memcpy(cachebuf->pkg, buf, buflen);
	// push the incoming package into the package queue
	g_queue_push_tail(p->pkg_queue, cachebuf);
	// add the player to the awaiting player list if he/she is not in it
	if (list_empty(&(p->wait_cmd_player_hook))) {
		list_add_tail(&(p->wait_cmd_player_hook), &awaiting_playerlist);
	}
}

#undef SET_CLI_HANDLE

#define REGISTER_TIMER_TYPE(nbr_, cb_) \
	do {\
			if (register_timer_callback(nbr_, cb_) == -1) {\
				ERROR_LOG("register timer callback error!");\
				return false;\
			}\
	} while(0)

bool init_timer_callback_type()
{
	REGISTER_TIMER_TYPE(send_bsw_keepalive_pkg_idx, send_bsw_keepalive_pkg);
	REGISTER_TIMER_TYPE(connect_to_battle_switch_timely_idx, connect_to_battle_switch_timely);
	REGISTER_TIMER_TYPE(connect_to_trade_switch_timely_idx, connect_to_trade_switch_timely);
	REGISTER_TIMER_TYPE(keep_players_alive_idx, keep_players_alive);
	REGISTER_TIMER_TYPE(set_app_buf_end_idx , set_app_buf_end);
	REGISTER_TIMER_TYPE(kick_all_users_offline_idx, kick_all_users_offline);
	REGISTER_TIMER_TYPE(send_online_tm_notification_idx, send_online_tm_notification);
	REGISTER_TIMER_TYPE(limit_players_online_time_idx, limit_players_online_time);
	REGISTER_TIMER_TYPE(send_sw_keepalive_pkg_idx, send_sw_keepalive_pkg);
	REGISTER_TIMER_TYPE(connect_to_switch_timely_idx, connect_to_switch_timely);
	return  true;
}

#undef REGISTER_TIMER_TYPE


