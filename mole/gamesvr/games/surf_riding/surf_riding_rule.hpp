#ifndef SURF_RIDING_RULE_HPP_
#define SURF_RIDING_RULE_HPP_

extern "C" {
#include <libxml/tree.h>

#include <gameserv/game.h>
#include <gameserv/proto.h>
}

#include <map>
#include <set>
#include <vector>

#include <ant/coordinate.hpp>
#include <ant/inet/pdumanip.hpp>
#include <ant/random/random.hpp>

class SurfRidingRule {
public:
	SurfRidingRule(game_group_t* grp);

	int  handle_player_action(sprite_t* p, const uint8_t body[], int len);
	int  handle_player_leave(sprite_t* p);

	void sendmap() const;

	void set_start_time(time_t t);
	//------------- static ---------------------
	static int load_tile_grps();
private:
	struct BarrierType {
		BarrierType(uint8_t bar_type, uint8_t x, uint8_t y)
			: coord(x, y) { type = bar_type; }
		ant::Coordinate2D<uint8_t> coord;
		uint8_t type;
	};
	// typedefs
	typedef std::vector<BarrierType> TileType;
	typedef std::vector<TileType> TileGrpType;
	typedef std::set<uint32_t> PlayerSet;
	typedef std::map<uint32_t, time_t> ArriverMap;

	int  get_rank(const sprite_t* p) const;
	int  handle_finished(sprite_t* p);
	void handle_pending_finished();
	bool is_cheater(const sprite_t* p, uint16_t action, uint32_t x, uint32_t y, uint32_t acttime) const;

	game_group_t*  gamegrp_;
	//time_t         start_tm_;
	uint32_t         start_tm_;

	int nfinishers_;
	std::set<uint32_t> pendings_;
	ArriverMap arrivers_;
	//------------- static ---------------------
	static int load_tile_grp(xmlNodePtr cur_node);
	static std::vector<TileGrpType> tilegrps;

	static const uint16_t sc_arrive_flag = ~0;
};

//
inline SurfRidingRule::
SurfRidingRule(game_group_t* grp)
{
	gamegrp_     = grp;
	nfinishers_  = 0;
}

//
inline int SurfRidingRule::
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
inline void SurfRidingRule::
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
inline void SurfRidingRule::
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
inline bool SurfRidingRule::
is_cheater(const sprite_t* p, uint16_t action, uint32_t x, uint32_t y, uint32_t acttime) const
{
	if ( (action == sc_arrive_flag) && p->score > 10000 ) {
		return true;
	}

	return false;
}

#endif // SURF_RIDING_RULE_HPP_
