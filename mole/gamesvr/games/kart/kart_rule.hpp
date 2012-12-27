#ifndef SURF_RIDING_RULE_HPP_
#define SURF_RIDING_RULE_HPP_

extern "C" {
#include <gameserv/game.h>
#include <gameserv/proto.h>
}

#include <map>
#include <set>
#include <vector>

#include <ant/coordinate.hpp>
#include <ant/inet/pdumanip.hpp>
#include <ant/random/random.hpp>

class KartRule {
public:
	KartRule(game_group_t* grp);

	int  handle_player_action(sprite_t* p, const uint8_t body[], int len);
	int  handle_player_leave(sprite_t* p);

	void sendmap() const;

	void set_start_time(time_t t);
private:
	// typedefs
	typedef std::set<uint32_t> PlayerSet;
	typedef std::map<uint32_t, time_t> ArriverMap;

	int  get_rank(const sprite_t* p) const;
	int  handle_finished(sprite_t* p);
	void handle_pending_finished();
	bool is_cheater(const sprite_t* p, uint32_t action, uint32_t x, uint32_t y, uint32_t acttime) const;

	game_group_t*  gamegrp_;
	//time_t         start_tm_;
	uint32_t         start_tm_;

	int nfinishers_;
	std::set<uint32_t> pendings_;
	ArriverMap arrivers_;

	//------------- static ---------------------
	static const int       max_tracks_num  = 5;
	static const int       max_track_len   = 800;
	static const uint32_t  arrive_flag     = ~0;
};

//
inline KartRule::
KartRule(game_group_t* grp)
{
	gamegrp_     = grp;
	nfinishers_  = 0;
}

//
inline int KartRule::
handle_player_leave(sprite_t* p)
{
	p->last_act_time = 0;
	//
	pendings_.erase(p->id);
	//
	if ( pendings_.size() ) {
		handle_pending_finished();
	}
	//
	if ( arrivers_.count(p->id) == 0 ) {
		++nfinishers_;
	}
	//
	if ( gamegrp_->count == 1 ) {
		return GER_end_of_game;
	}

	return 0;
}

//
inline void KartRule::
set_start_time(time_t t)
{
	start_tm_ = t;
}

//----------------------------
//	private methods
//
/**
 * handle_pending_finished -
 *
 */
inline void KartRule::
handle_pending_finished()
{
	assert(GAME_STARTED(gamegrp_));

	sprite_t* p;
	PlayerSet tmp(pendings_);
	for ( PlayerSet::iterator it = tmp.begin();	it != tmp.end(); ++it ) {
		if ( (p = get_sprite_from_gamegrp(*it, gamegrp_)) ) {
			handle_finished(p);
		} else {
			pendings_.erase(*it);
		}
	}
}
/**
 * is_cheater - judge if `p` is using a game cheater
 *
 * return: true if so, otherwise false
 */
inline bool KartRule::
is_cheater(const sprite_t* p, uint32_t action, uint32_t x, uint32_t y, uint32_t acttime) const
{
	time_t tdiff = acttime - start_tm_;
	DEBUG_LOG("is_cheater action %u, arrive_flag %u, tdiff %lu", action, arrive_flag, tdiff);
	return ((action == arrive_flag) && tdiff < 40);
}

#endif // SURF_RIDING_RULE_HPP_
