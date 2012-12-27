#ifndef GOBANG_HPP_
#define GOBANG_HPP_

#include <cassert>
#include <cmath>
#include <cstring>

#include <map>
#include <vector>

extern "C" {
#include <stdint.h>

#include <gameserv/game.h>
#include <libtaomee/log.h>
#include <gameserv/proto.h>
#include <gameserv/sprite.h>
#include <gameserv/timer.h>
#include <gameserv/game.h>
#include <gameserv/dbproxy.h>


#include <ant/utils.h>
}

#include "../mpog.hpp"

class Gobang : public mpog {
public:
	Gobang(game_group_t* gamegrp);

	void	init(sprite_t* p);
	int		handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
private:
	enum ChessColor {
		white = 1,
		black
	};

	typedef std::map<uint32_t, ChessColor>	ChessColorMap;
	typedef std::map<uint32_t, uint8_t> 	PlayerReqMap;
	typedef uint16_t						Chess_Pos_Type;

	static const Chess_Pos_Type	maxrows	 = 15;
	static const Chess_Pos_Type	maxcols  = 15;
	static const uint8_t		maxsteps = maxrows * maxcols;

	// Command ID for Gobang is ranged from 30031 to 30050
	enum {
		gobang_start	= 31031,
		gobang_action,
		gobang_winner,
		gobang_board_info
	};

	struct ChessCoord {
		uint8_t	row, col;
	} __attribute__((__packed__));

	struct BoardInfoPkg {
		protocol_t	header;
		uint32_t	uid1;
		uint8_t 	chess1;
		uint32_t	uid2;
		uint8_t 	chess2;
		uint8_t 	chessboard[maxrows][maxcols];
	} __attribute__((__packed__));

	bool	add_player_req(uint32_t uid, uint8_t reason);
	bool	chk_player_req(uint32_t uid, uint8_t reason);
	void	broadcast_action(const void* buf, uint32_t uid) const;
	int		handle_player_leave(sprite_t* p);
	void	pack_board_info(BoardInfoPkg& bpkg);
	void	notify_board_info();
	void	notify_board_info(sprite_t* p);
	void	notify_game_start() const;
	void	notify_player_req(uint32_t uid, uint8_t reason) const;
	void	notify_winner(uint32_t uid) const;
	void	notify_winner(uint32_t uid, const std::vector<ChessCoord>& adjchesses) const;
	int		place_chess(Chess_Pos_Type row, Chess_Pos_Type col);
	void	rsp_player_req(uint32_t approver, uint32_t asker, uint8_t result) const;
	bool	credit_cond_met(uint8_t reason) const;
	void	send_game_score(sprite_t* p, uint8_t preason, uint8_t opreason) const;
	void	set_nxmover();

	bool	determine_winner(const Chess_Pos_Type row, const Chess_Pos_Type col, std::vector<ChessCoord>& adjchesses) const;
	bool	draw() const { return (m_nsteps == maxsteps); };
	int		handle_player_action(sprite_t* p, const uint8_t body[], int len);
	int		handle_player_req(sprite_t* p, const uint8_t body[], int len);
	int		handle_rsp_player_req(sprite_t* p, const uint8_t body[], int len);

	game_group_t*	m_gamegrp;
	uint32_t		m_nxmover;		// next mover
	uint8_t			m_nsteps;		// number of chesses placed
	ChessColorMap	m_chesscolor;
	PlayerReqMap	m_req;			// request to draw or surrender
	uint8_t			m_chessboard[maxrows][maxcols];

	timer_id_t		timerid;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline Gobang::Gobang(game_group_t* gamegrp)
{
	assert(gamegrp);
	m_gamegrp = gamegrp;
	m_nxmover = gamegrp->players[0]->id;
	m_nsteps  = 0;
	m_chesscolor[gamegrp->players[0]->id] = white;
	m_chesscolor[gamegrp->players[1]->id] = black;
	std::memset(m_chessboard, 0, sizeof m_chessboard);

	timerid   = 0;
}

inline int Gobang::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	int err;
	switch (cmd) {
	case gobang_action:
		err = handle_player_action(p, body, len);
		break;
	case proto_player_req:
		err = handle_player_req(p, body, len);
		break;
	case proto_notify_player_req:
		err = handle_rsp_player_req(p, body, len);
		break;
	case proto_player_leave:
		err = handle_player_leave(p);
		break;
	default:
		ERROR_RETURN(("Gobang: Unrecognized Command ID %d from uid=%u", cmd, p->id), -1);
		break;
	}

	return err;
}

