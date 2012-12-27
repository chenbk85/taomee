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
#include "home_svr.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "plant.hpp"
#include "decorate.hpp"

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
static cli_proto_handle_t cli_handles[home_cmd_max];
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
//-----------------------------------------------------------------

/**
  * @brief set client protocol handle
  */
#define SET_CLI_HANDLE(op_, len_, cmp_) \
		do { \
			if (cli_handles[home_ ## op_].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", home_ ## op_, #op_); \
				return false; \
			} \
			cli_handles[home_ ## op_] = cli_proto_handle_t(op_ ## _cmd, len_, cmp_); \
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
	assert(len <= max_pkg_size);

    home_proto_t* proto = reinterpret_cast<home_proto_t*>(pkgbuf);

	proto->id	= p->id;
	proto->seq	= p ? p->id : 0;
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

int send_to_player_by_uid(fdsession_t* fdsess, uint32_t uid, void* pkgbuf, uint32_t len, int completed)
{
	assert(len <= max_pkg_size);

    home_proto_t* proto = reinterpret_cast<home_proto_t*>(pkgbuf);

	proto->id	= uid;
	proto->seq	= uid;
	if (send_pkg_to_client(fdsess, pkgbuf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", uid, proto->cmd);
		return -1;
	}

	//if (completed) {
	//	p->waitcmd = 0;
	//}
	TRACE_LOG("send to player %u cmd: %u len:%u err:%u", uid, proto->cmd, len, proto->ret);
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
	home_proto_t pkg;

	init_home_proto_head_full(&pkg, cmd, sizeof(pkg), err);
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
	home_proto_t pkg;

	init_home_proto_head_full(&pkg, cmd, sizeof(pkg), err);

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
	home_proto_t* pkg = reinterpret_cast<home_proto_t*>(data);
	if (pkg->cmd == cli_proto_probe) {
		return send_pkg_to_client(fdsess, pkg, pkg->len);
	}
    if (pkg->cmd == home_query_player_count) {
        return query_player_count(fdsess, pkg->body, pkg->len - sizeof(home_proto_t));
    }
	if (pkg->cmd == cli_get_version) {
		int idx = sizeof(home_proto_t);
		char tmpbuf[255] = "20110810";
		pack(pkgbuf, tmpbuf, sizeof(tmpbuf), idx);

		init_home_proto_head_full(pkgbuf, cli_get_version, idx, 0);
		TRACE_LOG("get version %s", tmpbuf);
		return send_pkg_to_client(fdsess, pkgbuf, idx);
	}


	if (pkg->cmd < home_cmd_start || pkg->cmd > home_cmd_max || cli_handles[pkg->cmd].hdlr == 0) {
		ERROR_LOG("invalid cmd=%u from fd=%d hdl=%p", pkg->cmd, fdsess->fd, cli_handles[pkg->cmd].hdlr);
		return -1;
	}

	if (pkg->cmd == home_enter_home || pkg->cmd == home_enter_friend_home) {
		Player* p_o = get_player(pkg->id);
		if (p_o) {
			WARN_LOG("USER [%u] HASN'T LEAVE MARKET BEFORE", pkg->id);
			del_player(p_o);
		}  
		p_o = add_player(pkg->id, fdsess);
	} 

	Player* p = get_player(pkg->id);

	if (p == 0) {
		send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, cli_err_system_error);
		return 0;
	} 

	if (first_tm) {
		if (p->waitcmd != 0) {
			if (g_queue_get_length(p->pkg_queue) < 50) {				
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

	uint32_t bodylen = pkg->len - sizeof(home_proto_t);
	if ( ((cli_handles[pkg->cmd].cmp_method == cmp_must_eq) && (bodylen != cli_handles[pkg->cmd].len))
			|| ((cli_handles[pkg->cmd].cmp_method == cmp_must_ge) && (bodylen < cli_handles[pkg->cmd].len)) ) {
		ERROR_LOG("invalid package cmd=%u len=%u %u cmpmethod=%d",
					pkg->cmd, bodylen, cli_handles[pkg->cmd].len, cli_handles[pkg->cmd].cmp_method);
		return 0;
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
	SET_CLI_HANDLE(enter_home, sizeof(enter_home_rsp_t), cmp_must_ge);
	SET_CLI_HANDLE(enter_friend_home, sizeof(enter_home_rsp_t), cmp_must_ge);
	SET_CLI_HANDLE(leave_home, 8, cmp_must_eq);
	SET_CLI_HANDLE(pet_call, 8, cmp_must_eq);
	SET_CLI_HANDLE(pet_move, 16, cmp_must_eq); 
	SET_CLI_HANDLE(get_plants_list, 0, cmp_must_eq);
	SET_CLI_HANDLE(set_plant_status, 20, cmp_must_eq);


	SET_CLI_HANDLE(sow_plant_seed, 12, cmp_must_eq);

	SET_CLI_HANDLE(pick_fruit, 8, cmp_must_eq);

	SET_CLI_HANDLE(pet_callback, 1, cmp_must_eq);
	SET_CLI_HANDLE(syn_player_info, 8, cmp_must_eq);

	SET_CLI_HANDLE(kick_off_player, 8, cmp_must_eq);

	//for common op
	SET_CLI_HANDLE(player_walk, 12, cmp_must_eq);
	SET_CLI_HANDLE(walk_keyboard, 10, cmp_must_eq);
	SET_CLI_HANDLE(player_jump, 8, cmp_must_eq);
	SET_CLI_HANDLE(player_stand, 9, cmp_must_eq);
	SET_CLI_HANDLE(player_talk, 8, cmp_must_ge);

	//for battle info
	SET_CLI_HANDLE(player_enter_battle, 0, cmp_must_eq);
	SET_CLI_HANDLE(player_leave_battle, sizeof(enter_home_rsp_t), cmp_must_ge);
	SET_CLI_HANDLE(player_off_line, 0, cmp_must_ge);
	//list user and pet info in the same map
	SET_CLI_HANDLE(list_user, 0, cmp_must_eq);
	SET_CLI_HANDLE(list_pet,  0, cmp_must_eq);
	SET_CLI_HANDLE(get_home_attr, 0, cmp_must_eq);
	SET_CLI_HANDLE(level_up, 0, cmp_must_eq);

    //------ for decorate -------
    SET_CLI_HANDLE(get_decorate_list, 0, cmp_must_eq);
    SET_CLI_HANDLE(set_decorate, 4, cmp_must_eq);
    SET_CLI_HANDLE(add_decorate_lv, 12, cmp_must_ge);
    SET_CLI_HANDLE(get_decorate_reward, 4, cmp_must_eq);

    SET_CLI_HANDLE(query_player_count, 8, cmp_must_eq);

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
			KDEBUG_LOG(it->id, "PROC A CACHED PACKAGE\t[uid=%u len=%lu]", it->id, pkg->len - sizeof(cached_pkg_t));

			uint32_t uid = it->id;
			Player* p = get_player(uid);
			if (!p) {
				WARN_LOG("PROC A CACHED ERROR!\t[uid=%u]",it->id);
				break;
			}

			int err = dispatch(pkg->pkg, it->fdsess, false);
			p = get_player(uid);
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
	return true;
}

#undef SET_CLI_HANDLE

