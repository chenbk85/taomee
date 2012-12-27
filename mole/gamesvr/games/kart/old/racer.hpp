#ifndef RACER_HPP_
#define RACER_HPP_

extern "C" {
#include <stdint.h>   // C99

#include <sys/time.h> // POSIX
}

#include <cassert>
#include <cmath>
#include <cstring>

#include <queue>

#include <ant/time/time.hpp>

#include "racetrack.hpp"

class KartGame;

class Racer {
public:
	struct SpecialEffect {
		SpecialEffect()
			{ memset(this, 0, sizeof *this); }
		SpecialEffect(uint8_t ef, const timeval& tv)
			{ effect_ = ef; endtv_ = tv; }
		uint8_t effect_;
		timeval endtv_;
	};

	// inline ctor
	Racer(sprite_t* p, RaceTrack* racetrack, const RaceTrack::PosType& pos, KartGame* kg);

	// non-inline methods
	void      chg_direction(uint16_t dir);
	ItemType  use_item();
	// inline methods
	bool      run(double tmintv);

	void   special_effect(const SpecialEffect& spec);
	void   upspeed(double rate) { upspeed_ *= rate; }
	void   sidespeed(double rate) { sidespeed_ *= rate; }

	uint32_t  id() const { return p_->id; }
	sprite_t* sprite() const { return p_; }
	double    upspeed() const { return fabs(upspeed_); }
	double    sidespeed() const { return fabs(sidespeed_); }
	int       sidespddir() const { if (sidespeed_ > 0.0) return 1; return 0; }
	double    upacc() const { return fabs(upacc_); }
	double    sideacc() const { return 0.0/*fabs(sideacc_)*/; }
	bool      has_arrived() const { return has_arrived_; }
	const RaceTrack::PosType& pos() const { return pos_; }
	RaceTrack::PosType& pos() { return pos_; }
private:
	typedef double SpeedType;
	enum DirectionType {
		DT_release    = 0,
		DT_up,
		DT_down,
		DT_left,
		DT_right,
		DT_upleft,
		DT_upright,
		DT_downleft,
		DT_downright,

		DT_max
	};

	static const SpeedType maxspeed            =  8.0;
	static const SpeedType sidespd             =  3.0;
	static const SpeedType acceleration        =  2.0;
	static const SpeedType brk_acceleration    = -4.0;
	static const SpeedType rls_acceleration    = -1.0;
	static const SpeedType turn_acceleration   = -1.0;

	// non-inline methods
	// TODO - remove `int dir` later
	void  do_run(double& pos, SpeedType& v0, SpeedType vt, double t, int dir = 1, bool is_up = true);
	// inline methods
	void  uprun(double tmintv);
	void  siderun(double tmintv);
	void  handle_collision(const RaceTrack::ColliType& collitype);

	void  notify_item_got(const RaceTrack::ColliType& collitype) const;
	bool  isvalid(DirectionType dir) const;

	// private fields
	sprite_t*    p_;
	RaceTrack*   racetrack_;
	KartGame*    kg_;

	SpeedType      upspeed_, upacc_;
	SpeedType      sidespeed_/*, sideacc_*/;
	SpeedType      actual_maxspd_; // TODO - to be removed
	DirectionType  last_direction_[2];
	bool           has_arrived_;
	bool           side_stoppable_; // TODO - to be removed
	SpecialEffect  spec_effect_;
	RaceTrack::PosType pos_;

	RaceTrack::ColliType last_colli_;