inline void Gobang::init(sprite_t* p)
{
	assert(GAME_READY(p->group));
	TRACE_LOG("Gobang Init: uid=%u, player=%d, grpid=%lu",
				p->id, IS_GAME_PLAYER(p), m_gamegrp->id);

	if ( IS_GAME_PLAYER(p) ) {
		SET_GAME_START(p->group);
		notify_game_start();
		notify_board_info();
		sprite_t* p = get_sprite_from_gamegrp(m_nxmover, m_gamegrp);
		time_t exptm = time(0) + 62;
		timerid = ADD_TIMER_EVENT(p, on_timer_expire, 0, exptm);
	} else {
		notify_board_info(p);
	}
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline bool	Gobang::add_player_req(uint32_t uid, uint8_t reason)
{
	assert(GAME_STARTED(m_gamegrp));
	TRACE_LOG("Gobang Add Player Req: uid=%u, grpid=%lu", uid, m_gamegrp->id);

	std::pair<PlayerReqMap::iterator, bool> ret
		= m_req.insert(PlayerReqMap::value_type(uid, reason));
	return ret.second;
}

/**
 * chk_player_req - verify rsp to req
 * @uid: req uid
 * @result: rsp result
 *
 * returns: true if everything goes well, otherwise false
 */
inline bool Gobang::chk_player_req(uint32_t uid, uint8_t result)
{
	assert(GAME_STARTED(m_gamegrp));
	TRACE_LOG("Gobang Chk Player Req: RSP to=%u, grpid=%lu, result=%d",
				uid, m_gamegrp->id, result);

	PlayerReqMap::iterator it = m_req.find(uid);
	if ( (it != m_req.end()) &&	((it->second == result) || (result == 0)) ) {
		m_req.erase(it);
		return true;
	}

	DEBUG_LOG("Gobang RSP to REQ Mismatched\t[rsp=%d, req=%d, to=%u, grpid=%lu]",
				result, it->second, uid, m_gamegrp->id);
	return false;
}

inline void Gobang::broadcast_action(const void* buf, uint32_t uid) const
{
struct actpkg {
	protocol_t	header;
	uint32_t	uid;
	Chess_Pos_Type row, col;
} __attribute__((__packed__));

	actpkg apkg;
	init_proto_head(&apkg, gobang_action, sizeof apkg);
	memcpy( &(apkg.uid), buf, (sizeof apkg - sizeof apkg.header) );
	send_to_group_except_self(m_gamegrp, uid, &apkg, sizeof apkg);
}

inline int Gobang::handle_player_leave(sprite_t* p)
{
	if ( IS_GAME_PLAYER(p) ) {
		send_game_score(p, 5, 0);
		return 1;
	}

	return 0;
}

inline void Gobang::pack_board_info(BoardInfoPkg& bpkg)
{
	init_proto_head(&bpkg, gobang_board_info, sizeof bpkg);
	HTON(bpkg.uid1, m_gamegrp->players[0]->id);
	HTON(bpkg.uid2, m_gamegrp->players[1]->id);
	HTON(bpkg.chess1, m_chesscolor[m_gamegrp->players[0]->id]);
	HTON(bpkg.chess2, m_chesscolor[m_gamegrp->players[1]->id]);
	std::memcpy(bpkg.chessboard, m_chessboard, sizeof m_chessboard);
}

inline void Gobang::notify_board_info()
{
	assert(GAME_STARTED(m_gamegrp));

	if ( m_gamegrp->nwatchers > 0 ) {
		BoardInfoPkg bpkg;
		pack_board_info(bpkg);
		send_to_watchers(m_gamegrp, &bpkg, sizeof bpkg);
	}
}

inline void Gobang::notify_board_info(sprite_t* p)
{
	assert(IS_GAME_WATCHER(p) && GAME_STARTED(p->group));

	BoardInfoPkg bpkg;
	pack_board_info(bpkg);
	send_to_self(p, &bpkg, sizeof bpkg, 1);
}

inline void	Gobang::notify_game_start() const
{
	assert(m_gamegrp);

struct startpkg {
	protocol_t	header;
	uint32_t	uid;
} __attribute__((__packed__));

	startpkg pkg;
	init_proto_head(&(pkg.header), gobang_start, sizeof pkg);
	pkg.uid = htonl(m_nxmover);
	send_to_players(m_gamegrp, &pkg, sizeof pkg);

	TRACE_LOG("Notify Gobang Start: nxmover=%u grpid=%lu",
				m_nxmover, m_gamegrp->id);
}

inline void	Gobang::notify_player_req(uint32_t uid, uint8_t reason) const
{
	assert(GAME_STARTED(m_gamegrp));
	TRACE_LOG("Gobang Notify Player Req: uid=%u, grpid=%lu, reason=%d",
				uid, m_gamegrp->id, reason);

struct notif_reqpkg {
	protocol_t	header;
	uint32_t	uid;
	uint8_t		reason;
} __attribute__((__packed__));

	notif_reqpkg nrpkg;

	init_proto_head(&nrpkg, proto_notify_player_req, sizeof nrpkg);
	HTON(nrpkg.uid, uid);
	HTON(nrpkg.reason, reason);
	send_to_group_except_self(m_gamegrp, uid, &nrpkg, sizeof nrpkg);
}

inline void Gobang::notify_winner(uint32_t uid) const
{
	assert(m_gamegrp);

struct winpkg {
	protocol_t	header;
	uint32_t	uid;
} __attribute__((__packed__));

	uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351372, 1, 1};
	send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, uid);

	winpkg wpkg;
	init_proto_head(&wpkg, gobang_winner, sizeof wpkg);
	HTON(wpkg.uid, uid);
	send_to_group(m_gamegrp, &wpkg, sizeof wpkg);
}

