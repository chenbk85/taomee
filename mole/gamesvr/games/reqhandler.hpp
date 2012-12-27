#ifndef PLAYER_REQUEST_HANDLER_HPP_
#define PLAYER_REQUEST_HANDLER_HPP_

extern "C" {
#include <gameserv/game.h>
#include <gameserv/proto.h>
}

#include <map>
#include <set>
#include <utility>

#include <ant/inet/pdumanip.hpp>

/*
  *  Handle players' requests such as draw, surrender, and so on...
  *  The caller should restrict a player to make request only if it's his/her turn
  */
class ReqHandler {
public:
	ReqHandler(game_group_t* gamegrp);

	int     handle_player_req(const sprite_t* p, const uint8_t body[], int len);
	int     handle_rsp_player_req(const sprite_t* p, const uint8_t body[], int len);
	bool    handle_player_leave(uint32_t uid);

	void    reset() { reqs_made_.clear(); }

	UidReq  req_confirmed();
	bool    has_pending_req() const { return (req_.uid != 0); }
private:
	typedef std::set<ReqType>   ReqSet;
//	typedef std::map<uint32_t, bool>  RspMap;
	typedef std::set<uint32_t>  RspSet;

	struct ReqAndRspCount {
		ReqAndRspCount()
			{ reset(); }

		void reset()
			{ uid = nagree = ndisagree = 0; req = REQ_invalid; rsp.clear(); }

		uint32_t  uid;
		ReqType   req;
		uint8_t   nagree, ndisagree, cntneeded;
//		RspMap    rsp;
		RspSet    rsp;
	};

	struct RspReqPkg {
		uint32_t  uid;
		uint8_t   result;
	};

	bool   add_req(uint32_t uid, ReqType req);
	bool   add_rsp(uint32_t ruid, uint32_t uid, ReqType result);

	// TODO - later we might move these 2 function to ChessGame
	void   notify_req(uint32_t uid, ReqType req);
	void   rsp_player_req(uint32_t approver, uint32_t asker, uint8_t result);

	bool   isvalid(ReqType req);

	ReqAndRspCount req_;
	RspReqPkg      rsppkg_;
	ReqSet         reqs_made_;
	game_group_t*  gamegrp_;
};

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
inline
ReqHandler::ReqHandler(game_group_t* gamegrp)
{
	assert(gamegrp);

	gamegrp_ = gamegrp;
}

inline int
ReqHandler::handle_player_req(const sprite_t* p, const uint8_t body[], int len)
{
	if ( started_check(p, len, 1) == -1 ) return -1;

	ReqType req = static_cast<ReqType>(*body);
	if ( !isvalid(req) ) {
		ERROR_RETURN( ("%s: Wrong Req\t[reason=%d, uid=%u, grpid=%lu]",
						gamegrp_->game->name, req, p->id, gamegrp_->id), -1 );
	}
	if ( !add_req(p->id, req) ) {
		ERROR_RETURN( ("%s: Req %d From %u Is Pending\t[uid=%u, grpid=%lu]",
						gamegrp_->game->name, req_.req, req_.uid, p->id, gamegrp_->id), -1 );
	}

	notify_req(p->id, req);
	return 0;
}

inline int
ReqHandler::handle_rsp_player_req(const sprite_t* p, const uint8_t body[], int len)
{
	assert(p && body && gamegrp_);

	if ( started_check(p, len, 5) == -1 ) return -1;

	int i = 0;
	ant::unpack(body, rsppkg_.uid, i);
	ant::unpack(body, rsppkg_.result, i);
	if ( (p->id == rsppkg_.uid) || (rsppkg_.result > REQ_max) ) {
		ERROR_RETURN( ("%s: Wrong Rsp %d to %u from %u, grpid=%lu",
						gamegrp_->game->name, rsppkg_.result, rsppkg_.uid, p->id, gamegrp_->id), -1 );
	}
	if ( !add_rsp(p->id, rsppkg_.uid, static_cast<ReqType>(rsppkg_.result)) ) {
		ERROR_RETURN( ("%s: Wrong Rsp %d to %u from %u, grpid=%lu",
						gamegrp_->game->name, rsppkg_.result, rsppkg_.uid, p->id, gamegrp_->id), -1 );
	}

	rsp_player_req(p->id, rsppkg_.uid, rsppkg_.result);
	return 0;
}

