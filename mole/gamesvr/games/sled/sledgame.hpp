#ifndef SLED_HPP_
#define SLED_HPP_

#include <cassert>
#include <ctime>

#include <map>
#include <set>
#include <utility>

extern "C" {
#include <sys/time.h>

#include <gameserv/game.h>
#include <libtaomee/log.h>
#include <gameserv/proto.h>
#include <gameserv/timer.h>
}

#include "../mpog.hpp"
#include "sledmap.hpp"

class Sledgame : public mpog {
public:
	Sledgame(game_group_t* gamegrp);

	void init(sprite_t *p);
	int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
private:
	typedef std::set<uint32_t> PlayerSet;
	typedef std::map<uint32_t, time_t> ArriverMap;
	struct PosType {
		uint32_t x, y; // in pixel
	};
	typedef std::map<uint32_t, PosType> SpritePosMap;
	struct CheatPkg {
		uint32_t action;
		PosType  pix;
		uint32_t   acttime;
	};

	bool make_ready(uint32_t uid);
	PlayerSet::size_type nready() const
		{ assert(m_gamegrp && !GAME_STARTED(m_gamegrp)); return m_players.size(); }
	void del_ready(uint32_t uid)
		{ assert(m_gamegrp && !GAME_STARTED(m_gamegrp)); m_players.erase(uid); }

	bool add_pending(uint32_t uid);
	void handle_pending_finished();
	void del_pending(uint32_t uid)
		{ assert(m_gamegrp && GAME_STARTED(m_gamegrp)); m_players.erase(uid); }
	PlayerSet::size_type npending() const
		{ assert(m_gamegrp && GAME_STARTED(m_gamegrp)); return m_players.size(); }
	bool is_pending(uint32_t uid) const
		{ assert(m_gamegrp && GAME_STARTED(m_gamegrp)); return m_players.count(uid); }

	bool add_arriver(uint32_t uid, time_t t);
	bool is_arriver(uint32_t uid) const
		{ assert(m_gamegrp && GAME_STARTED(m_gamegrp)); return m_arrivers.count(uid); }

	void add_timers(const time_t expiretime);
	void mod_timers(const time_t expiretime);

	void send_sledmap() const;
	bool game_startable() const;
	void notify_game_start();

	int handle_client_ready(sprite_t* sp, const uint8_t body[], int len);
	int handle_player_leave(sprite_t* p);
	int handle_player_action(sprite_t* p, const uint8_t body[], int len);

	int get_rank(const sprite_t* p) const;
	int handle_finished(sprite_t* p);
	bool is_cheater(const sprite_t* p, const CheatPkg& cp);

	game_group_t*	m_gamegrp;
	uint32_t		m_nfinishers;
	PlayerSet		m_players;  // means ready to play if GAME_READY or pending finished if GAME_STARTED
	ArriverMap		m_arrivers; // players who have arrived at terminal
	SpritePosMap	m_lastpos;

	//time_t		m_start_tm; // time at which the game starts
	uint32_t		m_start_tm;

	static int  s_min_interv; // minimum interval of 2 barries
	static int  s_max_interv; // maximum interval of 2 barries

	static const time_t c_timeout_s = 8;   // short timeout
	static const time_t c_timeout_l = 15;  // long timeout
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline Sledgame::Sledgame(game_group_t* gamegrp)
{
	assert(gamegrp);
	m_gamegrp = gamegrp;
	m_nfinishers = 0;
}

inline void Sledgame::init(sprite_t* p)
{
	assert(GAME_READY(p->group));

	send_sledmap();

	time_t exprtm = time(0) + c_timeout_l;
	add_timers(exprtm);
}

inline int
Sledgame::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	assert(GAME_READY(m_gamegrp));

	int err;
	switch (cmd) {
	case proto_player_leave:
		err = handle_player_leave(p);
		break;
	case client_ready:
		err = handle_client_ready(p, body, len);
		if (!err) {
			time_t exprtm = time(0) + Sledgame::c_timeout_l;
			if ( game_startable() ) {
				SET_GAME_START(p->group);
				notify_game_start();

				mod_timers(exprtm);

				DEBUG_LOG("Sled Started\t[grpid=%lu, nplayers=%d]",
							p->group->id, p->group->count);
			} else {
				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, exprtm);
			}
		}
		break;
	case player_action:
		err = handle_player_action(p, body, len);
		break;
	default:
		ERROR_RETURN(("Sled: Unrecognized Command ID %d from uid=%u", cmd, p->id), -1);
		break;
	}

	return err;
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline bool Sledgame::add_arriver(uint32_t uid, time_t t)
{
	assert(m_gamegrp && GAME_STARTED(m_gamegrp));

	std::pair<ArriverMap::iterator, bool> ret
		= m_arrivers.insert(ArriverMap::value_type(uid, t));
	return ret.second;
}

