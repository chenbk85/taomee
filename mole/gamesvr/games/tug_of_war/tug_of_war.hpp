#ifndef ANDY_TUG_OF_WAR_HPP_
#define ANDY_TUG_OF_WAR_HPP_

#include <cstdlib>

#include <ant/inet/pdumanip.hpp>
#include <ant/random/random.hpp>

extern "C" {
#include <gameserv/proto.h>
}

#include "../mpog.hpp"

class TugOfWar : public mpog {
public:
	TugOfWar(game_group_t* grp) { gamegrp_ = grp; }

	void init(sprite_t* p);
	int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
			{ return 0; }
	int  handle_timeout(void* data);
private:
	void send_gamescore(game_score_t* gs) const;

	static uint32_t accumulate_strength(game_group_t* grp, int reminder);

	uint8_t winning_team_;  // 0 for players belong to team 2, 1 for players belong to team 1
	game_group_t* gamegrp_;
};

//--------------------------------------------------
//     Public Methods
//
inline int
TugOfWar::handle_timeout(void* data)
{
	game_score_t gs[2];
	memset(&gs, 0, sizeof gs);

	gs[0].coins  = 60;
	gs[0].exp    = 6;
	gs[0].strong = 3;
	gs[1].coins  = 20;
	gs[1].exp    = 1;
	gs[1].strong = 1;

	send_gamescore(gs);
	return GER_end_of_game;
}

//--------------------------------------------------
//     Private Methods
//
inline void
TugOfWar::send_gamescore(game_score_t* gs) const
{
	for (int i = 0; i != gamegrp_->count; ++i) {
		sprite_t* p   = gamegrp_->players[i];

		int idx = ((p->pos_id % 2) != winning_team_);

		if (!idx) {
			gs[idx].score = ant::ranged_random(5001, 10000);
			//gs[idx].itmid = get_fire_medal(p, 0);
		} else {
			gs[idx].score = ant::ranged_random(1000, 5000);
			//gs[idx].itmid = get_fire_medal(p, 1);
		}
		pack_score_session(p, &(gs[idx]), 29, gs[idx].score);
		submit_game_score(p, &(gs[idx]));
	}
}

//--------------------------------------------------
//     Private Static Methods
//
inline uint32_t
TugOfWar::accumulate_strength(game_group_t* grp, int reminder)
{
	uint32_t sum = 0;
	for (int i = 0; i != grp->count; ++i) {
		sprite_t* p = grp->players[i];
		if ( (p->pos_id % 2) == reminder ) {
			sum += p->strength;
		}
	}

	return sum;
}

#endif // ANDY_TUG_OF_WAR_HPP_
