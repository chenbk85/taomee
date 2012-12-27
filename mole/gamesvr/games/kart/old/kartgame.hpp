#ifndef KARTGAME_HPP_
#define KARTGAME_HPP_

extern "C" {
#include <sys/time.h>

#include <gameserv/game.h>
#include <gameserv/proto.h>
#include <gameserv/timer.h>
#include <gameserv/dbproxy.h>

}

#include <cassert>
#include <cmath>
#include <ctime>

#include <vector>

#include <ant/inet/pdumanip.hpp>

#include "../mpog.hpp"
#include "../racingreadyer.hpp"
#include "cmdid.hpp"
#include "racer.hpp"
#include "racetrack.hpp"

class KartGame : public mpog {
public:
	KartGame(game_group_t* grp);

	void  init(sprite_t* p);
	int   handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
	int   handle_timeout(void* data);

	void  attack(const Racer* attacker, const Racer::SpecialEffect& spec);

	const timeval& last_proc_time() const { return tv_last_proc_; }
private:
	struct ActPkg {
		protocol_t header;
		uint32_t id;
		uint16_t action;
		uint32_t sec, usec;
		uint32_t x, y;
		uint16_t upspd, sidespd;
		uint8_t  sidespddir;
		uint16_t upacc, sideacc;
	} __attribute__((__packed__));

	typedef std::vector<Racer> RacerContainer;

	int  handle_player_action(const uint8_t body[], int len);
	int  handle_player_ready(const uint8_t body[], int len);
	int  handle_player_leave();

	int  process();
	int  process(double tmintv);
//	bool kart_collided(const RaceTrack::PosType& pos1, const RaceTrack::PosType& pos2);

	RacerContainer::iterator find_racer(const sprite_t* p);
	void remove_racer(const sprite_t* p);

	void broadcast_action(RacerContainer::iterator pRacer, uint16_t action) const;
	void broadcast_arriver(const Racer& racer) const;
	void notify_downcounter() const;
	void notify_game_start() const;
	void send_gamescore(const Racer& racer, int rank, double tmplus) const;

	static double max_proc_intv; // maximun time interval between each call of process

	static const time_t c_timeout_l = 15;
	static const time_t c_timeout_s = 8;

	game_group_t*   gamegrp_;
	sprite_t*       cur_sp_;

	RacerContainer  racers_;
	RaceTrack       track_;

	int     narrivers_;
	uint8_t downcnt_;

	timeval tv_start_;
	timeval tv_last_proc_;

	RacingReadyer readyer_;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline
KartGame::KartGame(game_group_t* grp)
	: track_(14, 22), readyer_(grp)
{
	assert(grp);

	gamegrp_   = grp;
	cur_sp_    = 0;
	narrivers_ = 0;
	downcnt_   = 3;

	for (uint8_t i = 0; i != gamegrp_->count; ++i) {
		racers_.push_back(Racer(gamegrp_->players[i], &track_, RaceTrack::start_pos(i, gamegrp_->game->players), this));
	}
}

inline void
KartGame::init(sprite_t* p)
{
	assert(GAME_READY(p->group));

	track_.sendmap(gamegrp_);

//	add_grp_timers(gamegrp_, time(0) + c_timeout_l);
}

inline void
KartGame::attack(const Racer* attacker, const Racer::SpecialEffect& spec)
{
	for (RacerContainer::size_type i = 0; i != racers_.size(); ++i) {
		if ( &(racers_[i]) != attacker ) {
			racers_[i].special_effect(spec);
		}
	}
}

inline int
KartGame::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	cur_sp_ = p;

	int err;

	switch (cmd) {
	case proto_player_leave:
		err = handle_player_leave();
		break;
	case KG_player_ready:
		err = handle_player_ready(body, len);
		if (!err) {
			time_t curtm = time(0);
			if ( readyer_.gamestartable() ) {
				gettimeofday(&tv_start_, 0);
//				mod_grp_timers(gamegrp_, curtm + c_timeout_l);
				for (uint8_t i = 0; i != downcnt_; ++i) {
					ADD_TIMER_EVENT(gamegrp_, on_game_timer_expire, &downcnt_, curtm + i + 1);
				}
				notify_downcounter();

				DEBUG_LOG("%s Started\t[grpid=%lu, nplayers=%d]",
							gamegrp_->game->name, gamegrp_->id, gamegrp_->count);
			} else {
//				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, curtm + c_timeout_l);
			}
		}
		break;
	case KG_player_action:
		err = handle_player_action(body, len);
		break;
	case proto_player_action:
		{
			int i = sizeof(protocol_t);
			ant::pack(pkg, cur_sp_->id, i);
			ant::pack(pkg, body, len, i);
			init_proto_head(pkg, proto_player_action, i);
			send_to_group_except_self(gamegrp_, cur_sp_->id, pkg, i);
			err = 0;
		}
		break;
	default:
		ERROR_RETURN(("%s: Unrecognized Command ID %d from uid=%u",
						gamegrp_->game->name, cmd, p->id), -1);
		break;
	}

	return err;
}

