#include "racer.hpp"
#include "kartgame.hpp"

const Racer::SpeedType Racer::maxspeed;
const Racer::SpeedType Racer::sidespd;
const Racer::SpeedType Racer::acceleration;
const Racer::SpeedType Racer::brk_acceleration;
const Racer::SpeedType Racer::rls_acceleration;
const Racer::SpeedType Racer::turn_acceleration;

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
void Racer::chg_direction(uint16_t dir)
{
	last_direction_[1] = last_direction_[0];
	DirectionType newdir = static_cast<DirectionType>(dir);
	if ( isvalid(newdir) ) {
		last_direction_[0] = newdir;
	} else {
		ERROR_LOG("%s: Invalid Direction\t[Action=%d uid=%u]",
						p_->group->game->name, dir, p_->id);
	}

	if ( last_direction_[0] != last_direction_[1] ) {
//		sideacc_ = 0.0;
		switch (last_direction_[0]) {
		case DT_down:
			upacc_ = brk_acceleration;
/*			if (sidespeed_ > 0.0) {
				sideacc_ = brk_acceleration;
			} else if (sidespeed_ < 0.0) {
				sideacc_ = -brk_acceleration;
			}*/
			sidespeed_ = 0;
			side_stoppable_ = true;
			break;
		case DT_left:
			upspeed_   *=  0.5;
//			sidespeed_	= -upspeed_;
			sidespeed_	= -sidespd;
			upacc_		= rls_acceleration + turn_acceleration;
//			sideacc_	= -(rls_acceleration + turn_acceleration);
			side_stoppable_ = true;
			break;
		case DT_right:
			upspeed_   *= 0.5;
//			sidespeed_	= upspeed_;
			sidespeed_	= sidespd;
			upacc_		= rls_acceleration + turn_acceleration;
//			sideacc_	= rls_acceleration + turn_acceleration;
			side_stoppable_ = true;
			break;
		case DT_up:
			upacc_ = acceleration;
/*			if (sidespeed_ > 0.0) {
				sideacc_ = rls_acceleration;
			} else if (sidespeed_ < 0.0) {
				sideacc_ = -rls_acceleration;
			}*/
			sidespeed_ = 0;
			side_stoppable_ = true;
			break;
		case DT_upleft:
//			sidespeed_	= -upspeed_ * 0.5;
			sidespeed_	= -sidespd;
//			upspeed_   *=  0.8;
			upacc_		=  acceleration/* * 0.5*/;
//			sideacc_	= -acceleration * 0.5;
			side_stoppable_ = false;
			break;
		case DT_upright:
//			sidespeed_	= upspeed_ * 0.5;
			sidespeed_	= sidespd;
//			upspeed_   *= 0.8;
			upacc_		= acceleration/* * 0.5*/;
//			sideacc_	= acceleration * 0.5;
			side_stoppable_ = false;
			break;
		case DT_downleft:
			upspeed_   *=  0.5;
//			sidespeed_	= -upspeed_;
			sidespeed_	= -sidespd;
			upacc_		= brk_acceleration + turn_acceleration;
//			sideacc_	= -(brk_acceleration + turn_acceleration);
			side_stoppable_ = true;
			break;
		case DT_downright:
			upspeed_   *= 0.5;
			sidespeed_	= upspeed_;
			sidespeed_	= sidespd;
			upacc_		= brk_acceleration + turn_acceleration;
//			sideacc_	= brk_acceleration + turn_acceleration;
			side_stoppable_ = true;
			break;
		case DT_release:
			upacc_	 = rls_acceleration;
/*			if (sidespeed_ > 0.0) {
				sideacc_ = rls_acceleration;
			} else if (sidespeed_ < 0.0) {
				sideacc_ = -rls_acceleration;
			}*/
			sidespeed_ = 0;
			side_stoppable_ = true;
			break;
		default: // this should never happen normally
			ERROR_LOG("Impossible Direction %d: uid=%u", dir, p_->id);
			break;
		}
	}

//DEBUG_LOG("uid=%u  Dir=%d  UpSpd=%f  SideSpd=%f  UpAcc=%f  SideAcc=%f  "
//		  "Pos=%f, %f\n", p_->id, newdir, upspeed_, sidespeed_, upacc_, 0.0/*sideacc_*/, pos_.y, pos_.x);
}

ItemType Racer::use_item()
{
	if ( !itemq_.empty() ) {
		ItemType itmtype = itemq_.front();
		switch (itmtype) {
		case ITEM_pending_banana:
			racetrack_->place_barrier(p_, pos_, ITEM_dropped_banana);
			break;
		case ITEM_ice_magic:
			{
				timeval tv = kg_->last_proc_time();
				tv.tv_sec += 5;
				SpecialEffect spec(ITEM_ice_magic, tv);
				kg_->attack(this, spec);
			}
			break;
		case ITEM_rocket_engine:
			spec_effect_.effect_ = ITEM_rocket_engine;
			spec_effect_.endtv_  = kg_->last_proc_time();
			spec_effect_.endtv_.tv_sec += 5;
			break;
		default: // this should never happen normally
			ERROR_LOG("%s: Unexpected Item Type %d!", p_->group->game->name, itmtype);
			break;
		}
		itemq_.pop();
		return itmtype;
	}
	return ITEM_invalid;
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
void Racer::do_run(double& pos, SpeedType& v0, SpeedType vt, double t, int dir, bool is_up)
{
// TODO - might need to add collision & banana effect
	if (spec_effect_.effect_ != 0) {
		if ( ant::timecmp(kg_->last_proc_time(), spec_effect_.endtv_) > 0 ) {
			memset(&spec_effect_, 0, sizeof spec_effect_);
		} else {
			switch (spec_effect_.effect_) {
			case ITEM_ice_magic: // ice
				{
					//SpeedType maxspd = actual_maxspd_ * 0.5;
					SpeedType maxspd = (is_up ? 2.0 : 1.0);
					if (vt > maxspd) {
						v0 = vt = maxspd;
					} else if (vt < -maxspd) {
						v0 = vt = -maxspd;
					}
				}
				break;
			case ITEM_rocket_engine: // rocket
				//v0 = vt = actual_maxspd_ * 1.5 * dir;
				//v0 = vt = (is_up ? (actual_maxspd_ * 1.5 * dir) : (actual_maxspd_ * dir));
				if (is_up) {
					v0 = vt = 12.0;
				}
				break;
			default: // this should never happen normally
				ERROR_LOG("%s: Impossible Effect %d",
							p_->group->game->name, spec_effect_.effect_);
				break;
			}
		}
	}

	if ( (v0 > vt) || (v0 < vt) || (v0 > 0.0) || (v0 < 0.0) ) {
		pos += (v0 + vt) * t / 2;
		v0	 = vt;
	}
}
