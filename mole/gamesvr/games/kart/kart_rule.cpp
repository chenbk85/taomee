#include <cmath>

extern "C" {
#include <gameserv/timer.h>
#include <gameserv/dbproxy.h>

}

#include "kart_rule.hpp"

enum ItemType {
	ITEM_invalid        = 0,
	ITEM_min            = 0,
	ITEM_tapered_bar,
	ITEM_rounded_hole,
	ITEM_pending_banana,
	ITEM_dropped_banana,
	ITEM_ice_magic,
	ITEM_rocket_engine,
	ITEM_naughty_pet,

	ITEM_max            = 18,
	ITEM_rate           = 40,

	ITEM_track_border   = 0xFF
};

//--------------------------
//	public methods
//
/**
 * handle_player_action -
 *
 * return: 0 if everything ok, -1 to close `p`, GER_end_of_game to end the whole game
 */
int KartRule::
handle_player_action(sprite_t* p, const uint8_t body[], int len)
{
	if ( started_check(p, len, 29) == -1 ) {
		return -1;
	}

	int i = 0;
	uint32_t x, y, action, acttime;
	ant::unpack(body, x, i);
	ant::unpack(body, y, i);
	ant::unpack(body, action, i);
	i += 9;
	ant::unpack(body, acttime, i);

	// judge if gamecheater is used
	if ( is_cheater(p, action, x, y, acttime) ) {
		ERROR_RETURN(("%s GameCheater: uid=%u grpid=%lu", gamegrp_->game->name, p->id, gamegrp_->id), -1);
	}
	// update last active time
	p->last_act_time = acttime;
	// player finished game
	if ( action == arrive_flag ) {
		DEBUG_LOG("%u Arrived at %s Termination: time=%lu", p->id, gamegrp_->game->name, p->last_act_time);
		if ( handle_finished(p) == -1 ) {
			return -1;
		}
		REMOVE_TIMERS(p);
	}

	if ( pendings_.size() ) {
		handle_pending_finished();
	} else if ( nfinishers_ == gamegrp_->game->players ) {
		return GER_end_of_game;
	}

	return 0;
}

/**
 * sendmap - generate a surf riding map and send it to all the clients
 *
 */
void KartRule::sendmap() const
{
	struct KartMapPkg {
		protocol_t	header;
		uint16_t	tracklen;
		uint8_t 	track[max_tracks_num][max_track_len];
	} __attribute__((__packed__));

	static char  buf[4096];

	static const uint8_t
	items[][ITEM_max] = {
							{
								ITEM_naughty_pet,
								ITEM_tapered_bar,
								ITEM_rounded_hole,
								ITEM_pending_banana,
								ITEM_tapered_bar,
								ITEM_ice_magic,
								ITEM_pending_banana,
								ITEM_rounded_hole,
								ITEM_naughty_pet,
								ITEM_pending_banana,
								ITEM_rocket_engine,
								ITEM_tapered_bar,
								ITEM_rocket_engine,
								ITEM_rounded_hole,
								ITEM_tapered_bar,
								ITEM_naughty_pet,
								ITEM_pending_banana,
								ITEM_tapered_bar
							},
							{
								ITEM_naughty_pet,
								ITEM_tapered_bar,
								ITEM_rounded_hole,
								ITEM_dropped_banana,
								ITEM_tapered_bar,
								ITEM_ice_magic,
								ITEM_dropped_banana,
								ITEM_rounded_hole,
								ITEM_naughty_pet,
								ITEM_dropped_banana,
								ITEM_rocket_engine,
								ITEM_tapered_bar,
								ITEM_rocket_engine,
								ITEM_rounded_hole,
								ITEM_tapered_bar,
								ITEM_naughty_pet,
								ITEM_dropped_banana,
								ITEM_tapered_bar
							}
						};

	memset(buf, 0, sizeof buf);

	KartMapPkg*    map   = reinterpret_cast<KartMapPkg*>(&buf);
	const uint8_t* itms  = items[gamegrp_->count == 1];
	const int upbound    = max_track_len - 10;
	for ( int i = 0; i != max_tracks_num; ++i ) {
		int j = 0;
		while ( (j += ant::ranged_random(14, 22)) < upbound ) {
			int tile = rand() % ITEM_rate;
			if ( tile < ITEM_max ) {
				map->track[i][j] = itms[tile];
			}
		}
	}
	map->tracklen = max_track_len;
	map->tracklen = ant::bswap(map->tracklen);

	init_proto_head(map, proto_game_map_info, sizeof *map);
	send_to_players(gamegrp_, map, sizeof *map);
}

//--------------------------
//	private methods
//
/**
 * handle_finished -
 *
 * return:
 */
int KartRule::handle_finished(sprite_t* p)
{
	static const float rates[] = { 1.0f, 0.67f, 0.34f, 0.17f };

	if ( !pendings_.count(p->id) ) {
		if ( arrivers_.insert(ArriverMap::value_type(p->id, p->last_act_time)).second ) {
			++nfinishers_;
		} else {
			ERROR_RETURN( ("%u Had Already Arrived At Terminal", p->id), -1 );
		}
	}

	int rank = get_rank(p);
	if ( rank == -1 ) {
		pendings_.insert(p->id);
	} else {
		pendings_.erase(p->id);

		const game_t* game = gamegrp_->game;
		game_score_t  gs;
		memset(&gs, 0, sizeof gs);
		gs.rank   = static_cast<uint16_t>(rank + 1);
		gs.exp	  = static_cast<int>(round(game->exp * rates[rank]));
		gs.lovely = static_cast<int>(round(game->lovely * rates[rank]));
		gs.coins  = static_cast<int>(round(game->yxb * rates[rank]));
		gs.time   = (p->last_act_time - start_tm_) * 100 + (rand() % 100);

		if (gamegrp_->count > 1 && rank == 0){		
			uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351380, 1, 1};
			send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, p->id);
		}

//		if ( (gs.time < 12000) && !(rand() % 5) ) {
//				gs.itmid   = 180014;
//				gs.itm_max = 9999;
//		}
		uint32_t gid = 5;

        //gs.itmid = get_fire_medal(p, rank);
		gid |= 0x00010000; // indicates the less the time is, the better
		pack_score_session(p, &gs, gid, gs.time);
		if ( submit_game_score(p, &gs) == -1 ) {
			ERROR_RETURN( ("Failed to Submit Game Score: uid=%u", p->id) , -1 );
		}
	}
	return 0;
}

/**
 * get_rank - get rank of `p`
 *
 * return: 0, 1, 2..., or -1 if rank cannot be determinated yet
 */
int KartRule::get_rank(const sprite_t* p) const
{
	// scan players still running
	for ( uint8_t i = 0; i != gamegrp_->count; ++i ) {
		const sprite_t* op = gamegrp_->players[i];
		if ( op != p ) {
			if ( !arrivers_.count(op->id) && (op->last_act_time < p->last_act_time)
					&& (op->last_act_time != 0) ) {
				// cannot determine rank while some other players are still runing and
				// their last act time is smaller than the one just arrived at terminal
				return -1;
			}
		}
	}

	int rank = 0;
	// scan players in ArriverMap
	for ( ArriverMap::const_iterator it = arrivers_.begin(); it != arrivers_.end(); ++it ) {
		if ( p->last_act_time > it->second ) {
			++rank;
		}
	}
	return rank;
}