inline int
KartGame::handle_timeout(void* data)
{
	if (--downcnt_) {
		notify_downcounter();
	} else {
		gettimeofday(&tv_start_, 0);
		tv_last_proc_ = tv_start_;
		SET_GAME_START(gamegrp_);
		notify_game_start();
	}

	return 0;
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline int
KartGame::handle_player_ready(const uint8_t body[], int len)
{
	if (ready_check(cur_sp_, len, 0) == -1) {
		return -1;
	}
	if ( !readyer_.add(cur_sp_->id) ) {
		ERROR_RETURN( ("%u Has Already Been In Stand-by Status", cur_sp_->id), -1 );
	}
	return 0;
}

inline int
KartGame::handle_player_leave()
{
	if ( !GAME_STARTED(gamegrp_) ) {
		readyer_.remove(cur_sp_->id);
	}
	remove_racer(cur_sp_);

	if (gamegrp_->count == 1) {
		return GER_end_of_game;
	}
	return 0;
}

inline int
KartGame::handle_player_action(const uint8_t body[], int len)
{
	if (started_check(cur_sp_, len, 2) == -1) {
		return -1;
	}

	RacerContainer::iterator pRacer = find_racer(cur_sp_);
	if (pRacer == racers_.end()) {
		ERROR_RETURN(("%s Racer %u Not Found in Group %lu",
						gamegrp_->game->name, cur_sp_->id, gamegrp_->id), -1);
	}

	if ( !pRacer->has_arrived() ) {
		int err = process();
		if ( err ) { // all racers have arrived at terminal
			return err;
		}

		uint16_t action = ant::bswap( *(reinterpret_cast<const uint16_t*>(body)) );
		if ( !(pRacer->has_arrived()) ) {
			if ( action > 10000 ) {
				action = pRacer->use_item() + 10000;
				if (action == 10000) {
					ERROR_RETURN( ("%s: Invalid Action\t[Action=%d uid=%u]",
									gamegrp_->game->name, action, cur_sp_->id), -1 );
				}
			} else {
				pRacer->chg_direction(action);
			}
			broadcast_action(pRacer, action);

			//mod_expire_time(pRacer->id(), time(0) + c_timeout_s);
			time_t exptm = time(0) + c_timeout_s;
//			MOD_EVENT_EXPIRE_TIME(cur_sp_, on_timer_expire, exptm);
		}
		// arrivers are removed only when they leave game or game over
	}

	return 0;
}

inline int
KartGame::process()
{
	timeval curtime;
	gettimeofday(&curtime, 0);
	const double tm_intv = ant::timediff(curtime, tv_last_proc_);

	if ( tm_intv > 0.0 ) {
		int     err;
		double  tmcnt;
		for ( tmcnt = max_proc_intv; tmcnt < tm_intv; tmcnt += max_proc_intv ) {
			if ( (err = process(max_proc_intv)) ) {
				return err;
			}
			ant::timeadd(tv_last_proc_, max_proc_intv);
		}
		// process the remaining time fragment
		if ( (err = process(tm_intv - tmcnt + max_proc_intv)) ) {
			return err;
		}
		tv_last_proc_ = curtime;
	}

	return 0;
}

inline int
KartGame::process(double tmintv)
{
	// run each kart
	for ( RacerContainer::size_type i = 0; i != racers_.size(); ++i ) {
		if ( !racers_[i].has_arrived() && !racers_[i].run(tmintv) ) {
			// racer arrives at terminal
			broadcast_arriver(racers_[i]);
			send_gamescore(racers_[i], narrivers_++, tmintv);
			//remove_timer_event(racers_[i].id());
//			remove_timers(&(racers_[i].sprite()->timer_list));
		}
	}
	// handle collision between karts
/*	for ( RacerContainer::size_type i = 0; i != racers_.size(); ++i ) {
		if ( !(racers_[i].has_arrived()) ) {
			for ( RacerContainer::size_type j = i + 1; j != racers_.size(); ++j ) {
				if ( !(racers_[j].has_arrived())
						&& kart_collided(racers_[i].pos(), racers_[j].pos()) ) {

DEBUG_LOG("Kart %u & %u Collided At (%f, %f) (%f, %f)",
			racers_[i].id(), racers_[j].id(), racers_[i].pos().x,
			racers_[i].pos().y, racers_[j].pos().x, racers_[j].pos().y);

					// TODO - handle collision - might also need to set unmovable period
					RaceTrack::adjust_pos(racers_[i].pos(), racers_[j].pos());
					racers_[i].upspeed(0.5);
					racers_[i].sidespeed(0.5);
					racers_[j].upspeed(0.5);
					racers_[j].sidespeed(0.5);
				}
			}
		}
	}*/
	// judge if all the karts have arrived at terminal
	for ( RacerContainer::size_type i = 0; i != racers_.size(); ++i ) {
		if ( !(racers_[i].has_arrived()) ) {
			return 0;
		}
	}
	return GER_end_of_game;
}

/*
inline bool
KartGame::kart_collided(const RaceTrack::PosType& pos1, const RaceTrack::PosType& pos2)
{
	RaceTrack::PosType pos(pos1);
	RaceTrack::PosType pos3(pos2.x + 1, pos2.y + 1);
	double adj[8][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1},
							{0, 0.5}, {0.5, 0}, {1, 0.5}, {0.5, 1}};

	for (int i = 0; i != 8; ++i) {
		pos.x = pos1.x + adj[i][0];
		pos.y = pos1.y + adj[i][1];
		if ( (pos.x > pos2.x) && (pos.y > pos2.y)
				&& (pos.x < pos3.x) && (pos.y < pos3.y) ) {
			return true;
		}
	}
	return false;
}*/

inline KartGame::RacerContainer::iterator
KartGame::find_racer(const sprite_t* p)
{
	RacerContainer::iterator it;
	for ( it = racers_.begin(); it != racers_.end(); ++it) {
		if ( (it->sprite() == p) && (it->id() == p->id) ) {
			break;
		}
	}
	return it;
}

inline void
KartGame::remove_racer(const sprite_t* p)
{
	RacerContainer::iterator it = find_racer(p);
	if ( it != racers_.end() ) {
		racers_.erase(it);
	}
}

inline void
KartGame::broadcast_action(RacerContainer::iterator pRacer, uint16_t action) const
{
	ActPkg apkg;
	init_proto_head(&apkg, KG_player_action, sizeof apkg);

	apkg.id          = ant::bswap(pRacer->id());
	apkg.action      = ant::bswap(action);
	apkg.sec         = ant::bswap(static_cast<uint32_t>(tv_last_proc_.tv_sec));
	apkg.usec        = ant::bswap(static_cast<uint32_t>(tv_last_proc_.tv_usec));
	apkg.x           = ant::bswap(static_cast<uint32_t>(pRacer->pos().x * 100));
	apkg.y           = ant::bswap(static_cast<uint32_t>(pRacer->pos().y * 100));
	apkg.upspd       = ant::bswap(static_cast<uint16_t>(pRacer->upspeed() * 100));
	apkg.sidespd     = ant::bswap(static_cast<uint16_t>(pRacer->sidespeed() * 100));
	apkg.sidespddir  = ant::bswap(static_cast<uint8_t>(pRacer->sidespddir()));
	apkg.upacc       = ant::bswap(static_cast<uint16_t>(pRacer->upacc() * 100));
	apkg.sideacc     = ant::bswap(static_cast<uint16_t>(pRacer->sideacc() * 100));

	send_to_players(gamegrp_, &apkg, sizeof apkg);
	// each client sends its action within a max interval of 1 sec,
	// so no need to broadcast others' action here
}

inline void
KartGame::broadcast_arriver(const Racer& racer) const
{
	ActPkg apkg;
	init_proto_head(&apkg, KG_player_action, sizeof apkg);

	apkg.id          = ant::bswap(racer.id());
	apkg.action      = ant::bswap(static_cast<uint16_t>(~(0u)));
	apkg.x           = ant::bswap(static_cast<uint32_t>(racer.pos().x * 100));
	apkg.y           = ant::bswap(static_cast<uint32_t>(racer.pos().y * 100));

	send_to_players(gamegrp_, &apkg, sizeof apkg);
}

inline void
KartGame::notify_downcounter() const
{
	int     i   = sizeof(protocol_t);

	ant::pack(pkg, tv_start_.tv_sec, i);
	ant::pack(pkg, downcnt_, i);
	// init protcol pkg
	init_proto_head(pkg, KG_player_ready, i);
	send_to_group(gamegrp_, pkg, i);
}

inline void
KartGame::notify_game_start() const
{
	int     i   = sizeof(protocol_t);

	ant::pack(pkg, static_cast<uint32_t>(tv_start_.tv_sec), i);
	ant::pack(pkg, static_cast<uint32_t>(tv_start_.tv_usec), i);
	// init protcol pkg
	init_proto_head(pkg, KG_start, i);
	send_to_group(gamegrp_, pkg, i);
}

inline void
KartGame::send_gamescore(const Racer& racer, int rank, double tmplus) const
{
	float rates[] = { 1.0f, 0.67f, 0.34f, 0.17f };

	game_score_t gs;
	memset(&gs, 0, sizeof gs);
	gs.rank   = static_cast<uint16_t>(rank+1);
	gs.exp	  = static_cast<int>(round(gamegrp_->game->exp * rates[rank]));
	gs.lovely = static_cast<int>(round(gamegrp_->game->lovely * rates[rank]));
	gs.coins  = static_cast<int>(round(gamegrp_->game->yxb * rates[rank]));
	gs.time   = static_cast<int>((ant::timediff(tv_last_proc_, tv_start_) + tmplus) * 100);

	if (gamegrp_->count > 1 && rank == 1){		
		uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351380, 1, 1};
		send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, uid);
	}

	if (!rank) {
		gs.itmid = get_medal(racer.sprite());
	}
//--------------------------------------
	uint32_t gid = 5;
	gid |= 0x00010000; // indicates the less the time is, the better

	pack_score_session(racer.sprite(), &gs, gid, gs.time);
	submit_game_score(racer.sprite(), &gs);
}

#endif // KARTGAME_HPP_
