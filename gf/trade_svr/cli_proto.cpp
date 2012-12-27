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
#include "player.hpp"
#include "item.hpp"
#include "trade.hpp"
#include "cli_proto.hpp"
#include "shop.hpp"
#include "safe_trade.hpp"

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
static cli_proto_handle_t cli_handles[trd_cmd_max];
/*! head node to link all players whose waitcmd is not 0 together */
static AwaitingPlayerList awaiting_playerlist;

/*! for packing protocol data and send to client. 2M */  
uint8_t pkgbuf[1 << 21];

//-----------------------------------------------------------------

/**
  * @brief cache a package for later processing
  */
inline void cache_a_pkg(Player *p, const void* buf, uint32_t buflen);

inline void cache_a_pkg_h(Player *p, const void * buf, uint32_t buflen);

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
			if (cli_handles[trd_ ## op_].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", trd_ ## op_, #op_); \
				return false; \
			} \
			cli_handles[trd_ ## op_] = cli_proto_handle_t(op_ ## _cmd, len_, cmp_); \
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

	tr_proto_t* proto = reinterpret_cast<tr_proto_t*>(pkgbuf);

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


int send_to_player_by_uid(uint32_t uid, fdsession_t* fdsess, void* pkgbuf, uint32_t len, int completed)
{
	assert(len <= max_pkg_size);

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

	init_tr_proto_head_full(&pkg, cmd, sizeof(pkg), err);
    return send_to_player_by_uid(uid, fdsess, &pkg, sizeof(pkg), completed);
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
	tr_proto_t pkg;

	init_tr_proto_head_full(&pkg, cmd, sizeof(pkg), err);
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
	tr_proto_t pkg;

	init_tr_proto_head_full(&pkg, cmd, sizeof(pkg), err);

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
	tr_proto_t* pkg = reinterpret_cast<tr_proto_t*>(data);

    // !NOTE: protocol from safe trade
    if (pkg->cmd >= trd_safe_trade_create_room && pkg->cmd <= trd_safe_trade_action_agree) {
        uint32_t bodylen = pkg->len - sizeof(tr_proto_t);
        handle_safe_trade_dispatch(fdsess, pkg->cmd, pkg->body, bodylen);
        return 0;
    }

	// protocol for testing if this server is OK
	if (pkg->cmd == cli_proto_probe) {
		return send_pkg_to_client(fdsess, pkg, pkg->len);
	}
	if (pkg->cmd == cli_get_version) {
		int idx = sizeof(tr_proto_t);
		char tmpbuf[255] = "20110810";
		pack(pkgbuf, tmpbuf, sizeof(tmpbuf), idx);
		init_tr_proto_head_full(pkgbuf, cli_get_version, idx, 0);
		TRACE_LOG("get version %s", tmpbuf);
		return send_pkg_to_client(fdsess, pkgbuf, idx);
	}

//	TRACE_LOG("get player pkg %u %u %u", pkg->id, pkg->cmd, pkg->len);

	if (pkg->cmd < trd_cmd_start || pkg->cmd > trd_cmd_max || cli_handles[pkg->cmd].hdlr == 0) {
		ERROR_LOG("invalid cmd=%u from fd=%d hdl=%p", pkg->cmd, fdsess->fd, cli_handles[pkg->cmd].hdlr);
		return -1;
	}

    if (pkg->cmd == trd_enter_market) {
		Player* p_o = get_player(pkg->id);
		if (p_o) {
			WARN_LOG("USER [%u] HASN'T LEAVE MARKET BEFORE", pkg->id);
			del_player(p_o);
			//WARN_LOG("DELETE OLD ERROR PLAYER INFO UID=[%u]", pkg->id);
		}
		p_o = add_player(pkg->id, fdsess);
	}

	Player* p = get_player(pkg->id);

	if (p == 0) {
		//WARN_LOG("NONE EXIST PLAYER IN TRADE [%u %u]", pkg->id, pkg->cmd);
		send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, cli_err_system_error);
		return 0;
	} 

	if (pkg->cmd == trd_pause_shop || pkg->cmd == trd_drop_shop || pkg->cmd == trd_leave_market) {
		if (p->p_market && p->p_shop) {
			/*pasue the shop status, then new deal will not happen*/
			TRACE_LOG("shop keeper %u want leave market or pause , drop a shop", p->id);
			p->p_shop->set_shop_constructing();
			/*check if all deal over */
			if (p->p_shop->has_locked_goods()) {
				TRACE_LOG("%u has goods locked cache the cmd", p->id);
				if (g_queue_get_length(p->pkg_queue) < 50) {				
					p->retry_times++;
					if (p->retry_times >= 15) {
						WARN_LOG("%u retry_times >=15 waitcmd=%u cmd=%u", p->id, p->waitcmd, pkg->cmd);
						p->retry_times = 0;
						if (pkg->cmd == trd_leave_market) {
							send_header_to_player(p, pkg->cmd, 0, 1);
							del_player(p);
							return 0;
						}
						//after try 10 times, the goods is still locked send  busy pkg to online
						send_header_to_player(p, pkg->cmd, cli_err_system_busy, 0);
					} else {
						KDEBUG_LOG(p->id, "CACHE_H A PACKAGE\t[uid=%u len=%u cmd=%u waitcmd=%u]",
								p->id, pkg->len, pkg->cmd, p->waitcmd);
						if (first_tm) { 
							//frist time cache the pkg in tail
							cache_a_pkg(p, pkg, pkg->len);
						} else {
							//this pkg should be handle first, cache in head
							cache_a_pkg_h(p, pkg, pkg->len);
						}
					}
				} else {
					WARN_LOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, pkg->cmd);
					send_header_to_player(p, p->waitcmd, cli_err_system_busy, 0);
				}
				return 0;
			}
		}
		p->retry_times = 0;
	}


	if (first_tm) {
		if (p->waitcmd != 0) {
			/*if(p->waitcmd == trd_leave_market) {
				send_header_to_player(p, pkg->cmd, cli_err_system_busy, 0);
				return 0;
			}*/

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

	uint32_t bodylen = pkg->len - sizeof(tr_proto_t);
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
	SET_CLI_HANDLE(enter_market, sizeof(player_info_t), cmp_must_ge);
	SET_CLI_HANDLE(list_user, 0, cmp_must_eq);
	SET_CLI_HANDLE(update_shop, sizeof(update_shop_t), cmp_must_ge);
	SET_CLI_HANDLE(change_market, 4, cmp_must_eq);
	SET_CLI_HANDLE(leave_market,  0, cmp_must_ge);
	SET_CLI_HANDLE(setup_shop, sizeof(setup_shop_t), cmp_must_ge);
	SET_CLI_HANDLE(drop_shop, sizeof(close_shop_t), cmp_must_ge);
	SET_CLI_HANDLE(buy_goods, sizeof(buy_goods_rsp_t), cmp_must_eq);
	SET_CLI_HANDLE(walk,  12, cmp_must_eq);
	SET_CLI_HANDLE(walk_keyboard,  10, cmp_must_eq);
	SET_CLI_HANDLE(jump,  8, cmp_must_eq);
	SET_CLI_HANDLE(stand,  9, cmp_must_eq);
	SET_CLI_HANDLE(talk, 8, cmp_must_ge);

	SET_CLI_HANDLE(list_shop, 0, cmp_must_eq);
	SET_CLI_HANDLE(decorate_shop, 8, cmp_must_eq);
	SET_CLI_HANDLE(get_shop_detail, 4, cmp_must_eq);
	SET_CLI_HANDLE(pause_shop, 4, cmp_must_eq);
	SET_CLI_HANDLE(change_shop_name, 36, cmp_must_eq);
	SET_CLI_HANDLE(wear_clothes, 4, cmp_must_ge);
	SET_CLI_HANDLE(change_summon, sizeof(change_summon_rsp_t), cmp_must_eq);
	SET_CLI_HANDLE(call_summon, 1, cmp_must_eq);

    //safe trade
	//SET_CLI_HANDLE(safe_trade_create_room, 16, cmp_must_eq);
	//SET_CLI_HANDLE(safe_trade_join_room, 12, cmp_must_eq);
	//SET_CLI_HANDLE(safe_trade_cancel_room, 12, cmp_must_eq);
	//SET_CLI_HANDLE(safe_trade_set_item, sizeof(set_item_t), cmp_must_ge);
	//SET_CLI_HANDLE(safe_trade_action_agree, 12, cmp_must_eq);
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

			/*tr_proto_t* p_pkg = reinterpret_cast<tr_proto_t*>(pkg->pkg);
			uint16_t pkgcmd = p_pkg->cmd;*/

			int err = dispatch(pkg->pkg, it->fdsess, false);
			p = get_player(uid);
			if (!p) {
				WARN_LOG("player delete already!\t[uid=%u]",uid);
				break;
			}

			g_slice_free1(pkg->len, pkg);

			/*if (pkgcmd == trd_leave_market) {
				DEBUG_LOG("PROC A CACHED ERROR!");
				break;
			}*/

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
  * @brief cache a package for later processing
  */
inline void cache_a_pkg_h(Player* p, const void* buf, uint32_t buflen)
{
	// alloc space to hold the incoming package
	uint16_t len = sizeof(cached_pkg_t) + buflen;
	cached_pkg_t* cachebuf = reinterpret_cast<cached_pkg_t*>(g_slice_alloc(len));
	cachebuf->len = len;
	memcpy(cachebuf->pkg, buf, buflen);
	// push the incoming package into the package queue
	g_queue_push_head(p->pkg_queue, cachebuf);
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

