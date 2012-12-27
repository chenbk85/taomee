#ifndef CHESS_GAME_HPP_
#define CHESS_GAME_HPP_

extern "C" {
#include <gameserv/proto.h>
#include <gameserv/timer.h>
}

#include <ant/inet/pdumanip.hpp>

#include "mpog.hpp"

template <typename RulePolicy, typename ReqHandlePolicy>
class ChessGame : public mpog {
public:
	ChessGame(game_group_t* grp);

	void    init(sprite_t* p);
	int     handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
protected:
	// TODO - might need to be virtual
	int     handle_player_action(const uint8_t body[], int len);
	int     handle_rsp_player_req(const uint8_t body[], int len);
	int     handle_player_leave();

 	void    broadcast_action(const void* buf, int buflen, int outlen) const;
	void    notify_game_start() const;
private:
	game_group_t*      gamegrp_;
	sprite_t*          cur_sp_;
	uint32_t           nxmover_;
	timer_id_t         timerid;

	RulePolicy         chessrule_;
	ReqHandlePolicy    reqhandler_;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
template <typename RulePolicy, typename ReqHandlePolicy>
inline ChessGame<RulePolicy, ReqHandlePolicy>::
ChessGame(game_group_t* gamegrp) : chessrule_(gamegrp), reqhandler_(gamegrp)
{
	assert(gamegrp);

	gamegrp_  = gamegrp;
	cur_sp_   = 0;
	nxmover_  = chessrule_.nextmover();
	timerid   = 0;
}

template <typename RulePolicy, typename ReqHandlePolicy>
inline void ChessGame<RulePolicy, ReqHandlePolicy>::
init(sprite_t* p)
{
	assert(GAME_READY(gamegrp_));

	if ( IS_GAME_PLAYER(p) ) {
		SET_GAME_START(gamegrp_);
		notify_game_start();
		chessrule_.notify_board_info(p, false);
		sprite_t* op = get_sprite_from_gamegrp(nxmover_, gamegrp_);
		time_t exptm = time(0) + 62;
		timerid = ADD_TIMER_EVENT(op, on_timer_expire, 0, exptm);
	} else {
		chessrule_.notify_board_info(p);
	}
}

template <typename RulePolicy, typename ReqHandlePolicy>
inline int ChessGame<RulePolicy, ReqHandlePolicy>::
handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	cur_sp_ = p;

	int err;
	switch (cmd) {
	case proto_player_action:
		err = handle_player_action(body, len);
		break;
	case proto_player_req:
		// a player can make a request only when in his/her turn
		if (nxmover_ == p->id) {
			err = reqhandler_.handle_player_req(p, body, len);
		} else {
			err = -1;
		}
		break;
	case proto_notify_player_req:
		err = handle_rsp_player_req(body, len);
		break;
	case proto_player_leave:
		// TODO
		err = handle_player_leave();
		break;
	default:
		ERROR_RETURN(("%s: Unrecognized Command ID %d from uid=%u",
						gamegrp_->game->name, cmd, p->id), -1);
		break;
	}

	return err;
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
template <typename RulePolicy, typename ReqHandlePolicy>
inline int ChessGame<RulePolicy, ReqHandlePolicy>::
handle_player_action(const uint8_t body[], int len)
{
	if ( started_check_ge(cur_sp_, len, 1) == -1 ) {
		return -1;
	}
	if ( (nxmover_ != cur_sp_->id) || reqhandler_.has_pending_req() ) {
		ERROR_RETURN( ("%s: Cannot Move Now!\t[uid=%u nxmover=%u pendreq=%u]",
					gamegrp_->game->name, cur_sp_->id, nxmover_, reqhandler_.has_pending_req()), -1 );
	}

	int padlen = sizeof(protocol_t) + 4 + len;
	int outlen = sizeof pkg - padlen;
	int err    = chessrule_.place_chess(cur_sp_, body, len, pkg + padlen, outlen);
	if (err == -2) {
		return send_to_self_error(cur_sp_, cur_sp_->waitcmd, 1, 1);
	} else if ( err == -1 ) {
		return -1;
	}

	REMOVE_TIMER(cur_sp_, timerid);
	broadcast_action(body, len, outlen);

	if ( chessrule_.winner_determinate() ) {
		chessrule_.notify_winner(cur_sp_);
		chessrule_.send_gamescore(cur_sp_);
		if ( chessrule_.ended() ) {
			return GER_end_of_game;
		}
	} else if ( chessrule_.draw() ) {
		chessrule_.notify_winner(cur_sp_);
		chessrule_.send_gamescore(cur_sp_);
		return GER_draw_game;
	}
	nxmover_ = chessrule_.nextmover();
	reqhandler_.reset();
	sprite_t* op = get_sprite_from_gamegrp(nxmover_, gamegrp_);
	time_t exptm = time(0) + 62;
	timerid = ADD_TIMER_EVENT(op, on_timer_expire, 0, exptm);
	return 0;
}

template <typename RulePolicy, typename ReqHandlePolicy>
inline int ChessGame<RulePolicy, ReqHandlePolicy>::
handle_player_leave()
{
	// return value from chessrule_.handle_player_leave() should >= 0
	int err = chessrule_.handle_player_leave(cur_sp_);
	if (!err) {
		if (reqhandler_.handle_player_leave(cur_sp_->id)) {
			// call handle_rsp_player_req() if Req status is changed
			err = handle_rsp_player_req(0, 0);
		}
	}
	return err;
}

template <typename RulePolicy, typename ReqHandlePolicy>
inline int ChessGame<RulePolicy, ReqHandlePolicy>::
handle_rsp_player_req(const uint8_t body[], int len)
{
	int err = 0;
	if (body) {
		err = reqhandler_.handle_rsp_player_req(cur_sp_, body, len);
	}
	if (!err) {
		UidReq ret = reqhandler_.req_confirmed();
		if ( (ret.req != REQ_unconfirmed) && (ret.req != REQ_rejected) ) {
			err = chessrule_.handle_confirmed_req(cur_sp_, ret);
			nxmover_ = chessrule_.nextmover();
			reqhandler_.reset();
		}
	}
	return err;
}

template <typename RulePolicy, typename ReqHandlePolicy>
inline void ChessGame<RulePolicy, ReqHandlePolicy>::
broadcast_action(const void* buf, int buflen, int outlen) const
{
	int i = sizeof(protocol_t);
	ant::pack(pkg, cur_sp_->id, i);
	ant::pack(pkg, buf, buflen, i);
	i += outlen;
	// TODO - might need to pack next mover here, which is needed by some games
	init_proto_head(pkg, proto_player_action, i);
	//send_to_group_except_self(gamegrp_, cur_sp_->id, pkg, i);
	send_to_group(gamegrp_, pkg, i);
}

template <typename RulePolicy, typename ReqHandlePolicy>
inline void ChessGame<RulePolicy, ReqHandlePolicy>::
notify_game_start() const
{
	int i = sizeof(protocol_t);
	// only the next mover is packed coz the actual move order is indeterminate for some games
	ant::pack(pkg, nxmover_, i);
	init_proto_head(pkg, proto_game_start, i);
	send_to_players(gamegrp_, pkg, i);
}

#endif // CHESS_GAME_HPP_
