#ifndef GAME_START_NOTIFIER_HPP_
#define GAME_START_NOTIFIER_HPP_

#include <ant/inet/pdumanip.hpp>

extern "C" {
#include <gameserv/game.h>
#include <gameserv/proto.h>
#include <gameserv/timer.h>
}

/**
 * notify_game_start0 - game start notification for common racing game
 *
 */
inline void notify_game_start0(const game_group_t* grp)
{
	int   i = sizeof(protocol_t);
	const timeval* tv = get_now_tv();
	uint8_t  gspkg[sizeof(protocol_t) + sizeof *tv];

	ant::pack(gspkg, tv->tv_sec, i);
	ant::pack(gspkg, tv->tv_usec, i);
	// init protcol pkg
	init_proto_head(gspkg, proto_game_start, i);
	send_to_group(grp, gspkg, i);
}

#endif // GAME_START_NOTIFIER_HPP_
