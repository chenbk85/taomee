extern "C" {
#include <gameserv/timer.h>
}

#include "tug_of_war.hpp"

//--------------------------------------------------
//     Public Methods
//
void TugOfWar::init(sprite_t* p)
{
	if ( IS_GAME_PLAYER(p) ) {
		uint32_t sum1 = accumulate_strength(gamegrp_, 0);
		uint32_t sum2 = accumulate_strength(gamegrp_, 1);

		if ( sum1 > sum2 ) {
			winning_team_ = 0;
		} else if (sum1 < sum2) {
			winning_team_ = 1;
		} else {
			winning_team_ = std::rand() % 2;
		}

		int i = sizeof(protocol_t);
		ant::pack(pkg, winning_team_, i);

		init_proto_head(pkg, proto_notify_game_winner, i);
		send_to_players(gamegrp_, pkg, i);

		ADD_TIMER_EVENT(gamegrp_, on_game_timer_expire, (void*)1, get_now_tv()->tv_sec + (rand() % 31) + 20);
	}
}