inline void Gobang::notify_winner(uint32_t uid, const std::vector<ChessCoord>& adjchesses) const
{
	assert(adjchesses.size() == 5);

struct winpkg {
	protocol_t	header;
	uint32_t	uid;
	ChessCoord	coords[5];
} __attribute__((__packed__));

	uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351372, 1, 1};
	send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, uid);
	
	winpkg wpkg;
	init_proto_head(&wpkg, gobang_winner, sizeof wpkg);
	HTON(wpkg.uid, uid);
	int i = 0;
	for ( std::vector<ChessCoord>::const_iterator it = adjchesses.begin();
			it != adjchesses.end(); ++it, ++i ) {
		wpkg.coords[i] = *it;
	}

	send_to_group(m_gamegrp, &wpkg, sizeof wpkg);
}

inline int Gobang::place_chess(Chess_Pos_Type row, Chess_Pos_Type col)
{
	assert(m_gamegrp);
	if ( (row >= maxrows) || (col >= maxcols) || m_chessboard[row][col] ) {
		ERROR_RETURN(("Chess Placed At Wrong Position [%u, %u]: uid=%u, grpid=%lu", row, col, m_nxmover, m_gamegrp->id), -1);
	}

	m_chessboard[row][col] = m_chesscolor[m_nxmover];
	TRACE_LOG("Chess Placed At: %d, %d", row, col);
	return 0;
}

inline void Gobang::rsp_player_req(uint32_t approver, uint32_t asker, uint8_t result) const
{
	assert(GAME_STARTED(m_gamegrp));

struct rspkg {
	protocol_t	header;
	uint32_t	approver;
	uint32_t	asker;
	uint8_t		result;
} __attribute__((__packed__));

	rspkg rpkg;
	init_proto_head(&rpkg, proto_player_req, sizeof rpkg);
	HTON(rpkg.approver, approver);
	HTON(rpkg.asker, asker);
	HTON(rpkg.result, result);
	send_to_group_except_self(m_gamegrp, approver, &rpkg, sizeof rpkg);
}

/**
  *  reasons: 0 - winner rate, 1 - loser rate, 2 - draw rate,
  *			3 - win coz of surrender, 4 - surrender rate, 5 - user offline
  */
inline bool Gobang::credit_cond_met(uint8_t reason) const
{
	return !( (reason > 1 && m_nsteps < 30) || (reason < 2 && m_nsteps < 15) );
}

inline void Gobang::send_game_score(sprite_t* p, uint8_t preason, uint8_t opreason) const
{
	assert(GAME_STARTED(p->group));

	static const float rates[] = { 1.0f, 0.5f, 0.82f, 1.0f, 0.5f, 0.0f };

	game_score_t gs;
	for ( uint8_t i = 0; i != m_gamegrp->count; ++i ) {
		memset(&gs, 0, sizeof gs);
		if ( credit_cond_met(preason) || (IS_SPRITE_ESCAPE(p) && (m_gamegrp->players[i] == p)) ) {
			if ( m_gamegrp->players[i] == p ) {
				if (IS_SPRITE_ESCAPE(p)) {
					gs.iq    = -1;
				} else {
					gs.iq    = static_cast<int>(round(m_gamegrp->game->IQ  * rates[preason]));
					gs.exp   = static_cast<int>(round(m_gamegrp->game->exp * rates[preason]));
					gs.coins = static_cast<int>(round(m_gamegrp->game->yxb * rates[preason]));
				//if ( (preason == 0u) || (preason == 3u) ) {
    //                    gs.itmid = get_fire_medal(p, 0);
    //                }
				}
//-----------------------------------------------------------------
//				if ( (preason == 0u) || (preason == 3u) ) {
//					gs.itmid = get_medal(p);
//				}
//-----------------------------------------------------------------
			} else {
				gs.iq    = static_cast<int>(round(m_gamegrp->game->IQ  * rates[opreason]));
				gs.exp   = static_cast<int>(round(m_gamegrp->game->exp * rates[opreason]));
				gs.coins = static_cast<int>(round(m_gamegrp->game->yxb * rates[opreason]));
				//if ( (opreason == 0u) || (opreason == 3u) ) {
    //                gs.itmid = get_fire_medal(m_gamegrp->players[i], 1);
    //            }
//-----------------------------------------------------------------
//				if ( (opreason == 0u) || (opreason == 3u) ) {
//					gs.itmid = get_medal(m_gamegrp->players[i]);
//				}
//-----------------------------------------------------------------
			}
		}
		submit_game_score(m_gamegrp->players[i], &gs);
	}
}

inline void Gobang::set_nxmover()
{
	assert(m_gamegrp);
	for ( uint8_t i = 0; i != m_gamegrp->count; ++i ) {
		if ( m_gamegrp->players[i]->id != m_nxmover ) {
			m_nxmover = m_gamegrp->players[i]->id;
			return;
		}
	}
}

#endif // GOBANG_HPP_
