#include <cstring>
using namespace std;

extern "C" {
#include <ant/utils.h>
}

#include <cmath>

#include "sledgame.hpp"

int Sledgame::s_max_interv = 8; // default minimum interval of 2 barries
int Sledgame::s_min_interv = 4; // default maximum interval of 2 barries

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
int	Sledgame::handle_player_action(sprite_t* p, const uint8_t body[], int len)
{
	assert(p && p->group);
	if ( started_check(p, len, 28) == -1 ) return -1;

	uint32_t uid = ntohl( *(reinterpret_cast<const uint32_t*>(body)) );
	if ( p->id != uid )
		ERROR_RETURN( ("UserID Mismatch, uid=%u, %u", p->id, uid), -1 );

	const int pkgsize = sizeof(protocol_t) + 28;
	uint8_t pkg[pkgsize];
	// broadcast player action
	init_proto_head(pkg, player_action, pkgsize);
	memcpy(pkg + sizeof(protocol_t), body, len);
	send_to_group_except_self(m_gamegrp, p->id, pkg, pkgsize);

	// unpkg some info to determine cheater
	CheatPkg cp;
	int i = 4;
	UNPKG_UINT(body, cp.pix.x, i);
	UNPKG_UINT(body, cp.pix.y, i);
	UNPKG_UINT(body, cp.action, i);
	i = 20;
	UNPKG_UINT(body, cp.acttime, i);
	if ( is_cheater(p, cp) ) {
		ERROR_RETURN(("GameCheater: uid=%u, grpid=%lu", p->id, m_gamegrp->id), -1);
	}

	if ( (cp.action < 400) || (cp.action == ~(0u)) ) {
		p->last_act_time = cp.acttime;
	}
	// player finished game
	if ( cp.action == ~(0u) ) {
		DEBUG_LOG("%u Arrived at Termination: time=%lu", p->id, p->last_act_time);
		if ( handle_finished(p) == -1 ) return -1;
		remove_timers(&(p->timer_list));
	}

	if ( npending() ) {
		handle_pending_finished();
	} else {
		if ( m_nfinishers == m_gamegrp->game->players ) return GER_end_of_game;
	}

	if ( cp.action != ~(0u) ) {
		time_t exptm = time(0) + c_timeout_s;
		MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, exptm);
	}

	return 0;
}

int Sledgame::get_rank(const sprite_t* p) const
{
	// scan players still running
	for ( uint8_t i = 0; i != m_gamegrp->count; ++i ) {
		const sprite_t* op = m_gamegrp->players[i];
		if ( op != p ) {
			if ( !is_arriver(op->id) && (op->last_act_time < p->last_act_time)
					&& (op->last_act_time != 0) ) {
				// cannot determine rank while some other players are still runing and
				// their last act time is smaller than the one just arrived at terminal
				return -1;
			}
		}
	}

	int rank = 0;
	// scan players in ArriverMap
	for ( ArriverMap::const_iterator it = m_arrivers.begin();
			it != m_arrivers.end(); ++it ) {
		if ( p->last_act_time > it->second ) {
			++rank;
		}
	}
	return rank;
}

int Sledgame::handle_finished(sprite_t* p)
{
	static const float rates[] = { 1.0f, 0.5f, 0.18f };

	if ( !is_pending(p->id) ) {
		if ( add_arriver(p->id, p->last_act_time) ) ++m_nfinishers;
		else ERROR_RETURN( ("%u Had Already Arrived At Terminal", p->id), -1 );
	}

	int rank = get_rank(p);
	if ( rank == -1 ) {
		if ( add_pending(p->id) ) {
			TRACE_LOG("%u Pending Finished. Pending Count: %u", p->id, npending());
		}
	} else {
		del_pending(p->id);

		game_score_t gs;
		memset(&gs, 0, sizeof gs);
		gs.rank   = static_cast<uint16_t>(rank + 1);
		gs.exp    = static_cast<int>(round(m_gamegrp->game->exp * rates[rank]));
		gs.strong = static_cast<int>(round(m_gamegrp->game->strong * rates[rank]));
		gs.coins  = static_cast<int>(round(m_gamegrp->game->yxb * rates[rank]));
		gs.time   = (p->last_act_time - m_start_tm) * 100 + (rand() % 100);
		// VIP double output
		if (is_item_on_body(p, 12299)) {
			gs.coins  *= 2;
			gs.exp    *= 2;
		}

        //gs.itmid = get_fire_medal(p, rank);
/*
		if (IS_ENABLE_YUANBAO(p) && p->group->game->players != 1 && rank == 0) {
			//multi game and be winner can get yuanbao
			gs.itmid = get_yuanbao(p);
		}
		*/
		uint32_t gid = 2;
		gid |= 0x00010000; // indicates the less the time is, the better
		pack_score_session(p, &gs, gid, gs.time);
		if ( submit_game_score(p, &gs) == -1 ) return -1;
	}
	return 0;
}

bool Sledgame::is_cheater(const sprite_t* p, const CheatPkg& cp)
{
	if ( (cp.action > 400) && (cp.action != ~(0u)) ) return false;

	int tdiff = cp.acttime - m_start_tm;
	// imposible to run more than 300 seconds, impossible to arrive at terminal in 42 seconds
	if ( (tdiff > 300) || ((cp.action == ~(0u)) && (tdiff < 42)) ) {
        ERROR_LOG("SLED %u is cheater, tdiff %d, cp.action %u", p->id, tdiff, cp.action);
        return true;
    }

	time_t t = time(0);
	tdiff = cp.acttime - p->last_act_time;

	if ( (tdiff < 0) || (tdiff > 5)	|| ((t - cp.acttime) > 10) ) {
        ERROR_LOG("SLED %u is cheater, acttime %u, last_act_time %lu, t %lu", p->id, cp.acttime, p->last_act_time, t);
        return true;
    }

	uint32_t pdiff = cp.pix.x - m_lastpos[p->id].x;

	if ( pdiff > 2000 ) {
        ERROR_LOG("SLED %u is cheater, pdiff %u", p->id, pdiff);
        return true;
    }


	if ( tdiff > 0 ) m_lastpos[p->id].x = cp.pix.x;

	return false;
}

/*---------------------------------------------------------
  *			global methods
  *---------------------------------------------------------*/
extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) Sledgame(grp);
}

extern "C" int game_init()
{
	return 0;
}

extern "C" void game_destroy()
{
}