/*
  * return: true if Req status is changed, otherwise false
  *
  */
inline bool
ReqHandler::
handle_player_leave(uint32_t uid)
{
	if ( has_pending_req() && (req_.uid != uid) && !(req_.rsp.count(uid)) ) {
		--req_.cntneeded;
		return true;
	}
	return false;
}

inline UidReq
ReqHandler::req_confirmed()
{
	assert(GAME_STARTED(gamegrp_));

	UidReq  uidreq     = { req_.uid, REQ_unconfirmed };
	uint8_t nagree     = req_.nagree;
	uint8_t ndisagree  = req_.ndisagree;
	ReqType req        = req_.req;
	if ( (nagree + ndisagree) == req_.cntneeded ) {
		uidreq.req = ((nagree > ndisagree) ? req : REQ_rejected);
		rsp_player_req(0, req_.uid, static_cast<uint8_t>(uidreq.req));
		req_.reset();
		return uidreq;
	}
	return uidreq;
}

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
inline bool
ReqHandler::add_req(uint32_t uid, ReqType req)
{
	assert(GAME_STARTED(gamegrp_));

	if (!req_.uid) { // if no pending req
		std::pair<ReqSet::iterator, bool> ret = reqs_made_.insert(req);
		if (ret.second) { // if req hasn't been made
			req_.uid = uid;
			req_.req = req;
			req_.cntneeded = gamegrp_->count - 1;
			return true;
		}
	}
	return false;
}

/**
 * add_rsp - add rsp to req
 * @ruid: responsor's uid
 * @uid: req uid
 * @result: rsp result
 *
 * returns: true if everything goes well, otherwise false
 */
inline bool
ReqHandler::add_rsp(uint32_t ruid, uint32_t uid, ReqType result)
{
	assert(GAME_STARTED(gamegrp_));

	if (req_.uid == uid) {
		if ( (result == req_.req) || (result == REQ_rejected) ) {
//			std::pair<RspMap::iterator, bool> ret
//				= req_.rsp.insert(RspMap::value_type(ruid, result));
			std::pair<RspSet::iterator, bool> ret = req_.rsp.insert(ruid);
			if (ret.second) {
				result ? ++(req_.nagree) : ++(req_.ndisagree);
				return true;
			}
		}
	}
	return false;
}

inline void
ReqHandler::notify_req(uint32_t uid, ReqType req)
{
	assert(GAME_STARTED(gamegrp_));

struct notif_reqpkg {
	protocol_t	header;
	uint32_t	uid;
	uint8_t		reason;
} __attribute__((__packed__));

	notif_reqpkg nrpkg;
	nrpkg.uid    = ant::bswap(uid);
	nrpkg.reason = ant::bswap(static_cast<uint8_t>(req));
	init_proto_head(&nrpkg, proto_notify_player_req, sizeof nrpkg);
	send_to_group_except_self(gamegrp_, uid, &nrpkg, sizeof nrpkg);
}

inline void
ReqHandler::rsp_player_req(uint32_t approver, uint32_t asker, uint8_t result)
{
	assert(GAME_STARTED(gamegrp_));

struct rspkg {
	protocol_t	header;
	uint32_t	approver;
	uint32_t	asker;
	uint8_t		result;
} __attribute__((__packed__));

	rspkg rpkg;
	init_proto_head(&rpkg, proto_player_req, sizeof rpkg);
	rpkg.approver  = ant::bswap(approver);
	rpkg.asker     = ant::bswap(asker);
	rpkg.result    = ant::bswap(result);
	send_to_group_except_self(gamegrp_, approver, &rpkg, sizeof rpkg);
}

inline bool
ReqHandler::isvalid(ReqType req)
{
	return ((req == REQ_draw) || req == (REQ_surrender));
}

#endif // PLAYER_REQUEST_HANDLER_HPP_
