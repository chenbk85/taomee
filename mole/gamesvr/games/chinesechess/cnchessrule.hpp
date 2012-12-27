#ifndef CHINESE_CHESS_RULE_HPP_
#define CHINESE_CHESS_RULE_HPP_

#include <cassert>
#include <cmath>
#include <cstdlib>

#include <map>
#include <vector>

extern "C" {
#include <gameserv/game.h>
#include <gameserv/util.h>
}

#include <ant/coordinate.hpp>
#include <ant/inet/pdumanip.hpp>

class ChineseChessRule {
public:
	ChineseChessRule(game_group_t* grp);

	int  place_chess(const sprite_t* p, const void* inbuf, int inlen, void* outbuf, int& outlen);
	int  handle_confirmed_req(sprite_t* p, const UidReq& ur);
	int  handle_player_leave(sprite_t* p);

	bool winner_determinate() const	{ return ((flag_ == 1) || (flag_ == 9)); }
	bool draw() const { return (flag_ == 3); }

	void notify_board_info(sprite_t* p, bool toself = true) const;
	// notify_winner should be written here coz different games would have different winner structure
	void notify_winner(const sprite_t* p, bool isdraw = false) const;
	void send_gamescore(sprite_t* p) const;

	int      ended() const { return GER_end_of_game; }
	uint32_t nextmover() const;
private:
	enum ChessType {
		CNCHESS_NoChess,

		CNCHESS_Chariot,
		CNCHESS_Horse,
		CNCHESS_Elephant,
		CNCHESS_Mandarin,
		CNCHESS_General,

		CNCHESS_Cannon,
		CNCHESS_Soldier,

		CNCHESS_Mask      =   7,
		CNCHESS_RedSide   =  64,
		CNCHESS_BlackSide = 128
	};

	// private methods
	void setbaseline(size_t idx, uint8_t side = CNCHESS_RedSide);
	void setsoldiers(size_t idx, uint8_t side = CNCHESS_RedSide);
	void setcannons(size_t idx, uint8_t side = CNCHESS_RedSide);

	bool isvalid(const sprite_t* p) const;
	bool isyourchess(const sprite_t* p, uint8_t chess) const;
	bool isyourchess(const sprite_t* p, uint8_t x, uint8_t y) const;
	bool offpalace(const sprite_t* p) const
			{ return ((to_y_ < 3) || (to_y_ > 5) || ((p->pos_id == 1) ? (to_x_ > 2) : (to_x_ < 7))); }
	bool offriver(const sprite_t* p) const
			{ return ((p->pos_id == 1) ? (to_x_ > 4) : (to_x_ < 5)); }

	void adjust_chessmap(const sprite_t* p);

	bool movable(const sprite_t* p) const;
	bool chariot_movable(const sprite_t* p, uint8_t beg, uint8_t end, bool flag) const;
	bool horse_movable(const sprite_t* p) const;
	bool horse_movable(const sprite_t* p, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) const;
	bool cannon_movable(const sprite_t* p, uint8_t beg, uint8_t end, bool flag) const;
	bool elephant_movable(const sprite_t* p) const;
	bool mandarin_movable(const sprite_t* p) const;
	bool soldier_movable(const sprite_t* p) const;
	bool general_movable(const sprite_t* p) const;

	//-------------- Check Related Methods Begin --------------
	bool ischeck(const sprite_t* p) const;
	bool checked_by_chariot(const sprite_t* p, uint8_t x, uint8_t y) const;
	bool checked_by_horse(const sprite_t* p, uint8_t x, uint8_t y) const;
	bool checked_by_cannon(const sprite_t* p, uint8_t x, uint8_t y) const;
	bool checked_by_soldier(const sprite_t* p, uint8_t x, uint8_t y) const;
	bool straight_check(uint8_t x, uint8_t y, int idx) const
		{ return ( (x == general_pos_[idx].x()) || (y == general_pos_[idx].y()) ); }
	bool general_face_to_face() const;
	//-------------- Check Related Methods End --------------

