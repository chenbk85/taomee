extern "C" {
#include <gameserv/dll.h>
}

#include "gobang.hpp"

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
// none

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
bool Gobang::determine_winner(const Chess_Pos_Type row, const Chess_Pos_Type col, std::vector<ChessCoord>& adjchesses) const
{
	assert(m_gamegrp && m_chessboard[row][col] &&
			(m_chessboard[row][col] == m_chesscolor[m_nxmover]) &&
			(row < maxrows) && (col < maxcols) && adjchesses.empty());

struct Adjustor {
	Chess_Pos_Type row, col;
};

	const int		maxdirs	= 4; // max directions
	const uint8_t	color	= m_chessboard[row][col];

	ChessCoord coord;
	Adjustor adj[maxdirs] = { {1, 1}, {0, 1}, {-1, 1}, {-1, 0} };
	for ( int i = 0; i != maxdirs; ++i ) {
		Chess_Pos_Type x = row, y = col;
		int j = 0;
		coord.row = x;
		coord.col = y;
		adjchesses.push_back(coord);
		for ( ; ; ) {
			x += adj[i].row;
			y += adj[i].col;
			if ( (x >= maxrows) || (y >= maxcols) || (m_chessboard[x][y] != color) ) {
				if ( ++j == 2 ) {
					adjchesses.clear();
					break;
				}

				x = row;
				y = col;
				adj[i].row = -adj[i].row;
				adj[i].col = -adj[i].col;
				continue;
			}

			coord.row = x;
			coord.col = y;
			adjchesses.push_back(coord);
			if (adjchesses.size() == 5) return true;
		}
	}

	return false;
}

int Gobang::handle_player_action(sprite_t* p, const uint8_t body[], int len)
{
	TRACE_LOG("Gobang Handle Player Action: uid=%u, grpid=%lu",
				p->id, m_gamegrp->id);

struct actpkg {
	uint32_t uid;
	Chess_Pos_Type row, col;
} __attribute__((__packed__));

	if ( started_check(p, len, sizeof(actpkg)) == -1 ) return -1;

	int i = 0;
	actpkg apkg;
	UNPKG_UINT(body, apkg.uid, i);
	UNPKG_UINT(body, apkg.row, i);
	UNPKG_UINT(body, apkg.col, i);
	if ( (p->id != apkg.uid) || (m_nxmover != apkg.uid) ) {
		ERROR_RETURN( ("UserID Mismatch, uid=%u, %u, %u",
						p->id, apkg.uid, m_nxmover), -1 );
	}

	REMOVE_TIMER(p, timerid);
	if ( place_chess(apkg.row, apkg.col) == -1 ) return -1;

	broadcast_action(body, p->id);
	++m_nsteps; // increase number of chesses placed

	std::vector<ChessCoord> adjchesses;
	if ( determine_winner(apkg.row, apkg.col, adjchesses) ) {
		notify_winner(p->id, adjchesses);
		m_nxmover = 0;
		send_game_score(p, 0, 1);

		DEBUG_LOG("Gobang Winner Determined\t[winner=%u]", p->id);
		return GER_end_of_game;
	} else if (draw()) {
		notify_winner(0);
		m_nxmover = 0;
		send_game_score(p, 2, 2);
		return GER_draw_game;
	}

	set_nxmover();
	sprite_t* op = get_sprite_from_gamegrp(m_nxmover, m_gamegrp);
	time_t exptm = time(0) + 62;
	timerid = ADD_TIMER_EVENT(op, on_timer_expire, 0, exptm);
	return 0;
}

int Gobang::handle_player_req(sprite_t* p, const uint8_t body[], int len)
{
	assert(p && body);
	TRACE_LOG("Gobang Handle Player Req: uid=%u, grpid=%lu",
				p->id, m_gamegrp->id);

	if ( started_check(p, len, 1) == -1 ) return -1;

	int i = 0;
	uint8_t reason;
	UNPKG_UINT(body, reason, i);
	if ( reason != 1 && reason != 2 ) {
		ERROR_RETURN( ("Wrong Req: reason=%d, uid=%u, grpid=%lu",
						reason, p->id, m_gamegrp->id), -1 );
	}

	if ( !add_player_req(p->id, reason) ) {
		ERROR_RETURN( ("A Req Is Pending: uid=%u, grpid=%lu",
						p->id, m_gamegrp->id), -1 );
	}

	notify_player_req(p->id, reason);

	return 0;
}

int Gobang::handle_rsp_player_req(sprite_t* p, const uint8_t body[], int len)
{
	assert(p && body && m_gamegrp);
	TRACE_LOG("Gobang Handle Rsp Player Req: uid=%u, grpid=%lu",
				p->id, m_gamegrp->id);

struct rsp_reqpkg {
	uint32_t	uid;
	uint8_t		result;
} __attribute__((__packed__));

	if ( started_check(p, len, sizeof(rsp_reqpkg)) == -1 ) return -1;

	int i = 0;
	rsp_reqpkg rsppkg;
	UNPKG_UINT(body, rsppkg.uid, i);
	UNPKG_UINT(body, rsppkg.result, i);
	if ( rsppkg.result > 2 ) {
		ERROR_RETURN( ("Wrong Rsp %d to %u from %u, grpid=%lu",
						rsppkg.result, rsppkg.uid, p->id, m_gamegrp->id), -1 );
	}

	if ( !chk_player_req(rsppkg.uid, rsppkg.result) ) {
		ERROR_RETURN( ("Wrong Rsp %d to %u from %u, grpid=%lu",
						rsppkg.result, rsppkg.uid, p->id, m_gamegrp->id), -1 );
	}

	rsp_player_req(p->id, rsppkg.uid, rsppkg.result);

	if ( rsppkg.result == 1 ) { // draw
		notify_winner(0);
		send_game_score(p, 2, 2);
		// endmultiplayergame should not be call with Gobang obj!!
		return GER_player_req_draw;
	} else if ( rsppkg.result == 2 ) { //surrender
		notify_winner(p->id);
		send_game_score(p, 3, 4);
		return GER_player_surrender;
	}

	return 0;
}

/*---------------------------------------------------------
  *			global methods
  *---------------------------------------------------------*/
extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) Gobang(grp);
}

extern "C" int game_init()
{
	return 0;
}

extern "C" void game_destroy()
{
}
