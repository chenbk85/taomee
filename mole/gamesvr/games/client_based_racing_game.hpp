#ifndef CLIENT_BASED_RACING_GAME_HPP_
#define CLIENT_BASED_RACING_GAME_HPP_

#include "downcounter.hpp"
#include "game_start_notifier.hpp"
#include "mpog.hpp"
#include "racingreadyer.hpp"

template <typename RulePolicy, typename DownCounterPolicy = DownCounter0>
class ClientBasedRacingGame : public mpog {
public:
	ClientBasedRacingGame(game_group_t* grp);

	void init(sprite_t* p);
	int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
	int  handle_timeout(void* data);
private:
	int  handle_player_action(const uint8_t body[], int len);
	int  handle_player_leave();
	int  handle_player_ready(const uint8_t body[], int len);

	void broadcast_action(const uint8_t body[], int len) const;

	sprite_t*     cur_player_;
	game_group_t* gamegrp_;

	RacingReadyer readyer_;
	RulePolicy    racingrule_;

	DownCounterPolicy downcounter_;

	static const time_t c_timeout_l = 15;
	static const time_t c_timeout_s = 8;
};

//---------------------------------------------------------------------
template <typename RulePolicy, typename DownCounterPolicy>
inline ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
ClientBasedRacingGame(game_group_t* grp)
	: readyer_(grp), racingrule_(grp), downcounter_(3, grp, on_game_timer_expire, (void*)1)
{
	gamegrp_ = grp;
}

template <typename RulePolicy, typename DownCounterPolicy>
inline void ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
init(sprite_t* p)
{
	racingrule_.sendmap();

	add_grp_timers(gamegrp_, get_now_tv()->tv_sec + c_timeout_l);
}

template <typename RulePolicy, typename DownCounterPolicy>
inline int ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	cur_player_ = p;

	int err;
	switch (cmd) {
	case proto_player_leave:
		err = handle_player_leave();
		break;
	case proto_client_ready:
		err = handle_player_ready(body, len);
		break;
	case proto_player_action:
		err = handle_player_action(body, len);
		break;
	default:
		ERROR_RETURN(("%s: Unrecognized Command ID %d from uid=%u", gamegrp_->game->name, cmd, p->id), -1);
		break;
	}

	return err;
}

template <typename RulePolicy, typename DownCounterPolicy>
inline int ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
handle_timeout(void* data)
{
	SET_GAME_START(gamegrp_);
	notify_game_start0(gamegrp_);
	racingrule_.set_start_time(get_now_tv()->tv_sec);

	DEBUG_LOG("%s Started\t[grpid=%lu, nplayers=%d]", gamegrp_->game->name, gamegrp_->id, gamegrp_->count);
	return 0;
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
template <typename RulePolicy, typename DownCounterPolicy>
inline int ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
handle_player_action(const uint8_t body[], int len)
{
	if ( started_check_ge(cur_player_, len, 1) == -1 ) {
		return -1;
	}

	int err = racingrule_.handle_player_action(cur_player_, body, len);
	if (!err) {
		broadcast_action(body, len);
		time_t exptm = get_now_tv()->tv_sec + c_timeout_s;
		MOD_EVENT_EXPIRE_TIME(cur_player_, on_timer_expire, exptm);
	}
	return err;
}

template <typename RulePolicy, typename DownCounterPolicy>
inline int ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
handle_player_leave()
{
	int err = 0;
	if ( GAME_STARTED(gamegrp_) ) {
		// return value from handle_player_leave should >= 0
		err = racingrule_.handle_player_leave(cur_player_);
	} else {
		readyer_.remove(cur_player_->id);
	}

	if ( err || (gamegrp_->count == 1) ) {
		return GER_end_of_game;
	}
	return 0;
}

template <typename RulePolicy, typename DownCounterPolicy>
inline int ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
handle_player_ready(const uint8_t body[], int len)
{
	if (ready_check(cur_player_, len, 0) == -1) {
		return -1;
	}
	if ( !readyer_.add(cur_player_->id) ) {
		ERROR_RETURN( ("%u Has Already Been In Stand-by Status", cur_player_->id), -1 );
	}

	time_t exprtm = get_now_tv()->tv_sec + c_timeout_l;
	if ( readyer_.gamestartable() ) {
		mod_grp_timers(gamegrp_, exprtm);
		downcounter_.start();
	} else {
		MOD_EVENT_EXPIRE_TIME(cur_player_, on_timer_expire, exprtm);
	}

	return 0;
}

template <typename RulePolicy, typename DownCounterPolicy>
inline void ClientBasedRacingGame<RulePolicy, DownCounterPolicy>::
broadcast_action(const uint8_t body[], int len) const
{
	int i = sizeof(protocol_t);
	ant::pack(pkg, cur_player_->id, i);
	ant::pack(pkg, body, len, i);
	init_proto_head(pkg, proto_player_action, i);
	send_to_group_except_self(gamegrp_, cur_player_->id, pkg, i);
}
//---------------------------------------------------------------------

#endif // CLIENT_BASED_RACING_GAME_HPP_
