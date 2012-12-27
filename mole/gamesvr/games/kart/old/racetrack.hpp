#ifndef RACETRACK_HPP_
#define RACETRACK_HPP_

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <ant/random/random.hpp>
#include <ant/inet/pdumanip.hpp>

#include "cmdid.hpp"

extern "C" {
#include <stdint.h>

#include <gameserv/game.h>
#include <gameserv/proto.h>
}

class RaceTrack {
public:
	struct PosType {
		PosType(double xpos = 0.0, double ypos = 0.0) { x = xpos; y = ypos; }
		double x, y;
	};

	struct ColliType {
		ColliType() { memset(this, 0, sizeof *this); }
		bool operator!=(const ColliType& colli) const
			{ return ((item != colli.item) || (x != colli.x) || (y != colli.y)); }
		ItemType item;
		uint16_t x, y;
	};

	static const int max_tracks_num = 5;
	static const int max_track_len  = 800;
	static const int xmax = max_track_len - 1;
	static const int xmin = 0;
	static const int ymax = max_tracks_num - 1;
	static const int ymin = 0;

	RaceTrack(int min_interv, int max_interv);

	bool    chk_collision(const PosType& pos, ColliType& colli) const;
	bool    terminal_arrived(const PosType& pos) const;
	bool    edible(ItemType itm) const;
	void    sendmap(game_group_t* grp) const;

	void    place_barrier(sprite_t* p, const PosType& pos, uint8_t bartype);

	static PosType  start_pos(int track_no, int nplayers);
	static void     adjust_pos(PosType& pos);
//	static void     adjust_pos(PosType& pos1, PosType& pos2);
private:
	bool do_chk_collision(int x, int* y, const PosType& pos, ColliType& colli) const;
	void set_collision(ColliType& colli, int x, int y) const;

	typedef uint8_t TrackInfo[max_tracks_num][max_track_len];

