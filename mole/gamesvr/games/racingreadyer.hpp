#ifndef RACINGREADYER_HPP_
#define RACINGREADYER_HPP_

extern "C" {
#include <stdint.h>

#include <gameserv/game.h>
}

#include <cassert>

#include <set>
#include <utility>

class RacingReadyer {
public:
	RacingReadyer(game_group_t* grp);

	bool add(uint32_t uid);
	void remove(uint32_t uid);

	bool gamestartable();
private:
	typedef std::set<uint32_t> PlayerSet;

	PlayerSet      players_;
	game_group_t*  grp_;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline RacingReadyer::RacingReadyer(game_group_t* grp)
{
	assert(grp);
	grp_ = grp;
}

inline bool
RacingReadyer::add(uint32_t uid)
{
	std::pair<PlayerSet::iterator, bool> ret = players_.insert(uid);
	return ret.second;
}

inline void
RacingReadyer::remove(uint32_t uid)
{
	players_.erase(uid);
}

inline bool RacingReadyer::gamestartable()
{
	if ( players_.size() == grp_->count ) {
		return true;
	}
	return false;
}

#endif // RACINGREADYER_HPP_
