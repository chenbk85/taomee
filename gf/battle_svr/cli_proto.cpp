#include <libtaomee++/inet/byteswap.hpp>

using namespace taomee;

extern "C" {
#include <assert.h>
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include "fwd_decl.hpp"

#include "battle.hpp"
#include "player.hpp"
#include "item.hpp"
#include "roll.hpp"
#include "cli_proto.hpp"
#include "indoor_test.hpp"

/**
  * @brief type of member option for type of 'awaiting player list'
  */
typedef boost::intrusive::member_hook<Player, ObjectHook, &Player::awaiting_hook> AwaitingOption;
/**
  * @brief data type for holding all the players whose waitcmd is no 0 together
  */
typedef boost::intrusive::list<Player, AwaitingOption, boost::intrusive::constant_time_size<false> > AwaitingPlayerList;

/**
  * @brief data type for handlers that handle protocol packages from client
  */
typedef int (*cli_proto_hdlr_t)(Player* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief data type for handles that handle protocol packages from client
  */
struct cli_proto_handle_t {
	cli_proto_handle_t(cli_proto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	cli_proto_hdlr_t	hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};

/*! save handles for handling protocol packages from client */
static cli_proto_handle_t cli_handles[btl_cmd_max];
/*! head node to link all players whose waitcmd is not 0 together */
static AwaitingPlayerList awaiting_playerlist;

/*! for packing protocol data and send to client. 2M */  
uint8_t pkgbuf[1 << 21];

//-----------------------------------------------------------------

/**
  * @brief cache a package for later processing
  */
inline void cache_a_pkg(Player *p, const void* buf, uint32_t buflen);

/**
  * @brief determine if the given command id is valid for starting a battle
  * @param cmd command id
  */
inline bool is_valid_fight_cmd(uint32_t cmd);

//-----------------------------------------------------------------

/**
  * @brief set client protocol handle
  */
#define SET_CLI_HANDLE(op_, len_, cmp_) \
		do { \
			if (cli_handles[btl_ ## op_].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", btl_ ## op_, #op_); \
				return false; \
			} \
			cli_handles[btl_ ## op_] = cli_proto_handle_t(op_ ## _cmd, len_, cmp_); \
		} while (0)


//-----------------------------------------------------------------

/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkgbuf package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed)
{
	assert(len <= btlproto_max_len);

	btl_proto_t* proto = reinterpret_cast<btl_proto_t*>(pkgbuf);

	proto->id	= p->id;
	proto->seq	= p->btl ? p->btl->id() : 0;
	if (send_pkg_to_client(p->fdsess, pkgbuf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", p->id, proto->cmd);
		return -1;
	}

	if (completed) {
		p->waitcmd = 0;
	}
	TRACE_LOG("send to player %u cmd: %u len:%u err:%u", p->id, proto->cmd, len, proto->ret);
	return 0;
}

/**
  * @brief send a package header to player p
  * @param p player to send a package to
  * @param cmd command id of the package
  * @param err errno to be set into the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_header_to_player(Player* p, uint32_t cmd, uint32_t err, int completed)
{
	btl_proto_t pkg;

	init_btl_proto_head_full(&pkg, cmd, sizeof(pkg), err);
	return send_to_player(p, &pkg, sizeof(pkg), completed);
}

/**
  * @brief send a package header to online server
  * @param uid user who will receive the package
  * @param fdsess
  * @param cmd
  * @param seqno
  * @param err errno to be set into the package
  * @return 0 on success, -1 on error
  */
int send_header_to_online(userid_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t seqno, uint32_t err)
{
	btl_proto_t pkg;

	init_btl_proto_head_full(&pkg, cmd, sizeof(pkg), err);

	pkg.id	= uid;
	pkg.seq = seqno;
	if (send_pkg_to_client(fdsess, &pkg, sizeof(pkg)) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", uid, cmd);
		return -1;
	}

	return 0;
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
	btl_proto_t* pkg = reinterpret_cast<btl_proto_t*>(data);
	// protocol for testing if this server is OK
	if (pkg->cmd == cli_proto_probe) {
		return send_pkg_to_client(fdsess, pkg, pkg->len);
	}
	
	if (pkg->cmd == cli_get_version) {
		int idx = sizeof(btl_proto_t);
		char tmpbuf[255] = "Just a Fuck date test 01";
		pack(pkgbuf, tmpbuf, sizeof(tmpbuf), idx);
		init_btl_proto_head(pkgbuf, cli_get_version, idx);
		TRACE_LOG("get version %s", tmpbuf);
		return send_pkg_to_client(fdsess, pkgbuf, idx);
	}
	
	if ( (pkg->cmd < btl_cmd_start) || (pkg->cmd >= btl_cmd_end)
			|| (cli_handles[pkg->cmd].hdlr == 0) ) {
		ERROR_LOG("invalid cmd=%u from fd=%d hdl=%p", pkg->cmd, fdsess->fd, cli_handles[pkg->cmd].hdlr);
		return -1;
	}

	Player* p = get_player(pkg->id);
	if (p == 0) {
		if (is_valid_fight_cmd(pkg->cmd) && pkg->cmd != btl_player_btl_restart) {
			p = add_player(pkg->id, fdsess);
		} else {
			ERROR_LOG("Player not found! BattleServer might have been restarted! %u", pkg->cmd);
			send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, btl_err_player_not_found);
			return 0;
		}
	} else {
		if (is_valid_fight_cmd(pkg->cmd) && pkg->cmd != btl_player_btl_restart) {
			DEBUG_LOG("PREVIOUS BATTLE NOT ENDED! IT MAY DUE TO THE CRASH OF ONLINE SERVER "
						"OR SOME OTHER ERRORS! WE END THE PREVIOUS ONE HERE AND INIT A NEW BATTLE\t[uid=%u]", p->id);
			set_unneed_save(p);
			del_player(p);
			p = add_player(pkg->id, fdsess);
		} else if ((p->watch_btl && p->watch_btl->id() != pkg->seq)
			   	||(p->btl&& p->btl->id() != pkg->seq)) {
			ERROR_LOG("Battle Group ID Mismatch! BattleServer might have been restarted: uid=%u grpid=%u seq=%u cmd=%u",
			p->id, p->btl->id(), pkg->seq, pkg->cmd);
			send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, btl_err_player_not_found);
			return 0;
	 	}
	}

	if (first_tm) {
		if (p->waitcmd != 0) {
			if (g_queue_get_length(p->pkg_queue) < 50) {				
				//DEBUG_LOG("CACHE A PACKAGE\t[uid=%u len=%u cmd=%u waitcmd=%u]",
				//			p->id, pkg->len, pkg->cmd, p->waitcmd);
				KDEBUG_LOG(p->id, "CACHE A PACKAGE\t[uid=%u len=%u cmd=%u waitcmd=%u]",
							p->id, pkg->len, pkg->cmd, p->waitcmd);
				cache_a_pkg(p, pkg, pkg->len);
				return 0;
			} else {
				WARN_LOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, pkg->cmd);
				return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 0);
			}
		}
	}

	uint32_t bodylen = pkg->len - sizeof(btl_proto_t);
	if ( ((cli_handles[pkg->cmd].cmp_method == cmp_must_eq) && (bodylen != cli_handles[pkg->cmd].len))
			|| ((cli_handles[pkg->cmd].cmp_method == cmp_must_ge) && (bodylen < cli_handles[pkg->cmd].len)) ) {
		ERROR_LOG("invalid package cmd=%u len=%u %u cmpmethod=%d",
					pkg->cmd, bodylen, cli_handles[pkg->cmd].len, cli_handles[pkg->cmd].cmp_method);
		return -1;
	}

	p->waitcmd = pkg->cmd;
	return cli_handles[pkg->cmd].hdlr(p, pkg->body, bodylen);
}

//----------------------------------------------------------------------
/**
  * @brief init handlers to handle protocol packages from client
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_cli_proto_handles()
{
	/* operation, bodylen, compare method */
	SET_CLI_HANDLE(lv_matching_battle, sizeof(player_attr_t), cmp_must_ge);
	SET_CLI_HANDLE(tmp_team_pvp, sizeof(player_attr_t), cmp_must_ge);
	SET_CLI_HANDLE(pve_battle, sizeof(player_attr_t), cmp_must_ge);

//	SET_CLI_HANDLE(pvp_matching_battle, sizeof(player_attr_t), cmp_must_ge);
//	SET_CLI_HAMDLE(pve_matching_battle, sizeof(player_attr_t), cmp_must_ge);
//	SET_CLI_HANDLE(pvp_invite_battle, sizeof(player_attr_t), cmp_must_ge);
//	SET_CLI_HANDLE(pve_invaite_battle, sizeof(player_attr_t), cmp_must_ge);
	
	SET_CLI_HANDLE(create_battle_team, sizeof(btl_team_attr_t) + sizeof(player_attr_t), cmp_must_ge);
	SET_CLI_HANDLE(join_battle_team, sizeof(join_team_attr_t) + sizeof(player_attr_t), cmp_must_ge);
	SET_CLI_HANDLE(hot_join_battle_team, sizeof(hot_join_team_attr_t) + sizeof(player_attr_t), cmp_must_ge);
	SET_CLI_HANDLE(ready_battle_team, 0, cmp_must_eq);
	SET_CLI_HANDLE(cancel_ready_battle_team, 0, cmp_must_eq);
	SET_CLI_HANDLE(set_btl_team_hot_join, 4, cmp_must_eq);
	SET_CLI_HANDLE(kick_user_from_room, 4, cmp_must_eq);
	
	SET_CLI_HANDLE(start_battle_team, 0, cmp_must_eq);
	SET_CLI_HANDLE(leave_battle_team, 0, cmp_must_eq);
	
	SET_CLI_HANDLE(player_btl_restart, sizeof(restart_player_attr_t), cmp_must_eq);
	SET_CLI_HANDLE(sync_process, 4, cmp_must_eq);

	SET_CLI_HANDLE(player_ready, 0, cmp_must_eq);
	SET_CLI_HANDLE(cancel_battle, 1, cmp_must_eq);

	SET_CLI_HANDLE(player_move, sizeof(player_move_t), cmp_must_eq);
	SET_CLI_HANDLE(player_jump, sizeof(player_jump_t), cmp_must_eq);
	SET_CLI_HANDLE(player_stop, sizeof(player_stop_t), cmp_must_eq);
	SET_CLI_HANDLE(player_rsp_lag, 4, cmp_must_eq);
	SET_CLI_HANDLE(player_attack, sizeof(player_attack_t), cmp_must_eq);
	SET_CLI_HANDLE(player_use_item, 4, cmp_must_eq);
	SET_CLI_HANDLE(player_pick_item, 12, cmp_must_eq);
	SET_CLI_HANDLE(player_enter_map, 4, cmp_must_eq);

	SET_CLI_HANDLE(player_clothes_duration, 0, cmp_must_eq);
	SET_CLI_HANDLE(player_open_treasure_box, 12, cmp_must_eq);
	
	SET_CLI_HANDLE(summon_unique_skill, 4, cmp_must_eq);

	//for test
	SET_CLI_HANDLE(get_player_attr, 0, cmp_must_eq);
	SET_CLI_HANDLE(get_mon_attr, 4, cmp_must_eq);

	//for mechanism
	SET_CLI_HANDLE(change_mechanism, 8, cmp_must_eq);
	SET_CLI_HANDLE(change_mechanism_end, 4, cmp_must_eq);

	SET_CLI_HANDLE(contest_watch_yazhu, 12, cmp_must_eq);
	SET_CLI_HANDLE(contest_watch_out, 4, cmp_must_eq);

	//for roll item in battle
	SET_CLI_HANDLE(player_roll_item,  sizeof(player_roll_item_t), cmp_must_eq);

    ///Just can use test
	SET_CLI_HANDLE(change_game_value,  1, cmp_must_ge);

	SET_CLI_HANDLE(summon_ai_change, 4, cmp_must_eq);
	SET_CLI_HANDLE(player_talk, 8, cmp_must_ge);

	return true;
}