	mutable TrackInfo  track_;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline
RaceTrack::RaceTrack(int min_interv, int max_interv)
{
	static const uint8_t itms[] = {	ITEM_rocket_engine,
									ITEM_tapered_bar,
									ITEM_rounded_hole,
									ITEM_tmp,
									ITEM_tapered_bar,
									ITEM_ice_magic,
									ITEM_pending_banana,
									ITEM_rounded_hole,
									ITEM_tmp,
									ITEM_pending_banana,
									ITEM_rocket_engine,
									ITEM_tapered_bar,
									ITEM_rocket_engine,
									ITEM_rounded_hole,
									ITEM_tapered_bar,
									ITEM_tmp,
									ITEM_pending_banana,
									ITEM_tapered_bar };

	uint8_t tile;
	srand(time(0));
	memset(track_, 0, sizeof track_);

	const int upbound = max_track_len - 10;
	for ( int i = 0; i != max_tracks_num; ++i ) {
		int j = 0;
		while ( (j += ant::ranged_random(min_interv, max_interv)) < upbound ) {
			tile = rand() % ITEM_rate;
			if ( tile < ITEM_max ) {
				track_[i][j] = itms[tile];
			}
		}
	}
}

inline void
RaceTrack::sendmap(game_group_t* grp) const
{
	uint8_t  msg[4096];
	int i = sizeof(protocol_t);

	ant::pack(msg, static_cast<uint16_t>(max_track_len), i);
	ant::pack(msg, track_, sizeof track_, i);
	init_proto_head(msg, KG_map_info, i);
	send_to_players(grp, msg, i);
}

inline bool
RaceTrack::chk_collision(const PosType& pos, ColliType& colli) const
{
	int x[] = { static_cast<int>(floor(pos.x)), static_cast<int>(ceil(pos.x)) };
	int y[] = { static_cast<int>(floor(pos.y)), static_cast<int>(ceil(pos.y)) };

	if ( (y[0] < ymin) || (y[0] > ymax) || (y[1] < ymin) || (y[1] > ymax) ) {
		colli.item = ITEM_track_border;
		return true;
	} else {
// TODO
		x[0] = static_cast<int>(floor(pos.x + 0.2));
		x[1] = static_cast<int>(ceil(pos.x - 0.2));
//		y[0] = static_cast<int>(floor(pos.y + 0.5));
//		y[1] = static_cast<int>(ceil(pos.y - 0.5));
		y[0] = static_cast<int>(floor(pos.y));
		y[1] = static_cast<int>(ceil(pos.y));

		if (do_chk_collision(x[0], y, pos, colli)) {
			return true;
		}
		if ( (x[0] != x[1]) && do_chk_collision(x[1], y, pos, colli) ) {
			return true;
		}
	}
	return false;
}

inline bool
RaceTrack::terminal_arrived(const PosType& pos) const
{
	int x = static_cast<int>(ceil(pos.x));
	if ( x > xmax ) {
		return true;
	}
	return false;
}

inline bool
RaceTrack::edible(ItemType itm) const
{
	return (itm > ITEM_rounded_hole);
/*	switch (itm) {
	case ITEM_pending_banana:
	case ITEM_ice_magic:
	case ITEM_rocket_engine:
		return true;
	default:
		return false;
	}*/
}

inline void
RaceTrack::place_barrier(sprite_t* p, const PosType& pos, uint8_t bartype)
{
	int x = static_cast<int>(floor(pos.y));
	int y = static_cast<int>(floor(pos.x) - 1);
	track_[x][y] = bartype;
}

inline void
RaceTrack::RaceTrack::adjust_pos(PosType& pos)
{
	if (pos.y > ymax) {
//		pos.y = ymax - 0.2;
		pos.y = ymax;
	} else if (pos.y < ymin) {
//		pos.y = ymin + 0.2;
		pos.y = ymin;
	}
}

/*inline void RaceTrack::adjust_pos(PosType& pos1, PosType& pos2)
{
	if (pos1.x > pos2.x) {
		pos1.x += 0.2;
	} else if (pos1.x < pos2.x) {
		pos2.x += 0.2;
	}

	if (pos1.y > pos2.y) {
		pos1.y += 0.2;
		pos2.y -= 0.2;
	} else if (pos1.y < pos2.y) {
		pos1.y -= 0.2;
		pos2.y += 0.2;
	}
}*/

inline
RaceTrack::PosType RaceTrack::start_pos(int track_no, int nplayers)
{
	switch (track_no) {
	case 0:
		return PosType(xmin, 1 - (nplayers == 4));
	case 1:
		return PosType(xmin, 2 + (nplayers == 2) - (nplayers == 4));
	case 2:
		return PosType(xmin, 3 - (nplayers == 4));
	case 3:
		return PosType(xmin, 4 - (nplayers == 4));
	default:
		ERROR_LOG("Impossible Track Number: %d", track_no);
		break;
	}
	return PosType();
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline bool
RaceTrack::do_chk_collision(int x, int* y, const PosType& pos, ColliType& colli) const
{
	switch (track_[y[0]][x]) {
	case ITEM_dropped_banana:
	case ITEM_pending_banana:
	case ITEM_ice_magic:
	case ITEM_rocket_engine:
	case ITEM_tapered_bar:
		if (y[0] == floor(pos.y + 0.5)) {
			set_collision(colli, x, y[0]);
			return true;
		}
		break;
	case ITEM_rounded_hole:
		if (y[0] == floor(pos.y + 0.3)) {
			set_collision(colli, x, y[0]);
			return true;
		}
		break;
	default:
		break;
	}

	switch (track_[y[1]][x]) {
	case ITEM_dropped_banana:
	case ITEM_pending_banana:
	case ITEM_ice_magic:
	case ITEM_rocket_engine:
	case ITEM_tapered_bar:
		if (y[1] == ceil(pos.y - 0.6)) {
			set_collision(colli, x, y[1]);
			return true;
		}
		break;
	case ITEM_rounded_hole:
		if (y[1] == ceil(pos.y - 0.4)) {
			set_collision(colli, x, y[1]);
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

inline void
RaceTrack::set_collision(ColliType& colli, int x, int y) const
{
	colli.item = static_cast<ItemType>(track_[y][x]);
	colli.x    = x;
	colli.y    = y;
	if ( edible(colli.item) ) {
		track_[y][x] = 0;
	}
}

#endif // RACETRACK_HPP_
