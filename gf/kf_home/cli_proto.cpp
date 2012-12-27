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

//#include "home.hpp"
#include "player.hpp"
#include "Online.hpp"

#include "cli_proto.hpp"

/**
  * @brief data type for handlers that handle protocol packages from client
  */
typedef int (*cli_proto_hdlr_t)(player_t* p, uint8_t* body, uint32_t bodylen);

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
static cli_proto_handle_t cli_handles[cli_proto_cmd_max];
/*! head node to link all players whose waitcmd is not 0 together */
static list_head_t awaiting_playerlist;


//-----------------------------------------------------------------

/**
  * @brief cache a package for later processing
  */
inline void cache_a_pkg(player_t *p, const void* buf, uint32_t buflen);

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
int send_to_player(player_t* p, void* pkgbuf, uint32_t len, int completed)
{
	assert(len <= cli_proto_max_len);

	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(pkgbuf);

	proto->id	= p->id;
	//proto->seq	= p->home ? p->home->id() : 0;
	if (send_pkg_to_client(p->fdsess, pkgbuf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", p->id, proto->cmd);
		return -1;
	}

	if (completed) {
		p->waitcmd = 0;
		p->sesslen = 0;
	}

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
int send_header_to_player(player_t* p, uint32_t cmd, uint32_t err, int completed)
{
	cli_proto_t pkg;

	init_cli_proto_head(&pkg, cmd, sizeof(pkg));
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
	cli_proto_t pkg;

	init_cli_proto_head(&pkg, cmd, sizeof(pkg));

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
	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);

	// protocol for testing if this server is OK
	if (pkg->cmd == cli_proto_probe) {
		DEBUG_LOG("test ok");
		return send_pkg_to_client(fdsess, pkg, pkg->len);
	}

	if ( (pkg->cmd < cli_proto_cmd_start) || (pkg->cmd >= cli_proto_cmd_end)
			|| (cli_handles[pkg->cmd].hdlr == 0) ) {
		ERROR_LOG("invalid cmd=%u from fd=%d hdl=%p", pkg->cmd, fdsess->fd, cli_handles[pkg->cmd].hdlr);
		return -1;
	}

	player_t* p = get_player(pkg->id);
	if (p == 0) {
		if ( pkg->cmd == cli_proto_login_home ) {
			p = add_player(pkg->id, fdsess);
		} else {
			ERROR_LOG("Player not found! BattleServer might have been restarted!");
			send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, cli_err_user_not_found);
			return 0;
		}
	} else {
		/*if (is_valid_fight_cmd(pkg->cmd)) {
			DEBUG_LOG("PREVIOUS BATTLE NOT ENDED! IT MAY DUE TO THE CRASH OF ONLINE SERVER "
						"OR SOME OTHER ERRORS! WE END THE PREVIOUS ONE HERE AND INIT A NEW BATTLE");
			del_player(p);
			p = add_player(pkg->id, fdsess);
		} else if (p->home->id() != pkg->seq) {
			ERROR_LOG("Battle Group ID Mismatch! BattleServer might have been restarted: uid=%u grpid=%u seq=%u cmd=%u",
						p->id, p->home->id(), pkg->seq, pkg->cmd);
			send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, home_err_player_not_found);
			return 0;
		}*/
	}

	if (first_tm && p) {
		if (p->waitcmd != 0) {
			if (g_queue_get_length(p->pkg_queue) < 50) {				
				DEBUG_LOG("CACHE A PACKAGE\t[uid=%u len=%u cmd=%u waitcmd=%u]",
							p->id, pkg->len, pkg->cmd, p->waitcmd);
				cache_a_pkg(p, pkg, pkg->len);
				return 0;
			} else {
				WARN_LOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, pkg->cmd);
				return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 0);
			}
		}
	}

	uint32_t bodylen = pkg->len - sizeof(cli_proto_t);
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
	INIT_LIST_HEAD(&awaiting_playerlist);	
	/* operation, bodylen, compare method */
	//SET_CLI_HANDLE(Online::login_home, 0, cmp_must_eq);
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
			DEBUG_LOG("PROC A CACHED PACKAGE\t[uid=%u len=%d]", p->id, pkg->len - sizeof(cached_pkg_t));

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

/**
  * @brief cache a package for later processing
  */
inline void cache_a_pkg(player_t* p, const void* buf, uint32_t buflen)
{
	// alloc space to hold the incoming package
	uint16_t len = sizeof(cached_pkg_t) + buflen;
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