	typedef std::queue<ItemType> ItemQueue;
	ItemQueue   itemq_;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline
Racer::Racer(sprite_t* p, RaceTrack* racetrack, const RaceTrack::PosType& pos, KartGame* kg)
	: pos_(pos)
{
	assert(p && racetrack && kg);

	p_              = p;
	racetrack_      = racetrack;
	kg_             = kg;

	upspeed_        = 0.0;
	sidespeed_      = 0.0;
	upacc_          = 0.0;
//	sideacc_        = 0.0;
	last_direction_[0] = DT_release;
	last_direction_[1] = DT_release;
	has_arrived_    = false;
	side_stoppable_ = true;
}

inline bool
Racer::run(double tmintv)
{
	assert(!has_arrived_);

	// up run
	uprun(tmintv);
	// side run
	siderun(tmintv);

	RaceTrack::ColliType colli;
	if ( racetrack_->terminal_arrived(pos_) ) {
		return !(has_arrived_ = true);
	} else if ( racetrack_->chk_collision(pos_, colli)
				&& (last_colli_ != colli) ) {
		handle_collision(colli);

//DEBUG_LOG("1: %u Collision %d At (%f, %f)", p_->id, colli.item, pos_.y, pos_.x);

		if ( colli.item == ITEM_track_border ) { // collided at border
			if ( racetrack_->chk_collision(pos_, colli)
					&& (last_colli_!= colli) ) {
				handle_collision(colli);
				if ( !racetrack_->edible(colli.item) ) {
					last_colli_ = colli;
				}
			}

//DEBUG_LOG("2: %u Collision %d At (%f, %f)", p_->id, colli.item, pos_.y, pos_.x);

		} else if ( !racetrack_->edible(colli.item) ) {
			last_colli_ = colli;
		}
	}

//DEBUG_LOG("id=%u  UpSpd=%f   SideSpd=%f   UpAcc=%f   SideAcc=%f   "
//		  "Pos=%f, %f\n", p_->id, upspeed_, sidespeed_, upacc_, 0.0/*sideacc_*/, pos_.y, pos_.x);

	return true;
}

inline void
Racer::special_effect(const SpecialEffect& spec)
{
	spec_effect_ = spec;
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline void
Racer::uprun(double tmintv)
{
	SpeedType& v0	  = upspeed_;
	SpeedType  vt	  = v0 + upacc_ * tmintv;
//	actual_maxspd_	  = ((side_stoppable_ && (upacc_ > -1.1)) ? maxspeed : maxspeed * 0.8);
	actual_maxspd_	  = maxspeed;

	if (vt > actual_maxspd_) {
		vt = actual_maxspd_;
	} else if (vt < 0.0) {
		vt = 0.0;
	}
	do_run(pos_.x, v0, vt, tmintv);
}

inline void
Racer::siderun(double tmintv)
{
	SpeedType& v0   = sidespeed_;
	SpeedType  vt   = v0;
	actual_maxspd_  = sidespd;
//	SpeedType  vt   = v0 + sideacc_ * tmintv;
//	actual_maxspd_  = maxspeed * 0.5;

	if (v0 > 0.0) {
		do_run(pos_.y, v0, vt, tmintv, 1, false);
	} else if (v0 < 0.0) {
		do_run(pos_.y, v0, vt, tmintv, -1, false);
	}

/*	if ( side_stoppable_ ) { // stoppable side run
		if ( v0 < 0.0 ) {
			if ( vt > 0.0 ) {
				vt = 0.0;
			}
			do_run(pos_.y, v0, vt, tmintv, -1);
		} else if ( v0 > 0.0 ) {
			if ( vt < 0.0 ) {
				vt = 0.0;
			}
			do_run(pos_.y, v0, vt, tmintv, 1);
		}
	} else { // unstoppable side run
		if ( vt > actual_maxspd_ ) {
			vt = actual_maxspd_;
		} else if (vt < -actual_maxspd_) {
			vt = -actual_maxspd_;
		}

		static const int dir[] = { -1, 1 };
		do_run(pos_.y, v0, vt, tmintv, dir[sideacc_ > 0.0]);
	}*/
}

inline void
Racer::handle_collision(const RaceTrack::ColliType& collitype)
{
	switch (collitype.item) {
	case ITEM_tapered_bar: // taper
		upspeed_   *= 0.2;
//		sidespeed_ *= 0.5;
		break;
	case ITEM_rounded_hole:	// hole
		upspeed_   *= 0.2;
//		sidespeed_ *= 0.5;
		break;
	case ITEM_pending_banana: // pending banana
		itemq_.push(ITEM_pending_banana);
		break;
	case ITEM_dropped_banana: // dropped banana
		upspeed_   *= 0.2;
//		sidespeed_ *= 0.5;
		break;
	case ITEM_ice_magic: // ice magic
		itemq_.push(ITEM_ice_magic);
		break;
	case ITEM_rocket_engine: // rocket engine
		itemq_.push(ITEM_rocket_engine);
		break;
	case ITEM_track_border:
//		upspeed_   *= 0.5;
//		sidespeed_  = 0.0;
		RaceTrack::adjust_pos(pos_);
		return;
//		break;
	default: // this should never happen normally
		ERROR_LOG("%s: Impossible Collision %d",
					p_->group->game->name, collitype.item);
		return;
	}

	if ( (spec_effect_.effect_ != ITEM_rocket_engine) || racetrack_->edible(collitype.item) ) {
		notify_item_got(collitype);
	}
}

inline bool
Racer::isvalid(DirectionType dir) const
{
	return (dir < DT_max);
}

inline void
Racer::notify_item_got(const RaceTrack::ColliType& collitype) const
{
	uint8_t pkg[sizeof(protocol_t) + 9];

	int i = sizeof(protocol_t);
	ant::pack(pkg, p_->id, i);
	ant::pack(pkg, static_cast<uint8_t>(((spec_effect_.effect_ == ITEM_rocket_engine) && (collitype.item == ITEM_dropped_banana)) ? 0 : collitype.item), i);
	ant::pack(pkg, collitype.x, i);
	ant::pack(pkg, collitype.y, i);
	init_proto_head(pkg, KG_item_got, i);
	send_to_players(p_->group, pkg, i);
}

#endif // RACER_HPP_