	bool credit_cond_met() const
			{ return nsteps_ > 25; }

	ChessType chess(uint8_t chess) const
			{ return static_cast<ChessType>(chess & CNCHESS_Mask); }
	ChessType chess(uint8_t x, uint8_t y) const
			{ return static_cast<ChessType>(board_[x][y] & CNCHESS_Mask); }

	// private fields
	uint8_t  board_[10][9];
	uint8_t  from_x_, from_y_, to_x_, to_y_;
	uint8_t  flag_; // 1 winner determinate, 3 draw req comfirmed, 5 surrender, 7 player leave, 9 general face to face
	uint8_t  nsteps_;

	// For Player Arrangement
	typedef std::vector<uint32_t> UidVec;
	UidVec move_order_;
	mutable UidVec::const_iterator nxmover_;
	mutable uint32_t nx_mover_;

	typedef ant::Coordinate2D<uint8_t> CoordinateType;
	typedef std::multimap< uint8_t, CoordinateType > ChessMultiMap;
	ChessMultiMap   chess_multimap_[2];
	CoordinateType  general_pos_[2];
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline
ChineseChessRule::ChineseChessRule(game_group_t* grp)
{
	assert(grp);

	memset(board_, 0, sizeof board_);

	setbaseline(0);
	setbaseline(9, CNCHESS_BlackSide);
	setsoldiers(3);
	setsoldiers(6, CNCHESS_BlackSide);
	setcannons(2);
	setcannons(7, CNCHESS_BlackSide);

	flag_   = 0;
	nsteps_ = 0;

	// Arrange players
	move_order_.reserve(grp->count);
	for (uint8_t i = 1; i <= grp->count; ++i) {
		uint8_t j = 0;
		for (; (j != grp->count) && (grp->players[j]->pos_id != i); ++j);
		move_order_.push_back(grp->players[j]->id);
	}
	nxmover_  = move_order_.begin();
	nx_mover_ = *nxmover_;
}

inline int
ChineseChessRule::handle_confirmed_req(sprite_t* p, const UidReq& ur)
{
	switch (ur.req) {
	case REQ_draw:
		notify_winner(p, true);
		flag_ = 3;
		break;
	case REQ_surrender:
		notify_winner(p);
		flag_ = 5;
		break;
	default:
		ERROR_LOG("%s: Impossible Case\t[Req=%d]", p->group->game->name, ur.req);
		break;
	}
	send_gamescore(p);
	return GER_end_of_game;
}

inline int
ChineseChessRule::handle_player_leave(sprite_t* p)
{
	if ( IS_GAME_PLAYER(p) ) {
		flag_ = 7;
		send_gamescore(p);
		return GER_end_of_game;
	}
	return 0;
}

inline int
ChineseChessRule::place_chess(const sprite_t* p, const void* inbuf, int inlen, void* outbuf, int& outlen)
{
	assert(move_order_[p->pos_id - 1] == p->id);
	CHECK_BODY_LEN(inlen, 4);

	int i = 0;
	ant::unpack(inbuf, from_x_, i);
	ant::unpack(inbuf, from_y_, i);
	ant::unpack(inbuf, to_x_, i);
	ant::unpack(inbuf, to_y_, i);

	//static const char* names[] = { "The Opponent's", "His" };
	//DEBUG_LOG("%u Attempts To Place %s Chess %d from (%d %d) to (%d %d) Where %s Chess %d Lies",
	//			p->id, names[isyourchess(p, from_x_, from_y_)], chess(from_x_, from_y_),
	//			from_x_, from_y_, to_x_, to_y_, names[isyourchess(p, to_x_, to_y_)], chess(to_x_, to_y_));

	if (!isvalid(p)) {
		//ERROR_RETURN(("CNCHESS: Invalid Action! Uid=%u From=(%d, %d) To=(%d, %d)",
		//				p->id, from_x_, from_y_, to_x_, to_y_), -1);
		ERROR_RETURN(("CNCHESS: Invalid Action! Uid=%u From=(%d, %d) To=(%d, %d)",
						p->id, from_x_, from_y_, to_x_, to_y_), -2);
	}
	if (movable(p)) {
		adjust_chessmap(p);

		//DEBUG_LOG("Chess %d Eaten", chess(to_x_, to_y_));
		if (chess(to_x_, to_y_) == CNCHESS_General) {
			flag_ = 1;
		}
		board_[to_x_][to_y_]     = board_[from_x_][from_y_];
		board_[from_x_][from_y_] = 0;

		if (general_face_to_face()) {
			flag_ = 9;
		}

		outlen = 0;
		ant::pack(outbuf, static_cast<uint8_t>(flag_ ? 0 : ischeck(p)), outlen);
	} else {
		//ERROR_RETURN(("CNCHESS: Cannot Move Chess %d From=(%d %d) To=(%d %d) Uid=%u",
		//				board_[from_x_][from_y_], from_x_, from_y_, to_x_, to_y_, p->id), -1);
		ERROR_RETURN(("CNCHESS: Cannot Move Chess %d From=(%d %d) To=(%d %d) Uid=%u",
						board_[from_x_][from_y_], from_x_, from_y_, to_x_, to_y_, p->id), -2);

	}
	++nsteps_;
	return 0;
}

inline void
ChineseChessRule::notify_board_info(sprite_t* p, bool toself) const
{
	if (toself || (p->group->nwatchers > 0)) {
		uint8_t boardinfo[sizeof(protocol_t) + 9 + sizeof board_];
		int i = sizeof(protocol_t);
		ant::pack(boardinfo, move_order_[0], i);
		ant::pack(boardinfo, move_order_[1], i);
		ant::pack(boardinfo, (nx_mover_ == move_order_[1]), i);
		ant::pack(boardinfo, board_, sizeof board_, i);
		init_proto_head(boardinfo, proto_chessboard_info, i);
		if (toself) {
			send_to_self(p, boardinfo, i, 1);
		} else {
			send_to_watchers(p->group, boardinfo, i);
		}
	}
}

inline void
ChineseChessRule::notify_winner(const sprite_t* p, bool isdraw) const
{
	uint8_t pkg[sizeof(protocol_t) + 4];

	uint32_t uid = ( ((flag_ == 7) || (flag_ == 9)) ? move_order_[p->pos_id % 2] : p->id);
	int i = sizeof(protocol_t);
	ant::pack(pkg, ((draw() || isdraw) ? 0 : uid), i);
	init_proto_head(pkg, proto_notify_game_winner, i);
	send_to_group(p->group, pkg, i);
}

inline void
ChineseChessRule::send_gamescore(sprite_t* p) const
{
	static const float rates[] = { 1.0f, 0.4f, 0.5f, 0.5f, 1.0f, 0.4f, 0.0f, 1.0f, 0.4f, 1.0f };

	game_group_t* grp = p->group;
	game_score_t  gs;
	for ( uint8_t i = 0; i != grp->count; ++i ) {
		memset(&gs, 0, sizeof gs);
		if ( credit_cond_met() || (IS_SPRITE_ESCAPE(p) && (grp->players[i] == p)) ) {
			if ( grp->players[i] == p ) {
				if (IS_SPRITE_ESCAPE(p)) {
					gs.iq    = -1;
				} else {
					gs.iq    = static_cast<int>(round(grp->game->IQ  * rates[flag_ - 1]));
					gs.exp   = static_cast<int>(round(grp->game->exp * rates[flag_ - 1]));
					gs.coins = static_cast<int>(round(grp->game->yxb * rates[flag_ - 1]));
				}
				//if ( (flag_ == 1) || (flag_ == 5) ) {
				//	gs.itmid = get_fire_medal(p, 0);
				//}
			} else {
				gs.iq    = static_cast<int>(round(grp->game->IQ  * rates[flag_]));
				gs.exp   = static_cast<int>(round(grp->game->exp * rates[flag_]));
				gs.coins = static_cast<int>(round(grp->game->yxb * rates[flag_]));
				//if ( flag_ == 7 ) {
				//	gs.itmid = get_fire_medal(grp->players[i], 1);
				//}
			}
		}
		submit_game_score(grp->players[i], &gs);
	}
}

inline uint32_t
ChineseChessRule::nextmover() const
{
	if ( nxmover_ == move_order_.end() ) {
		nxmover_ = move_order_.begin();
	}
	return (nx_mover_ = *(nxmover_++));
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline void
ChineseChessRule::setbaseline(size_t idx, uint8_t side)
{
	uint8_t i = 0;
	for (; i != CNCHESS_General; ++i) {
		board_[idx][i] = ((i + 1) | side);
		switch ( chess(board_[idx][i]) ) {
		case CNCHESS_Chariot:
		case CNCHESS_Horse:
			chess_multimap_[side == CNCHESS_BlackSide].insert(std::make_pair(board_[idx][i], CoordinateType(idx, i)));
			break;
		case CNCHESS_General:
			general_pos_[side == CNCHESS_BlackSide].set(idx, i);
			break;
		default:
			break;
		}
	}

	for (uint8_t rl = i; --rl; ++i) {
		board_[idx][i] = (rl | side);
		switch ( chess(board_[idx][i]) ) {
		case CNCHESS_Chariot:
		case CNCHESS_Horse:
			chess_multimap_[side == CNCHESS_BlackSide].insert(std::make_pair(board_[idx][i], CoordinateType(idx, i)));
			break;
		default:
			break;
		}
	}
}

inline void
ChineseChessRule::setcannons(size_t idx, uint8_t side)
{
	board_[idx][1] = (CNCHESS_Cannon | side);
	board_[idx][7] = (CNCHESS_Cannon | side);
	chess_multimap_[side == CNCHESS_BlackSide].insert(std::make_pair(board_[idx][1], CoordinateType(idx, 1)));
	chess_multimap_[side == CNCHESS_BlackSide].insert(std::make_pair(board_[idx][7], CoordinateType(idx, 7)));
}

inline void
ChineseChessRule::setsoldiers(size_t idx, uint8_t side)
{
	for (uint8_t i = 0; i < 9; i += 2) {
		board_[idx][i] = (CNCHESS_Soldier | side);
		chess_multimap_[side == CNCHESS_BlackSide].insert(std::make_pair(board_[idx][i], CoordinateType(idx, i)));
	}
}

inline bool
ChineseChessRule::isvalid(const sprite_t* p) const
{
	assert(p->pos_id < 3);

	return !( (from_x_ > 9) || (from_y_ > 8) || (to_x_ > 9) || (to_y_ > 8) // bound exceeded
				|| ((from_x_ == to_x_) && (from_y_ == to_y_)) // position unchanged
				|| !isyourchess(p, from_x_, from_y_)          // chess selected not belong to p
				|| isyourchess(p, to_x_, to_y_) );            // p's chess at destination
}

inline bool
ChineseChessRule::isyourchess(const sprite_t* p, uint8_t chess) const
{
	static const ChessType types[] = { CNCHESS_RedSide, CNCHESS_BlackSide };

	return (chess & types[p->pos_id - 1]);
}

inline bool
ChineseChessRule::isyourchess(const sprite_t* p, uint8_t x, uint8_t y) const
{
	static const ChessType types[] = { CNCHESS_RedSide, CNCHESS_BlackSide };

	return (board_[x][y] & types[p->pos_id - 1]);
}

inline void
ChineseChessRule::adjust_chessmap(const sprite_t* p)
{
	CoordinateType coord(from_x_, from_y_);
	std::pair<ChessMultiMap::iterator, ChessMultiMap::iterator>	pos;

	// adjust chessmap of the current active player
	int idx = p->pos_id - 1;
	if (chess(from_x_, from_y_) == CNCHESS_General) {
		general_pos_[idx].set(to_x_, to_y_);
	} else {
		pos = chess_multimap_[idx].equal_range(board_[from_x_][from_y_]);
		while (pos.first != pos.second) {
			if ( pos.first->second == coord ) {
				pos.first->second.set(to_x_, to_y_);
				break;
			}
			++(pos.first);
		}
	}
	// adjust chessmap of the curent inactive player
	if ( board_[to_x_][to_y_] ) {
		coord.set(to_x_, to_y_);
		idx = p->pos_id % 2;
		pos = chess_multimap_[idx].equal_range(board_[to_x_][to_y_]);
		while (pos.first != pos.second) {
			if ( pos.first->second == coord ) {
				chess_multimap_[idx].erase(pos.first);
				break;
			}
			++(pos.first);
		}
	}
}

inline bool
ChineseChessRule::chariot_movable(const sprite_t* p, uint8_t beg, uint8_t end, bool flag) const
{
	uint8_t upbound, i;

	if (end > beg) {
		i = beg + 1;
		upbound = end;
	} else {
		i = end + 1;
		upbound = beg;
	}
	if (flag) {
		for ( ; i != upbound; ++i ) {
			if (board_[i][to_y_]) {
				return false;
			}
		}
	} else {
		for ( ; i != upbound; ++i ) {
			if (board_[to_x_][i]) {
				return false;
			}
		}
	}
	return true;
}

inline bool
ChineseChessRule::horse_movable(const sprite_t* p) const
{
	return (((abs(to_y_ - from_y_) == 1) && ((((to_x_ - from_x_) == 2) && !board_[from_x_ + 1][from_y_])
												|| (((from_x_ - to_x_) == 2) && !board_[from_x_ - 1][from_y_])))
			|| ((abs(to_x_ - from_x_) == 1) && ((((to_y_ - from_y_) == 2) && !board_[from_x_][from_y_ + 1])
												|| (((from_y_ - to_y_) == 2) && !board_[from_x_][from_y_ - 1]))));
}

inline bool
ChineseChessRule::horse_movable(const sprite_t* p, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) const
{
	return (((abs(to_y - from_y) == 1) && ((((to_x - from_x) == 2) && !board_[from_x + 1][from_y])
												|| (((from_x - to_x) == 2) && !board_[from_x - 1][from_y])))
			|| ((abs(to_x - from_x) == 1) && ((((to_y - from_y) == 2) && !board_[from_x][from_y + 1])
												|| (((from_y - to_y) == 2) && !board_[from_x][from_y - 1]))));
}

inline bool
ChineseChessRule::cannon_movable(const sprite_t* p, uint8_t beg, uint8_t end, bool flag) const
{
	uint8_t upbound, i, cnt = 0;

	if (end > beg) {
		i = beg + 1;
		upbound = end;
	} else {
		i = end + 1;
		upbound = beg;
	}
	if (flag) {
		for ( ; i != upbound; ++i ) {
			if (board_[i][to_y_] && (++cnt == 2)) {
				return false;
			}
		}
	} else {
		for ( ; i != upbound; ++i ) {
			if (board_[to_x_][i] && (++cnt == 2)) {
				return false;
			}
		}
	}
	return (chess(to_x_, to_y_) ? cnt : !cnt);
}

inline bool
ChineseChessRule::elephant_movable(const sprite_t* p) const
{
	static const uint8_t adj[2] = { -1, 1 };

	return ( (abs(from_x_ - to_x_) == 2) && (abs(from_y_ - to_y_) == 2) // correct moving range
			 && !offriver(p)                                            // within its own side
			 && !chess(from_x_ + adj[to_x_ > from_x_], from_y_ + adj[to_y_ > from_y_]) ); // path not blocked
}

inline bool
ChineseChessRule::mandarin_movable(const sprite_t* p) const
{
	return ( (abs(from_x_ - to_x_) == 1) && (abs(from_y_ - to_y_) == 1) // correct moving range
			 && !offpalace(p) );                                        // within its own palace
}

inline bool
ChineseChessRule::soldier_movable(const sprite_t* p) const
{
	return ( (from_x_ == to_x_) ? ((abs(from_y_ - to_y_) == 1) && offriver(p))
				: ((from_y_ == to_y_) ? ((to_x_ - from_x_) == 1 * ((p->pos_id == 1) ? 1 : -1)) : false) );
}

inline bool
ChineseChessRule::general_movable(const sprite_t* p) const
{
	return (!offpalace(p) && ((from_x_ == to_x_) ? (abs(from_y_ - to_y_) == 1)
				: ((from_y_ == to_y_) ? (abs(from_x_ - to_x_) == 1) : false)));
}

inline bool
ChineseChessRule::movable(const sprite_t* p) const
{
	switch (chess(from_x_, from_y_)) {
	case CNCHESS_Chariot:
		if ( (from_x_ != to_x_) && (from_y_ != to_y_) ) {
			return false;
		}
		return ((from_x_ != to_x_) ? chariot_movable(p, from_x_, to_x_, true) : chariot_movable(p, from_y_, to_y_, false));
	case CNCHESS_Horse:
		return horse_movable(p);
	case CNCHESS_Cannon:
		if ( (from_x_ != to_x_) && (from_y_ != to_y_) ) {
			return false;
		}
		return ((from_x_ != to_x_) ? cannon_movable(p, from_x_, to_x_, true) : cannon_movable(p, from_y_, to_y_, false));
	case CNCHESS_Elephant:
		return elephant_movable(p);
	case CNCHESS_Mandarin:
		return mandarin_movable(p);
	case CNCHESS_Soldier:
		return soldier_movable(p);
	case CNCHESS_General:
		return general_movable(p);
	default:
		return false;
	}
}

//-------------- Check Related Methods Begin --------------
inline bool
ChineseChessRule::ischeck(const sprite_t* p) const
{
	for (ChessMultiMap::const_iterator it = chess_multimap_[p->pos_id - 1].begin(); it != chess_multimap_[p->pos_id - 1].end(); ++it) {
		if ( isyourchess(p, it->first) ) {
			switch (chess(it->first)) {

#define DO_CHECK(func_, p_, x_, y_) \
		if ( func_((p_), (x_), (y_)) ) { \
			return true; \
		} \
		break

			case CNCHESS_Chariot:
				DO_CHECK(checked_by_chariot, p, it->second.x(), it->second.y());
			case CNCHESS_Horse:
				DO_CHECK(checked_by_horse, p, it->second.x(), it->second.y());
			case CNCHESS_Cannon:
				DO_CHECK(checked_by_cannon, p, it->second.x(), it->second.y());
			case CNCHESS_Soldier:
				DO_CHECK(checked_by_soldier, p, it->second.x(), it->second.y());
			default:
				break;

#undef DO_CHECK

			}
		}
	}
	return false;
}

inline bool
ChineseChessRule::checked_by_chariot(const sprite_t* p, uint8_t x, uint8_t y) const
{
	static const int adjs[] = { 1, -1 };

	int idx = p->pos_id % 2;
	if ( !straight_check(x, y, idx) ) {
		return false;
	}

	uint8_t    i;
	ChessType  chs;
	if ( y == general_pos_[idx].y() ) {
		// check x coord
		i = (x > general_pos_[idx].x());
		for (x += adjs[i]; !(chs = chess(x, y)); x += adjs[i]) ;
	} else {
		// check y coord
		i = (y > general_pos_[idx].y());
		for (y += adjs[i]; !(chs = chess(x, y)); y += adjs[i]) ;
	}

	return ( (chs == CNCHESS_General) && !isyourchess(p, x, y) );
}

inline bool
ChineseChessRule::checked_by_horse(const sprite_t* p, uint8_t x, uint8_t y) const
{
	static const int adjs[2][2] = { {2, -2}, {1, -1} };

	int idx = p->pos_id % 2;
	if ( (x == general_pos_[idx].x()) || (y == general_pos_[idx].y()) ) {
		return false;
	}

	ChessType chs = CNCHESS_NoChess;

	uint8_t i   = (y > general_pos_[idx].y());
	uint8_t tmp = x - general_pos_[idx].x();
	switch (tmp) {
	case 1:
		if ( horse_movable(p, x, y, x - 1, y + adjs[0][i]) ) {
			chs = chess(x -= 1, y += adjs[0][i]);
		}
		break;
	case 255:
		if ( horse_movable(p, x, y, x + 1, y + adjs[0][i]) ) {
			chs = chess(x += 1, y += adjs[0][i]);
		}
		break;
	case 2:
		if ( horse_movable(p, x, y, x - 2, y + adjs[1][i]) ) {
			chs = chess(x -= 2, y += adjs[1][i]);
		}
		break;
	case 254:
		if ( horse_movable(p, x, y, x + 2, y + adjs[1][i]) ) {
			chs = chess(x += 2, y += adjs[1][i]);
		}
		break;
	default:
		break;
	}

	return ( (chs == CNCHESS_General) && !isyourchess(p, x, y) );
}

inline bool
ChineseChessRule::checked_by_cannon(const sprite_t* p, uint8_t x, uint8_t y) const
{
	static const int adjs[] = { 1, -1 };

	int idx = p->pos_id % 2;
	if ( !straight_check(x, y, idx) ) {
		return false;
	}

	int       cnt = 0;
	uint8_t   tmp, i;
	ChessType chs;
	if ( y == general_pos_[idx].y() ) {
		// check x coord
		i = (x > general_pos_[idx].x());
		for (tmp = x + adjs[i]; ; tmp += adjs[i]) {
			if ( (chs = chess(tmp, y)) ) {
				if (cnt) {
					cnt += ( (chs == CNCHESS_General) && !isyourchess(p, tmp, y) );
					break;
				} else if ( (chs != CNCHESS_General) || isyourchess(p, tmp, y) ) {
					++cnt;
				} else {
					break;
				}
			}
		}
	} else {
		// check y coord
		i = (y > general_pos_[idx].y());
		for (tmp = y + adjs[i]; ; tmp += adjs[i]) {
			if ( (chs = chess(x, tmp)) ) {
				if (cnt) {
					cnt += ( (chs == CNCHESS_General) && !isyourchess(p, x, tmp) );
					break;
				} else if (	(chs != CNCHESS_General) || isyourchess(p, x, tmp) ) {
					++cnt;
				} else {
					break;
				}
			}
		}
	}

	return (cnt == 2);
}

inline bool
ChineseChessRule::checked_by_soldier(const sprite_t* p, uint8_t x, uint8_t y) const
{
	static const int adjs[] = { 1, -1 };

	int idx = p->pos_id % 2;
	if ( !straight_check(x, y, idx) ) {
		return false;
	}

	uint8_t    i;
	ChessType  chs;
	if ( y == general_pos_[idx].y() ) {
		// check x coord
		i   = (x > general_pos_[idx].x());
		chs = ( (i == (p->pos_id - 1)) ? chess(x + adjs[i], y) : CNCHESS_NoChess );
	} else {
		// check y coord
		i   = (y > general_pos_[idx].y());
		chs = chess(x, y + adjs[i]);
	}

	return (chs == CNCHESS_General);
}

inline bool
ChineseChessRule::general_face_to_face() const
{
	if (general_pos_[0].y() == general_pos_[1].y()) {
		uint8_t upbound, i, idx = (general_pos_[1].x() < general_pos_[0].x());

		i = general_pos_[idx].x() + 1;
		upbound = general_pos_[!idx].x();
		for ( ; i != upbound; ++i ) {
			if (board_[i][general_pos_[1].y()]) {
				return false;
			}
		}
		return true;
	}
	return false;
}
//-------------- Check Related Methods End --------------

#endif // CHINESE_CHESS_RULE_HPP_