/**
  * @brief scan the 'awaiting_playerlist' and process cached packages of those players whose waitcmd is 0
  */
void proc_cached_pkgs()
{
	AwaitingPlayerList::iterator nx = awaiting_playerlist.begin();
	for (AwaitingPlayerList::iterator it = nx; it != awaiting_playerlist.end(); it = nx) {
		++nx;
		while (it->waitcmd == 0) {
			cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(it->pkg_queue));
			DEBUG_LOG("PROC A CACHED PACKAGE\t[uid=%u len=%d]", it->id, pkg->len - (int32_t)sizeof(cached_pkg_t));

			uint32_t uid = it->id;
			int err = dispatch(pkg->pkg, it->fdsess, false);
			Player* p = get_player(uid);
			if (!p) {
				WARN_LOG("player delete already!\t[uid=%u]",uid);
				break;												            
			}

			g_slice_free1(pkg->len, pkg);
			if (!err) {
				if (g_queue_is_empty(it->pkg_queue)) {
					awaiting_playerlist.erase(it);
					break;
				}
			} else {
				close_client_conn(it->fd);
				break;
			}
		}
	}
}

/**
  * @brief cache a package for later processing
  */
inline void cache_a_pkg(Player* p, const void* buf, uint32_t buflen)
{
	// alloc space to hold the incoming package
	uint16_t len = sizeof(cached_pkg_t) + buflen;
	cached_pkg_t* cachebuf = reinterpret_cast<cached_pkg_t*>(g_slice_alloc(len));
	cachebuf->len = len;
	memcpy(cachebuf->pkg, buf, buflen);
	// push the incoming package into the package queue
	g_queue_push_tail(p->pkg_queue, cachebuf);
	// add the player to the awaiting player list if he/she is not in it
	if (!p->awaiting_hook.is_linked()) {
		awaiting_playerlist.push_back(*p);
	}
}

/**
  * @brief determine if the given command id is valid for starting a battle
  * @param cmd command id
  */
inline bool is_valid_fight_cmd(uint32_t cmd)
{
	return ((cmd >= btl_fight_mode_start) && (cmd <= btl_fight_mode_end));
}

#undef SET_CLI_HANDLE