inline bool Sledgame::add_pending(uint32_t uid)
{
	assert(m_gamegrp && GAME_STARTED(m_gamegrp));

	std::pair<PlayerSet::iterator, bool> ret = m_players.insert(uid);
	return ret.second;
}

inline void Sledgame::add_timers(const time_t expiretime)
{
	// add timeout events
	for ( uint8_t i = 0; i != m_gamegrp->count; ++i ) {
		ADD_TIMER_EVENT(m_gamegrp->players[i], on_timer_expire, 0, expiretime);
	}
}

inline void Sledgame::mod_timers(const time_t expiretime)
{
	for ( uint8_t i = 0; i != m_gamegrp->count; ++i ) {
		MOD_EVENT_EXPIRE_TIME(m_gamegrp->players[i], on_timer_expire, expiretime);
	}
}

inline void Sledgame::handle_pending_finished()
{
	assert(m_gamegrp && GAME_STARTED(m_gamegrp) );

	sprite_t* p;
	PlayerSet tmp(m_players);
	for ( PlayerSet::iterator it = tmp.begin();	it != tmp.end(); ++it ) {
		if ( (p = get_sprite_from_gamegrp(*it, m_gamegrp)) ) {
			handle_finished(p);
		} else {
			del_pending(*it);
		}
	}

	TRACE_LOG("Sled Handle Pending Fin: pending=%u, grpid=%lu", npending(), m_gamegrp->id);
}

inline bool Sledgame::make_ready(uint32_t uid)
{
	std::pair<PlayerSet::iterator, bool> ret = m_players.insert(uid);
	return ret.second;
}

inline void Sledgame::send_sledmap() const
{
	Sled_map sledmap(m_gamegrp);
	sledmap.generate_barries(s_min_interv, s_max_interv);
	sledmap.send();
}

inline bool Sledgame::game_startable() const
{
	if ( nready() == m_gamegrp->count )
		return true;

	return false;
}

inline int Sledgame::handle_client_ready(sprite_t* sp, const uint8_t body[], int len)
{
	CHECK_BODY_LEN(len, 0);
	if ( GAME_STARTED(sp->group) || IS_GAME_WATCHER(sp) )
		ERROR_RETURN( ("GameStarted=%d, GameWatcher=%d, uid=%u", GAME_STARTED(sp->group), IS_GAME_WATCHER(sp), sp->id), -1 );

	if ( !make_ready(sp->id) )
		ERROR_RETURN( ("%u Has Already Been In Stand-by Status", sp->id), -1 );

	return 0;
}

inline int Sledgame::handle_player_leave(sprite_t* p)
{
	assert(m_gamegrp);
	TRACE_LOG("Sled Handle Player Leave: grpid=%lu", m_gamegrp->id);

	uint32_t uid = p->id;
	// del pending finished if GAME_STARTED or del ready if GAME_READY
	if ( GAME_STARTED(m_gamegrp) ) {
		del_pending(uid);
	} else {
		del_ready(uid);
	}

	p->last_act_time = 0;

	if ( GAME_STARTED(m_gamegrp) && npending() ) handle_pending_finished();

	if ( m_arrivers.count(uid) == 0 ) ++m_nfinishers;

	if (m_gamegrp->count == 1) return GER_end_of_game;

	return 0;
}

inline void Sledgame::notify_game_start()
{
	assert(m_gamegrp);
	TRACE_LOG("Sled Notify Game Start: grpid=%lu", m_gamegrp->id);

struct startpkg {
	protocol_t	header;
//	timeval		tv;
	uint32_t t_sec;
	uint32_t t_usec;
} __attribute__((__packed__));

	startpkg spkg;

	timeval tv;
	gettimeofday(&tv, 0);
	//spkg.tv.tv_sec  = htonl(tv.tv_sec);
	//spkg.tv.tv_usec = htonl(tv.tv_usec);

	spkg.t_sec  = htonl(tv.tv_sec);
	spkg.t_usec = htonl(tv.tv_usec);

	DEBUG_LOG("Sled notify_game_start %lu %lu", sizeof(protocol_t), sizeof spkg);

	// init protcol pkg
	init_proto_head(&spkg, start_game, sizeof spkg);
	send_to_group(m_gamegrp, &spkg, sizeof spkg);

	m_players.clear();
	m_start_tm = tv.tv_sec;
	for ( uint8_t i = 0; i != m_gamegrp->count; ++i ) {
		m_gamegrp->players[i]->last_act_time = m_start_tm;
	}
}

#endif // SLED_HPP_
